/*
 * sd_config.c — see sd_config.h for the file-format contract.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "sd_config.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "board_esp32p4_wifi6.h"
#include "peregrine_voice.h"

static const char *TAG = "sd_config";

#define CONF_PATH  BSP_SD_MOUNT_POINT "/environment.conf"
#define MAX_LINE   256

static bool s_have_url;
static bool s_have_token;

/* Trim ASCII whitespace from both ends, in place. Returns the new start, which
 * may be past the buffer's own start. */
static char *trim(char *s)
{
    while (*s && isspace((unsigned char)*s)) {
        s++;
    }
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) {
        end--;
    }
    *end = '\0';
    return s;
}

/* Strip one layer of matching quotes, so a value with trailing spaces can be
 * written as "…  " when that is genuinely wanted. */
static char *unquote(char *s)
{
    const size_t len = strlen(s);
    if (len >= 2 && (s[0] == '"' || s[0] == '\'') && s[len - 1] == s[0]) {
        s[len - 1] = '\0';
        return s + 1;
    }
    return s;
}

esp_err_t sd_config_load(void)
{
    s_have_url = false;
    s_have_token = false;

    FILE *f = fopen(CONF_PATH, "r");
    if (f == NULL) {
        ESP_LOGI(TAG, "No %s — using built-in defaults", CONF_PATH);
        return ESP_OK;
    }

    char line[MAX_LINE];
    int lineno = 0;
    while (fgets(line, sizeof(line), f) != NULL) {
        lineno++;

        char *p = strchr(line, '#');
        if (p) {
            *p = '\0';              /* comment runs to end of line */
        }
        char *key = trim(line);
        if (*key == '\0') {
            continue;
        }

        char *eq = strchr(key, '=');
        if (eq == NULL) {
            ESP_LOGW(TAG, "%s:%d — no '=', skipping", CONF_PATH, lineno);
            continue;
        }
        *eq = '\0';
        key = trim(key);
        char *value = unquote(trim(eq + 1));

        if (strcmp(key, "PEREGRINE_URL") == 0) {
            if (*value) {
                peregrine_voice_set_url(value);
                s_have_url = true;
            }
        } else if (strcmp(key, "PEREGRINE_VOICE_TOKEN") == 0) {
            if (*value) {
                peregrine_voice_set_token(value);
                s_have_token = true;
            }
        } else {
            /* Not ours — another consumer may claim it in a later build. */
            ESP_LOGI(TAG, "%s:%d — ignoring unknown key %s", CONF_PATH, lineno, key);
        }
    }
    fclose(f);

    if (s_have_url && s_have_token) {
        ESP_LOGI(TAG, "Loaded Peregrine settings from %s", CONF_PATH);
    } else {
        /* Naming the missing key matters: the usual mistake is a token that is
         * present but empty, which looks identical to a correct file at a
         * glance. */
        ESP_LOGW(TAG, "%s is missing %s%s%s — push-to-talk stays disabled",
                 CONF_PATH,
                 s_have_url ? "" : "PEREGRINE_URL",
                 (!s_have_url && !s_have_token) ? " and " : "",
                 s_have_token ? "" : "PEREGRINE_VOICE_TOKEN");
    }
    return ESP_OK;
}

bool sd_config_peregrine_present(void)
{
    return s_have_url && s_have_token;
}
