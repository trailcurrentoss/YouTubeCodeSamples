/*
 * wifi_port_esp - the device's radio.
 *
 * The ESP32-P4 has no radio of its own. Wi-Fi comes from the ESP32-C6
 * co-processor over SDIO, reached through esp_wifi_remote, which re-exports
 * the ordinary esp_wifi_* API and forwards each call to the slave.
 *
 * THREADING. Three threads touch this file and the split is deliberate:
 *
 *   LVGL task   calls the public API only. Holds the display lock while it
 *               does, so nothing it calls may block or issue a radio RPC.
 *               Every entry point here just posts to a queue or copies state
 *               out under a mutex.
 *   worker      owned by this file, PINNED TO CORE 0. The only thread that
 *               calls esp_wifi_* or writes NVS. Core 0 because the LVGL port
 *               task is pinned to core 1 (see main.c) to keep the panel
 *               refresh cadence away from the network stack -- an unpinned
 *               worker could be scheduled onto core 1 and reintroduce exactly
 *               the jitter that pinning was meant to remove.
 *   sys_evt     ESP-IDF's event task. Kept deliberately thin: it updates a
 *               little state under the mutex and hands real work to the
 *               worker. It must never do anything large, because its stack is
 *               CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE (2304 bytes by
 *               default) and it is shared with the rest of the system.
 *
 * That last point is not theoretical. harvest_scan() used to declare
 * wifi_ap_record_t recs[WIFI_MAX_APS] as a local and run in sys_evt. At
 * 8 entries that is 8 x 92 = 736 bytes and fits; raising the list to 20 for a
 * scrollable UI made it 1840 bytes and the task overflowed its stack the
 * instant a scan completed:
 *
 *     Guru Meditation Error: Core 0 panic'ed (Stack protection fault).
 *     Detected in task "sys_evt"
 *
 * The scan buffer is now static and owned by the worker.
 *
 * Two more things that cost time and are not visible in the API:
 *
 *  1. esp_wifi_remote's Kconfig defaults the slave chip to ESP32. On a C6
 *     board that must be overridden (sdkconfig.defaults carries
 *     CONFIG_SLAVE_IDF_TARGET_ESP32C6=y) or the SDIO link comes up, the host
 *     reads the slave chip id, and the handshake then fails with
 *     "chip mismatch: expect=0x00 got=0x0d". The co-processor also reports
 *     version 0.0.0 in that state, which reads like stale slave firmware and
 *     is not.
 *
 *  2. esp_wifi_connect() issued before WIFI_EVENT_STA_START is silently
 *     dropped -- no error, no event, nothing associates. And the radio will
 *     not scan while a connect is in flight; it returns ESP_ERR_WIFI_STATE.
 *     The worker serialises both, so neither can happen by accident.
 *
 * This file lives in main/ rather than main/UI/ on purpose: the EEZ Studio
 * simulator copies only the export folder into its container and compiles
 * every .c it finds there, and this one would not build under Emscripten.
 */

#ifndef EEZ_LVGL_SIMULATOR

#include "wifi_port.h"

#include <stdio.h>
#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "wifi_port";

/* strncpy(dst, src, sizeof(dst) - 1) trips -Wstringop-truncation whenever the
 * source can be as long as the destination, which is exactly the case for
 * every SSID and passphrase here. This says the same thing without the
 * ambiguity, and always NUL-terminates. */
static void copy_str(char *dst, size_t dstlen, const char *src)
{
    if (!dst || dstlen == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    size_t n = strlen(src);
    if (n >= dstlen) n = dstlen - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
}

#define NVS_NAMESPACE "wifi"
#define NVS_KEY_SSID  "ssid"
#define NVS_KEY_PASS  "pass"

/* Core 0: the LVGL port task owns core 1 (main.c). */
#define WIFI_WORKER_CORE       0
#define WIFI_WORKER_STACK      4096
#define WIFI_WORKER_PRIO       (tskIDLE_PRIORITY + 3)
#define WIFI_CONNECT_TIMEOUT_MS 20000u

typedef enum { CMD_SCAN, CMD_CONNECT, CMD_FORGET, CMD_HARVEST } cmd_kind_t;

typedef struct {
    cmd_kind_t  kind;
    char        ssid[WIFI_SSID_MAX];
    char        pass[WIFI_PASS_MAX];
    wifi_auth_t auth;
} cmd_t;

/* ---- shared state: every field guarded by s_mtx -------------------------- */
static SemaphoreHandle_t s_mtx;
static struct {
    wifi_state_t state;
    char         status[24];
    wifi_ap_t    aps[WIFI_MAX_APS];
    int          n_aps;
} s;

/* ---- worker-private ------------------------------------------------------ */
static QueueHandle_t s_q;
static bool          s_started;        /* esp_wifi_start() succeeded  */
static bool          s_sta_started;    /* WIFI_EVENT_STA_START seen   */
static bool          s_connecting;
static TickType_t    s_connect_deadline;
static int           s_retries;
static char          s_want_ssid[WIFI_SSID_MAX];

static void set_state(wifi_state_t st, const char *text)
{
    if (!s_mtx) return;
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    s.state = st;
    copy_str(s.status, sizeof(s.status), text);
    xSemaphoreGive(s_mtx);
}

static wifi_auth_t auth_from_idf(wifi_auth_mode_t m)
{
    switch (m) {
    case WIFI_AUTH_OPEN:    return TCWIFI_AUTH_OPEN;
    case WIFI_AUTH_WEP:     return TCWIFI_AUTH_WEP;
    case WIFI_AUTH_WPA_PSK: return TCWIFI_AUTH_WPA;
    default:                return TCWIFI_AUTH_WPA2;
    }
}

/* ---- credential storage (worker thread only) ----------------------------- */

static void creds_save(const char *ssid, const char *pass)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK) return;
    nvs_set_str(h, NVS_KEY_SSID, ssid ? ssid : "");
    nvs_set_str(h, NVS_KEY_PASS, pass ? pass : "");
    nvs_commit(h);
    nvs_close(h);
}

static bool creds_load(char *ssid, size_t ssid_len, char *pass, size_t pass_len)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK) return false;
    bool ok = (nvs_get_str(h, NVS_KEY_SSID, ssid, &ssid_len) == ESP_OK) &&
              (nvs_get_str(h, NVS_KEY_PASS, pass, &pass_len) == ESP_OK) &&
              ssid[0] != '\0';
    nvs_close(h);
    return ok;
}

/* ---- scan harvest (worker thread only) ----------------------------------- */

/* Static, not a local. See the stack note at the top of this file: this array
 * is 20 x 92 bytes and there is no task stack in this system that wants it. */
static wifi_ap_record_t s_recs[WIFI_MAX_APS];

static void harvest_scan(void)
{
    uint16_t found = 0;
    esp_wifi_scan_get_ap_num(&found);

    uint16_t want = WIFI_MAX_APS;
    memset(s_recs, 0, sizeof(s_recs));
    if (esp_wifi_scan_get_ap_records(&want, s_recs) != ESP_OK) {
        want = 0;
    }

    xSemaphoreTake(s_mtx, portMAX_DELAY);
    s.n_aps = 0;
    for (uint16_t i = 0; i < want && s.n_aps < WIFI_MAX_APS; i++) {
        if (s_recs[i].ssid[0] == '\0') continue;   /* hidden: nothing to tap */
        wifi_ap_t *ap = &s.aps[s.n_aps++];
        copy_str(ap->ssid, sizeof(ap->ssid), (const char *)s_recs[i].ssid);
        ap->ssid[sizeof(ap->ssid) - 1] = '\0';
        ap->rssi = s_recs[i].rssi;
        ap->auth = auth_from_idf(s_recs[i].authmode);
    }
    const int shown = s.n_aps;
    xSemaphoreGive(s_mtx);

    ESP_LOGI(TAG, "scan complete: %u seen, %u returned, %d shown",
             (unsigned)found, (unsigned)want, shown);
    set_state(WIFI_STATE_IDLE, shown ? "SCAN DONE" : "NO NETWORKS");
}

/* ---- events (sys_evt task -- keep everything here small) ----------------- */

static void on_wifi(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg; (void)base;

    switch (id) {
    case WIFI_EVENT_STA_START:
        s_sta_started = true;
        ESP_LOGI(TAG, "station started");
        break;

    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "associated");
        break;

    case WIFI_EVENT_SCAN_DONE: {
        /* Hand the work to the worker rather than reading results here: this
         * task's stack cannot afford the record array. */
        const cmd_t c = { .kind = CMD_HARVEST };
        xQueueSend(s_q, &c, 0);
        break;
    }

    case WIFI_EVENT_STA_DISCONNECTED: {
        const wifi_event_sta_disconnected_t *d =
            (const wifi_event_sta_disconnected_t *)data;
        const int reason = d ? d->reason : -1;
        ESP_LOGW(TAG, "disconnected: reason=%d connecting=%d retries=%d",
                 reason, (int)s_connecting, s_retries);
        if (s_connecting && s_retries < 2) {
            s_retries++;
            esp_wifi_connect();
        } else if (s_connecting) {
            s_connecting = false;
            /* Reason 15 is a 4-way handshake timeout, which in practice means
             * the passphrase is wrong; say so rather than "failed". */
            set_state(WIFI_STATE_FAILED,
                      reason == WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT
                          ? "BAD PASSWORD" : "CONNECT FAILED");
        } else {
            set_state(WIFI_STATE_IDLE, "DISCONNECTED");
        }
        break;
    }

    default:
        ESP_LOGD(TAG, "wifi event id=%d", (int)id);
        break;
    }
}

static void on_ip(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg; (void)base;
    if (id == IP_EVENT_STA_GOT_IP) {
        const ip_event_got_ip_t *e = (const ip_event_got_ip_t *)data;
        if (e) ESP_LOGI(TAG, "got ip " IPSTR, IP2STR(&e->ip_info.ip));
        s_connecting = false;
        s_retries = 0;
        set_state(WIFI_STATE_CONNECTED, "CONNECTED");
    }
}

/* ---- worker ------------------------------------------------------------- */

static void do_scan(void)
{
    /* The radio refuses to scan while a connect is in flight. Anyone asking
     * for a scan is choosing a network, so the in-flight attempt to rejoin
     * the old one is exactly what they are replacing. */
    if (s_connecting) {
        ESP_LOGI(TAG, "cancelling in-flight connect to scan");
        s_connecting = false;
        esp_wifi_disconnect();
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    set_state(WIFI_STATE_SCANNING, "SCANNING");
    const wifi_scan_config_t cfg = { 0 };

    /* esp_wifi_disconnect() is asynchronous, so the first attempt can still
     * be refused with ESP_ERR_WIFI_STATE. Retry briefly here, on the worker,
     * where sleeping is free. */
    for (int attempt = 0; attempt < 20; attempt++) {
        const esp_err_t err = esp_wifi_scan_start(&cfg, false);
        if (err == ESP_OK) return;
        if (err != ESP_ERR_WIFI_STATE) {
            ESP_LOGE(TAG, "esp_wifi_scan_start: %s", esp_err_to_name(err));
            set_state(WIFI_STATE_FAILED, "SCAN FAILED");
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    ESP_LOGE(TAG, "scan refused: radio stayed busy");
    set_state(WIFI_STATE_FAILED, "SCAN FAILED");
}

static void do_connect(const cmd_t *c)
{
    if (!s_started) return;

    ESP_LOGI(TAG, "connect: ssid='%s' auth=%s pass_len=%u",
             c->ssid, wifi_auth_name(c->auth), (unsigned)strlen(c->pass));

    /* Already on it? esp_wifi_connect() would be a no-op AND fire no event,
     * leaving anything waiting on GOT_IP waiting forever. */
    wifi_ap_record_t cur;
    if (esp_wifi_sta_get_ap_info(&cur) == ESP_OK) {
        if (strncmp((const char *)cur.ssid, c->ssid, sizeof(cur.ssid)) == 0) {
            ESP_LOGI(TAG, "already associated with '%s'", c->ssid);
            creds_save(c->ssid, c->pass);
            set_state(WIFI_STATE_CONNECTED, "CONNECTED");
            return;
        }
        esp_wifi_disconnect();
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    wifi_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    copy_str((char *)cfg.sta.ssid, sizeof(cfg.sta.ssid), c->ssid);
    copy_str((char *)cfg.sta.password, sizeof(cfg.sta.password), c->pass);

    esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_config: %s", esp_err_to_name(err));
        set_state(WIFI_STATE_FAILED, "CONNECT FAILED");
        return;
    }

    /* esp_wifi_connect() before STA_START is silently discarded. Wait for it
     * rather than firing and hoping. */
    for (int i = 0; i < 50 && !s_sta_started; i++) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (!s_sta_started) {
        ESP_LOGE(TAG, "station never started");
        set_state(WIFI_STATE_FAILED, "CONNECT FAILED");
        return;
    }

    copy_str(s_want_ssid, sizeof(s_want_ssid), c->ssid);
    s_retries = 0;
    s_connecting = true;
    s_connect_deadline = xTaskGetTickCount() +
                         pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS);
    set_state(WIFI_STATE_CONNECTING, "CONNECTING");

    err = esp_wifi_connect();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "connect: %s", esp_err_to_name(err));
        s_connecting = false;
        set_state(WIFI_STATE_FAILED, "CONNECT FAILED");
        return;
    }
    creds_save(c->ssid, c->pass);
}

static void worker(void *arg)
{
    (void)arg;
    for (;;) {
        cmd_t c;
        /* Short block so the connect deadline is checked regularly. */
        if (xQueueReceive(s_q, &c, pdMS_TO_TICKS(250)) == pdTRUE) {
            switch (c.kind) {
            case CMD_SCAN:    do_scan();        break;
            case CMD_HARVEST: harvest_scan();   break;
            case CMD_CONNECT: do_connect(&c);   break;
            case CMD_FORGET:
                creds_save("", "");
                s_connecting = false;
                esp_wifi_disconnect();
                set_state(WIFI_STATE_IDLE, "IDLE");
                break;
            }
        }

        /* "No event ever arrives" is a real outcome -- an AP that ignores the
         * association, a DHCP server that never answers. Without a bound the
         * UI sits on CONNECTING forever, which is indistinguishable from a
         * hung device. */
        if (s_connecting &&
            (int32_t)(xTaskGetTickCount() - s_connect_deadline) >= 0) {
            ESP_LOGW(TAG, "connect timed out after %u ms",
                     (unsigned)WIFI_CONNECT_TIMEOUT_MS);
            s_connecting = false;
            esp_wifi_disconnect();
            set_state(WIFI_STATE_FAILED, "TIMED OUT");
        }
    }
}

/* ---- public API (LVGL task) ---------------------------------------------- */
/* Everything below only queues work or copies state out. Nothing here blocks,
 * touches esp_wifi_*, or writes flash -- see the threading note at the top. */

static bool bring_up_radio(void)
{
    esp_err_t err = esp_netif_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_netif_init: %s", esp_err_to_name(err));
        return false;
    }
    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "event loop: %s", esp_err_to_name(err));
        return false;
    }
    if (esp_netif_create_default_wifi_sta() == NULL) {
        ESP_LOGE(TAG, "esp_netif_create_default_wifi_sta returned NULL");
        return false;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        /* Check CONFIG_SLAVE_IDF_TARGET_* first; it defaults to ESP32 and
         * this board carries a C6. See the note at the top of the file. */
        ESP_LOGE(TAG, "esp_wifi_init: %s", esp_err_to_name(err));
        ESP_LOGE(TAG, "check CONFIG_SLAVE_IDF_TARGET_* matches the fitted "
                      "co-processor (this board is esp32c6)");
        return false;
    }

    if (esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                            &on_wifi, NULL, NULL) != ESP_OK ||
        esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                            &on_ip, NULL, NULL) != ESP_OK) {
        ESP_LOGE(TAG, "event handler registration failed");
        return false;
    }

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    /* Keep station config in RAM: this project owns credential storage and
     * two sources of truth for "which network" is one too many. */
    if (err == ESP_OK) err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (err == ESP_OK) err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wifi start: %s", esp_err_to_name(err));
        return false;
    }

    /* Power save off: over SDIO to the C6 the modem-sleep default makes
     * association slow and intermittent. */
    esp_wifi_set_ps(WIFI_PS_NONE);

    /* Let the C6 settle before anything is asked of it. Without this a
     * request issued immediately after esp_wifi_start() is accepted and then
     * simply never acted on. Carried over from the Fireside firmware on the
     * same panel, where it is commented "let C6 slave settle before
     * scan/connect" -- a property of this host/co-processor pairing, not of
     * any one project. */
    vTaskDelay(pdMS_TO_TICKS(2000));
    ESP_LOGI(TAG, "co-processor settled");
    return true;
}

/* Runs once on the worker so the 2 s settle does not stall app_main. */
static void startup(void)
{
    if (!bring_up_radio()) {
        s_started = false;
        set_state(WIFI_STATE_FAILED, "NO RADIO");
        ESP_LOGE(TAG, "Wi-Fi unavailable; dashboard continues without it");
        return;
    }
    s_started = true;
    set_state(WIFI_STATE_IDLE, "IDLE");

    char ssid[WIFI_SSID_MAX] = {0}, pass[WIFI_PASS_MAX] = {0};
    if (creds_load(ssid, sizeof(ssid), pass, sizeof(pass))) {
        ESP_LOGI(TAG, "reconnecting to saved network");
        cmd_t c = { .kind = CMD_CONNECT, .auth = TCWIFI_AUTH_WPA2 };
        copy_str(c.ssid, sizeof(c.ssid), ssid);
        copy_str(c.pass, sizeof(c.pass), pass);
        do_connect(&c);
    }
    /* No scan here: scanning is driven by the setup screen, so a panel with
     * saved credentials boots straight to the dashboard. */
}

static void worker_entry(void *arg)
{
    startup();
    worker(arg);
}

void wifi_port_init(void)
{
    memset(&s, 0, sizeof(s));
    s.state = WIFI_STATE_IDLE;
    copy_str(s.status, sizeof(s.status), "STARTING");

    s_mtx = xSemaphoreCreateMutex();
    s_q   = xQueueCreate(4, sizeof(cmd_t));
    if (!s_mtx || !s_q) {
        ESP_LOGE(TAG, "out of memory starting wifi port");
        set_state(WIFI_STATE_FAILED, "NO RADIO");
        return;
    }

    if (xTaskCreatePinnedToCore(worker_entry, "wifi_port", WIFI_WORKER_STACK,
                                NULL, WIFI_WORKER_PRIO, NULL,
                                WIFI_WORKER_CORE) != pdPASS) {
        ESP_LOGE(TAG, "could not start wifi worker");
        set_state(WIFI_STATE_FAILED, "NO RADIO");
    }
}

void wifi_port_scan(void)
{
    if (!s_q) return;
    const cmd_t c = { .kind = CMD_SCAN };
    xQueueSend(s_q, &c, 0);
}

void wifi_port_connect(const char *ssid, wifi_auth_t auth, const char *pass)
{
    if (!s_q || !ssid) return;
    cmd_t c = { .kind = CMD_CONNECT, .auth = auth };
    copy_str(c.ssid, sizeof(c.ssid), ssid);
    if (pass) copy_str(c.pass, sizeof(c.pass), pass);
    xQueueSend(s_q, &c, 0);
}

void wifi_port_forget(void)
{
    if (!s_q) return;
    const cmd_t c = { .kind = CMD_FORGET };
    xQueueSend(s_q, &c, 0);
}

/* The worker owns its own timing, so there is nothing to do here. Kept so the
 * two backends present the same shape to the UI. */
void wifi_port_tick(uint32_t now_ms) { (void)now_ms; }

int wifi_port_ap_count(void)
{
    if (!s_mtx) return 0;
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    const int n = s.n_aps;
    xSemaphoreGive(s_mtx);
    return n;
}

bool wifi_port_ap(int index, wifi_ap_t *out)
{
    if (!s_mtx || !out) return false;
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    const bool ok = (index >= 0 && index < s.n_aps);
    if (ok) *out = s.aps[index];
    xSemaphoreGive(s_mtx);
    return ok;
}

wifi_state_t wifi_port_state(void)
{
    if (!s_mtx) return WIFI_STATE_IDLE;
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    const wifi_state_t st = s.state;
    xSemaphoreGive(s_mtx);
    return st;
}

void wifi_port_status_text(char *out, size_t len)
{
    if (!out || len == 0) return;
    if (!s_mtx) { out[0] = '\0'; return; }
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    copy_str(out, len, s.status);
    xSemaphoreGive(s_mtx);
}

#else  /* simulator build - wifi_port_sim.c provides the port */

typedef int wifi_port_esp_placeholder;

#endif /* EEZ_LVGL_SIMULATOR */
