/*
 * Meshtastic phone API: the ToRadio/FromRadio protocol the official apps
 * speak over BLE.
 *
 * The app writes ToRadio{want_config_id}; we then answer each FromRadio read
 * with the next item of the config dump (my_info, own node, metadata,
 * channels, config, module config, other nodes, config_complete_id). After
 * that, reads drain a queue of live mesh packets and are empty when idle.
 */

#include <stdio.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "mesh_internal.h"
#include "mesh_pb.h"
#include "mesh_proto.h"

static const char *TAG = "mesh_phone";

#define FIRMWARE_VERSION   "2.7.0.trailcurrent"
#define MIN_APP_VERSION    30200
#define PHONE_QUEUE_LEN    32
#define NUM_CONFIGS        9    /* device .. sessionkey */
#define NUM_MODULE_CONFIGS 13   /* mqtt .. paxcounter */

typedef enum {
    ST_IDLE,
    ST_MY_INFO,
    ST_OWN_NODE,
    ST_METADATA,
    ST_CHANNELS,
    ST_CONFIG,
    ST_MODULE_CONFIG,
    ST_NODES,
    ST_COMPLETE,
    ST_PACKETS,
} phone_state_t;

typedef struct {
    uint16_t len;
    uint8_t  buf[MESH_FROMRADIO_MAX];
} queued_t;

static SemaphoreHandle_t s_plock;
static phone_state_t     s_state;
static uint32_t          s_nonce;
static size_t            s_sub;
static uint32_t          s_fromradio_id;
static uint32_t          s_fromnum;
static queued_t         *s_queue;
static size_t            s_q_head, s_q_count;
static uint8_t           s_passkey[8];
static esp_timer_handle_t s_reboot_timer;

/* --- Queue --- */

static void queue_push(const uint8_t *buf, size_t len)
{
    if (len > MESH_FROMRADIO_MAX) return;
    if (s_q_count == PHONE_QUEUE_LEN) {
        s_q_head = (s_q_head + 1) % PHONE_QUEUE_LEN;    /* drop oldest */
        s_q_count--;
    }
    queued_t *q = &s_queue[(s_q_head + s_q_count) % PHONE_QUEUE_LEN];
    memcpy(q->buf, buf, len);
    q->len = len;
    s_q_count++;
}

static size_t queue_pop(uint8_t *buf, size_t cap)
{
    if (!s_q_count) return 0;
    queued_t *q = &s_queue[s_q_head];
    size_t len = q->len <= cap ? q->len : 0;
    memcpy(buf, q->buf, len);
    s_q_head = (s_q_head + 1) % PHONE_QUEUE_LEN;
    s_q_count--;
    return len;
}

/* Wrap a payload in FromRadio{id, <field>} and hand it to the phone. */
static void fromradio_begin(pbw_t *w, uint8_t *buf, size_t cap)
{
    pbw_init(w, buf, cap);
    pbw_varint(w, FROMRADIO_ID, ++s_fromradio_id);
}

static void push_and_notify(const pbw_t *w)
{
    if (w->overflow) return;
    queue_push(w->buf, w->len);
    if (s_state == ST_PACKETS) mesh_ble_notify_fromnum(++s_fromnum);
}

/* --- Encoders --- */

static void enc_channel(pbw_t *w, uint32_t field, int idx)
{
    mesh_settings_t *s = mesh_settings();
    const mesh_channel_t *c = &s->channels[idx];
    size_t ch = pbw_sub_begin(w, field);
    pbw_varint(w, CH_INDEX, idx);
    size_t st = pbw_sub_begin(w, CH_SETTINGS);
    if (c->psk_len) pbw_bytes(w, CHS_PSK, c->psk, c->psk_len);
    if (c->name[0]) pbw_string(w, CHS_NAME, c->name);
    if (c->uplink) pbw_bool(w, CHS_UPLINK_ENABLED, true);
    if (c->downlink) pbw_bool(w, CHS_DOWNLINK_ENABLED, true);
    size_t ms = pbw_sub_begin(w, CHS_MODULE_SETTINGS);
    if (c->position_precision) pbw_varint(w, 1, c->position_precision);
    pbw_sub_end(w, ms);
    pbw_sub_end(w, st);
    if (c->role) pbw_varint(w, CH_ROLE, c->role);
    pbw_sub_end(w, ch);
}

/* Config oneof member `type` (AdminMessage.ConfigType numbering). */
static void enc_config(pbw_t *w, uint32_t field, int type)
{
    mesh_settings_t *s = mesh_settings();
    size_t cfg = pbw_sub_begin(w, field);
    size_t v = pbw_sub_begin(w, type + 1);
    switch (type) {
    case CFGTYPE_DEVICE:
        pbw_varint(w, DEVCFG_ROLE, DEVROLE_CLIENT);
        pbw_varint(w, DEVCFG_NODE_INFO_BCAST_SECS, s->node_info_secs);
        break;
    case CFGTYPE_LORA:
        pbw_bool(w, LORA_USE_PRESET, true);
        pbw_varint(w, LORA_MODEM_PRESET, s->modem_preset);
        pbw_varint(w, LORA_REGION, s->region);
        pbw_varint(w, LORA_HOP_LIMIT, s->hop_limit);
        pbw_bool(w, LORA_TX_ENABLED, s->tx_enabled);
        pbw_int32(w, LORA_TX_POWER, s->tx_power);
        pbw_varint(w, LORA_CHANNEL_NUM, s->channel_num);
        pbw_bool(w, LORA_RX_BOOSTED, true);
        break;
    case CFGTYPE_BLUETOOTH:
        pbw_bool(w, BT_ENABLED, true);
        pbw_varint(w, BT_MODE, BT_MODE_NO_PIN);
        break;
    default:
        break;      /* position, power, network, display, security: defaults */
    }
    pbw_sub_end(w, v);
    pbw_sub_end(w, cfg);
}

static void enc_module_config(pbw_t *w, uint32_t field, int type)
{
    size_t mc = pbw_sub_begin(w, field);
    size_t v = pbw_sub_begin(w, type + 1);
    pbw_sub_end(w, v);
    pbw_sub_end(w, mc);
}

static void enc_metadata(pbw_t *w, uint32_t field)
{
    size_t m = pbw_sub_begin(w, field);
    pbw_string(w, META_FIRMWARE_VERSION, FIRMWARE_VERSION);
    pbw_varint(w, META_DEVICE_STATE_VERSION, 24);
    pbw_bool(w, META_HAS_BLUETOOTH, true);
    pbw_varint(w, META_ROLE, DEVROLE_CLIENT);
    pbw_varint(w, META_HW_MODEL, HW_MODEL_PRIVATE_HW);
    pbw_sub_end(w, m);
}

static void enc_own_nodeinfo(pbw_t *w)
{
    uint8_t user[128];
    size_t ulen = mesh_encode_own_user(user, sizeof(user));
    size_t ni = pbw_sub_begin(w, FROMRADIO_NODE_INFO);
    pbw_varint(w, NI_NUM, mesh_my_num());
    pbw_bytes(w, NI_USER, user, ulen);
    uint32_t now = mesh_now();
    if (now) pbw_fixed32(w, NI_LAST_HEARD, now);
    pbw_sub_end(w, ni);
}

static void enc_nodeinfo(pbw_t *w, const mesh_node_t *n)
{
    size_t ni = pbw_sub_begin(w, FROMRADIO_NODE_INFO);
    pbw_varint(w, NI_NUM, n->num);
    if (n->user_len) pbw_bytes(w, NI_USER, n->user, n->user_len);
    pbw_float(w, NI_SNR, n->snr);
    if (n->last_heard) pbw_fixed32(w, NI_LAST_HEARD, n->last_heard);
    if (n->channel) pbw_varint(w, NI_CHANNEL, n->channel);
    if (n->hops_away >= 0) pbw_varint(w, NI_HOPS_AWAY, n->hops_away);
    pbw_sub_end(w, ni);
}

static void enc_my_info(pbw_t *w)
{
    const uint8_t *mac = mesh_my_mac();
    uint8_t device_id[16] = {0};
    memcpy(device_id, mac, 6);
    size_t mi = pbw_sub_begin(w, FROMRADIO_MY_INFO);
    pbw_varint(w, MYINFO_NODE_NUM, mesh_my_num());
    pbw_varint(w, MYINFO_REBOOT_COUNT, mesh_settings()->reboot_count);
    pbw_varint(w, MYINFO_MIN_APP_VERSION, MIN_APP_VERSION);
    pbw_bytes(w, MYINFO_DEVICE_ID, device_id, sizeof(device_id));
    pbw_string(w, MYINFO_PIO_ENV, "trailcurrent-crowpanel-p4");
    pbw_varint(w, MYINFO_NODEDB_COUNT, mesh_node_count() + 1);
    pbw_sub_end(w, mi);
}

/* --- Config state machine --- */

static size_t next_config_item(uint8_t *buf, size_t cap)
{
    pbw_t w;
    for (;;) {
        fromradio_begin(&w, buf, cap);
        mesh_lock();
        switch (s_state) {
        case ST_MY_INFO:
            enc_my_info(&w);
            s_state = ST_OWN_NODE;
            break;
        case ST_OWN_NODE:
            enc_own_nodeinfo(&w);
            s_state = s_nonce == NONCE_ONLY_NODES ? ST_NODES : ST_METADATA;
            s_sub = 0;
            break;
        case ST_METADATA:
            enc_metadata(&w, FROMRADIO_METADATA);
            s_state = ST_CHANNELS;
            s_sub = 0;
            break;
        case ST_CHANNELS:
            enc_channel(&w, FROMRADIO_CHANNEL, s_sub);
            if (++s_sub == MESH_MAX_CHANNELS) { s_state = ST_CONFIG; s_sub = 0; }
            break;
        case ST_CONFIG:
            enc_config(&w, FROMRADIO_CONFIG, s_sub);
            if (++s_sub == NUM_CONFIGS) { s_state = ST_MODULE_CONFIG; s_sub = 0; }
            break;
        case ST_MODULE_CONFIG:
            enc_module_config(&w, 9 /* FromRadio.moduleConfig */, s_sub);
            if (++s_sub == NUM_MODULE_CONFIGS) {
                s_state = s_nonce == NONCE_ONLY_CONFIG ? ST_COMPLETE : ST_NODES;
                s_sub = 0;
            }
            break;
        case ST_NODES: {
            mesh_node_t *n = mesh_node_at(s_sub++);
            if (!n) {
                s_state = ST_COMPLETE;
                mesh_unlock();
                continue;
            }
            enc_nodeinfo(&w, n);
            break;
        }
        case ST_COMPLETE:
            pbw_varint(&w, FROMRADIO_CONFIG_COMPLETE_ID, s_nonce);
            s_state = ST_PACKETS;
            ESP_LOGI(TAG, "config sent (nonce %lu), %u nodes", (unsigned long)s_nonce,
                     (unsigned)mesh_node_count());
            break;
        default:
            mesh_unlock();
            return 0;
        }
        mesh_unlock();
        return w.overflow ? 0 : w.len;
    }
}

/* --- Admin --- */

static void deliver_admin_response(uint32_t request_id, uint8_t channel, const uint8_t *admin, size_t admin_len)
{
    uint8_t data[MESH_FROMRADIO_MAX];
    pbw_t d;
    pbw_init(&d, data, sizeof(data));
    pbw_varint(&d, DATA_PORTNUM, PORT_ADMIN);
    pbw_bytes(&d, DATA_PAYLOAD, admin, admin_len);
    pbw_fixed32(&d, DATA_REQUEST_ID, request_id);

    uint8_t buf[MESH_FROMRADIO_MAX];
    pbw_t w;
    xSemaphoreTake(s_plock, portMAX_DELAY);
    fromradio_begin(&w, buf, sizeof(buf));
    size_t mp = pbw_sub_begin(&w, FROMRADIO_PACKET);
    pbw_fixed32(&w, MP_FROM, mesh_my_num());
    pbw_fixed32(&w, MP_TO, mesh_my_num());
    if (channel) pbw_varint(&w, MP_CHANNEL, channel);
    pbw_bytes(&w, MP_DECODED, d.buf, d.len);
    pbw_fixed32(&w, MP_ID, esp_random());
    pbw_sub_end(&w, mp);
    push_and_notify(&w);
    xSemaphoreGive(s_plock);
}

static void parse_user_into_settings(const uint8_t *buf, size_t len)
{
    mesh_settings_t *s = mesh_settings();
    pbr_t r;
    pbr_field_t f;
    pbr_init(&r, buf, len);
    while (pbr_next(&r, &f)) {
        if (f.wire != 2) continue;
        if (f.field == USER_LONG_NAME && f.len) pbr_strcpy(&f, s->long_name, sizeof(s->long_name));
        if (f.field == USER_SHORT_NAME && f.len) pbr_strcpy(&f, s->short_name, sizeof(s->short_name));
    }
}

static void parse_channel_into_settings(const uint8_t *buf, size_t len)
{
    mesh_channel_t c = {0};
    int idx = 0;
    pbr_t r, rs, rm;
    pbr_field_t f, fs, fm;
    pbr_init(&r, buf, len);
    while (pbr_next(&r, &f)) {
        if (f.field == CH_INDEX && f.wire == 0) idx = (int)f.u;
        if (f.field == CH_ROLE && f.wire == 0) c.role = f.u;
        if (f.field == CH_SETTINGS && f.wire == 2) {
            pbr_init(&rs, f.ptr, f.len);
            while (pbr_next(&rs, &fs)) {
                if (fs.field == CHS_PSK && fs.wire == 2 && fs.len <= sizeof(c.psk)) {
                    memcpy(c.psk, fs.ptr, fs.len);
                    c.psk_len = fs.len;
                }
                if (fs.field == CHS_NAME && fs.wire == 2) pbr_strcpy(&fs, c.name, sizeof(c.name));
                if (fs.field == CHS_UPLINK_ENABLED && fs.wire == 0) c.uplink = fs.u;
                if (fs.field == CHS_DOWNLINK_ENABLED && fs.wire == 0) c.downlink = fs.u;
                if (fs.field == CHS_MODULE_SETTINGS && fs.wire == 2) {
                    pbr_init(&rm, fs.ptr, fs.len);
                    while (pbr_next(&rm, &fm))
                        if (fm.field == 1 && fm.wire == 0) c.position_precision = fm.u;
                }
            }
        }
    }
    if (idx < 0 || idx >= MESH_MAX_CHANNELS) return;
    if (idx == 0) c.role = CHROLE_PRIMARY;
    mesh_settings()->channels[idx] = c;
    ESP_LOGI(TAG, "channel %d set: role %u name \"%s\" psk %u bytes", idx, c.role, c.name, c.psk_len);
}

static void parse_config_into_settings(const uint8_t *buf, size_t len)
{
    mesh_settings_t *s = mesh_settings();
    pbr_t r, rv;
    pbr_field_t f, fv;
    pbr_init(&r, buf, len);
    while (pbr_next(&r, &f)) {
        if (f.wire != 2) continue;
        pbr_init(&rv, f.ptr, f.len);
        if (f.field == CFG_LORA) {
            /* proto3: absent fields are zero, so start from zero like the firmware does */
            uint32_t region = 0, preset = 0, hops = 0, chnum = 0;
            int32_t power = 0;
            bool tx = false;
            while (pbr_next(&rv, &fv)) {
                if (fv.wire != 0) continue;
                switch (fv.field) {
                case LORA_REGION:       region = fv.u; break;
                case LORA_MODEM_PRESET: preset = fv.u; break;
                case LORA_HOP_LIMIT:    hops = fv.u; break;
                case LORA_TX_ENABLED:   tx = fv.u; break;
                case LORA_TX_POWER:     power = (int32_t)fv.u; break;
                case LORA_CHANNEL_NUM:  chnum = fv.u; break;
                }
            }
            if (mesh_region_supported(region)) s->region = region;
            else ESP_LOGW(TAG, "region %lu not supported by this 915 MHz module; keeping %u",
                          (unsigned long)region, s->region);
            if (mesh_preset_supported(preset)) s->modem_preset = preset;
            else ESP_LOGW(TAG, "modem preset %lu not offered in 915 MHz regions; keeping %s",
                          (unsigned long)preset, mesh_preset_name(s->modem_preset));
            s->hop_limit = hops ? (hops > 7 ? 7 : hops) : 3;
            s->tx_enabled = tx;
            s->tx_power = power;
            s->channel_num = chnum;
            ESP_LOGI(TAG, "lora config: region %u preset %s hops %u tx %d power %d slot %lu", s->region,
                     mesh_preset_name(s->modem_preset), s->hop_limit, s->tx_enabled, s->tx_power,
                     (unsigned long)s->channel_num);
        } else if (f.field == CFG_DEVICE) {
            while (pbr_next(&rv, &fv))
                if (fv.field == DEVCFG_NODE_INFO_BCAST_SECS && fv.wire == 0 && fv.u >= 3600)
                    s->node_info_secs = fv.u;
        }
    }
}

static void reboot_cb(void *arg) { esp_restart(); }

static void handle_admin(const uint8_t *payload, size_t len, uint32_t request_id, uint8_t channel)
{
    uint8_t resp[MESH_FROMRADIO_MAX - 64];
    pbw_t w;
    pbw_init(&w, resp, sizeof(resp));
    bool respond = false, save = false, retune = false, nodeinfo = false;

    pbr_t r;
    pbr_field_t f;
    pbr_init(&r, payload, len);
    mesh_lock();
    while (pbr_next(&r, &f)) {
        switch (f.field) {
        case ADM_GET_CHANNEL_REQ:
            if (f.u >= 1 && f.u <= MESH_MAX_CHANNELS) {
                enc_channel(&w, ADM_GET_CHANNEL_RESP, f.u - 1);
                respond = true;
            }
            break;
        case ADM_GET_OWNER_REQ: {
            uint8_t user[128];
            size_t ulen = mesh_encode_own_user(user, sizeof(user));
            pbw_bytes(&w, ADM_GET_OWNER_RESP, user, ulen);
            respond = true;
            break;
        }
        case ADM_GET_CONFIG_REQ:
            if (f.u < NUM_CONFIGS) {
                enc_config(&w, ADM_GET_CONFIG_RESP, f.u);
                respond = true;
            }
            break;
        case ADM_GET_MODCFG_REQ:
            if (f.u < NUM_MODULE_CONFIGS) {
                enc_module_config(&w, ADM_GET_MODCFG_RESP, f.u);
                respond = true;
            }
            break;
        case ADM_GET_METADATA_REQ:
            enc_metadata(&w, ADM_GET_METADATA_RESP);
            respond = true;
            break;
        case ADM_SET_OWNER:
            if (f.wire == 2) {
                parse_user_into_settings(f.ptr, f.len);
                save = nodeinfo = true;
            }
            break;
        case ADM_SET_CHANNEL:
            if (f.wire == 2) {
                parse_channel_into_settings(f.ptr, f.len);
                save = retune = true;
            }
            break;
        case ADM_SET_CONFIG:
            if (f.wire == 2) {
                parse_config_into_settings(f.ptr, f.len);
                save = retune = true;
            }
            break;
        case ADM_SET_TIME_ONLY:
            mesh_set_time((uint32_t)f.u);
            break;
        case ADM_REMOVE_BY_NODENUM:
            mesh_node_remove((uint32_t)f.u);
            break;
        case ADM_NODEDB_RESET:
            mesh_nodedb_reset();
            break;
        case ADM_COMMIT_EDIT:
            save = retune = true;
            break;
        case ADM_REBOOT_SECONDS: {
            int32_t secs = (int32_t)f.u;
            if (secs >= 0) {
                ESP_LOGW(TAG, "reboot requested in %ld s", (long)secs);
                esp_timer_create_args_t a = { .callback = reboot_cb, .name = "mesh_reboot" };
                if (!s_reboot_timer) esp_timer_create(&a, &s_reboot_timer);
                esp_timer_start_once(s_reboot_timer, (uint64_t)(secs ? secs : 1) * 1000000ULL);
            }
            break;
        }
        default:
            ESP_LOGD(TAG, "admin field %lu ignored", (unsigned long)f.field);
            break;
        }
    }
    mesh_unlock();

    if (save) mesh_settings_save();
    if (retune) mesh_apply_radio_settings();
    if (nodeinfo) mesh_broadcast_nodeinfo();
    if (respond) {
        pbw_bytes(&w, ADM_SESSION_PASSKEY, s_passkey, sizeof(s_passkey));
        if (!w.overflow) deliver_admin_response(request_id, channel, w.buf, w.len);
    }
}

/* --- ToRadio --- */

static void handle_phone_packet(const uint8_t *buf, size_t len)
{
    uint32_t to = 0, id = 0, channel = 0, hop_limit = 0;
    bool want_ack = false;
    const uint8_t *data = NULL;
    size_t data_len = 0;

    pbr_t r;
    pbr_field_t f;
    pbr_init(&r, buf, len);
    while (pbr_next(&r, &f)) {
        switch (f.field) {
        case MP_TO:        to = f.u; break;
        case MP_CHANNEL:   channel = f.u; break;
        case MP_ID:        id = f.u; break;
        case MP_HOP_LIMIT: hop_limit = f.u; break;
        case MP_WANT_ACK:  want_ack = f.u; break;
        case MP_DECODED:
            if (f.wire == 2) { data = f.ptr; data_len = f.len; }
            break;
        default:
            break;
        }
    }
    if (!data) {
        ESP_LOGW(TAG, "phone packet without decoded payload ignored");
        return;
    }

    mesh_packet_t *p = calloc(1, sizeof(*p));
    if (!p) return;
    if (data_len <= sizeof(p->data)) {
        memcpy(p->data, data, data_len);
        p->data_len = data_len;
        mesh_parse_data(p);
    }

    if (to == mesh_my_num() || to == 0) {
        if (p->portnum == PORT_ADMIN)
            handle_admin(p->data + p->payload_off, p->payload_len, id, channel);
        free(p);
        return;
    }
    free(p);

    uint32_t sent_id = 0;
    esp_err_t err = mesh_send_from_local(to, channel, data, data_len, id, want_ack, hop_limit, &sent_id);
    ESP_LOGI(TAG, "phone -> !%08lx ch %lu id %08lx ack %d: %s", (unsigned long)to, (unsigned long)channel,
             (unsigned long)sent_id, want_ack, esp_err_to_name(err));
    if (err != ESP_OK) phone_deliver_routing(mesh_my_num(), sent_id, channel, ROUTING_ERR_NO_ROUTE);
    phone_deliver_queue_status(err == ESP_OK ? 0 : -1, sent_id);
}

void phone_handle_toradio(const uint8_t *buf, size_t len)
{
    pbr_t r;
    pbr_field_t f;
    pbr_init(&r, buf, len);
    while (pbr_next(&r, &f)) {
        switch (f.field) {
        case TORADIO_WANT_CONFIG_ID:
            xSemaphoreTake(s_plock, portMAX_DELAY);
            s_nonce = f.u;
            s_state = f.u == NONCE_ONLY_NODES ? ST_OWN_NODE : ST_MY_INFO;
            s_sub = 0;
            xSemaphoreGive(s_plock);
            ESP_LOGI(TAG, "want_config %lu", (unsigned long)f.u);
            break;
        case TORADIO_PACKET:
            if (f.wire == 2) handle_phone_packet(f.ptr, f.len);
            break;
        case TORADIO_DISCONNECT:
            phone_disconnected();
            break;
        case TORADIO_HEARTBEAT:
            break;
        default:
            ESP_LOGD(TAG, "ToRadio field %lu ignored", (unsigned long)f.field);
            break;
        }
    }
}

size_t phone_get_fromradio(uint8_t *buf, size_t cap)
{
    xSemaphoreTake(s_plock, portMAX_DELAY);
    size_t n = 0;
    if (s_state != ST_IDLE && s_state != ST_PACKETS) n = next_config_item(buf, cap);
    else if (s_state == ST_PACKETS) n = queue_pop(buf, cap);
    xSemaphoreGive(s_plock);
    return n;
}

/* --- Delivery from the mesh --- */

void phone_deliver_packet(const mesh_packet_t *p)
{
    uint8_t buf[MESH_FROMRADIO_MAX];
    xSemaphoreTake(s_plock, portMAX_DELAY);
    if (s_state == ST_IDLE) {
        xSemaphoreGive(s_plock);
        return;
    }
    pbw_t w;
    fromradio_begin(&w, buf, sizeof(buf));
    size_t mp = pbw_sub_begin(&w, FROMRADIO_PACKET);
    pbw_fixed32(&w, MP_FROM, p->from);
    pbw_fixed32(&w, MP_TO, p->to);
    if (p->channel > 0) pbw_varint(&w, MP_CHANNEL, p->channel);
    pbw_bytes(&w, MP_DECODED, p->data, p->data_len);
    pbw_fixed32(&w, MP_ID, p->id);
    if (p->rx_time) pbw_fixed32(&w, MP_RX_TIME, p->rx_time);
    pbw_float(&w, MP_RX_SNR, p->rx_snr);
    pbw_varint(&w, MP_HOP_LIMIT, p->hop_limit);
    if (p->want_ack) pbw_bool(&w, MP_WANT_ACK, true);
    pbw_int32(&w, MP_RX_RSSI, p->rx_rssi);
    if (p->hop_start) pbw_varint(&w, MP_HOP_START, p->hop_start);
    if (p->next_hop) pbw_varint(&w, MP_NEXT_HOP, p->next_hop);
    if (p->relay_node) pbw_varint(&w, MP_RELAY_NODE, p->relay_node);
    pbw_sub_end(&w, mp);
    push_and_notify(&w);
    xSemaphoreGive(s_plock);
}

void phone_deliver_routing(uint32_t from, uint32_t request_id, uint8_t channel, uint32_t error)
{
    uint8_t routing[8], data[32], buf[96];
    pbw_t r, d, w;
    pbw_init(&r, routing, sizeof(routing));
    pbw_varint(&r, ROUTING_ERROR_REASON, error);
    pbw_init(&d, data, sizeof(data));
    pbw_varint(&d, DATA_PORTNUM, PORT_ROUTING);
    pbw_bytes(&d, DATA_PAYLOAD, routing, r.len);
    pbw_fixed32(&d, DATA_REQUEST_ID, request_id);

    xSemaphoreTake(s_plock, portMAX_DELAY);
    if (s_state != ST_IDLE) {
        fromradio_begin(&w, buf, sizeof(buf));
        size_t mp = pbw_sub_begin(&w, FROMRADIO_PACKET);
        pbw_fixed32(&w, MP_FROM, from);
        pbw_fixed32(&w, MP_TO, mesh_my_num());
        if (channel) pbw_varint(&w, MP_CHANNEL, channel);
        pbw_bytes(&w, MP_DECODED, d.buf, d.len);
        pbw_fixed32(&w, MP_ID, esp_random());
        pbw_sub_end(&w, mp);
        push_and_notify(&w);
    }
    xSemaphoreGive(s_plock);
    ESP_LOGI(TAG, "routing %s for %08lx -> phone", error ? "error" : "ack", (unsigned long)request_id);
}

void phone_deliver_queue_status(int32_t res, uint32_t mesh_packet_id)
{
    uint8_t buf[48];
    xSemaphoreTake(s_plock, portMAX_DELAY);
    if (s_state != ST_IDLE) {
        pbw_t w;
        fromradio_begin(&w, buf, sizeof(buf));
        size_t qs = pbw_sub_begin(&w, FROMRADIO_QUEUE_STATUS);
        if (res) pbw_int32(&w, QS_RES, res);
        pbw_varint(&w, QS_FREE, mesh_tx_free());
        pbw_varint(&w, QS_MAXLEN, mesh_tx_max());
        pbw_varint(&w, QS_MESH_PACKET_ID, mesh_packet_id);
        pbw_sub_end(&w, qs);
        push_and_notify(&w);
    }
    xSemaphoreGive(s_plock);
}

/* --- Connection lifecycle --- */

void phone_init(void)
{
    s_plock = xSemaphoreCreateMutex();
    s_queue = heap_caps_calloc(PHONE_QUEUE_LEN, sizeof(queued_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!s_queue) s_queue = calloc(PHONE_QUEUE_LEN, sizeof(queued_t));
    esp_fill_random(s_passkey, sizeof(s_passkey));
}

void phone_connected(void)
{
    xSemaphoreTake(s_plock, portMAX_DELAY);
    s_state = ST_IDLE;
    s_q_head = s_q_count = 0;
    xSemaphoreGive(s_plock);
}

void phone_disconnected(void)
{
    xSemaphoreTake(s_plock, portMAX_DELAY);
    s_state = ST_IDLE;
    s_q_head = s_q_count = 0;
    xSemaphoreGive(s_plock);
}

bool phone_is_streaming(void)
{
    return s_state == ST_PACKETS;
}
