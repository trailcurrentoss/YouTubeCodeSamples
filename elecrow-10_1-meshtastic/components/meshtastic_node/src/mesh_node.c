/*
 * Meshtastic node core: settings, channels, packet crypto, managed-flood
 * routing and the radio task.
 *
 * Wire format (all little-endian):
 *   to(4) from(4) id(4) flags(1) channel_hash(1) next_hop(1) relay_node(1)
 *   then the `Data` protobuf, AES-CTR encrypted with the channel key.
 * flags: bits 0-2 hop_limit, bit 3 want_ack, bit 4 via_mqtt, bits 5-7 hop_start.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_random.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "mbedtls/aes.h"
#include "mbedtls/base64.h"
#include "nvs.h"
#include "sdkconfig.h"

#include "meshtastic_node.h"
#include "mesh_internal.h"
#include "mesh_pb.h"
#include "mesh_proto.h"
#include "sx1262.h"

static const char *TAG = "mesh";

#define SETTINGS_MAGIC    0x4D534833u   /* "MSH3" */
#define DEDUPE_SLOTS      64
#define TX_SLOTS          16
#define ACK_TIMEOUT_US    (12 * 1000000LL)
#define ACK_TRANSMISSIONS 3
#define TX_TIMEOUT_MS     5000
#define RX_STALE_US       (3 * 1000000LL)

/* Default channel key ("AQ==" expands to this, last byte + index - 1). */
static const uint8_t DEFAULT_PSK[16] = {
    0xd4, 0xf1, 0xbb, 0x3a, 0x20, 0x29, 0x07, 0x59,
    0xf0, 0xbc, 0xff, 0xab, 0xcf, 0x4e, 0x69, 0x01,
};

/* --- Regions and presets --- */

#define MODULE_MAX_DBM 22

/*
 * The Elecrow module and antenna are 915 MHz parts, so only the Meshtastic
 * regions inside 902-928 MHz. Bands and power limits follow the Meshtastic
 * firmware's region table; all of them use the standard preset list with no
 * channel spacing, so one slot formula covers them.
 */
static const meshtastic_region_t REGIONS[] = {
    { REGION_US,     "US",     902.0f, 928.0f },
    { REGION_JP,     "JP",     920.5f, 923.5f },
    { REGION_ANZ,    "ANZ",    915.0f, 928.0f },
    { REGION_KR,     "KR",     920.0f, 923.0f },
    { REGION_TW,     "TW",     920.0f, 925.0f },
    { REGION_TH,     "TH",     920.0f, 925.0f },
    { REGION_MY_919, "MY_919", 919.0f, 924.0f },
    { REGION_SG_923, "SG_923", 917.0f, 925.0f },
    { REGION_PH_915, "PH_915", 915.0f, 918.0f },
    { REGION_BR_902, "BR_902", 902.0f, 907.5f },
};
#define NUM_REGIONS (sizeof(REGIONS) / sizeof(REGIONS[0]))

/* Regulatory TX limits in dBm, same order as REGIONS. */
static const int8_t REGION_POWER[NUM_REGIONS] = { 30, 13, 30, 23, 27, 27, 27, 20, 24, 30 };

typedef struct {
    float   bw_khz;
    uint8_t sf, cr;
} modem_t;

static const modem_t MODEMS[] = {
    [PRESET_LONG_FAST]     = { 250.0f, 11, 5 },
    [PRESET_LONG_SLOW]     = { 125.0f, 12, 8 },
    [PRESET_MEDIUM_SLOW]   = { 250.0f, 10, 5 },
    [PRESET_MEDIUM_FAST]   = { 250.0f,  9, 5 },
    [PRESET_SHORT_SLOW]    = { 250.0f,  8, 5 },
    [PRESET_SHORT_FAST]    = { 250.0f,  7, 5 },
    [PRESET_LONG_MODERATE] = { 125.0f, 11, 8 },
    [PRESET_SHORT_TURBO]   = { 500.0f,  7, 5 },
    [PRESET_LONG_TURBO]    = { 500.0f, 11, 8 },
    [PRESET_MEDIUM_TURBO]  = { 500.0f,  9, 5 },
};
#define NUM_MODEMS (sizeof(MODEMS) / sizeof(MODEMS[0]))

/* Display names double as the default channel name, so they must match the firmware's exactly. */
static const meshtastic_preset_t PRESETS[] = {
    { PRESET_LONG_FAST,     "LongFast" },
    { PRESET_LONG_SLOW,     "LongSlow" },
    { PRESET_MEDIUM_SLOW,   "MediumSlow" },
    { PRESET_MEDIUM_FAST,   "MediumFast" },
    { PRESET_SHORT_SLOW,    "ShortSlow" },
    { PRESET_SHORT_FAST,    "ShortFast" },
    { PRESET_LONG_MODERATE, "LongMod" },
    { PRESET_SHORT_TURBO,   "ShortTurbo" },
    { PRESET_LONG_TURBO,    "LongTurbo" },
    { PRESET_MEDIUM_TURBO,  "MediumTurbo" },
};
#define NUM_PRESETS (sizeof(PRESETS) / sizeof(PRESETS[0]))

const meshtastic_region_t *meshtastic_regions(size_t *count) { *count = NUM_REGIONS; return REGIONS; }
const meshtastic_preset_t *meshtastic_presets(size_t *count) { *count = NUM_PRESETS; return PRESETS; }

static int region_index(uint8_t code)
{
    for (size_t i = 0; i < NUM_REGIONS; i++)
        if (REGIONS[i].code == code) return (int)i;
    return -1;
}

static int preset_index(uint8_t code)
{
    for (size_t i = 0; i < NUM_PRESETS; i++)
        if (PRESETS[i].code == code) return (int)i;
    return -1;
}

bool mesh_region_supported(uint8_t region) { return region_index(region) >= 0; }
bool mesh_preset_supported(uint8_t preset) { return preset_index(preset) >= 0; }

/* Unknown presets fall back to LongFast, as in the firmware. */
static const modem_t *modem_get(uint8_t p)
{
    return mesh_preset_supported(p) ? &MODEMS[p] : &MODEMS[PRESET_LONG_FAST];
}

const char *mesh_preset_name(uint8_t preset)
{
    int i = preset_index(preset);
    return PRESETS[i < 0 ? 0 : i].name;
}

/* --- State --- */

typedef struct {
    bool     used;
    int64_t  due_us;
    uint8_t  buf[255];
    uint8_t  len;
    uint32_t from, to, id;
    uint8_t  channel;
    bool     rebroadcast;     /* cancel if another node relays it first */
    bool     track_ack;       /* we originated it with want_ack */
    uint8_t  sends_left;
    bool     notify_phone;
} tx_slot_t;

typedef struct {
    uint32_t from, id;
    int64_t  seen_us;
} seen_t;

static SemaphoreHandle_t s_lock;
static mesh_settings_t   s_settings;
static mesh_node_t      *s_nodes;
static size_t            s_node_count;
static uint32_t          s_my_num;
static uint8_t           s_mac[6];
static tx_slot_t         s_tx[TX_SLOTS];
static seen_t            s_seen[DEDUPE_SLOTS];
static size_t            s_seen_next;
static volatile bool     s_retune;
static meshtastic_text_cb_t s_text_cb;
static meshtastic_sent_cb_t s_sent_cb;
static meshtastic_ack_cb_t  s_ack_cb;
static int64_t           s_rx_busy_since;
static int64_t           s_next_nodeinfo_us;
static uint32_t          s_freq_hz;
static bool              s_radio_ok;

void mesh_lock(void)   { xSemaphoreTakeRecursive(s_lock, portMAX_DELAY); }
void mesh_unlock(void) { xSemaphoreGiveRecursive(s_lock); }

uint32_t         mesh_my_num(void)   { return s_my_num; }
const uint8_t   *mesh_my_mac(void)   { return s_mac; }
mesh_settings_t *mesh_settings(void) { return &s_settings; }
uint32_t meshtastic_node_num(void)   { return s_my_num; }

void meshtastic_node_set_text_callback(meshtastic_text_cb_t cb) { s_text_cb = cb; }
void meshtastic_node_set_sent_callback(meshtastic_sent_cb_t cb) { s_sent_cb = cb; }
void meshtastic_node_set_ack_callback(meshtastic_ack_cb_t cb)   { s_ack_cb = cb; }

uint32_t mesh_now(void)
{
    time_t t = time(NULL);
    return t > 1700000000 ? (uint32_t)t : 0;
}

void mesh_set_time(uint32_t unix_time)
{
    if (unix_time < 1700000000 || mesh_now()) return;   /* keep SNTP time if we have it */
    struct timeval tv = { .tv_sec = unix_time };
    settimeofday(&tv, NULL);
    ESP_LOGI(TAG, "clock set from phone: %lu", (unsigned long)unix_time);
}

/* --- Settings --- */

static void settings_defaults(mesh_settings_t *s)
{
    memset(s, 0, sizeof(*s));
    snprintf(s->long_name, sizeof(s->long_name), "TrailCurrent %02x%02x", s_mac[4], s_mac[5]);
    snprintf(s->short_name, sizeof(s->short_name), "%02x%02x", s_mac[4], s_mac[5]);
#if CONFIG_MESH_REGION_ANZ
    s->region = REGION_ANZ;
#else
    s->region = REGION_US;
#endif
    s->modem_preset = 0;
    s->hop_limit = 3;
    s->tx_power = CONFIG_MESH_TX_POWER_DBM;
    s->tx_enabled = true;
    s->node_info_secs = 3 * 3600;
    s->channels[0].role = CHROLE_PRIMARY;
    s->channels[0].psk[0] = 1;
    s->channels[0].psk_len = 1;
}

static void settings_load(void)
{
    settings_defaults(&s_settings);
    nvs_handle_t h;
    if (nvs_open("meshtastic", NVS_READWRITE, &h) != ESP_OK) return;

    uint32_t magic = 0;
    size_t len = sizeof(s_settings);
    mesh_settings_t loaded;
    if (nvs_get_u32(h, "magic", &magic) == ESP_OK && magic == SETTINGS_MAGIC &&
        nvs_get_blob(h, "settings", &loaded, &len) == ESP_OK && len == sizeof(loaded)) {
        s_settings = loaded;
    }
    s_settings.reboot_count++;
    nvs_set_u32(h, "magic", SETTINGS_MAGIC);
    nvs_set_blob(h, "settings", &s_settings, sizeof(s_settings));
    nvs_commit(h);
    nvs_close(h);
}

void mesh_settings_save(void)
{
    nvs_handle_t h;
    if (nvs_open("meshtastic", NVS_READWRITE, &h) != ESP_OK) return;
    mesh_lock();
    nvs_set_blob(h, "settings", &s_settings, sizeof(s_settings));
    mesh_unlock();
    nvs_set_u32(h, "magic", SETTINGS_MAGIC);
    nvs_commit(h);
    nvs_close(h);
}

void mesh_apply_radio_settings(void)
{
    s_retune = true;
}

/* --- Channels and crypto --- */

static int channel_key(int idx, uint8_t key[32])
{
    const mesh_channel_t *c = &s_settings.channels[idx];
    if (c->role == CHROLE_DISABLED) return -1;

    memset(key, 0, 32);
    if (c->psk_len == 0) {
        if (c->role == CHROLE_SECONDARY && idx != 0) return channel_key(0, key);
        return 0;                               /* encryption off */
    }
    if (c->psk_len == 1) {
        if (c->psk[0] == 0) return 0;
        memcpy(key, DEFAULT_PSK, 16);
        key[15] += c->psk[0] - 1;
        return 16;
    }
    memcpy(key, c->psk, c->psk_len);
    return c->psk_len <= 16 ? 16 : 32;
}

static const char *channel_name(int idx)
{
    const char *n = s_settings.channels[idx].name;
    return n[0] ? n : mesh_preset_name(s_settings.modem_preset);
}

static int channel_hash(int idx)
{
    uint8_t key[32];
    int klen = channel_key(idx, key);
    if (klen < 0) return -1;
    uint8_t h = 0;
    for (const char *p = channel_name(idx); *p; p++) h ^= (uint8_t)*p;
    for (int i = 0; i < klen; i++) h ^= key[i];
    return h;
}

static void crypt_ctr(int chan, uint32_t from, uint32_t id, const uint8_t *in, uint8_t *out, size_t len)
{
    uint8_t key[32];
    int klen = channel_key(chan, key);
    if (klen <= 0) {
        memmove(out, in, len);
        return;
    }
    uint8_t nonce[16] = {0}, stream[16];
    memcpy(nonce, &id, 4);          /* packet id as a 64-bit value */
    memcpy(nonce + 8, &from, 4);
    size_t off = 0;
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, klen * 8);
    mbedtls_aes_crypt_ctr(&aes, len, &off, nonce, stream, in, out);
    mbedtls_aes_free(&aes);
}

esp_err_t meshtastic_psk_from_base64(const char *b64, uint8_t psk[32], uint8_t *psk_len)
{
    uint8_t buf[48];
    size_t n = 0;
    if (mbedtls_base64_decode(buf, sizeof(buf), &n, (const unsigned char *)b64, strlen(b64)) != 0)
        return ESP_ERR_INVALID_ARG;
    if (n > 1 && n != 16 && n != 32) return ESP_ERR_INVALID_SIZE;
    memcpy(psk, buf, n);
    *psk_len = n;
    return ESP_OK;
}

void meshtastic_psk_to_base64(const uint8_t *psk, uint8_t psk_len, char *out, size_t out_size)
{
    size_t n = 0;
    if (mbedtls_base64_encode((unsigned char *)out, out_size, &n, psk, psk_len) != 0) n = 0;
    out[n] = 0;
}

bool mesh_parse_data(mesh_packet_t *p)
{
    pbr_t r;
    pbr_field_t f;
    pbr_init(&r, p->data, p->data_len);
    p->portnum = 0;
    p->request_id = 0;
    p->want_response = false;
    p->payload_off = 0;
    p->payload_len = 0;

    while (pbr_next(&r, &f)) {
        switch (f.field) {
        case DATA_PORTNUM:       if (f.wire == 0) p->portnum = f.u; break;
        case DATA_WANT_RESPONSE: if (f.wire == 0) p->want_response = f.u; break;
        case DATA_REQUEST_ID:    if (f.wire == 5) p->request_id = f.u; break;
        case DATA_PAYLOAD:
            if (f.wire == 2) {
                p->payload_off = f.ptr - p->data;
                p->payload_len = f.len;
            }
            break;
        default:
            break;
        }
    }
    return r.p == r.end && p->portnum != 0;
}

/* --- Node database --- */

size_t       mesh_node_count(void)    { return s_node_count; }
mesh_node_t *mesh_node_at(size_t i)   { return i < s_node_count ? &s_nodes[i] : NULL; }

mesh_node_t *mesh_node_find(uint32_t num)
{
    for (size_t i = 0; i < s_node_count; i++)
        if (s_nodes[i].num == num) return &s_nodes[i];
    return NULL;
}

static mesh_node_t *node_get_or_add(uint32_t num)
{
    mesh_node_t *n = mesh_node_find(num);
    if (n) return n;
    if (s_node_count < MESH_MAX_NODES) {
        n = &s_nodes[s_node_count++];
    } else {
        n = &s_nodes[0];                       /* evict the least recently heard */
        for (size_t i = 1; i < s_node_count; i++)
            if (s_nodes[i].last_heard < n->last_heard) n = &s_nodes[i];
    }
    memset(n, 0, sizeof(*n));
    n->num = num;
    n->hops_away = -1;
    n->channel = 0;
    return n;
}

void mesh_node_remove(uint32_t num)
{
    mesh_lock();
    mesh_node_t *n = mesh_node_find(num);
    if (n) {
        *n = s_nodes[--s_node_count];
    }
    mesh_unlock();
}

void mesh_nodedb_reset(void)
{
    mesh_lock();
    s_node_count = 0;
    mesh_unlock();
}

static void node_store_user(mesh_node_t *n, const uint8_t *user, size_t len)
{
    if (len > sizeof(n->user)) return;
    memcpy(n->user, user, len);
    n->user_len = len;
    pbr_t r;
    pbr_field_t f;
    pbr_init(&r, user, len);
    while (pbr_next(&r, &f)) {
        if (f.wire != 2) continue;
        if (f.field == USER_LONG_NAME) pbr_strcpy(&f, n->long_name, sizeof(n->long_name));
        if (f.field == USER_SHORT_NAME) pbr_strcpy(&f, n->short_name, sizeof(n->short_name));
    }
}

size_t mesh_encode_own_user(uint8_t *buf, size_t cap)
{
    char id[12];
    snprintf(id, sizeof(id), "!%08lx", (unsigned long)s_my_num);
    pbw_t w;
    pbw_init(&w, buf, cap);
    mesh_lock();
    pbw_string(&w, USER_ID, id);
    pbw_string(&w, USER_LONG_NAME, s_settings.long_name);
    pbw_string(&w, USER_SHORT_NAME, s_settings.short_name);
    mesh_unlock();
    pbw_bytes(&w, USER_MACADDR, s_mac, 6);
    pbw_varint(&w, USER_HW_MODEL, HW_MODEL_PRIVATE_HW);
    return w.overflow ? 0 : w.len;
}

/* --- Transmit queue --- */

static uint32_t new_packet_id(void)
{
    uint32_t id;
    do { id = esp_random(); } while (id == 0);
    return id;
}

static tx_slot_t *tx_alloc(void)
{
    for (int i = 0; i < TX_SLOTS; i++)
        if (!s_tx[i].used) {
            memset(&s_tx[i], 0, sizeof(s_tx[i]));
            s_tx[i].used = true;
            return &s_tx[i];
        }
    return NULL;
}

static void build_header(uint8_t *h, uint32_t to, uint32_t from, uint32_t id, uint8_t hop_limit,
                         bool want_ack, uint8_t hop_start, uint8_t chan_hash, uint8_t next_hop,
                         uint8_t relay)
{
    memcpy(h, &to, 4);
    memcpy(h + 4, &from, 4);
    memcpy(h + 8, &id, 4);
    h[12] = (hop_limit & 7) | (want_ack ? 0x08 : 0) | ((hop_start & 7) << 5);
    h[13] = chan_hash;
    h[14] = next_hop;
    h[15] = relay;
}

/* Caller holds mesh_lock. */
static esp_err_t queue_originated(uint32_t to, uint8_t chan, const uint8_t *data, size_t len, uint32_t id,
                                  bool want_ack, uint8_t hop_limit, int64_t delay_us, bool notify_phone)
{
    if (len > MESH_MAX_DATA) return ESP_ERR_INVALID_SIZE;
    if (chan >= MESH_MAX_CHANNELS) return ESP_ERR_INVALID_ARG;
    int hash = channel_hash(chan);
    if (hash < 0) return ESP_ERR_INVALID_STATE;
    tx_slot_t *t = tx_alloc();
    if (!t) return ESP_ERR_NO_MEM;

    if (hop_limit == 0 || hop_limit > 7) hop_limit = s_settings.hop_limit;
    build_header(t->buf, to, s_my_num, id, hop_limit, want_ack, hop_limit, hash, 0, s_my_num & 0xFF);
    crypt_ctr(chan, s_my_num, id, data, t->buf + MESH_HEADER_LEN, len);
    t->len = MESH_HEADER_LEN + len;
    t->from = s_my_num;
    t->to = to;
    t->id = id;
    t->channel = chan;
    t->due_us = esp_timer_get_time() + delay_us;
    t->track_ack = want_ack;
    t->sends_left = want_ack ? ACK_TRANSMISSIONS : 1;
    t->notify_phone = notify_phone;

    /* Remember our own id so the echo from a relay isn't treated as new. */
    s_seen[s_seen_next] = (seen_t){ s_my_num, id, esp_timer_get_time() };
    s_seen_next = (s_seen_next + 1) % DEDUPE_SLOTS;
    return ESP_OK;
}

esp_err_t mesh_send_from_local(uint32_t to, uint8_t channel, const uint8_t *data, size_t data_len,
                               uint32_t id, bool want_ack, uint8_t hop_limit, uint32_t *id_out)
{
    if (!id) id = new_packet_id();
    mesh_lock();
    esp_err_t err = s_settings.tx_enabled
        ? queue_originated(to, channel, data, data_len, id, want_ack, hop_limit,
                           (esp_random() % 300) * 1000LL, true)
        : ESP_ERR_INVALID_STATE;
    mesh_unlock();
    if (id_out) *id_out = id;

    /* Let the screen show texts sent from either the screen or the phone. */
    if (err == ESP_OK && s_sent_cb && data_len <= MESH_MAX_DATA) {
        mesh_packet_t *p = calloc(1, sizeof(*p));
        if (p) {
            memcpy(p->data, data, data_len);
            p->data_len = data_len;
            if (mesh_parse_data(p) && p->portnum == PORT_TEXT_MESSAGE)
                s_sent_cb(to, channel, id, (const char *)p->data + p->payload_off, p->payload_len);
            free(p);
        }
    }
    return err;
}

esp_err_t meshtastic_node_send_text(uint32_t to, uint8_t channel, const char *text, uint32_t *id_out)
{
    uint8_t data[MESH_MAX_DATA];
    pbw_t w;
    pbw_init(&w, data, sizeof(data));
    pbw_varint(&w, DATA_PORTNUM, PORT_TEXT_MESSAGE);
    pbw_string(&w, DATA_PAYLOAD, text);
    if (w.overflow) return ESP_ERR_INVALID_SIZE;
    /* want_ack on broadcasts too, like the apps: a relay heard is the "ack". */
    return mesh_send_from_local(to, channel, data, w.len, 0, true, 0, id_out);
}

static void send_nodeinfo(uint32_t to, bool want_response, uint32_t request_id, uint8_t chan)
{
    uint8_t user[128], data[MESH_MAX_DATA];
    size_t ulen = mesh_encode_own_user(user, sizeof(user));
    pbw_t w;
    pbw_init(&w, data, sizeof(data));
    pbw_varint(&w, DATA_PORTNUM, PORT_NODEINFO);
    pbw_bytes(&w, DATA_PAYLOAD, user, ulen);
    if (want_response) pbw_bool(&w, DATA_WANT_RESPONSE, true);
    if (request_id) pbw_fixed32(&w, DATA_REQUEST_ID, request_id);
    mesh_lock();
    queue_originated(to, chan, data, w.len, new_packet_id(), false, 0, (esp_random() % 500) * 1000LL, false);
    mesh_unlock();
}

void mesh_broadcast_nodeinfo(void)
{
    send_nodeinfo(MESH_BROADCAST, false, 0, 0);
}

static void send_routing_ack(uint32_t to, uint32_t request_id, uint8_t chan, uint8_t hop_limit)
{
    uint8_t routing[4], data[32];
    pbw_t r;
    pbw_init(&r, routing, sizeof(routing));
    pbw_varint(&r, ROUTING_ERROR_REASON, ROUTING_ERR_NONE);
    pbw_t w;
    pbw_init(&w, data, sizeof(data));
    pbw_varint(&w, DATA_PORTNUM, PORT_ROUTING);
    pbw_bytes(&w, DATA_PAYLOAD, routing, r.len);
    pbw_fixed32(&w, DATA_REQUEST_ID, request_id);
    mesh_lock();
    queue_originated(to, chan, data, w.len, new_packet_id(), false, hop_limit, 0, false);
    mesh_unlock();
}

/* --- Receive path --- */

static bool seen_before(uint32_t from, uint32_t id)
{
    int64_t now = esp_timer_get_time();
    for (int i = 0; i < DEDUPE_SLOTS; i++) {
        if (s_seen[i].from == from && s_seen[i].id == id && now - s_seen[i].seen_us < 10 * 60 * 1000000LL)
            return true;
    }
    s_seen[s_seen_next] = (seen_t){ from, id, now };
    s_seen_next = (s_seen_next + 1) % DEDUPE_SLOTS;
    return false;
}

/* Finish a want_ack transmission we originated; returns true if it matched. */
static bool complete_tracked(uint32_t id, uint32_t error, uint32_t ack_from)
{
    for (int i = 0; i < TX_SLOTS; i++) {
        tx_slot_t *t = &s_tx[i];
        if (!t->used || !t->track_ack || t->id != id) continue;
        bool notify = t->notify_phone;
        uint8_t chan = t->channel;
        t->used = false;
        mesh_unlock();
        if (notify) phone_deliver_routing(ack_from, id, chan, error);
        if (s_ack_cb) s_ack_cb(id, error);
        mesh_lock();
        return true;
    }
    return false;
}

static int64_t rebroadcast_delay_us(float snr)
{
    /* Weaker links relay sooner, so the edge of the mesh extends coverage first. */
    float frac = (snr + 20.0f) / 35.0f;
    if (frac < 0) frac = 0;
    if (frac > 1) frac = 1;
    uint32_t window_ms = 200 + (uint32_t)(frac * 1800.0f);
    return (100 + esp_random() % window_ms) * 1000LL;
}

static void handle_rx(const uint8_t *raw, int len, float rssi, float snr)
{
    if (len < MESH_HEADER_LEN) return;

    mesh_packet_t *p = calloc(1, sizeof(*p));
    if (!p) return;
    memcpy(&p->to, raw, 4);
    memcpy(&p->from, raw + 4, 4);
    memcpy(&p->id, raw + 8, 4);
    p->hop_limit = raw[12] & 7;
    p->want_ack = raw[12] & 0x08;
    p->via_mqtt = raw[12] & 0x10;
    p->hop_start = raw[12] >> 5;
    p->chan_hash = raw[13];
    p->next_hop = raw[14];
    p->relay_node = raw[15];
    p->rx_snr = snr;
    p->rx_rssi = (int32_t)rssi;
    p->rx_time = mesh_now();
    p->channel = -1;

    ESP_LOGI(TAG, "rx !%08lx -> !%08lx id %08lx hop %u/%u ch 0x%02X len %d rssi %.0f snr %.1f",
             (unsigned long)p->from, (unsigned long)p->to, (unsigned long)p->id, p->hop_limit,
             p->hop_start, p->chan_hash, len, rssi, snr);

    mesh_lock();

    if (p->from == s_my_num) {
        /* Our own packet relayed back: that's an implicit ACK for broadcasts. */
        if (p->relay_node != (s_my_num & 0xFF)) complete_tracked(p->id, ROUTING_ERR_NONE, s_my_num);
        goto done;
    }

    if (seen_before(p->from, p->id)) {
        /* Someone else relayed it first; drop our pending copy. */
        for (int i = 0; i < TX_SLOTS; i++)
            if (s_tx[i].used && s_tx[i].rebroadcast && s_tx[i].from == p->from && s_tx[i].id == p->id)
                s_tx[i].used = false;
        goto done;
    }

    /* Decrypt with whichever channel's hash matches. */
    size_t dlen = len - MESH_HEADER_LEN;
    for (int c = 0; c < MESH_MAX_CHANNELS && p->channel < 0; c++) {
        if (channel_hash(c) != p->chan_hash) continue;
        crypt_ctr(c, p->from, p->id, raw + MESH_HEADER_LEN, p->data, dlen);
        p->data_len = dlen;
        if (mesh_parse_data(p)) p->channel = c;
    }

    /* Managed flood: relay anything not addressed to us, decryptable or not. */
    if (p->to != s_my_num && p->hop_limit > 0 && s_settings.tx_enabled &&
        (p->next_hop == 0 || p->next_hop == (s_my_num & 0xFF))) {
        tx_slot_t *t = tx_alloc();
        if (t) {
            memcpy(t->buf, raw, len);
            t->buf[12] = (raw[12] & ~0x07) | ((p->hop_limit - 1) & 7);
            t->buf[15] = s_my_num & 0xFF;
            t->len = len;
            t->from = p->from;
            t->id = p->id;
            t->rebroadcast = true;
            t->sends_left = 1;
            t->due_us = esp_timer_get_time() + rebroadcast_delay_us(snr);
        }
    }

    if (p->channel < 0) {
        ESP_LOGD(TAG, "not decodable on our channels");
        goto done;
    }

    mesh_node_t *n = node_get_or_add(p->from);
    n->last_heard = p->rx_time ? p->rx_time : (uint32_t)(esp_timer_get_time() / 1000000);
    n->heard_us = esp_timer_get_time();
    n->snr = snr;
    n->channel = p->channel;
    if (p->hop_start >= p->hop_limit && p->hop_start) n->hops_away = p->hop_start - p->hop_limit;

    bool for_us = p->to == s_my_num;
    bool for_phone = for_us || p->to == MESH_BROADCAST;

    switch (p->portnum) {
    case PORT_NODEINFO:
        node_store_user(n, p->data + p->payload_off, p->payload_len);
        ESP_LOGI(TAG, "nodeinfo from !%08lx: %s (%s)", (unsigned long)p->from, n->long_name, n->short_name);
        if (p->want_response && for_phone &&
            esp_timer_get_time() - n->nodeinfo_replied_us > 10 * 60 * 1000000LL) {
            n->nodeinfo_replied_us = esp_timer_get_time();
            mesh_unlock();
            send_nodeinfo(p->from, false, p->id, p->channel);
            mesh_lock();
        }
        break;
    case PORT_ROUTING:
        if (for_us && p->request_id) {
            uint32_t err = ROUTING_ERR_NONE;
            pbr_t r;
            pbr_field_t f;
            pbr_init(&r, p->data + p->payload_off, p->payload_len);
            while (pbr_next(&r, &f))
                if (f.field == ROUTING_ERROR_REASON && f.wire == 0) err = f.u;
            /* Forward the real routing packet below; just stop retrying here. */
            bool ours = false;
            for (int i = 0; i < TX_SLOTS; i++)
                if (s_tx[i].used && s_tx[i].track_ack && s_tx[i].id == p->request_id) {
                    s_tx[i].used = false;
                    ours = true;
                }
            ESP_LOGI(TAG, "routing %s for %08lx from !%08lx", err ? "error" : "ack",
                     (unsigned long)p->request_id, (unsigned long)p->from);
            if (ours && s_ack_cb) {
                mesh_unlock();
                s_ack_cb(p->request_id, err);
                mesh_lock();
            }
        }
        break;
    case PORT_TEXT_MESSAGE:
        if (for_phone && s_text_cb) {
            char name[40];
            snprintf(name, sizeof(name), "%s", n->long_name[0] ? n->long_name : "?");
            mesh_unlock();
            s_text_cb(p->from, name, p->to, p->channel, (const char *)p->data + p->payload_off, p->payload_len);
            mesh_lock();
        }
        ESP_LOGI(TAG, "text from !%08lx: %.*s", (unsigned long)p->from, p->payload_len,
                 (const char *)p->data + p->payload_off);
        break;
    default:
        break;
    }

    if (for_us && p->want_ack && p->portnum != PORT_ROUTING) {
        uint8_t hops = p->hop_start ? p->hop_start : s_settings.hop_limit;
        mesh_unlock();
        send_routing_ack(p->from, p->id, p->channel, hops);
        mesh_lock();
    }

    mesh_unlock();
    if (for_phone) phone_deliver_packet(p);
    free(p);
    return;

done:
    mesh_unlock();
    free(p);
}

/* --- Radio task --- */

void meshtastic_radio_plan(const meshtastic_radio_config_t *cfg, meshtastic_radio_plan_t *out)
{
    int ri = region_index(cfg->region);
    if (ri < 0) ri = 0;
    const meshtastic_region_t *reg = &REGIONS[ri];
    const modem_t *m = modem_get(cfg->preset);

    double slot_mhz = m->bw_khz / 1000.0;
    uint32_t slots = (uint32_t)((reg->end_mhz - reg->start_mhz) / slot_mhz + 0.5);
    if (!slots) slots = 1;
    uint32_t slot;
    if (cfg->slot && cfg->slot <= slots) {
        slot = cfg->slot - 1;
    } else {
        /* djb2 of the primary channel name; a slot past the end falls back to it too, like the firmware. */
        const char *name = cfg->channel_name[0] ? cfg->channel_name : mesh_preset_name(cfg->preset);
        uint32_t h = 5381;
        for (const char *c = name; *c; c++) h = h * 33 + (uint8_t)*c;
        slot = h % slots;
    }
    out->slots = slots;
    out->slot = slot + 1;
    out->freq_hz = (uint32_t)((reg->start_mhz + m->bw_khz / 2000.0 + slot * slot_mhz) * 1e6 + 0.5);

    int8_t max = REGION_POWER[ri] < MODULE_MAX_DBM ? REGION_POWER[ri] : MODULE_MAX_DBM;
    out->max_tx_power = max;
    out->tx_power = (cfg->tx_power <= 0 || cfg->tx_power > max) ? max : cfg->tx_power;
}

void meshtastic_node_get_radio_config(meshtastic_radio_config_t *out)
{
    memset(out, 0, sizeof(*out));
    if (!s_lock) return;
    mesh_lock();
    const mesh_channel_t *ch = &s_settings.channels[0];
    out->region = s_settings.region;
    out->preset = s_settings.modem_preset;
    out->slot = s_settings.channel_num;
    out->hop_limit = s_settings.hop_limit;
    out->tx_power = s_settings.tx_power;
    strlcpy(out->channel_name, ch->name, sizeof(out->channel_name));
    memcpy(out->psk, ch->psk, ch->psk_len);
    out->psk_len = ch->psk_len;
    mesh_unlock();
}

esp_err_t meshtastic_node_set_radio_config(const meshtastic_radio_config_t *cfg)
{
    if (!mesh_region_supported(cfg->region) || !mesh_preset_supported(cfg->preset) ||
        cfg->hop_limit < 1 || cfg->hop_limit > 7 ||
        (cfg->psk_len > 1 && cfg->psk_len != 16 && cfg->psk_len != 32)) {
        ESP_LOGW(TAG, "radio config refused: region %u preset %u hops %u psk %u bytes", cfg->region,
                 cfg->preset, cfg->hop_limit, cfg->psk_len);
        return ESP_ERR_INVALID_ARG;
    }

    meshtastic_radio_plan_t plan;
    meshtastic_radio_plan(cfg, &plan);
    ESP_LOGI(TAG, "radio config: region %u preset %s slot %lu (%s) hops %u power %d channel \"%s\" psk %u bytes",
             cfg->region, mesh_preset_name(cfg->preset), (unsigned long)plan.slot, cfg->slot ? "set" : "auto",
             cfg->hop_limit, plan.tx_power, cfg->channel_name, cfg->psk_len);

    mesh_lock();
    mesh_channel_t *ch = &s_settings.channels[0];
    s_settings.region = cfg->region;
    s_settings.modem_preset = cfg->preset;
    s_settings.channel_num = cfg->slot <= plan.slots ? cfg->slot : 0;
    s_settings.hop_limit = cfg->hop_limit;
    s_settings.tx_power = cfg->tx_power;
    ch->role = CHROLE_PRIMARY;
    strlcpy(ch->name, cfg->channel_name, sizeof(ch->name));
    memset(ch->psk, 0, sizeof(ch->psk));
    memcpy(ch->psk, cfg->psk, cfg->psk_len);
    ch->psk_len = cfg->psk_len;
    mesh_unlock();

    mesh_settings_save();
    mesh_apply_radio_settings();
    /* Queued now, sent after the retune: other nodes learn our name on the new channel. */
    if (s_radio_ok) mesh_broadcast_nodeinfo();
    return ESP_OK;
}

static void configure_radio(void)
{
    meshtastic_radio_config_t cfg;
    meshtastic_radio_plan_t plan;
    meshtastic_node_get_radio_config(&cfg);
    meshtastic_radio_plan(&cfg, &plan);
    const modem_t *mod = modem_get(cfg.preset);

    sx1262_modem_t m = {
        .freq_hz = plan.freq_hz,
        .bw_khz = mod->bw_khz,
        .sf = mod->sf,
        .cr = mod->cr,
        .preamble = 16,
        .sync_word = 0x2B,
        .tx_dbm = plan.tx_power,
        .rx_boosted = true,
    };
    s_freq_hz = m.freq_hz;
    mesh_lock();
    ESP_LOGI(TAG, "preset %s, slot %lu of %lu, primary channel \"%s\" hash 0x%02X",
             mesh_preset_name(cfg.preset), (unsigned long)plan.slot, (unsigned long)plan.slots,
             channel_name(0), channel_hash(0));
    mesh_unlock();

    sx1262_configure(&m);
    sx1262_start_rx();
}

static void service_tx(void)
{
    int64_t now = esp_timer_get_time();
    mesh_lock();
    tx_slot_t *pick = NULL;
    for (int i = 0; i < TX_SLOTS; i++) {
        tx_slot_t *t = &s_tx[i];
        if (!t->used || t->due_us > now) continue;
        if (t->sends_left == 0) {
            /* want_ack ran out of retries */
            uint32_t id = t->id;
            complete_tracked(id, ROUTING_ERR_MAX_RETRANSMIT, s_my_num);
            continue;
        }
        if (!pick || t->due_us < pick->due_us) pick = t;
    }
    if (!pick) {
        mesh_unlock();
        return;
    }
    if (sx1262_receiving()) {
        pick->due_us = now + (100 + esp_random() % 200) * 1000LL;
        mesh_unlock();
        return;
    }

    uint8_t buf[255];
    uint8_t len = pick->len;
    memcpy(buf, pick->buf, len);
    uint32_t id = pick->id;
    if (--pick->sends_left == 0 && !pick->track_ack) {
        pick->used = false;
    } else {
        pick->due_us = now + ACK_TIMEOUT_US;
    }
    mesh_unlock();

    esp_err_t err = sx1262_transmit(buf, len, TX_TIMEOUT_MS);
    ESP_LOGI(TAG, "tx id %08lx len %u: %s", (unsigned long)id, len, esp_err_to_name(err));
    sx1262_start_rx();
}

size_t mesh_tx_free(void)
{
    size_t n = 0;
    mesh_lock();
    for (int i = 0; i < TX_SLOTS; i++) n += !s_tx[i].used;
    mesh_unlock();
    return n;
}

size_t mesh_tx_max(void) { return TX_SLOTS; }

static void mesh_task(void *arg)
{
    uint8_t *buf = malloc(256);
    configure_radio();
    s_next_nodeinfo_us = esp_timer_get_time() + 20 * 1000000LL;

    for (;;) {
        sx1262_wait_irq(pdMS_TO_TICKS(20));

        if (s_retune) {
            s_retune = false;
            configure_radio();
        }

        uint16_t irq = sx1262_get_irq();
        if (irq & SX_IRQ_RX_DONE) {
            float rssi, snr;
            int len = sx1262_read_packet(buf, 256, &rssi, &snr);
            sx1262_clear_irq(0xFFFF);
            s_rx_busy_since = 0;
            if (irq & SX_IRQ_CRC_ERR) ESP_LOGW(TAG, "rx CRC error (rssi %.0f)", rssi);
            else if (len > 0) handle_rx(buf, len, rssi, snr);
        } else if (irq & (SX_IRQ_HEADER_ERR | SX_IRQ_CRC_ERR | SX_IRQ_TIMEOUT)) {
            sx1262_clear_irq(0xFFFF);
            s_rx_busy_since = 0;
        } else if (irq & (SX_IRQ_PREAMBLE | SX_IRQ_HEADER_VALID)) {
            /* A false preamble detect never completes; clear it so TX isn't blocked. */
            int64_t now = esp_timer_get_time();
            if (!s_rx_busy_since) s_rx_busy_since = now;
            else if (now - s_rx_busy_since > RX_STALE_US) {
                sx1262_clear_irq(SX_IRQ_PREAMBLE | SX_IRQ_HEADER_VALID);
                s_rx_busy_since = 0;
            }
        }

        service_tx();

        if (esp_timer_get_time() > s_next_nodeinfo_us) {
            s_next_nodeinfo_us = esp_timer_get_time() + s_settings.node_info_secs * 1000000LL;
            send_nodeinfo(MESH_BROADCAST, true, 0, 0);
        }
    }
}

/* --- Start-up --- */

esp_err_t meshtastic_node_start(void)
{
    s_lock = xSemaphoreCreateRecursiveMutex();
    s_nodes = heap_caps_calloc(MESH_MAX_NODES, sizeof(mesh_node_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!s_nodes) s_nodes = calloc(MESH_MAX_NODES, sizeof(mesh_node_t));
    if (!s_nodes) return ESP_ERR_NO_MEM;

    esp_read_mac(s_mac, ESP_MAC_BASE);
    s_my_num = (uint32_t)s_mac[2] << 24 | s_mac[3] << 16 | s_mac[4] << 8 | s_mac[5];
    settings_load();

    esp_err_t err = sx1262_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "LoRa module not available: %s", esp_err_to_name(err));
        return err;
    }
    s_radio_ok = true;

    phone_init();

    char ble_name[16];
    snprintf(ble_name, sizeof(ble_name), "%s_%02x%02x", s_settings.short_name, s_mac[4], s_mac[5]);
    err = mesh_ble_start(ble_name);
    if (err != ESP_OK) ESP_LOGE(TAG, "BLE start failed: %s (mesh still runs)", esp_err_to_name(err));

    ESP_LOGI(TAG, "node !%08lx \"%s\" (%s), reboot #%lu", (unsigned long)s_my_num, s_settings.long_name,
             s_settings.short_name, (unsigned long)s_settings.reboot_count);

    xTaskCreatePinnedToCore(mesh_task, "mesh", 6144, NULL, 6, NULL, 1);
    return ESP_OK;
}

/* --- Queries for a local UI --- */

static void fill_info(const mesh_node_t *n, meshtastic_node_info_t *o, int64_t now)
{
    memset(o, 0, sizeof(*o));
    o->num = n->num;
    o->has_user = n->user_len > 0;
    strlcpy(o->long_name, n->long_name, sizeof(o->long_name));
    strlcpy(o->short_name, n->short_name, sizeof(o->short_name));
    o->heard_ago_s = n->heard_us ? (uint32_t)((now - n->heard_us) / 1000000) : UINT32_MAX;
    o->snr = n->snr;
    o->hops_away = n->hops_away;
}

static int by_recent(const void *a, const void *b)
{
    uint32_t x = ((const meshtastic_node_info_t *)a)->heard_ago_s;
    uint32_t y = ((const meshtastic_node_info_t *)b)->heard_ago_s;
    return x < y ? -1 : x > y;
}

size_t meshtastic_node_get_nodes(meshtastic_node_info_t *out, size_t max)
{
    if (!s_lock || !s_nodes) return 0;
    int64_t now = esp_timer_get_time();
    mesh_lock();
    size_t total = s_node_count;
    meshtastic_node_info_t *all = malloc((total ? total : 1) * sizeof(*all));
    if (!all) {
        mesh_unlock();
        return 0;
    }
    for (size_t i = 0; i < total; i++) fill_info(&s_nodes[i], &all[i], now);
    mesh_unlock();

    qsort(all, total, sizeof(*all), by_recent);
    size_t n = total < max ? total : max;
    memcpy(out, all, n * sizeof(*all));
    free(all);
    return n;
}

bool meshtastic_node_get_info(uint32_t num, meshtastic_node_info_t *out)
{
    if (!s_lock) return false;
    mesh_lock();
    mesh_node_t *n = s_nodes ? mesh_node_find(num) : NULL;
    if (n) fill_info(n, out, esp_timer_get_time());
    mesh_unlock();
    return n != NULL;
}

void meshtastic_node_get_status(meshtastic_status_t *out)
{
    memset(out, 0, sizeof(*out));
    if (!s_lock) return;
    mesh_lock();
    out->num = s_my_num;
    strlcpy(out->long_name, s_settings.long_name, sizeof(out->long_name));
    strlcpy(out->short_name, s_settings.short_name, sizeof(out->short_name));
    strlcpy(out->channel_name, channel_name(0), sizeof(out->channel_name));
    int ri = region_index(s_settings.region);
    out->region = ri < 0 ? "-" : REGIONS[ri].name;
    out->preset = mesh_preset_name(s_settings.modem_preset);
    out->hop_limit = s_settings.hop_limit;
    out->node_count = s_node_count;
    mesh_unlock();
    meshtastic_radio_config_t cfg;
    meshtastic_radio_plan_t plan;
    meshtastic_node_get_radio_config(&cfg);
    meshtastic_radio_plan(&cfg, &plan);
    out->tx_power = plan.tx_power;
    out->freq_hz = s_freq_hz;
    out->radio_ok = s_radio_ok;
    out->phone_connected = mesh_ble_connected();
    strlcpy(out->ble_name, mesh_ble_name(), sizeof(out->ble_name));
}

esp_err_t meshtastic_node_set_owner(const char *long_name, const char *short_name)
{
    if (!long_name || !long_name[0] || !short_name || !short_name[0]) return ESP_ERR_INVALID_ARG;
    mesh_lock();
    strlcpy(s_settings.long_name, long_name, sizeof(s_settings.long_name));
    strlcpy(s_settings.short_name, short_name, sizeof(s_settings.short_name));
    mesh_unlock();
    mesh_settings_save();
    if (s_radio_ok) mesh_broadcast_nodeinfo();
    return ESP_OK;
}
