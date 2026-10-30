#pragma once

/* Shared state between the mesh task, the phone API and the BLE glue. */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#define MESH_BROADCAST      0xFFFFFFFFu
#define MESH_MAX_CHANNELS   8
#define MESH_MAX_NODES      100
#define MESH_HEADER_LEN     16
#define MESH_MAX_DATA       (255 - MESH_HEADER_LEN)
#define MESH_FROMRADIO_MAX  512

/* --- Packets --- */

typedef struct {
    uint32_t from, to, id;
    uint8_t  chan_hash;
    int8_t   channel;         /* local channel index, -1 if not decrypted */
    uint8_t  hop_limit, hop_start, next_hop, relay_node;
    bool     want_ack, via_mqtt;

    /* Decoded `Data` message, exactly as carried on air. */
    uint8_t  data[MESH_MAX_DATA];
    uint16_t data_len;

    /* Fields parsed out of `data` for routing decisions. */
    uint32_t portnum, request_id;
    bool     want_response;
    uint16_t payload_off, payload_len;

    float    rx_snr;
    int32_t  rx_rssi;
    uint32_t rx_time;
} mesh_packet_t;

bool mesh_parse_data(mesh_packet_t *p);

/* --- Channels --- */

typedef struct {
    uint8_t role;             /* CHROLE_* */
    char    name[12];
    uint8_t psk[32];          /* as configured (may be the 1-byte shorthand) */
    uint8_t psk_len;
    bool    uplink, downlink;
    uint32_t position_precision;
} mesh_channel_t;

/* --- Persistent settings --- */

typedef struct {
    char     long_name[40];
    char     short_name[5];
    uint8_t  region;
    uint8_t  modem_preset;
    uint8_t  hop_limit;
    int8_t   tx_power;
    bool     tx_enabled;
    uint32_t channel_num;     /* 0 = slot derived from the channel name */
    uint32_t node_info_secs;
    uint32_t reboot_count;
    mesh_channel_t channels[MESH_MAX_CHANNELS];
} mesh_settings_t;

/* --- Node database --- */

typedef struct {
    uint32_t num;
    uint8_t  user[128];       /* raw User protobuf as last heard */
    uint8_t  user_len;
    char     long_name[40];
    char     short_name[5];
    uint32_t last_heard;
    float    snr;
    int8_t   channel;
    int8_t   hops_away;       /* -1 unknown */
    int64_t  nodeinfo_replied_us;
    int64_t  heard_us;        /* esp_timer time of the last packet, for "heard ago" */
} mesh_node_t;

/* --- mesh_node.c --- */

void mesh_lock(void);
void mesh_unlock(void);

uint32_t         mesh_my_num(void);
const uint8_t   *mesh_my_mac(void);
mesh_settings_t *mesh_settings(void);   /* call under mesh_lock */
void             mesh_settings_save(void);
void             mesh_apply_radio_settings(void);   /* re-tune after a config change */

size_t       mesh_node_count(void);
mesh_node_t *mesh_node_at(size_t i);     /* call under mesh_lock */
mesh_node_t *mesh_node_find(uint32_t num);
void         mesh_nodedb_reset(void);
void         mesh_node_remove(uint32_t num);

const char *mesh_preset_name(uint8_t preset);
bool        mesh_region_supported(uint8_t region);
bool        mesh_preset_supported(uint8_t preset);
uint32_t    mesh_now(void);              /* unix time, 0 if unknown */
void        mesh_set_time(uint32_t unix_time);

size_t mesh_encode_own_user(uint8_t *buf, size_t cap);
void   mesh_broadcast_nodeinfo(void);

/*
 * Queue a packet originated by the phone (or local UI). `data` is an encoded
 * Data message. Returns ESP_OK and the id used, or an error if the TX queue
 * is full.
 */
esp_err_t mesh_send_from_local(uint32_t to, uint8_t channel, const uint8_t *data, size_t data_len,
                               uint32_t id, bool want_ack, uint8_t hop_limit, uint32_t *id_out);

size_t mesh_tx_free(void);
size_t mesh_tx_max(void);

/* --- mesh_phone.c --- */

void   phone_init(void);
void   phone_connected(void);
void   phone_disconnected(void);
void   phone_handle_toradio(const uint8_t *buf, size_t len);
size_t phone_get_fromradio(uint8_t *buf, size_t cap);
bool   phone_is_streaming(void);

void phone_deliver_packet(const mesh_packet_t *p);
void phone_deliver_routing(uint32_t from, uint32_t request_id, uint8_t channel, uint32_t error);
void phone_deliver_queue_status(int32_t res, uint32_t mesh_packet_id);

/* --- mesh_ble.c --- */

esp_err_t   mesh_ble_start(const char *device_name);
void        mesh_ble_notify_fromnum(uint32_t num);
bool        mesh_ble_connected(void);
const char *mesh_ble_name(void);
