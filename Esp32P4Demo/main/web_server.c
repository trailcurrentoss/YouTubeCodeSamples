/*
 * HTTP server serving the TrailCurrent-branded web UI and its REST API.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cJSON.h"
#include "esp_app_desc.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "ai_camera.h"
#include "audio_recorder.h"
#include "board_esp32p4_wifi6.h"
#include "sd_card.h"
#include "web_server.h"
#include "webcam.h"
#include "wifi_manager.h"

static const char *TAG = "web";

/* Web assets, embedded into the binary by EMBED_FILES in main/CMakeLists.txt. */
extern const uint8_t setup_html_start[]     asm("_binary_setup_html_start");
extern const uint8_t setup_html_end[]       asm("_binary_setup_html_end");
extern const uint8_t dashboard_html_start[] asm("_binary_dashboard_html_start");
extern const uint8_t dashboard_html_end[]   asm("_binary_dashboard_html_end");
extern const uint8_t style_css_start[]      asm("_binary_style_css_start");
extern const uint8_t style_css_end[]        asm("_binary_style_css_end");
extern const uint8_t setup_js_start[]       asm("_binary_setup_js_start");
extern const uint8_t setup_js_end[]         asm("_binary_setup_js_end");
extern const uint8_t dashboard_js_start[]   asm("_binary_dashboard_js_start");
extern const uint8_t dashboard_js_end[]     asm("_binary_dashboard_js_end");

#define MAX_BODY_LEN   512
#define MAX_PATH_LEN   256

static httpd_handle_t s_server;
static httpd_handle_t s_stream_server;   /* port 81, MJPEG only */
static web_ui_mode_t  s_mode;

/* -------------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------------- */
static esp_err_t send_asset(httpd_req_t *req, const uint8_t *start,
                            const uint8_t *end, const char *type)
{
    httpd_resp_set_type(req, type);
    /* Assets are rebuilt with the firmware, so caching them across a reflash
     * would serve stale UI against new API responses. */
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, (const char *)start, end - start);
}

static esp_err_t send_json(httpd_req_t *req, cJSON *root)
{
    char *text = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (text == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "out of memory");
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    const esp_err_t ret = httpd_resp_send(req, text, strlen(text));
    free(text);
    return ret;
}

static esp_err_t send_error(httpd_req_t *req, const char *status, const char *message)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "ok", false);
    cJSON_AddStringToObject(root, "error", message);
    httpd_resp_set_status(req, status);
    return send_json(req, root);
}

static esp_err_t read_body(httpd_req_t *req, char *buf, size_t buf_len)
{
    if (req->content_len >= buf_len) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t got = 0;
    while (got < req->content_len) {
        const int r = httpd_req_recv(req, buf + got, req->content_len - got);
        if (r <= 0) {
            return ESP_FAIL;
        }
        got += r;
    }
    buf[got] = '\0';
    return ESP_OK;
}

/*
 * Resolve a client-supplied path to somewhere under the mount point.
 *
 * Rejects any path containing "..", which is the whole point: without it a
 * request for /api/sd/delete?path=../../nvs would let a browser reach outside
 * the card. Also rejects backslashes, which FATFS would accept as separators.
 */
static bool safe_sd_path(const char *rel, char *out, size_t out_len)
{
    if (rel == NULL || rel[0] == '\0') {
        rel = "/";
    }
    if (strstr(rel, "..") != NULL || strchr(rel, '\\') != NULL) {
        return false;
    }
    if (rel[0] != '/') {
        return false;
    }
    const int n = snprintf(out, out_len, "%s%s", BSP_SD_MOUNT_POINT, rel);
    if (n < 0 || (size_t)n >= out_len) {
        return false;
    }
    /* Strip a trailing slash so stat() and opendir() behave consistently. */
    const size_t len = strlen(out);
    if (len > strlen(BSP_SD_MOUNT_POINT) && out[len - 1] == '/') {
        out[len - 1] = '\0';
    }
    return true;
}

/*
 * Percent-decode in place.
 *
 * httpd_query_key_value() hands back the raw query value with no decoding, so
 * a path of "/" arrives as the literal three characters "%2F" and every path
 * check downstream rejects it.
 *
 * '+' is deliberately NOT treated as a space. That convention belongs to HTML
 * form bodies; encodeURIComponent() emits %2B for a literal plus, so a bare
 * '+' here is part of a filename and turning it into a space would break the
 * very files it is meant to address.
 */
static int hex_val(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static void url_decode(char *s)
{
    char *out = s;
    for (const char *in = s; *in != '\0'; in++) {
        if (*in == '%') {
            const int hi = hex_val(in[1]);
            const int lo = (hi >= 0) ? hex_val(in[2]) : -1;
            if (lo >= 0) {
                *out++ = (char)((hi << 4) | lo);
                in += 2;
                continue;
            }
        }
        *out++ = *in;
    }
    *out = '\0';
}

static esp_err_t query_param(httpd_req_t *req, const char *key,
                             char *out, size_t out_len)
{
    const size_t qlen = httpd_req_get_url_query_len(req) + 1;
    if (qlen <= 1) {
        return ESP_ERR_NOT_FOUND;
    }
    char *query = malloc(qlen);
    if (query == NULL) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t ret = httpd_req_get_url_query_str(req, query, qlen);
    if (ret == ESP_OK) {
        ret = httpd_query_key_value(query, key, out, out_len);
    }
    free(query);

    if (ret == ESP_OK) {
        url_decode(out);
    }
    return ret;
}

/* -------------------------------------------------------------------------
 * Static asset handlers
 * ------------------------------------------------------------------------- */
static esp_err_t handle_root(httpd_req_t *req)
{
    if (s_mode == WEB_UI_SETUP) {
        return send_asset(req, setup_html_start, setup_html_end, "text/html");
    }
    return send_asset(req, dashboard_html_start, dashboard_html_end, "text/html");
}

static esp_err_t handle_style(httpd_req_t *req)
{
    return send_asset(req, style_css_start, style_css_end, "text/css");
}

static esp_err_t handle_app_js(httpd_req_t *req)
{
    if (s_mode == WEB_UI_SETUP) {
        return send_asset(req, setup_js_start, setup_js_end, "application/javascript");
    }
    return send_asset(req, dashboard_js_start, dashboard_js_end, "application/javascript");
}

/*
 * Captive-portal probes. Phones and laptops fetch a known URL and decide they
 * are behind a portal if the answer is not what they expect. Redirecting to the
 * setup page is what makes the UI appear on its own.
 */
static esp_err_t handle_portal_probe(httpd_req_t *req)
{
    if (s_mode != WEB_UI_SETUP) {
        httpd_resp_set_status(req, "404 Not Found");
        return httpd_resp_send(req, NULL, 0);
    }
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    return httpd_resp_send(req, NULL, 0);
}

/* -------------------------------------------------------------------------
 * API: status
 * ------------------------------------------------------------------------- */
static esp_err_t api_status(httpd_req_t *req)
{
    wifi_mgr_status_t st;
    wifi_manager_get_status(&st);

    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "ok", true);
    cJSON_AddStringToObject(root, "mode",
                            s_mode == WEB_UI_SETUP ? "setup" : "dashboard");
    cJSON_AddBoolToObject(root, "connected", st.connected);
    cJSON_AddStringToObject(root, "ssid", st.ssid);
    cJSON_AddStringToObject(root, "ip", st.ip);
    cJSON_AddStringToObject(root, "mac", st.mac);
    cJSON_AddNumberToObject(root, "rssi", st.rssi);
    cJSON_AddNumberToObject(root, "channel", st.channel);
    return send_json(req, root);
}

/* -------------------------------------------------------------------------
 * API: scan
 * ------------------------------------------------------------------------- */
static const char *auth_name(wifi_auth_mode_t mode)
{
    switch (mode) {
    case WIFI_AUTH_OPEN:            return "open";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/WPA3";
    case WIFI_AUTH_ENTERPRISE:      return "Enterprise";
    default:                        return "secured";
    }
}

static esp_err_t api_scan(httpd_req_t *req)
{
    wifi_ap_record_t *records = NULL;
    uint16_t count = 0;

    const esp_err_t ret = wifi_manager_scan(&records, &count);
    if (ret != ESP_OK) {
        return send_error(req, HTTPD_500, esp_err_to_name(ret));
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "ok", true);
    cJSON *list = cJSON_AddArrayToObject(root, "networks");

    for (uint16_t i = 0; i < count; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "ssid", (const char *)records[i].ssid);
        cJSON_AddNumberToObject(item, "rssi", records[i].rssi);
        cJSON_AddNumberToObject(item, "channel", records[i].primary);
        cJSON_AddStringToObject(item, "auth", auth_name(records[i].authmode));
        cJSON_AddBoolToObject(item, "open", records[i].authmode == WIFI_AUTH_OPEN);
        cJSON_AddItemToArray(list, item);
    }
    free(records);
    return send_json(req, root);
}

/* -------------------------------------------------------------------------
 * API: connect
 * ------------------------------------------------------------------------- */
static void reboot_task(void *arg)
{
    /* Give the HTTP response time to reach the browser before the radio and
     * the whole system go away. */
    vTaskDelay(pdMS_TO_TICKS(1500));
    ESP_LOGI(TAG, "Restarting into station mode");
    esp_restart();
}

static esp_err_t api_connect(httpd_req_t *req)
{
    char body[MAX_BODY_LEN];
    if (read_body(req, body, sizeof(body)) != ESP_OK) {
        return send_error(req, HTTPD_400, "could not read the request body");
    }

    cJSON *root = cJSON_Parse(body);
    if (root == NULL) {
        return send_error(req, HTTPD_400, "malformed JSON");
    }

    const cJSON *j_ssid = cJSON_GetObjectItem(root, "ssid");
    const cJSON *j_pass = cJSON_GetObjectItem(root, "password");

    if (!cJSON_IsString(j_ssid) || j_ssid->valuestring[0] == '\0') {
        cJSON_Delete(root);
        return send_error(req, HTTPD_400, "a network name is required");
    }

    char ssid[WIFI_MGR_SSID_MAX + 1] = {0};
    char pass[WIFI_MGR_PASS_MAX + 1] = {0};
    strlcpy(ssid, j_ssid->valuestring, sizeof(ssid));
    if (cJSON_IsString(j_pass)) {
        strlcpy(pass, j_pass->valuestring, sizeof(pass));
    }
    cJSON_Delete(root);

    const esp_err_t ret = wifi_manager_try_connect(ssid, pass,
                                                   CONFIG_DEMO_WIFI_CONNECT_TIMEOUT_MS);

    cJSON *resp = cJSON_CreateObject();
    if (ret == ESP_OK) {
        /* Only persist once the network has actually been proven to work —
         * saving first would strand the board on a bad password after reboot. */
        const esp_err_t save = wifi_manager_save_credentials(ssid, pass);
        if (save != ESP_OK) {
            cJSON_AddBoolToObject(resp, "ok", false);
            cJSON_AddStringToObject(resp, "error", "connected, but saving failed");
            return send_json(req, resp);
        }

        wifi_mgr_status_t st;
        wifi_manager_get_status(&st);

        cJSON_AddBoolToObject(resp, "ok", true);
        cJSON_AddStringToObject(resp, "ssid", ssid);
        cJSON_AddStringToObject(resp, "ip", st.ip);
        cJSON_AddNumberToObject(resp, "rssi", st.rssi);
        cJSON_AddBoolToObject(resp, "rebooting", true);

        const esp_err_t sent = send_json(req, resp);
        xTaskCreate(reboot_task, "reboot", 2048, NULL, 5, NULL);
        return sent;
    }

    /* Report what actually happened. Blaming the password for every failure
     * sends people off checking a password that was right all along. */
    const char *message;
    switch (ret) {
    case ESP_ERR_WIFI_PASSWORD:
        message = "That password was rejected by the network.";
        break;
    case ESP_ERR_WIFI_SSID:
        message = "That network stopped responding. It may be out of range — "
                  "rescan and try again.";
        break;
    default:
        message = "Could not join that network after several attempts. "
                  "Check it is a 2.4 GHz network and in range.";
        break;
    }

    cJSON_AddBoolToObject(resp, "ok", false);
    cJSON_AddStringToObject(resp, "error", message);
    cJSON_AddNumberToObject(resp, "reason", wifi_manager_last_disconnect_reason());
    httpd_resp_set_status(req, HTTPD_400);
    return send_json(req, resp);
}

/* -------------------------------------------------------------------------
 * API: board details
 * ------------------------------------------------------------------------- */
static esp_err_t api_board(httpd_req_t *req)
{
    esp_chip_info_t chip = {0};
    esp_chip_info(&chip);

    uint32_t flash_bytes = 0;
    esp_flash_get_physical_size(NULL, &flash_bytes);

    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "ok", true);

    cJSON_AddStringToObject(root, "board", "Waveshare ESP32-P4-WIFI6");
    cJSON_AddStringToObject(root, "chip", "ESP32-P4");
    cJSON_AddNumberToObject(root, "cores", chip.cores);
    char rev[16];
    snprintf(rev, sizeof(rev), "v%d.%d", chip.revision / 100, chip.revision % 100);
    cJSON_AddStringToObject(root, "revision", rev);
    cJSON_AddStringToObject(root, "idf", esp_get_idf_version());

    const esp_app_desc_t *app = esp_app_get_description();
    cJSON_AddStringToObject(root, "app", app->project_name);
    cJSON_AddStringToObject(root, "built", app->date);

    cJSON_AddNumberToObject(root, "flashBytes", flash_bytes);
#if CONFIG_SPIRAM
    cJSON_AddNumberToObject(root, "psramBytes", esp_psram_get_size());
#else
    cJSON_AddNumberToObject(root, "psramBytes", 0);
#endif
    cJSON_AddNumberToObject(root, "psramFree", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    cJSON_AddNumberToObject(root, "internalFree", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    cJSON_AddNumberToObject(root, "uptimeSeconds", esp_timer_get_time() / 1000000);

    /* microSD */
    cJSON *sd = cJSON_AddObjectToObject(root, "sd");
    const sdmmc_card_t *card = sd_card_get();
    if (card == NULL) {
        cJSON_AddBoolToObject(sd, "present", false);
    } else {
        uint64_t total = 0, freeb = 0;
        sd_card_get_fs_usage(&total, &freeb);

        cJSON_AddBoolToObject(sd, "present", true);
        cJSON_AddStringToObject(sd, "name", card->cid.name);
        cJSON_AddStringToObject(sd, "type", (card->ocr & (1 << 30)) ? "SDHC/SDXC" : "SDSC");
        cJSON_AddNumberToObject(sd, "capacityBytes",
                                (double)card->csd.capacity * card->csd.sector_size);
        cJSON_AddNumberToObject(sd, "speedKhz", card->max_freq_khz);
        cJSON_AddNumberToObject(sd, "busWidth",
                                card->log_bus_width ? (1 << card->log_bus_width) : 1);
        cJSON_AddNumberToObject(sd, "fsTotalBytes", (double)total);
        cJSON_AddNumberToObject(sd, "fsFreeBytes", (double)freeb);
        cJSON_AddNumberToObject(sd, "fsUsedBytes", (double)(total - freeb));
    }

    cJSON *mic = cJSON_AddObjectToObject(root, "mic");
    cJSON_AddBoolToObject(mic, "ready", audio_recorder_ready());
    cJSON_AddNumberToObject(mic, "sampleRate", CONFIG_DEMO_AUDIO_SAMPLE_RATE);
    cJSON_AddNumberToObject(mic, "maxSeconds", CONFIG_DEMO_AUDIO_MAX_SECONDS);

    wifi_mgr_status_t st;
    wifi_manager_get_status(&st);
    cJSON *net = cJSON_AddObjectToObject(root, "network");
    cJSON_AddStringToObject(net, "ssid", st.ssid);
    cJSON_AddStringToObject(net, "ip", st.ip);
    cJSON_AddStringToObject(net, "mac", st.mac);
    cJSON_AddNumberToObject(net, "rssi", st.rssi);
    cJSON_AddNumberToObject(net, "channel", st.channel);
    cJSON_AddBoolToObject(net, "connected", st.connected);

    return send_json(req, root);
}

/* -------------------------------------------------------------------------
 * API: SD file listing
 * ------------------------------------------------------------------------- */
static esp_err_t api_sd_list(httpd_req_t *req)
{
    if (sd_card_get() == NULL) {
        return send_error(req, HTTPD_500, "no microSD card is mounted");
    }

    /* Fall back to the root on anything other than a clean hit — a truncated
     * value would otherwise be used as a half-formed path. */
    char rel[MAX_PATH_LEN];
    if (query_param(req, "path", rel, sizeof(rel)) != ESP_OK) {
        strlcpy(rel, "/", sizeof(rel));
    }

    char full[MAX_PATH_LEN];
    if (!safe_sd_path(rel, full, sizeof(full))) {
        return send_error(req, HTTPD_400, "invalid path");
    }

    DIR *dir = opendir(full);
    if (dir == NULL) {
        return send_error(req, HTTPD_404, "no such folder");
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "ok", true);
    cJSON_AddStringToObject(root, "path", rel);
    cJSON *list = cJSON_AddArrayToObject(root, "entries");

    const struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char child[MAX_PATH_LEN];
        if (snprintf(child, sizeof(child), "%s/%s", full, ent->d_name) >= (int)sizeof(child)) {
            continue;   /* name too long to address; skip rather than truncate */
        }

        struct stat st = {0};
        const bool have_stat = (stat(child, &st) == 0);

        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "name", ent->d_name);
        cJSON_AddBoolToObject(item, "dir", ent->d_type == DT_DIR);
        cJSON_AddNumberToObject(item, "size",
                                have_stat && ent->d_type != DT_DIR ? (double)st.st_size : 0);
        cJSON_AddItemToArray(list, item);
    }
    closedir(dir);

    return send_json(req, root);
}

/* -------------------------------------------------------------------------
 * API: SD delete
 * ------------------------------------------------------------------------- */
static esp_err_t api_sd_delete(httpd_req_t *req)
{
    if (sd_card_get() == NULL) {
        return send_error(req, HTTPD_500, "no microSD card is mounted");
    }

    char body[MAX_BODY_LEN];
    if (read_body(req, body, sizeof(body)) != ESP_OK) {
        return send_error(req, HTTPD_400, "could not read the request body");
    }

    cJSON *root = cJSON_Parse(body);
    if (root == NULL) {
        return send_error(req, HTTPD_400, "malformed JSON");
    }
    const cJSON *j_path = cJSON_GetObjectItem(root, "path");
    if (!cJSON_IsString(j_path)) {
        cJSON_Delete(root);
        return send_error(req, HTTPD_400, "a path is required");
    }

    char full[MAX_PATH_LEN];
    const bool ok_path = safe_sd_path(j_path->valuestring, full, sizeof(full));
    cJSON_Delete(root);

    if (!ok_path) {
        return send_error(req, HTTPD_400, "invalid path");
    }
    /* Refuse to delete the mount point itself. */
    if (strcmp(full, BSP_SD_MOUNT_POINT) == 0) {
        return send_error(req, HTTPD_400, "cannot delete the root folder");
    }

    struct stat st = {0};
    if (stat(full, &st) != 0) {
        return send_error(req, HTTPD_404, "no such file");
    }

    int rc;
    if (S_ISDIR(st.st_mode)) {
        /* Only empty directories; recursive deletion from a web button is too
         * easy to trigger by accident. */
        rc = rmdir(full);
        if (rc != 0) {
            return send_error(req, HTTPD_400, "folder is not empty");
        }
    } else {
        rc = unlink(full);
        if (rc != 0) {
            return send_error(req, HTTPD_500, "delete failed");
        }
    }

    ESP_LOGI(TAG, "Deleted %s", full);

    uint64_t total = 0, freeb = 0;
    sd_card_get_fs_usage(&total, &freeb);

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddNumberToObject(resp, "fsFreeBytes", (double)freeb);
    cJSON_AddNumberToObject(resp, "fsUsedBytes", (double)(total - freeb));
    return send_json(req, resp);
}

/* -------------------------------------------------------------------------
 * API: microphone
 * ------------------------------------------------------------------------- */
static esp_err_t api_audio_start(httpd_req_t *req)
{
    if (!audio_recorder_ready()) {
        return send_error(req, HTTPD_500,
                          "The microphone did not initialise — check the console log");
    }
    if (sd_card_get() == NULL) {
        return send_error(req, HTTPD_500,
                          "No microSD card is mounted, so there is nowhere to save");
    }
    if (audio_recorder_state() != AUDIO_STATE_IDLE) {
        return send_error(req, HTTPD_400, "A recording is already running");
    }

    const esp_err_t ret = audio_recorder_start();
    if (ret != ESP_OK) {
        return send_error(req, HTTPD_500, "Could not start recording");
    }

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "recording", true);
    cJSON_AddNumberToObject(resp, "maxSeconds", CONFIG_DEMO_AUDIO_MAX_SECONDS);
    return send_json(req, resp);
}

static esp_err_t api_audio_state(httpd_req_t *req)
{
    float pos = 0, dur = 0;
    audio_player_progress(&pos, &dur);

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "ready", audio_recorder_ready());
    cJSON_AddBoolToObject(resp, "recording",
                          audio_recorder_state() == AUDIO_STATE_RECORDING);
    cJSON_AddNumberToObject(resp, "elapsed", audio_recorder_elapsed());
    cJSON_AddNumberToObject(resp, "maxSeconds", CONFIG_DEMO_AUDIO_MAX_SECONDS);
    cJSON_AddBoolToObject(resp, "playing", audio_player_is_playing());
    cJSON_AddStringToObject(resp, "playingPath", audio_player_path());
    cJSON_AddNumberToObject(resp, "playPosition", pos);
    cJSON_AddNumberToObject(resp, "playDuration", dur);
    return send_json(req, resp);
}

/* Play a file on the board itself, through the ES8311 DAC and the speaker. */
static esp_err_t api_audio_play(httpd_req_t *req)
{
    if (!audio_recorder_ready()) {
        return send_error(req, HTTPD_500, "The audio codec is not available");
    }
    if (sd_card_get() == NULL) {
        return send_error(req, HTTPD_500, "No microSD card is mounted");
    }

    char body[MAX_BODY_LEN];
    if (read_body(req, body, sizeof(body)) != ESP_OK) {
        return send_error(req, HTTPD_400, "could not read the request body");
    }
    cJSON *root = cJSON_Parse(body);
    if (root == NULL) {
        return send_error(req, HTTPD_400, "malformed JSON");
    }
    const cJSON *j_path = cJSON_GetObjectItem(root, "path");
    if (!cJSON_IsString(j_path)) {
        cJSON_Delete(root);
        return send_error(req, HTTPD_400, "a path is required");
    }
    char rel[MAX_PATH_LEN];
    strlcpy(rel, j_path->valuestring, sizeof(rel));
    cJSON_Delete(root);

    const esp_err_t ret = audio_player_start(rel);
    if (ret != ESP_OK) {
        const char *msg;
        switch (ret) {
        case ESP_ERR_INVALID_STATE:
            msg = "Busy — stop the recording or the current playback first. "
                  "The microphone and speaker share one codec.";
            break;
        case ESP_ERR_NOT_FOUND:
            msg = "That file could not be opened";
            break;
        case ESP_ERR_NOT_SUPPORTED:
            msg = "Only uncompressed 16-bit PCM WAV files can be played on the board";
            break;
        default:
            msg = "Playback failed — see the console log";
            break;
        }
        return send_error(req, HTTPD_400, msg);
    }

    float pos = 0, dur = 0;
    audio_player_progress(&pos, &dur);

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "playing", true);
    cJSON_AddStringToObject(resp, "path", rel);
    cJSON_AddNumberToObject(resp, "duration", dur);
    return send_json(req, resp);
}

static esp_err_t api_audio_playstop(httpd_req_t *req)
{
    audio_player_stop();
    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "playing", false);
    return send_json(req, resp);
}

static esp_err_t api_audio_stop(httpd_req_t *req)
{
    audio_recording_t rec;
    const esp_err_t ret = audio_recorder_stop(&rec);
    if (ret != ESP_OK) {
        return send_error(req, HTTPD_500,
                          ret == ESP_ERR_TIMEOUT
                              ? "The recorder did not stop cleanly"
                              : "Nothing was recorded");
    }

    uint64_t total = 0, freeb = 0;
    sd_card_get_fs_usage(&total, &freeb);

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddStringToObject(resp, "path", rec.path);
    cJSON_AddNumberToObject(resp, "bytes", rec.bytes);
    cJSON_AddNumberToObject(resp, "seconds", rec.seconds);
    cJSON_AddNumberToObject(resp, "sampleRate", rec.sample_rate);
    cJSON_AddNumberToObject(resp, "peak", rec.peak);
    /* JSON has no way to spell -Infinity, so pure silence is clamped to a
     * finite floor rather than emitting a token no parser will accept. */
    cJSON_AddNumberToObject(resp, "peakDbfs",
                            isfinite(rec.peak_dbfs) ? rec.peak_dbfs : -120.0);
    cJSON_AddNumberToObject(resp, "rmsDbfs",
                            isfinite(rec.rms_dbfs) ? rec.rms_dbfs : -120.0);
    cJSON_AddBoolToObject(resp, "silent", rec.silent);
    cJSON_AddNumberToObject(resp, "fsFreeBytes", (double)freeb);
    cJSON_AddNumberToObject(resp, "fsUsedBytes", (double)(total - freeb));
    return send_json(req, resp);
}

/* -------------------------------------------------------------------------
 * API: download a file (so a recording can be played back in the browser)
 * ------------------------------------------------------------------------- */
static const char *content_type_for(const char *path)
{
    const char *dot = strrchr(path, '.');
    if (dot == NULL) {
        return "application/octet-stream";
    }
    if (strcasecmp(dot, ".wav") == 0)  return "audio/wav";
    if (strcasecmp(dot, ".txt") == 0)  return "text/plain";
    if (strcasecmp(dot, ".json") == 0) return "application/json";
    if (strcasecmp(dot, ".csv") == 0)  return "text/csv";
    return "application/octet-stream";
}

static esp_err_t api_sd_download(httpd_req_t *req)
{
    if (sd_card_get() == NULL) {
        return send_error(req, HTTPD_500, "no microSD card is mounted");
    }

    char rel[MAX_PATH_LEN];
    if (query_param(req, "path", rel, sizeof(rel)) != ESP_OK) {
        return send_error(req, HTTPD_400, "a path is required");
    }

    char full[MAX_PATH_LEN];
    if (!safe_sd_path(rel, full, sizeof(full))) {
        return send_error(req, HTTPD_400, "invalid path");
    }

    struct stat st = {0};
    if (stat(full, &st) != 0 || S_ISDIR(st.st_mode)) {
        return send_error(req, HTTPD_404, "no such file");
    }

    FILE *f = fopen(full, "rb");
    if (f == NULL) {
        return send_error(req, HTTPD_500, "could not open the file");
    }

    httpd_resp_set_type(req, content_type_for(full));
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");

    /* With ?dl=1 the browser saves the file under its own name instead of
     * rendering it — the same endpoint feeds both the inline audio player
     * (no disposition) and the Download button (attachment).
     * disposition[] must outlive httpd_resp_send_chunk: the header is stored
     * by reference, not copied. */
    char dl[4] = {0};
    char disposition[MAX_PATH_LEN + 40];
    if (query_param(req, "dl", dl, sizeof(dl)) == ESP_OK && dl[0] == '1') {
        const char *base = strrchr(full, '/');
        base = base ? base + 1 : full;
        snprintf(disposition, sizeof(disposition),
                 "attachment; filename=\"%s\"", base);
        httpd_resp_set_hdr(req, "Content-Disposition", disposition);
    }

    /* Stream in chunks — a recording can be far larger than any sane buffer. */
    char *chunk = malloc(4096);
    if (chunk == NULL) {
        fclose(f);
        return send_error(req, HTTPD_500, "out of memory");
    }

    esp_err_t ret = ESP_OK;
    for (;;) {
        const size_t n = fread(chunk, 1, 4096, f);
        if (n == 0) {
            break;
        }
        if (httpd_resp_send_chunk(req, chunk, n) != ESP_OK) {
            /* Client went away mid-download; stop rather than keep reading. */
            ret = ESP_FAIL;
            break;
        }
    }
    free(chunk);
    fclose(f);

    if (ret == ESP_OK) {
        httpd_resp_send_chunk(req, NULL, 0);   /* terminate the chunked body */
    }
    return ret;
}

/* -------------------------------------------------------------------------
 * API: AI camera
 * ------------------------------------------------------------------------- */
static esp_err_t api_ai_start(httpd_req_t *req)
{
    if (webcam_is_running()) {
        return send_error(req, HTTPD_400,
                          "The Camera tab is using the sensor — stop its stream first");
    }
    const esp_err_t ret = ai_camera_start();
    if (ret != ESP_OK) {
        return send_error(req, HTTPD_500,
                          ret == ESP_ERR_NOT_FOUND
                              ? "No camera detected on the CSI connector"
                              : "Camera pipeline failed to start — see the console log");
    }
    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "running", true);
    return send_json(req, resp);
}

static esp_err_t api_ai_stop(httpd_req_t *req)
{
    ai_camera_stop();
    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "running", false);
    return send_json(req, resp);
}

static esp_err_t api_ai_status(httpd_req_t *req)
{
    ai_status_t st;
    ai_camera_status(&st);

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "running", st.running);
    cJSON_AddNumberToObject(resp, "width", st.width);
    cJSON_AddNumberToObject(resp, "height", st.height);
    cJSON_AddNumberToObject(resp, "fps", st.fps);
    cJSON_AddNumberToObject(resp, "inferMs", st.infer_ms);
    cJSON_AddNumberToObject(resp, "frames", st.frames);
    cJSON_AddStringToObject(resp, "model", st.model);
    cJSON_AddStringToObject(resp, "sensor", st.sensor);
    return send_json(req, resp);
}

static esp_err_t api_ai_detections(httpd_req_t *req)
{
    ai_detection_t det[AI_MAX_DETECTIONS];
    uint32_t seq = 0;
    const int n = ai_camera_get_detections(det, AI_MAX_DETECTIONS, &seq);

    ai_status_t st;
    ai_camera_status(&st);

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddNumberToObject(resp, "seq", seq);
    cJSON_AddNumberToObject(resp, "width", st.width);
    cJSON_AddNumberToObject(resp, "height", st.height);
    cJSON_AddNumberToObject(resp, "fps", st.fps);
    cJSON_AddNumberToObject(resp, "inferMs", st.infer_ms);
    cJSON *list = cJSON_AddArrayToObject(resp, "detections");
    for (int i = 0; i < n; i++) {
        cJSON *d = cJSON_CreateObject();
        cJSON_AddStringToObject(d, "label", ai_camera_class_name(det[i].category));
        cJSON_AddNumberToObject(d, "score", det[i].score);
        cJSON_AddNumberToObject(d, "x0", det[i].x0);
        cJSON_AddNumberToObject(d, "y0", det[i].y0);
        cJSON_AddNumberToObject(d, "x1", det[i].x1);
        cJSON_AddNumberToObject(d, "y1", det[i].y1);
        cJSON_AddItemToArray(list, d);
    }
    return send_json(req, resp);
}

/* MJPEG stream: multipart/x-mixed-replace, one JPEG per part. Runs until the
 * client disconnects or the pipeline stops. */
static esp_err_t api_ai_stream(httpd_req_t *req)
{
    static const char *BOUNDARY = "\r\n--tcframe\r\n";

    ai_status_t st;
    ai_camera_status(&st);
    if (!st.running) {
        return send_error(req, HTTPD_400, "Start the camera first");
    }

    uint8_t *buf = malloc(256 * 1024);
    if (buf == NULL) {
        return send_error(req, HTTPD_500, "out of memory");
    }

    httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=tcframe");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");

    uint32_t last_seq = 0;
    esp_err_t ret = ESP_OK;

    while (ret == ESP_OK) {
        ai_camera_status(&st);
        if (!st.running) {
            break;
        }

        size_t len = 0;
        uint32_t seq = 0;
        if (ai_camera_get_jpeg(buf, 256 * 1024, &len, &seq) != ESP_OK ||
            seq == last_seq) {
            vTaskDelay(pdMS_TO_TICKS(30));   /* wait for a NEW frame */
            continue;
        }
        last_seq = seq;

        char part_hdr[96];
        const int hl = snprintf(part_hdr, sizeof(part_hdr),
                                "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
                                (unsigned)len);

        if ((ret = httpd_resp_send_chunk(req, BOUNDARY, strlen(BOUNDARY))) != ESP_OK ||
            (ret = httpd_resp_send_chunk(req, part_hdr, hl)) != ESP_OK ||
            (ret = httpd_resp_send_chunk(req, (const char *)buf, len)) != ESP_OK) {
            break;   /* client went away — normal end of a stream */
        }
    }

    free(buf);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* -------------------------------------------------------------------------
 * API: webcam (hardware H.264, no inference)
 * ------------------------------------------------------------------------- */
static esp_err_t api_cam_start(httpd_req_t *req)
{
    const esp_err_t ret = webcam_start();
    if (ret != ESP_OK) {
        const char *msg;
        switch (ret) {
        case ESP_ERR_INVALID_STATE:
            msg = "The AI tab is using the sensor — stop its camera first";
            break;
        case ESP_ERR_NOT_FOUND:
            msg = "No camera detected on the CSI connector";
            break;
        default:
            msg = "Camera pipeline failed to start — see the console log";
            break;
        }
        return send_error(req, HTTPD_500, msg);
    }
    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "running", true);
    return send_json(req, resp);
}

static esp_err_t api_cam_stop(httpd_req_t *req)
{
    webcam_stop();
    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "running", false);
    return send_json(req, resp);
}

static esp_err_t api_cam_status(httpd_req_t *req)
{
    webcam_status_t st;
    webcam_get_status(&st);

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "running", st.running);
    cJSON_AddNumberToObject(resp, "width", st.width);
    cJSON_AddNumberToObject(resp, "height", st.height);
    cJSON_AddNumberToObject(resp, "fps", st.fps);
    cJSON_AddNumberToObject(resp, "frames", st.frames);
    cJSON_AddNumberToObject(resp, "bitrate", st.bitrate);
    cJSON_AddStringToObject(resp, "sensor", st.sensor);
    return send_json(req, resp);
}

/* H.264 stream: 8-byte header (uint32 LE length, uint32 LE PTS ms) followed by
 * one Annex-B access unit, repeated. The dashboard decodes it with WebCodecs.
 * Runs until the client disconnects or the pipeline stops. */
static esp_err_t api_cam_h264_stream(httpd_req_t *req)
{
    if (!webcam_is_running()) {
        return send_error(req, HTTPD_400, "Start the camera first");
    }
    if (webcam_stream_open() != ESP_OK) {
        return send_error(req, HTTPD_400, "Another client is already streaming");
    }

    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    /* The dashboard fetch()es this from port 81 — a DIFFERENT origin than the
     * page on port 80, so without CORS consent the browser discards the
     * response. (The AI tab's MJPEG needs none of this: <img> loads are
     * exempt from CORS.) */
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    esp_err_t ret = ESP_OK;
    while (ret == ESP_OK && webcam_is_running()) {
        uint8_t *item = NULL;
        size_t len = 0;
        if (webcam_stream_read(&item, &len, 1000) != ESP_OK) {
            continue;   /* no frame within the wait — re-check and keep going */
        }
        ret = httpd_resp_send_chunk(req, (const char *)item, len);
        webcam_stream_return(item);
    }

    webcam_stream_close();
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* -------------------------------------------------------------------------
 * API: forget the network
 * ------------------------------------------------------------------------- */
static esp_err_t api_forget(httpd_req_t *req)
{
    const esp_err_t ret = wifi_manager_erase_credentials();
    if (ret != ESP_OK) {
        return send_error(req, HTTPD_500, "could not erase the stored network");
    }

    cJSON *resp = cJSON_CreateObject();
    cJSON_AddBoolToObject(resp, "ok", true);
    cJSON_AddBoolToObject(resp, "rebooting", true);
    const esp_err_t sent = send_json(req, resp);
    xTaskCreate(reboot_task, "reboot", 2048, NULL, 5, NULL);
    return sent;
}

/* -------------------------------------------------------------------------
 * Registration
 * ------------------------------------------------------------------------- */
typedef struct {
    const char       *uri;
    httpd_method_t    method;
    esp_err_t       (*handler)(httpd_req_t *);
    bool              setup_only;
    bool              dashboard_only;
} route_t;

static const route_t s_routes[] = {
    { "/",                  HTTP_GET,  handle_root,         false, false },
    { "/style.css",         HTTP_GET,  handle_style,        false, false },
    { "/app.js",            HTTP_GET,  handle_app_js,       false, false },

    { "/api/status",        HTTP_GET,  api_status,          false, false },
    { "/api/scan",          HTTP_GET,  api_scan,            true,  false },
    { "/api/connect",       HTTP_POST, api_connect,         true,  false },

    { "/api/board",         HTTP_GET,  api_board,           false, true  },
    { "/api/sd/list",       HTTP_GET,  api_sd_list,         false, true  },
    { "/api/sd/delete",     HTTP_POST, api_sd_delete,       false, true  },
    { "/api/sd/download",   HTTP_GET,  api_sd_download,     false, true  },
    { "/api/audio/start",   HTTP_POST, api_audio_start,     false, true  },
    { "/api/audio/stop",    HTTP_POST, api_audio_stop,      false, true  },
    { "/api/audio/state",   HTTP_GET,  api_audio_state,     false, true  },
    { "/api/audio/play",    HTTP_POST, api_audio_play,      false, true  },
    { "/api/audio/playstop",HTTP_POST, api_audio_playstop,  false, true  },

    { "/api/ai/start",      HTTP_POST, api_ai_start,        false, true  },
    { "/api/ai/stop",       HTTP_POST, api_ai_stop,         false, true  },
    { "/api/ai/status",     HTTP_GET,  api_ai_status,       false, true  },
    { "/api/ai/detections", HTTP_GET,  api_ai_detections,   false, true  },
    /* NOTE: /api/ai/stream is NOT here — the MJPEG stream occupies its HTTP
     * worker for its whole lifetime, and esp_http_server runs one task, so it
     * lives on a second server instance on port 81 (see below). */
    { "/api/cam/start",     HTTP_POST, api_cam_start,       false, true  },
    { "/api/cam/stop",      HTTP_POST, api_cam_stop,        false, true  },
    { "/api/cam/status",    HTTP_GET,  api_cam_status,      false, true  },
    /* /h264 lives on port 81 with the MJPEG stream, for the same reason. */
    { "/api/forget",        HTTP_POST, api_forget,          false, true  },

    /* Captive-portal detection endpoints used by the major platforms. */
    { "/generate_204",      HTTP_GET,  handle_portal_probe, true,  false },
    { "/gen_204",           HTTP_GET,  handle_portal_probe, true,  false },
    { "/hotspot-detect.html", HTTP_GET, handle_portal_probe, true, false },
    { "/library/test/success.html", HTTP_GET, handle_portal_probe, true, false },
    { "/connecttest.txt",   HTTP_GET,  handle_portal_probe, true,  false },
    { "/ncsi.txt",          HTTP_GET,  handle_portal_probe, true,  false },
    { "/redirect",          HTTP_GET,  handle_portal_probe, true,  false },
};

esp_err_t web_server_start(web_ui_mode_t mode)
{
    if (s_server != NULL) {
        return ESP_OK;
    }
    s_mode = mode;

    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.max_uri_handlers = sizeof(s_routes) / sizeof(s_routes[0]) + 2;
    cfg.stack_size       = 8192;   /* scan + JSON building need the headroom */
    cfg.lru_purge_enable = true;
    cfg.uri_match_fn     = httpd_uri_match_wildcard;

    esp_err_t ret = httpd_start(&s_server, &cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Could not start the HTTP server: %s", esp_err_to_name(ret));
        return ret;
    }

    for (size_t i = 0; i < sizeof(s_routes) / sizeof(s_routes[0]); i++) {
        const route_t *r = &s_routes[i];
        if ((r->setup_only && mode != WEB_UI_SETUP) ||
            (r->dashboard_only && mode != WEB_UI_DASHBOARD)) {
            continue;
        }
        const httpd_uri_t u = {
            .uri = r->uri, .method = r->method, .handler = r->handler, .user_ctx = NULL,
        };
        httpd_register_uri_handler(s_server, &u);
    }

    /* Second instance on port 81 for the MJPEG stream: it occupies its worker
     * for the stream's whole lifetime, so it must not share the UI server. */
    if (mode == WEB_UI_DASHBOARD) {
        httpd_config_t scfg = HTTPD_DEFAULT_CONFIG();
        scfg.server_port = 81;
        scfg.ctrl_port = 32769;          /* default instance owns 32768 */
        scfg.max_uri_handlers = 2;
        /* Streams are long-lived, one per viewer; two covers a tab switch
         * overlap. The default (7) here plus the UI server's 7 exhausts the
         * LWIP socket pool and new connections die in accept() with ENFILE. */
        scfg.max_open_sockets = 2;
        scfg.stack_size = 6144;
        scfg.lru_purge_enable = true;
        if (httpd_start(&s_stream_server, &scfg) == ESP_OK) {
            const httpd_uri_t stream_uri = {
                .uri = "/stream", .method = HTTP_GET,
                .handler = api_ai_stream, .user_ctx = NULL,
            };
            httpd_register_uri_handler(s_stream_server, &stream_uri);
            const httpd_uri_t h264_uri = {
                .uri = "/h264", .method = HTTP_GET,
                .handler = api_cam_h264_stream, .user_ctx = NULL,
            };
            httpd_register_uri_handler(s_stream_server, &h264_uri);
        } else {
            ESP_LOGW(TAG, "Stream server failed to start — AI video disabled");
        }
    }

    ESP_LOGI(TAG, "Web UI serving in %s mode",
             mode == WEB_UI_SETUP ? "setup" : "dashboard");
    return ESP_OK;
}

void web_server_stop(void)
{
    if (s_stream_server) {
        httpd_stop(s_stream_server);
        s_stream_server = NULL;
    }
    if (s_server) {
        httpd_stop(s_server);
        s_server = NULL;
    }
}
