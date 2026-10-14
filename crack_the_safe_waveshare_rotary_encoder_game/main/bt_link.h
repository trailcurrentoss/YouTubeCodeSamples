/*
 * Bluetooth headphones, through the ESP32 co-processor (coprocessor/).
 *
 * The S3 has no Classic Bluetooth; the board's second chip does. This side
 * speaks the line protocol documented in coprocessor/main/main.c and keeps a
 * snapshot of what the ESP32 last reported, for the pairing screen to show.
 *
 * Sounds played on the board while headphones are connected are also
 * streamed to them (see stream_tap() in bt_link.c).
 *
 * Thread-safety: every function may be called from any task.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BT_LINK_ROWS     8           /* rows on the pairing screen */
#define BT_LINK_NAME_MAX 48

typedef enum {
    BT_CONN_IDLE = 0,
    BT_CONN_CONNECTING,
    BT_CONN_CONNECTED,
} bt_conn_t;

typedef struct {
    char addr[18];                   /* "aa:bb:cc:dd:ee:ff" */
    int8_t rssi;
    char name[BT_LINK_NAME_MAX];
} bt_dev_t;

typedef struct {
    uint32_t gen;                    /* bumps on every change */
    bool present;                    /* the ESP32 answered: our firmware */
    bool searching;
    bool failed;                     /* the last connect did not succeed */
    bt_conn_t conn;
    bt_dev_t peer;                   /* while connecting / connected */
    bool has_saved;                  /* headphones the ESP32 remembers */
    bt_dev_t saved;
    uint8_t ndev;
    bt_dev_t dev[BT_LINK_ROWS];      /* found by the last search */
} bt_link_state_t;

/** Open the link and start looking for the ESP32. Call once. */
void bt_link_init(void);

/** A copy of the current state. */
void bt_link_get(bt_link_state_t *out);

/** Row `i` of the pairing list: the "home" headphones first -- connected,
 *  connecting, or just remembered -- then the rest of the search results.
 *  False past the end. */
bool bt_link_row(const bt_link_state_t *s, int i, bt_dev_t *out);

/** The headphones in row 0, if any: connected, connecting or remembered. */
bool bt_link_home(const bt_link_state_t *s, bt_dev_t *out);

void bt_link_search(void);
void bt_link_stop_search(void);
void bt_link_connect(const char *addr);
/** Disconnect and forget the remembered headphones. */
void bt_link_forget(void);

/** True while headphones are connected: sounds should go to them. */
bool bt_link_connected(void);

/**
 * Register sound `id` (0-3): signed 16-bit mono at `rate`. Uploaded to the
 * ESP32 now and again whenever it restarts. The samples must stay valid.
 */
void bt_link_set_clip(int id, const int16_t *pcm, size_t samples, uint32_t rate);

/** Play sound `id` on the headphones. */
void bt_link_play(int id);

/** The volume on the headphones: a gain, 256 = sounds as uploaded. Sent now
 *  and again whenever the ESP32 restarts. */
void bt_link_set_gain(uint16_t gain_q8);

/** Play the police siren on the headphones for `ms`; 0 stops it. The ESP32
 *  synthesises it (it is too long to upload as a sound). */
void bt_link_siren(uint32_t ms);
