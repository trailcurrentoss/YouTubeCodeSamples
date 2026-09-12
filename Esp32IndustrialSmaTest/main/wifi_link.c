#include "wifi_link.h"

#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "sdkconfig.h"

static const char *TAG = "wifi_link";

#define BIT_GOT_IP    BIT0
#define BIT_DISCONNECT BIT1

static EventGroupHandle_t s_events;
static esp_netif_t       *s_netif;
static bool               s_initialised;
static bool               s_connected;

/* Association is retried only while a connect attempt is in flight. Outside of
 * one we let the link stay down so an idle board is not transmitting during
 * someone else's measurement run. */
static bool s_connect_in_flight;

static void on_wifi_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg;
    (void)base;

    switch (id) {
    case WIFI_EVENT_STA_DISCONNECTED: {
        const wifi_event_sta_disconnected_t *ev = data;
        s_connected = false;
        if (s_connect_in_flight) {
            ESP_LOGD(TAG, "disconnected (reason %d), retrying", ev->reason);
            esp_wifi_connect();
        } else {
            xEventGroupSetBits(s_events, BIT_DISCONNECT);
        }
        break;
    }
    case WIFI_EVENT_STA_CONNECTED:
        s_connected = true;
        break;
    default:
        break;
    }
}

static void on_ip_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg;
    (void)base;

    if (id == IP_EVENT_STA_GOT_IP) {
        const ip_event_got_ip_t *ev = data;
        ESP_LOGI(TAG, "got ip " IPSTR, IP2STR(&ev->ip_info.ip));
        xEventGroupSetBits(s_events, BIT_GOT_IP);
    }
}

esp_err_t wifi_link_init(void)
{
    if (s_initialised) {
        return ESP_OK;
    }

    s_events = xEventGroupCreate();
    if (s_events == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    s_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, on_wifi_event, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, on_ip_event, NULL, NULL));

    /* Nothing here needs to survive a reboot, and a flash-backed config would
     * be one more thing that differs between the two runs. */
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Modem sleep would let the radio doze between beacons; the resulting gaps
     * show up as variance in the RSSI series rather than as antenna gain. */
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    s_initialised = true;
    ESP_LOGI(TAG, "station started, power save disabled");
    return ESP_OK;
}

/* Descending by RSSI, so the strongest APs land at the front of the list. */
static int cmp_ap_rssi_desc(const void *a, const void *b)
{
    const wifi_ap_record_t *ra = a;
    const wifi_ap_record_t *rb = b;
    return (int)rb->rssi - (int)ra->rssi;
}

esp_err_t wifi_link_scan(wifi_scan_census_t *out)
{
    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));

    wifi_scan_config_t scan_cfg = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0, /* every channel */
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active = { .min = 60, .max = 150 },
    };

    esp_err_t err = esp_wifi_scan_start(&scan_cfg, true /* block */);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "scan failed: %s", esp_err_to_name(err));
        return err;
    }

    uint16_t found = 0;
    err = esp_wifi_scan_get_ap_num(&found);
    if (err != ESP_OK) {
        return err;
    }

    uint16_t wanted = found;
    if (wanted > CONFIG_AB_SCAN_MAX_AP) {
        ESP_LOGW(TAG, "scan heard %u APs, recording the first %d",
                 (unsigned)found, CONFIG_AB_SCAN_MAX_AP);
        wanted = CONFIG_AB_SCAN_MAX_AP;
    }

    if (wanted == 0) {
        esp_wifi_clear_ap_list();
        return ESP_OK;
    }

    wifi_ap_record_t *records = calloc(wanted, sizeof(wifi_ap_record_t));
    if (records == NULL) {
        esp_wifi_clear_ap_list();
        return ESP_ERR_NO_MEM;
    }

    err = esp_wifi_scan_get_ap_records(&wanted, records);
    if (err != ESP_OK) {
        free(records);
        return err;
    }

    int32_t sum = 0;
    int8_t  best = INT8_MIN;
    for (uint16_t i = 0; i < wanted; i++) {
        sum += records[i].rssi;
        if (records[i].rssi > best) {
            best = records[i].rssi;
        }
        if (!out->target_found &&
            strncmp((const char *)records[i].ssid, CONFIG_AB_WIFI_SSID,
                    sizeof(records[i].ssid)) == 0) {
            out->target_found = true;
            out->target_rssi = records[i].rssi;
        }
    }

    out->ap_count = wanted;
    out->rssi_mean = (float)sum / (float)wanted;
    out->rssi_best = best;

    /* Retain the strongest few by BSSID. The strong end of the list is the
     * stable end -- those APs are heard on every scan, so they are the ones
     * that can be paired up across two runs. */
    qsort(records, wanted, sizeof(records[0]), cmp_ap_rssi_desc);

    out->top_n = (wanted < RF_CENSUS_TOP_N) ? wanted : RF_CENSUS_TOP_N;
    for (uint32_t i = 0; i < out->top_n; i++) {
        memcpy(out->top[i].bssid, records[i].bssid, 6);
        out->top[i].rssi = records[i].rssi;
    }

    free(records);
    return ESP_OK;
}

esp_err_t wifi_link_connect(uint32_t *elapsed_ms_out)
{
    wifi_config_t wifi_cfg = { 0 };

    /* sta.ssid is uint8_t[32] and is NOT required to be NUL-terminated -- a
     * full 32-character SSID fills the field exactly. strlcpy() would reserve
     * a byte for the terminator and silently truncate such an SSID to 31
     * characters, which then fails to associate for no visible reason. */
    const size_t ssid_len = strlen(CONFIG_AB_WIFI_SSID);
    if (ssid_len > sizeof(wifi_cfg.sta.ssid)) {
        ESP_LOGE(TAG, "SSID is %u bytes; the 802.11 maximum is %u",
                 (unsigned)ssid_len, (unsigned)sizeof(wifi_cfg.sta.ssid));
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(wifi_cfg.sta.ssid, CONFIG_AB_WIFI_SSID, ssid_len);

    strlcpy((char *)wifi_cfg.sta.password, CONFIG_AB_WIFI_PASSWORD,
            sizeof(wifi_cfg.sta.password));

    /* Take whatever the AP offers rather than pinning WPA2. An open reference
     * AP is a legitimate test setup. */
    wifi_cfg.sta.threshold.authmode = WIFI_AUTH_OPEN;
    wifi_cfg.sta.pmf_cfg.capable = true;
    wifi_cfg.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));

    xEventGroupClearBits(s_events, BIT_GOT_IP | BIT_DISCONNECT);
    s_connect_in_flight = true;

    const int64_t started = esp_timer_get_time();
    esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK && err != ESP_ERR_WIFI_CONN) {
        s_connect_in_flight = false;
        return err;
    }

    EventBits_t bits = xEventGroupWaitBits(
        s_events, BIT_GOT_IP, pdFALSE, pdFALSE,
        pdMS_TO_TICKS(CONFIG_AB_WIFI_CONNECT_TIMEOUT_MS));

    s_connect_in_flight = false;

    if ((bits & BIT_GOT_IP) == 0) {
        ESP_LOGE(TAG, "no IP within %d ms", CONFIG_AB_WIFI_CONNECT_TIMEOUT_MS);
        esp_wifi_disconnect();
        return ESP_ERR_TIMEOUT;
    }

    if (elapsed_ms_out != NULL) {
        *elapsed_ms_out = (uint32_t)((esp_timer_get_time() - started) / 1000);
    }
    return ESP_OK;
}

esp_err_t wifi_link_disconnect(void)
{
    if (!s_connected) {
        return ESP_OK;
    }
    s_connect_in_flight = false;
    xEventGroupClearBits(s_events, BIT_DISCONNECT);

    esp_err_t err = esp_wifi_disconnect();
    if (err != ESP_OK) {
        return err;
    }
    xEventGroupWaitBits(s_events, BIT_DISCONNECT, pdTRUE, pdFALSE,
                        pdMS_TO_TICKS(3000));
    return ESP_OK;
}

bool wifi_link_is_connected(void)
{
    return s_connected;
}

esp_err_t wifi_link_ap_info(wifi_ap_record_t *out)
{
    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return esp_wifi_sta_get_ap_info(out);
}

esp_err_t wifi_link_gateway_str(char *buf, size_t len)
{
    if (buf == NULL || len < 8 || s_netif == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_netif_ip_info_t ip_info;
    esp_err_t err = esp_netif_get_ip_info(s_netif, &ip_info);
    if (err != ESP_OK) {
        return err;
    }
    if (ip_info.gw.addr == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    snprintf(buf, len, IPSTR, IP2STR(&ip_info.gw));
    return ESP_OK;
}

int8_t wifi_link_tx_power_qdbm(void)
{
    int8_t power = 0;
    if (esp_wifi_get_max_tx_power(&power) != ESP_OK) {
        return 0;
    }
    return power;
}
