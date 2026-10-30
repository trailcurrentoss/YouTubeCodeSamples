#pragma once

/*
 * Meshtastic-compatible node for the CrowPanel Advance P4.
 *
 * The SX1262 module in the wireless socket carries the LoRa mesh; the
 * Meshtastic phone apps can also connect over BLE, which the P4 reaches
 * through the ESP32-C6 coprocessor (ESP-Hosted VHCI). Call after NVS is
 * initialised.
 *
 * Callbacks run on the mesh or BLE task. Keep them short and never touch
 * LVGL from them; copy the data and let the UI task paint it.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MESHTASTIC_BROADCAST 0xFFFFFFFFu

/* Routing outcome passed to the ack callback. */
#define MESHTASTIC_ACK_OK             0
#define MESHTASTIC_ACK_NO_ROUTE       1
#define MESHTASTIC_ACK_MAX_RETRANSMIT 5

typedef struct {
    uint32_t num;
    char     long_name[40];
    char     short_name[5];
    bool     has_user;          /* a NodeInfo has been heard */
    uint32_t heard_ago_s;
    float    snr;
    int8_t   hops_away;         /* -1 unknown */
} meshtastic_node_info_t;

typedef struct {
    uint32_t    num;
    char        long_name[40];
    char        short_name[5];
    char        ble_name[16];
    char        channel_name[12];
    const char *region;
    const char *preset;
    uint32_t    freq_hz;
    int8_t      tx_power;
    uint8_t     hop_limit;
    bool        radio_ok;
    bool        phone_connected;
    size_t      node_count;
} meshtastic_status_t;

/* A text message heard on the mesh, addressed to us or broadcast. */
typedef void (*meshtastic_text_cb_t)(uint32_t from, const char *from_name, uint32_t to,
                                     uint8_t channel, const char *text, size_t len);

/* A text message this node originated (from the screen or the phone app). */
typedef void (*meshtastic_sent_cb_t)(uint32_t to, uint8_t channel, uint32_t id,
                                     const char *text, size_t len);

/* Final outcome of a want_ack message we sent: MESHTASTIC_ACK_*. */
typedef void (*meshtastic_ack_cb_t)(uint32_t id, uint32_t error);

esp_err_t meshtastic_node_start(void);

uint32_t meshtastic_node_num(void);

void meshtastic_node_set_text_callback(meshtastic_text_cb_t cb);
void meshtastic_node_set_sent_callback(meshtastic_sent_cb_t cb);
void meshtastic_node_set_ack_callback(meshtastic_ack_cb_t cb);

/* Send a text message on `channel` to a node number or MESHTASTIC_BROADCAST. */
esp_err_t meshtastic_node_send_text(uint32_t to, uint8_t channel, const char *text, uint32_t *id_out);

/* Copy up to `max` known nodes, most recently heard first. Returns the count. */
size_t meshtastic_node_get_nodes(meshtastic_node_info_t *out, size_t max);

/* Look up one node's names; returns false if unknown. */
bool meshtastic_node_get_info(uint32_t num, meshtastic_node_info_t *out);

void meshtastic_node_get_status(meshtastic_status_t *out);

/* Rename this node, persist it and announce it to the mesh. */
esp_err_t meshtastic_node_set_owner(const char *long_name, const char *short_name);

/* --- Radio settings, as the Meshtastic apps and other nodes see them --- */

/* A region this module can use (902-928 MHz only: the module and antenna are 915 MHz parts). */
typedef struct {
    uint8_t     code;           /* Meshtastic RegionCode */
    const char *name;
    float       start_mhz, end_mhz;
} meshtastic_region_t;

typedef struct {
    uint8_t     code;           /* Meshtastic ModemPreset */
    const char *name;
} meshtastic_preset_t;

/* In the order the Meshtastic firmware lists them. */
const meshtastic_region_t *meshtastic_regions(size_t *count);
const meshtastic_preset_t *meshtastic_presets(size_t *count);

typedef struct {
    uint8_t  region;
    uint8_t  preset;
    uint32_t slot;              /* frequency slot, 1-based; 0 = picked from the channel name */
    uint8_t  hop_limit;         /* 1..7 */
    int8_t   tx_power;          /* dBm; 0 = the most the region and module allow */
    char     channel_name[12];  /* primary channel; empty = the preset name */
    uint8_t  psk[32];
    uint8_t  psk_len;           /* 0 = no encryption, 1 = default-key shorthand, 16 or 32 = AES key */
} meshtastic_radio_config_t;

/* What a config works out to on air. */
typedef struct {
    uint32_t freq_hz;
    uint32_t slot;              /* 1-based slot in use */
    uint32_t slots;             /* slots the region has at this preset's bandwidth */
    int8_t   tx_power;          /* dBm actually used */
    int8_t   max_tx_power;
} meshtastic_radio_plan_t;

void meshtastic_node_get_radio_config(meshtastic_radio_config_t *out);
void meshtastic_radio_plan(const meshtastic_radio_config_t *cfg, meshtastic_radio_plan_t *out);

/* Persist, retune and announce this node on the new settings. */
esp_err_t meshtastic_node_set_radio_config(const meshtastic_radio_config_t *cfg);

/* Channel keys in the base64 form the Meshtastic apps show ("AQ==" is the default key). */
esp_err_t meshtastic_psk_from_base64(const char *b64, uint8_t psk[32], uint8_t *psk_len);
void      meshtastic_psk_to_base64(const uint8_t *psk, uint8_t psk_len, char *out, size_t out_size);

#ifdef __cplusplus
}
#endif
