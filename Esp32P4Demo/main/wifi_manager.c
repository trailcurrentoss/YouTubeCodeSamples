/*
 * Wi-Fi lifecycle for the Waveshare ESP32-P4-WIFI6.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_mac.h"
#include "esp_timer.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "lwip/ip4_addr.h"

#include "wifi_manager.h"

static const char *TAG = "wifi_mgr";

#define NVS_NAMESPACE  "tc_wifi"
#define NVS_KEY_SSID   "ssid"
#define NVS_KEY_PASS   "pass"

#define BIT_CONNECTED  BIT0
#define BIT_FAILED     BIT1

static EventGroupHandle_t s_events;
static esp_netif_t      *s_netif_sta;
static esp_netif_t      *s_netif_ap;
static wifi_mgr_mode_t   s_mode = WIFI_MGR_MODE_IDLE;
static char              s_ap_ssid[WIFI_MGR_SSID_MAX + 1];
static char              s_sta_ssid[WIFI_MGR_SSID_MAX + 1];
static bool              s_connected;

/* While a candidate network is being tested we must not let the background
 * reconnect logic fight the test, so retries are suppressed for the duration. */
static bool              s_auto_reconnect = true;

/* Reason code from the most recent disconnect, so a failed verification can
 * say what actually went wrong instead of blaming the password every time. */
static uint8_t           s_last_reason;

static esp_timer_handle_t s_reconnect_timer;

/* -------------------------------------------------------------------------
 * Event handling
 * ------------------------------------------------------------------------- */
static void reconnect_timer_cb(void *arg)
{
    esp_wifi_connect();
}

static void on_wifi_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    switch (id) {
    case WIFI_EVENT_STA_START:
        /* Only chase a connection when one is actually wanted. In provisioning
         * mode the station interface exists purely to scan and to test a
         * candidate, and connecting here with no SSID configured produces a
         * stream of failure events that later poison the verification. */
        if (s_mode == WIFI_MGR_MODE_STATION) {
            esp_wifi_connect();
        }
        break;

    case WIFI_EVENT_STA_DISCONNECTED: {
        const wifi_event_sta_disconnected_t *ev = data;
        s_connected = false;
        s_last_reason = ev->reason;
        xEventGroupClearBits(s_events, BIT_CONNECTED);
        xEventGroupSetBits(s_events, BIT_FAILED);
        if (s_auto_reconnect) {
            ESP_LOGW(TAG, "Disconnected (reason %d), retrying in 2 s", ev->reason);
            /* Never block in the event handler — it runs on the shared event
             * task, and sleeping here stalls every other event including the
             * ones this module depends on. */
            esp_timer_start_once(s_reconnect_timer, 2000 * 1000);
        } else {
            ESP_LOGW(TAG, "Connection attempt failed (reason %d)", ev->reason);
        }
        break;
    }

    case WIFI_EVENT_AP_STACONNECTED: {
        const wifi_event_ap_staconnected_t *ev = data;
        ESP_LOGI(TAG, "Client "MACSTR" joined the setup network", MAC2STR(ev->mac));
        break;
    }

    case WIFI_EVENT_AP_STADISCONNECTED: {
        const wifi_event_ap_stadisconnected_t *ev = data;
        ESP_LOGI(TAG, "Client "MACSTR" left the setup network", MAC2STR(ev->mac));
        break;
    }

    default:
        break;
    }
}

static void on_ip_event(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    if (id == IP_EVENT_STA_GOT_IP) {
        const ip_event_got_ip_t *ev = data;
        ESP_LOGI(TAG, "Got IP " IPSTR, IP2STR(&ev->ip_info.ip));
        s_connected = true;
        xEventGroupClearBits(s_events, BIT_FAILED);
        xEventGroupSetBits(s_events, BIT_CONNECTED);
    }
}

/* -------------------------------------------------------------------------
 * Init
 * ------------------------------------------------------------------------- */
esp_err_t wifi_manager_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "NVS init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    s_events = xEventGroupCreate();
    if (s_events == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    s_netif_sta = esp_netif_create_default_wifi_sta();
    s_netif_ap  = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        /* On this board esp_wifi_init talks to the C6 over SDIO. A failure here
         * is nearly always the companion radio, not the Wi-Fi settings. */
        ESP_LOGE(TAG, "esp_wifi_init failed: %s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "The ESP32-P4 has no radio of its own — check that the "
                      "onboard ESP32-C6 is running ESP-Hosted slave firmware");
        return ret;
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &on_wifi_event, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &on_ip_event, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    const esp_timer_create_args_t timer_args = {
        .callback = &reconnect_timer_cb,
        .name = "wifi_reconnect",
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &s_reconnect_timer));

    /* Name the setup network after the radio MAC so two boards on a bench do
     * not advertise the same SSID. */
    uint8_t mac[6] = {0};
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(s_ap_ssid, sizeof(s_ap_ssid), "%s-%02X%02X",
             CONFIG_DEMO_WIFI_AP_SSID_PREFIX, mac[4], mac[5]);

    return ESP_OK;
}

/* -------------------------------------------------------------------------
 * Credential storage
 * ------------------------------------------------------------------------- */
static esp_err_t load_credentials(char *ssid, size_t ssid_len,
                                  char *pass, size_t pass_len)
{
    nvs_handle_t h;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &h);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = nvs_get_str(h, NVS_KEY_SSID, ssid, &ssid_len);
    if (ret == ESP_OK && pass) {
        size_t plen = pass_len;
        if (nvs_get_str(h, NVS_KEY_PASS, pass, &plen) != ESP_OK) {
            pass[0] = '\0';  /* open network */
        }
    }
    nvs_close(h);
    return ret;
}

bool wifi_manager_has_credentials(void)
{
    char ssid[WIFI_MGR_SSID_MAX + 1] = {0};
    return load_credentials(ssid, sizeof(ssid), NULL, 0) == ESP_OK && ssid[0] != '\0';
}

esp_err_t wifi_manager_save_credentials(const char *ssid, const char *password)
{
    if (ssid == NULL || ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t h;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = nvs_set_str(h, NVS_KEY_SSID, ssid);
    if (ret == ESP_OK) {
        ret = nvs_set_str(h, NVS_KEY_PASS, password ? password : "");
    }
    if (ret == ESP_OK) {
        ret = nvs_commit(h);
    }
    nvs_close(h);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Saved credentials for \"%s\"", ssid);
    } else {
        ESP_LOGE(TAG, "Failed to save credentials: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t wifi_manager_erase_credentials(void)
{
    nvs_handle_t h;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
    if (ret != ESP_OK) {
        return ret;
    }
    nvs_erase_key(h, NVS_KEY_SSID);
    nvs_erase_key(h, NVS_KEY_PASS);
    ret = nvs_commit(h);
    nvs_close(h);
    ESP_LOGW(TAG, "Stored Wi-Fi credentials erased");
    return ret;
}

/* -------------------------------------------------------------------------
 * Provisioning (SoftAP)
 * ------------------------------------------------------------------------- */
esp_err_t wifi_manager_start_provisioning(void)
{
    wifi_config_t ap_cfg = {0};
    strlcpy((char *)ap_cfg.ap.ssid, s_ap_ssid, sizeof(ap_cfg.ap.ssid));
    ap_cfg.ap.ssid_len       = strlen(s_ap_ssid);
    ap_cfg.ap.channel        = CONFIG_DEMO_WIFI_AP_CHANNEL;
    ap_cfg.ap.max_connection = 4;
    /* Open by default: the setup network exists only until provisioning
     * finishes, and a shared documented PSK would deter casual sniffing at
     * best. Anyone who wants link encryption can set a password in menuconfig.
     *
     * Worth knowing either way: the setup page is served over plain HTTP, so
     * on an open network the Wi-Fi password being entered is visible to anyone
     * in radio range for the duration of setup. */
    const char *ap_pass = CONFIG_DEMO_WIFI_AP_PASSWORD;
    const size_t pass_len = strlen(ap_pass);

    if (pass_len == 0) {
        ap_cfg.ap.authmode = WIFI_AUTH_OPEN;
    } else if (pass_len < 8) {
        /* WPA2 cannot use a key this short, so honour the intent as far as
         * possible but say plainly that the network is not protected. */
        ap_cfg.ap.authmode = WIFI_AUTH_OPEN;
        ESP_LOGW(TAG, "Setup password is only %u characters — WPA2 needs 8, "
                      "so the setup network is OPEN", (unsigned)pass_len);
    } else {
        ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
        strlcpy((char *)ap_cfg.ap.password, ap_pass, sizeof(ap_cfg.ap.password));
    }

    /* APSTA, not AP: the station interface is needed to scan and to test a
     * candidate network without kicking off the client running the setup UI. */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    s_auto_reconnect = false;   /* nothing to reconnect to yet */
    s_mode = WIFI_MGR_MODE_PROVISIONING;

    esp_netif_ip_info_t ip = {0};
    esp_netif_get_ip_info(s_netif_ap, &ip);
    ESP_LOGI(TAG, "Setup network \"%s\" is up on " IPSTR, s_ap_ssid, IP2STR(&ip.ip));
    return ESP_OK;
}

/* -------------------------------------------------------------------------
 * Station
 * ------------------------------------------------------------------------- */
esp_err_t wifi_manager_start_station(void)
{
    char ssid[WIFI_MGR_SSID_MAX + 1] = {0};
    char pass[WIFI_MGR_PASS_MAX + 1] = {0};

    esp_err_t ret = load_credentials(ssid, sizeof(ssid), pass, sizeof(pass));
    if (ret != ESP_OK) {
        return ret;
    }

    wifi_config_t sta_cfg = {0};
    strlcpy((char *)sta_cfg.sta.ssid, ssid, sizeof(sta_cfg.sta.ssid));
    strlcpy((char *)sta_cfg.sta.password, pass, sizeof(sta_cfg.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));

    strlcpy(s_sta_ssid, ssid, sizeof(s_sta_ssid));
    s_auto_reconnect = true;
    s_mode = WIFI_MGR_MODE_STATION;

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Joining \"%s\"", ssid);
    return ESP_OK;
}

esp_err_t wifi_manager_try_connect(const char *ssid, const char *password,
                                   uint32_t timeout_ms)
{
    if (ssid == NULL || ssid[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    wifi_config_t sta_cfg = {0};
    strlcpy((char *)sta_cfg.sta.ssid, ssid, sizeof(sta_cfg.sta.ssid));
    if (password) {
        strlcpy((char *)sta_cfg.sta.password, password, sizeof(sta_cfg.sta.password));
    }

    /* Keep the background retry loop out of the way; this function drives its
     * own attempts so it can report a result rather than looping forever. */
    s_auto_reconnect = false;

    /* Drop any existing association and let the resulting disconnect event land
     * BEFORE the event bits are cleared. Clearing first and disconnecting after
     * leaves a stale DISCONNECTED event in flight that sets BIT_FAILED as soon
     * as the wait starts, which reads as "wrong password" on the very first
     * attempt no matter what was typed. */
    esp_wifi_disconnect();
    vTaskDelay(pdMS_TO_TICKS(150));

    esp_err_t ret = esp_wifi_set_config(WIFI_IF_STA, &sta_cfg);
    if (ret != ESP_OK) {
        return ret;
    }

    /*
     * Retry before giving up. A single disconnect event is not proof of a bad
     * password: the first association regularly fails with a transient reason
     * (the AP is mid-scan, busy, or the radio is still settling after the scan
     * that populated the picker) and succeeds immediately afterwards. Treating
     * attempt one as final is what made correct passwords look wrong.
     */
    const int max_attempts = 3;
    const uint32_t per_attempt_ms = timeout_ms / max_attempts;

    for (int attempt = 1; attempt <= max_attempts; attempt++) {
        xEventGroupClearBits(s_events, BIT_CONNECTED | BIT_FAILED);
        s_last_reason = 0;

        ESP_LOGI(TAG, "Testing \"%s\" (attempt %d/%d)", ssid, attempt, max_attempts);
        ret = esp_wifi_connect();
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "esp_wifi_connect failed: %s", esp_err_to_name(ret));
            continue;
        }

        const EventBits_t bits = xEventGroupWaitBits(
            s_events, BIT_CONNECTED | BIT_FAILED, pdFALSE, pdFALSE,
            pdMS_TO_TICKS(per_attempt_ms));

        if (bits & BIT_CONNECTED) {
            strlcpy(s_sta_ssid, ssid, sizeof(s_sta_ssid));
            ESP_LOGI(TAG, "\"%s\" verified", ssid);
            return ESP_OK;
        }

        if (bits & BIT_FAILED) {
            /* Only an authentication-specific reason is worth failing early
             * for — retrying a genuinely wrong password just wastes the
             * user's time. Everything else gets another go. */
            switch (s_last_reason) {
            case WIFI_REASON_AUTH_FAIL:
            case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
            case WIFI_REASON_HANDSHAKE_TIMEOUT:
                ESP_LOGW(TAG, "\"%s\" rejected the password (reason %d)",
                         ssid, s_last_reason);
                return ESP_ERR_WIFI_PASSWORD;
            case WIFI_REASON_NO_AP_FOUND:
                ESP_LOGW(TAG, "\"%s\" not found on attempt %d", ssid, attempt);
                break;
            default:
                ESP_LOGW(TAG, "\"%s\" attempt %d failed (reason %d), retrying",
                         ssid, attempt, s_last_reason);
                break;
            }
        } else {
            ESP_LOGW(TAG, "\"%s\" attempt %d timed out", ssid, attempt);
        }

        vTaskDelay(pdMS_TO_TICKS(300));
    }

    if (s_last_reason == WIFI_REASON_NO_AP_FOUND) {
        return ESP_ERR_WIFI_SSID;
    }
    ESP_LOGW(TAG, "\"%s\" did not connect after %d attempts (last reason %d)",
             ssid, max_attempts, s_last_reason);
    return ESP_ERR_TIMEOUT;
}

uint8_t wifi_manager_last_disconnect_reason(void)
{
    return s_last_reason;
}

/* -------------------------------------------------------------------------
 * Scan
 * ------------------------------------------------------------------------- */
static int cmp_rssi(const void *a, const void *b)
{
    const wifi_ap_record_t *ra = a, *rb = b;
    return rb->rssi - ra->rssi;   /* strongest first */
}

esp_err_t wifi_manager_scan(wifi_ap_record_t **out_records, uint16_t *out_count)
{
    if (out_records == NULL || out_count == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    *out_records = NULL;
    *out_count = 0;

    const wifi_scan_config_t scan_cfg = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = false,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active = { .min = 120, .max = 300 },
    };

    esp_err_t ret = esp_wifi_scan_start(&scan_cfg, true /* block */);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Scan failed: %s", esp_err_to_name(ret));
        return ret;
    }

    uint16_t found = 0;
    esp_wifi_scan_get_ap_num(&found);
    if (found == 0) {
        return ESP_OK;
    }
    if (found > CONFIG_DEMO_WIFI_SCAN_MAX_AP) {
        found = CONFIG_DEMO_WIFI_SCAN_MAX_AP;
    }

    wifi_ap_record_t *records = calloc(found, sizeof(*records));
    if (records == NULL) {
        esp_wifi_clear_ap_list();
        return ESP_ERR_NO_MEM;
    }

    ret = esp_wifi_scan_get_ap_records(&found, records);
    if (ret != ESP_OK) {
        free(records);
        return ret;
    }

    qsort(records, found, sizeof(*records), cmp_rssi);

    /* Collapse duplicate SSIDs — a mesh or a dual-band router shows the same
     * name several times, which is just noise in a picker. Records are sorted
     * strongest-first, so the first of each name is the one worth keeping. */
    uint16_t unique = 0;
    for (uint16_t i = 0; i < found; i++) {
        if (records[i].ssid[0] == '\0') {
            continue;
        }
        bool seen = false;
        for (uint16_t j = 0; j < unique; j++) {
            if (strcmp((char *)records[i].ssid, (char *)records[j].ssid) == 0) {
                seen = true;
                break;
            }
        }
        if (!seen) {
            records[unique++] = records[i];
        }
    }

    *out_records = records;
    *out_count = unique;
    ESP_LOGI(TAG, "Scan found %u networks (%u after deduplication)", found, unique);
    return ESP_OK;
}

/* -------------------------------------------------------------------------
 * Status
 * ------------------------------------------------------------------------- */
void wifi_manager_get_status(wifi_mgr_status_t *out)
{
    if (out == NULL) {
        return;
    }
    memset(out, 0, sizeof(*out));

    out->mode = s_mode;
    out->connected = s_connected;

    uint8_t mac[6] = {0};
    esp_wifi_get_mac(s_mode == WIFI_MGR_MODE_PROVISIONING ? WIFI_IF_AP : WIFI_IF_STA, mac);
    snprintf(out->mac, sizeof(out->mac), MACSTR, MAC2STR(mac));

    esp_netif_ip_info_t ip = {0};
    if (s_mode == WIFI_MGR_MODE_PROVISIONING) {
        strlcpy(out->ssid, s_ap_ssid, sizeof(out->ssid));
        esp_netif_get_ip_info(s_netif_ap, &ip);
    } else {
        strlcpy(out->ssid, s_sta_ssid, sizeof(out->ssid));
        esp_netif_get_ip_info(s_netif_sta, &ip);

        wifi_ap_record_t ap = {0};
        if (s_connected && esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
            out->rssi = ap.rssi;
            out->channel = ap.primary;
        }
    }
    snprintf(out->ip, sizeof(out->ip), IPSTR, IP2STR(&ip.ip));
}

const char *wifi_manager_ap_ssid(void)
{
    return s_ap_ssid;
}
