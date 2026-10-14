/*
 * Bluetooth headphones, through the ESP32 co-processor. See bt_link.h, and
 * coprocessor/main/main.c for the other end of every line.
 */

#include "bt_link.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_rom_crc.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "mbedtls/base64.h"

#include "knob_board.h"

static const char *TAG = "bt_link";

#define CLIPS      4
#define HELLO_US   (2 * 1000 * 1000)    /* ask again until the ESP32 answers */

static SemaphoreHandle_t s_lock;
static bt_link_state_t s_st;

static uint16_t s_gain_q8 = 256;

static struct {
    const int16_t *pcm;
    size_t samples;
    uint32_t rate;
} s_clips[CLIPS];

static void send(const char *line)
{
    knob_board_coproc_send(line, strlen(line));
}

static void upload_clip(int id)
{
    if (!s_clips[id].pcm) {
        return;
    }
    const size_t bytes = s_clips[id].samples * sizeof(int16_t);
    size_t b64_len = 0;
    mbedtls_base64_encode(NULL, 0, &b64_len, (const unsigned char *)s_clips[id].pcm, bytes);
    char head[32];
    const int hn = snprintf(head, sizeof(head), "CLIP %d %lu ", id, (unsigned long)s_clips[id].rate);
    char *line = malloc(hn + b64_len + 1);
    if (!line) {
        return;
    }
    memcpy(line, head, hn);
    size_t out = 0;
    if (mbedtls_base64_encode((unsigned char *)line + hn, b64_len + 1, &out,
                              (const unsigned char *)s_clips[id].pcm, bytes) == 0) {
        knob_board_coproc_send(line, hn + out);
        ESP_LOGI(TAG, "sound %d sent (%u samples)", id, (unsigned)s_clips[id].samples);
    }
    free(line);
}

/* Copy a device name (UTF-8) into what the screen's Roboto subset can draw:
 * printable ASCII plus the few extras in GUI/tmp/fonts.py's ROBOTO_RANGE.
 * Curly quotes become straight ones -- Apple names AirPods "Name’s AirPods"
 * with U+2019 -- and anything else becomes '?', rather than a blank box. */
static void name_copy(char *dst, size_t size, const char *src)
{
    size_t n = 0;
    const unsigned char *p = (const unsigned char *)src;
    while (*p && n + 4 < size) {
        uint32_t cp;
        int len;
        if (*p < 0x80) {
            cp = *p;
            len = 1;
        } else if ((*p & 0xE0) == 0xC0 && p[1]) {
            cp = ((*p & 0x1Fu) << 6) | (p[1] & 0x3Fu);
            len = 2;
        } else if ((*p & 0xF0) == 0xE0 && p[1] && p[2]) {
            cp = ((*p & 0x0Fu) << 12) | ((p[1] & 0x3Fu) << 6) | (p[2] & 0x3Fu);
            len = 3;
        } else {
            cp = '?';
            len = 1;
            while (p[len] && (p[len] & 0xC0) == 0x80) {
                len++;             /* skip the rest of a 4-byte sequence */
            }
        }
        p += len;
        if (cp == 0x2018 || cp == 0x2019 || cp == 0x201B || cp == 0x2032) {
            cp = '\'';
        } else if (cp == 0x201C || cp == 0x201D) {
            cp = '"';
        }
        if (cp >= 0x20 && cp < 0x7F) {
            dst[n++] = (char)cp;
        } else if (cp == 0xB0 || cp == 0xB7) {
            dst[n++] = (char)(0xC0 | (cp >> 6));
            dst[n++] = (char)(0x80 | (cp & 0x3F));
        } else if (cp == 0x2013 || cp == 0x2014 || cp == 0x2026) {
            dst[n++] = (char)(0xE0 | (cp >> 12));
            dst[n++] = (char)(0x80 | ((cp >> 6) & 0x3F));
            dst[n++] = (char)(0x80 | (cp & 0x3F));
        } else {
            dst[n++] = '?';
        }
    }
    dst[n] = '\0';
}

static void dev_from(bt_dev_t *d, const char *addr, int rssi, const char *name)
{
    strlcpy(d->addr, addr, sizeof(d->addr));
    d->rssi = (int8_t)rssi;
    name_copy(d->name, sizeof(d->name), name && *name ? name : addr);
}

/* One line from the ESP32, on the board's reader task. */
static void on_line(const char *line, void *ctx)
{
    (void)ctx;
    ESP_LOGD(TAG, "<- %s", line);
    bool upload = false;

    if (strncmp(line, "LOG ", 4) == 0) {
        /* The ESP32's own log, so it can be read on the S3's USB port. */
        ESP_LOGI(TAG, "esp32| %s", line + 4);
        return;
    }

    xSemaphoreTake(s_lock, portMAX_DELAY);
    if (strncmp(line, "READY", 5) == 0) {
        /* Just started (or answering HELLO): it has no sounds yet, and
         * whatever it was doing before -- a search, a connection -- is gone.
         * Its SAVED and STATE lines follow at once. */
        upload = true;
        s_st.present = true;
        s_st.searching = false;
        s_st.failed = false;
        s_st.conn = BT_CONN_IDLE;
    } else if (strcmp(line, "SCAN start") == 0) {
        s_st.searching = true;
        s_st.ndev = 0;
        s_st.failed = false;
    } else if (strcmp(line, "SCAN done") == 0) {
        s_st.searching = false;
    } else if (strncmp(line, "DEV ", 4) == 0) {
        char addr[18];
        int rssi, used = 0;
        if (sscanf(line + 4, "%17s %d %n", addr, &rssi, &used) >= 2) {
            const char *name = used ? line + 4 + used : "";
            int i;
            for (i = 0; i < s_st.ndev && strcmp(s_st.dev[i].addr, addr) != 0; i++) {
            }
            if (i < BT_LINK_ROWS) {
                dev_from(&s_st.dev[i], addr, rssi, name);
                if (i == s_st.ndev) {
                    s_st.ndev++;
                }
            }
        }
    } else if (strncmp(line, "STATE ", 6) == 0) {
        char st[16], addr[18];
        int used = 0;
        if (sscanf(line + 6, "%15s %17s %n", st, addr, &used) >= 2) {
            const char *name = used ? line + 6 + used : "";
            s_st.conn = strcmp(st, "connected") == 0    ? BT_CONN_CONNECTED
                        : strcmp(st, "connecting") == 0 ? BT_CONN_CONNECTING
                                                        : BT_CONN_IDLE;
            if (s_st.conn != BT_CONN_IDLE) {
                dev_from(&s_st.peer, addr, 0, name);
            }
            if (s_st.conn == BT_CONN_CONNECTED) {
                s_st.failed = false;
            }
        }
    } else if (strncmp(line, "SAVED ", 6) == 0) {
        char addr[18];
        int used = 0;
        if (sscanf(line + 6, "%17s %n", addr, &used) >= 1) {
            s_st.has_saved = strcmp(addr, "-") != 0;
            if (s_st.has_saved) {
                dev_from(&s_st.saved, addr, 0, used ? line + 6 + used : "");
            }
        }
    } else if (strcmp(line, "FAIL") == 0) {
        s_st.failed = true;
    } else {
        xSemaphoreGive(s_lock);
        return;                          /* not ours -- leave gen alone */
    }
    s_st.gen++;
    xSemaphoreGive(s_lock);

    if (upload) {
        for (int i = 0; i < CLIPS; i++) {
            upload_clip(i);
        }
        bt_link_set_gain(s_gain_q8);
    }
}

static void hello_cb(void *arg)
{
    (void)arg;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    const bool present = s_st.present;
    xSemaphoreGive(s_lock);
    if (!present) {
        /* Still on the factory firmware, or not up yet. */
        send("HELLO");
    }
}

/* ---- Streaming -------------------------------------------------------------
 *
 * Sounds too long to upload (the vault door, the prizes) reach the
 * headphones as they play: the board's audio tap hands over each block the
 * DAC gets, and while headphones are connected it goes to the ESP32 as
 * "PCM <crc> <base64>" -- 256 samples, ~700 bytes every 11.6 ms, about two
 * thirds of the link at 921600 baud. The CRC lets the ESP32 drop a line
 * damaged on the wire instead of playing it as noise. The ESP32 buffers ~90 ms of it and mixes it
 * into the headphones' stream. The wired output is muted meanwhile, so the
 * same blocks going to the DAC are harmless. */
static void stream_tap(const int16_t *pcm, size_t samples, void *ctx)
{
    (void)ctx;
    if (!bt_link_connected()) {
        return;
    }
    static char line[4 + 5 + 4 * ((256 * 2 + 2) / 3) + 1] = "PCM ";
    const size_t bytes = samples * sizeof(int16_t);
    snprintf(line + 4, 6, "%04x ", esp_rom_crc16_le(0, (const uint8_t *)pcm, bytes));
    size_t out = 0;
    if (mbedtls_base64_encode((unsigned char *)line + 9, sizeof(line) - 9, &out,
                              (const unsigned char *)pcm, bytes) == 0) {
        knob_board_coproc_send(line, 9 + out);
    }
}

void bt_link_init(void)
{
    s_lock = xSemaphoreCreateMutex();
    if (knob_board_coproc_start(on_line, NULL) != ESP_OK) {
        return;
    }
    send("HELLO");
    knob_board_audio_set_tap(stream_tap, NULL);
    const esp_timer_create_args_t args = { .callback = hello_cb, .name = "bt_hello" };
    esp_timer_handle_t t;
    if (esp_timer_create(&args, &t) == ESP_OK) {
        esp_timer_start_periodic(t, HELLO_US);
    }
}

void bt_link_get(bt_link_state_t *out)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);
    *out = s_st;
    xSemaphoreGive(s_lock);
}

bool bt_link_home(const bt_link_state_t *s, bt_dev_t *out)
{
    if (s->conn != BT_CONN_IDLE) {
        *out = s->peer;
        return true;
    }
    if (s->has_saved) {
        *out = s->saved;
        return true;
    }
    return false;
}

bool bt_link_row(const bt_link_state_t *s, int i, bt_dev_t *out)
{
    bt_dev_t home;
    const bool has_home = bt_link_home(s, &home);
    if (has_home) {
        if (i == 0) {
            *out = home;
            return true;
        }
        i--;
    }
    for (int k = 0; k < s->ndev; k++) {
        if (has_home && strcmp(s->dev[k].addr, home.addr) == 0) {
            continue;                    /* already row 0 */
        }
        if (i-- == 0) {
            *out = s->dev[k];
            return true;
        }
    }
    return false;
}

void bt_link_search(void)
{
    send("SCAN");
}

void bt_link_stop_search(void)
{
    send("STOP");
}

void bt_link_connect(const char *addr)
{
    char line[32];
    snprintf(line, sizeof(line), "CONNECT %s", addr);
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_st.failed = false;
    s_st.gen++;
    xSemaphoreGive(s_lock);
    send(line);
}

void bt_link_forget(void)
{
    send("FORGET");
}

bool bt_link_connected(void)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);
    const bool c = s_st.present && s_st.conn == BT_CONN_CONNECTED;
    xSemaphoreGive(s_lock);
    return c;
}

void bt_link_set_clip(int id, const int16_t *pcm, size_t samples, uint32_t rate)
{
    if (id < 0 || id >= CLIPS) {
        return;
    }
    s_clips[id].pcm = pcm;
    s_clips[id].samples = samples;
    s_clips[id].rate = rate;
    upload_clip(id);
}

void bt_link_play(int id)
{
    char line[12];
    snprintf(line, sizeof(line), "PLAY %d", id);
    send(line);
}

void bt_link_set_gain(uint16_t gain_q8)
{
    s_gain_q8 = gain_q8;
    char line[16];
    snprintf(line, sizeof(line), "VOL %u", (unsigned)gain_q8);
    send(line);
}

void bt_link_siren(uint32_t ms)
{
    char line[20];
    snprintf(line, sizeof(line), "SIREN %lu", (unsigned long)ms);
    send(line);
}
