/*
 * broker_cfg_esp - NVS-backed Headwaters settings.
 *
 * Namespace "mqtt", beside the Wi-Fi credentials in "wifi". Device-only, so it
 * lives in main/ where the EEZ Studio simulator never copies it.
 *
 * Called from the LVGL task when the user saves. NVS writes are short and the
 * screen is not animating at that moment, so they run inline rather than on a
 * worker -- unlike the radio, which is why wifi_port has a task and this does
 * not.
 */

#ifndef EEZ_LVGL_SIMULATOR

#include "broker_cfg.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs.h"

static const char *TAG = "broker_cfg";

/* Implemented in mqtt_source.c; both are device-only. */
void mqtt_source_restart(void);

#define NS        "mqtt"
#define KEY_HOST  "host"
#define KEY_USER  "user"
#define KEY_PASS  "pass"

static char s_status[24] = "NOT CONFIGURED";

static void copy_str(char *dst, size_t dstlen, const char *src)
{
    if (!dst || dstlen == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    size_t n = strlen(src);
    if (n >= dstlen) n = dstlen - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
}

void broker_cfg_init(void)
{
    broker_cfg_t c;
    copy_str(s_status, sizeof(s_status),
             broker_cfg_get(&c) ? "CONFIGURED" : "NOT CONFIGURED");
}

bool broker_cfg_get(broker_cfg_t *out)
{
    if (!out) return false;
    memset(out, 0, sizeof(*out));
    /* Prefilled so the screen opens with something sensible even before
     * anything is stored; the rig server's mDNS name is the same on every
     * TrailCurrent install. */
    copy_str(out->host, sizeof(out->host), "headwaters.local");

    nvs_handle_t h;
    if (nvs_open(NS, NVS_READONLY, &h) != ESP_OK) return false;

    size_t n = sizeof(out->host);
    const bool have_host = (nvs_get_str(h, KEY_HOST, out->host, &n) == ESP_OK)
                           && out->host[0] != '\0';
    n = sizeof(out->user);
    const bool have_user = (nvs_get_str(h, KEY_USER, out->user, &n) == ESP_OK)
                           && out->user[0] != '\0';
    n = sizeof(out->pass);
    nvs_get_str(h, KEY_PASS, out->pass, &n);
    nvs_close(h);

    if (!have_host) copy_str(out->host, sizeof(out->host), "headwaters.local");
    return have_host && have_user;
}

bool broker_cfg_set(const broker_cfg_t *in)
{
    if (!in || in->host[0] == '\0' || in->user[0] == '\0') {
        copy_str(s_status, sizeof(s_status), "HOST/USER REQUIRED");
        return false;
    }

    nvs_handle_t h;
    if (nvs_open(NS, NVS_READWRITE, &h) != ESP_OK) {
        copy_str(s_status, sizeof(s_status), "SAVE FAILED");
        return false;
    }
    esp_err_t err = nvs_set_str(h, KEY_HOST, in->host);
    if (err == ESP_OK) err = nvs_set_str(h, KEY_USER, in->user);
    if (err == ESP_OK) err = nvs_set_str(h, KEY_PASS, in->pass);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "save: %s", esp_err_to_name(err));
        copy_str(s_status, sizeof(s_status), "SAVE FAILED");
        return false;
    }
    /* Host only. The username is not secret but there is no reason to put it
     * in a log that gets pasted into issues, and the password never appears
     * here at all. */
    ESP_LOGI(TAG, "broker settings saved (host '%s')", in->host);
    copy_str(s_status, sizeof(s_status), "SAVED");
    return true;
}

void broker_cfg_apply(void)
{
    mqtt_source_restart();
}

void broker_cfg_status_text(char *out, size_t len)
{
    if (!out || len == 0) return;
    copy_str(out, len, s_status);
}

#else

typedef int broker_cfg_esp_placeholder;

#endif /* EEZ_LVGL_SIMULATOR */
