/*
 * Messenger UI: conversation store + painter for the EEZ Studio screens.
 *
 * Geometry, fonts and colours all live in the .eez-project. This file only
 * sets text, HIDDEN flags and CHECKED states, so the EEZ Studio canvas stays
 * an exact picture of what the panel shows.
 */

#include "mesh_ui.h"

#if __has_include("ui/screens.h")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "lvgl.h"
#include "meshtastic_node.h"
#include "screens.h"
#include "ui.h"

static const char *TAG = "mesh_ui";

#define NUM_CONV      8      /* rows authored in the Conversations card */
#define NUM_MSG_ROWS  20     /* message rows authored per thread */
#define NUM_NODE_ROWS 40     /* node rows authored on the Nodes page */
#define TEXT_MAX      232

enum { PAGE_MESSAGES, PAGE_NODES, PAGE_SETTINGS };

typedef enum { ST_RECEIVED, ST_SENDING, ST_DELIVERED, ST_FAILED } msg_status_t;

typedef struct {
    uint32_t     from;
    bool         mine;
    uint32_t     id;
    int64_t      at_us;
    msg_status_t status;
    uint8_t      channel;
    char         text[TEXT_MAX];
} msg_t;

/* Conversation 0 is the primary channel; 1.. are direct messages. */
typedef struct {
    bool     used;
    uint32_t peer;
    int64_t  last_us;
    int      unread;
    int      count;
    int      head;              /* index of the newest message */
    msg_t    msgs[NUM_MSG_ROWS];
} conv_t;

static conv_t           *s_conv;
static SemaphoreHandle_t s_lock;
static volatile bool     s_dirty = true;
static int               s_current = 0;
static int               s_page = PAGE_MESSAGES;
static uint32_t          s_node_rows[NUM_NODE_ROWS];

/* --- Exported widget tables --- */

#define CONV_ROW(n) { objects.msg_conv_##n, objects.msg_conv_##n##_name, \
                      objects.msg_conv_##n##_sub, objects.msg_conv_##n##_unread }
#define MSG_ROW(n)  { objects.msg_row_##n, objects.msg_row_##n##_from, \
                      objects.msg_row_##n##_meta, objects.msg_row_##n##_text }
#define NODE_ROW(n) { objects.nodes_row_##n, objects.nodes_row_##n##_short, objects.nodes_row_##n##_name, \
                      objects.nodes_row_##n##_sub, objects.nodes_row_##n##_id, \
                      objects.nodes_row_##n##_heard, objects.nodes_row_##n##_snr }

typedef struct { lv_obj_t *row, *name, *sub, *unread; } conv_row_t;
typedef struct { lv_obj_t *row, *from, *meta, *text; } msg_row_t;
typedef struct { lv_obj_t *row, *badge, *name, *sub, *id, *heard, *snr; } node_row_t;

static conv_row_t s_conv_rows[NUM_CONV];
static msg_row_t  s_msg_rows[NUM_MSG_ROWS];
static node_row_t s_node_row_objs[NUM_NODE_ROWS];

/* The TopBar user widget is instanced on every page; each child exists once per page. */
typedef struct { lv_obj_t *node, *count, *phone, *nav[3]; } topbar_t;
static topbar_t s_topbars[3];

static void bind_widgets(void)
{
    const conv_row_t conv[NUM_CONV] = {
        CONV_ROW(0), CONV_ROW(1), CONV_ROW(2), CONV_ROW(3),
        CONV_ROW(4), CONV_ROW(5), CONV_ROW(6), CONV_ROW(7),
    };
    const msg_row_t msg[NUM_MSG_ROWS] = {
        MSG_ROW(0), MSG_ROW(1), MSG_ROW(2), MSG_ROW(3), MSG_ROW(4),
        MSG_ROW(5), MSG_ROW(6), MSG_ROW(7), MSG_ROW(8), MSG_ROW(9),
        MSG_ROW(10), MSG_ROW(11), MSG_ROW(12), MSG_ROW(13), MSG_ROW(14),
        MSG_ROW(15), MSG_ROW(16), MSG_ROW(17), MSG_ROW(18), MSG_ROW(19),
    };
    const node_row_t node[NUM_NODE_ROWS] = {
        NODE_ROW(0), NODE_ROW(1), NODE_ROW(2), NODE_ROW(3), NODE_ROW(4),
        NODE_ROW(5), NODE_ROW(6), NODE_ROW(7), NODE_ROW(8), NODE_ROW(9),
        NODE_ROW(10), NODE_ROW(11), NODE_ROW(12), NODE_ROW(13), NODE_ROW(14),
        NODE_ROW(15), NODE_ROW(16), NODE_ROW(17), NODE_ROW(18), NODE_ROW(19),
        NODE_ROW(20), NODE_ROW(21), NODE_ROW(22), NODE_ROW(23), NODE_ROW(24),
        NODE_ROW(25), NODE_ROW(26), NODE_ROW(27), NODE_ROW(28), NODE_ROW(29),
        NODE_ROW(30), NODE_ROW(31), NODE_ROW(32), NODE_ROW(33), NODE_ROW(34),
        NODE_ROW(35), NODE_ROW(36), NODE_ROW(37), NODE_ROW(38), NODE_ROW(39),
    };
    const topbar_t bars[3] = {
        { objects.msg_topbar__tb_node, objects.msg_topbar__tb_count, objects.msg_topbar__tb_phone,
          { objects.msg_topbar__tb_nav_messages, objects.msg_topbar__tb_nav_nodes,
            objects.msg_topbar__tb_nav_settings } },
        { objects.nodes_topbar__tb_node, objects.nodes_topbar__tb_count, objects.nodes_topbar__tb_phone,
          { objects.nodes_topbar__tb_nav_messages, objects.nodes_topbar__tb_nav_nodes,
            objects.nodes_topbar__tb_nav_settings } },
        { objects.set_topbar__tb_node, objects.set_topbar__tb_count, objects.set_topbar__tb_phone,
          { objects.set_topbar__tb_nav_messages, objects.set_topbar__tb_nav_nodes,
            objects.set_topbar__tb_nav_settings } },
    };
    memcpy(s_conv_rows, conv, sizeof(conv));
    memcpy(s_msg_rows, msg, sizeof(msg));
    memcpy(s_node_row_objs, node, sizeof(node));
    memcpy(s_topbars, bars, sizeof(bars));
}

/* --- Small helpers --- */

static void set_hidden(lv_obj_t *o, bool hidden)
{
    if (!o) return;
    if (hidden) lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_clear_flag(o, LV_OBJ_FLAG_HIDDEN);
}

static void set_checked(lv_obj_t *o, bool on)
{
    if (!o) return;
    if (on) lv_obj_add_state(o, LV_STATE_CHECKED);
    else lv_obj_clear_state(o, LV_STATE_CHECKED);
}

static void set_text(lv_obj_t *o, const char *s)
{
    if (o && strcmp(lv_label_get_text(o), s) != 0) lv_label_set_text(o, s);
}

static void format_ago(char *buf, size_t n, uint32_t secs)
{
    if (secs == UINT32_MAX) snprintf(buf, n, "-");
    else if (secs < 60) snprintf(buf, n, "just now");
    else if (secs < 3600) snprintf(buf, n, "%lu min ago", (unsigned long)(secs / 60));
    else if (secs < 86400) snprintf(buf, n, "%lu h ago", (unsigned long)(secs / 3600));
    else snprintf(buf, n, "%lu d ago", (unsigned long)(secs / 86400));
}

static void node_name(uint32_t num, char *buf, size_t n)
{
    meshtastic_node_info_t info;
    if (meshtastic_node_get_info(num, &info) && info.long_name[0])
        snprintf(buf, n, "%s", info.long_name);
    else
        snprintf(buf, n, "!%08lx", (unsigned long)num);
}

/* --- Store (any task) --- */

static int conv_for_peer(uint32_t peer, bool create)
{
    if (peer == MESHTASTIC_BROADCAST) return 0;
    int free_slot = -1, oldest = 1;
    for (int i = 1; i < NUM_CONV; i++) {
        if (s_conv[i].used && s_conv[i].peer == peer) return i;
        if (!s_conv[i].used && free_slot < 0) free_slot = i;
        if (s_conv[i].used && s_conv[i].last_us < s_conv[oldest].last_us) oldest = i;
    }
    if (!create) return -1;
    int slot = free_slot >= 0 ? free_slot : oldest;
    if (slot == s_current) s_current = 0;
    memset(&s_conv[slot], 0, sizeof(s_conv[slot]));
    s_conv[slot].used = true;
    s_conv[slot].peer = peer;
    return slot;
}

static void add_message(int c, uint32_t from, bool mine, uint32_t id, msg_status_t st, uint8_t channel,
                        const char *text, size_t len)
{
    conv_t *cv = &s_conv[c];
    cv->head = cv->count ? (cv->head + 1) % NUM_MSG_ROWS : 0;
    if (cv->count < NUM_MSG_ROWS) cv->count++;
    msg_t *m = &cv->msgs[cv->head];
    memset(m, 0, sizeof(*m));
    m->from = from;
    m->mine = mine;
    m->id = id;
    m->status = st;
    m->channel = channel;
    m->at_us = esp_timer_get_time();
    if (len >= sizeof(m->text)) len = sizeof(m->text) - 1;
    memcpy(m->text, text, len);
    cv->last_us = m->at_us;
    if (st == ST_RECEIVED && !(s_page == PAGE_MESSAGES && s_current == c)) cv->unread++;
    s_dirty = true;
}

static void on_text(uint32_t from, const char *from_name, uint32_t to, uint8_t channel,
                    const char *text, size_t len)
{
    (void)from_name;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    int c = to == MESHTASTIC_BROADCAST ? 0 : conv_for_peer(from, true);
    add_message(c, from, false, 0, ST_RECEIVED, channel, text, len);
    xSemaphoreGive(s_lock);
}

static void on_sent(uint32_t to, uint8_t channel, uint32_t id, const char *text, size_t len)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);
    int c = conv_for_peer(to, true);
    add_message(c, 0, true, id, ST_SENDING, channel, text, len);
    xSemaphoreGive(s_lock);
}

static void on_ack(uint32_t id, uint32_t error)
{
    xSemaphoreTake(s_lock, portMAX_DELAY);
    for (int c = 0; c < NUM_CONV; c++)
        for (int i = 0; i < s_conv[c].count; i++)
            if (s_conv[c].msgs[i].id == id && s_conv[c].msgs[i].status == ST_SENDING) {
                s_conv[c].msgs[i].status = error == MESHTASTIC_ACK_OK ? ST_DELIVERED : ST_FAILED;
                s_dirty = true;
            }
    xSemaphoreGive(s_lock);
}

/* --- Painting (LVGL task) --- */

static const char *status_text(const msg_t *m, bool broadcast)
{
    switch (m->status) {
    case ST_SENDING:   return "Sending...";
    case ST_DELIVERED: return broadcast ? "Relayed by the mesh" : "Delivered";
    case ST_FAILED:    return broadcast ? "Sent - no relay heard" : "Not delivered";
    default:           return "";
    }
}

static void paint_conversations(void)
{
    meshtastic_status_t st;
    meshtastic_node_get_status(&st);
    char buf[TEXT_MAX + 16];

    for (int c = 0; c < NUM_CONV; c++) {
        const conv_t *cv = &s_conv[c];
        conv_row_t *r = &s_conv_rows[c];
        set_hidden(r->row, !cv->used);
        if (!cv->used) continue;
        set_checked(r->row, c == s_current);

        if (c == 0) snprintf(buf, sizeof(buf), "%s", st.channel_name[0] ? st.channel_name : "LongFast");
        else node_name(cv->peer, buf, sizeof(buf));
        set_text(r->name, buf);

        if (cv->count) {
            const msg_t *m = &cv->msgs[cv->head];
            snprintf(buf, sizeof(buf), "%s%s", m->mine ? "You: " : "", m->text);
        } else {
            snprintf(buf, sizeof(buf), "%s", c == 0 ? "Everyone on the mesh" : "Direct message");
        }
        set_text(r->sub, buf);

        snprintf(buf, sizeof(buf), "%d", cv->unread > 99 ? 99 : cv->unread);
        set_text(r->unread, buf);
        set_hidden(r->unread, cv->unread == 0);
    }
}

static void paint_thread(void)
{
    const conv_t *cv = &s_conv[s_current];
    char buf[TEXT_MAX + 64], ago[24];
    meshtastic_status_t st;
    meshtastic_node_get_status(&st);
    bool broadcast = s_current == 0;

    if (broadcast) {
        set_text(objects.msg_thread_title, st.channel_name[0] ? st.channel_name : "LongFast");
        set_text(objects.msg_thread_sub, "Everyone on the mesh");
    } else {
        node_name(cv->peer, buf, sizeof(buf));
        set_text(objects.msg_thread_title, buf);
        snprintf(buf, sizeof(buf), "Direct message  !%08lx", (unsigned long)cv->peer);
        set_text(objects.msg_thread_sub, buf);
    }

    int64_t now = esp_timer_get_time();
    for (int i = 0; i < NUM_MSG_ROWS; i++) {
        msg_row_t *r = &s_msg_rows[i];
        bool shown = i < cv->count;
        set_hidden(r->row, !shown);
        if (!shown) continue;
        /* Newest first, right under the compose bar. */
        const msg_t *m = &cv->msgs[(cv->head - i + NUM_MSG_ROWS) % NUM_MSG_ROWS];
        bool mine = m->mine;
        set_checked(r->row, mine);

        if (mine) snprintf(buf, sizeof(buf), "You");
        else node_name(m->from, buf, sizeof(buf));
        set_text(r->from, buf);

        format_ago(ago, sizeof(ago), (uint32_t)((now - m->at_us) / 1000000));
        if (mine) snprintf(buf, sizeof(buf), "%s - %s", ago, status_text(m, broadcast));
        else if (m->channel) snprintf(buf, sizeof(buf), "%s - channel %u", ago, m->channel);
        else snprintf(buf, sizeof(buf), "%s", ago);
        set_text(r->meta, buf);
        set_text(r->text, m->text);
    }
    set_hidden(objects.msg_empty, cv->count > 0);
}

static void paint_nodes(void)
{
    static meshtastic_node_info_t nodes[NUM_NODE_ROWS];
    size_t n = meshtastic_node_get_nodes(nodes, NUM_NODE_ROWS);
    char buf[48];

    meshtastic_status_t st;
    meshtastic_node_get_status(&st);
    snprintf(buf, sizeof(buf), "%u node%s heard", (unsigned)st.node_count, st.node_count == 1 ? "" : "s");
    set_text(objects.nodes_count, buf);
    set_hidden(objects.nodes_empty, n > 0);

    for (int i = 0; i < NUM_NODE_ROWS; i++) {
        node_row_t *r = &s_node_row_objs[i];
        bool shown = (size_t)i < n;
        set_hidden(r->row, !shown);
        if (!shown) {
            s_node_rows[i] = 0;
            continue;
        }
        const meshtastic_node_info_t *ni = &nodes[i];
        s_node_rows[i] = ni->num;

        if (ni->short_name[0]) snprintf(buf, sizeof(buf), "%s", ni->short_name);
        else snprintf(buf, sizeof(buf), "%04lx", (unsigned long)(ni->num & 0xFFFF));
        set_text(r->badge, buf);
        set_text(r->name, ni->long_name[0] ? ni->long_name : "Unknown node");

        if (ni->hops_away < 0) snprintf(buf, sizeof(buf), "%s", ni->has_user ? "" : "Waiting for its name");
        else if (ni->hops_away == 0) snprintf(buf, sizeof(buf), "Heard directly");
        else snprintf(buf, sizeof(buf), "%d hop%s away", ni->hops_away, ni->hops_away == 1 ? "" : "s");
        set_text(r->sub, buf);

        snprintf(buf, sizeof(buf), "!%08lx", (unsigned long)ni->num);
        set_text(r->id, buf);
        format_ago(buf, sizeof(buf), ni->heard_ago_s);
        set_text(r->heard, buf);
        snprintf(buf, sizeof(buf), "%.1f dB", ni->snr);
        set_text(r->snr, buf);
    }
}

/* Radio settings as edited on screen; they reach the node only on Apply. */
static meshtastic_radio_config_t s_radio;

static void paint_radio(void)
{
    meshtastic_radio_plan_t plan;
    meshtastic_radio_plan(&s_radio, &plan);
    char buf[48];
    size_t n;

    snprintf(buf, sizeof(buf), "-");
    const meshtastic_region_t *regions = meshtastic_regions(&n);
    for (size_t i = 0; i < n; i++)
        if (regions[i].code == s_radio.region)
            snprintf(buf, sizeof(buf), "%s  %g-%g MHz", regions[i].name, regions[i].start_mhz, regions[i].end_mhz);
    set_text(objects.set_region, buf);

    const char *preset = "-";
    const meshtastic_preset_t *presets = meshtastic_presets(&n);
    for (size_t i = 0; i < n; i++)
        if (presets[i].code == s_radio.preset) preset = presets[i].name;
    set_text(objects.set_preset, preset);
    /* An unnamed channel is called after its preset, so say which name it will get. */
    if (strcmp(lv_textarea_get_placeholder_text(objects.set_ch_name), preset) != 0)
        lv_textarea_set_placeholder_text(objects.set_ch_name, preset);

    snprintf(buf, sizeof(buf), "%s%lu of %lu", s_radio.slot ? "" : "Auto: ", (unsigned long)plan.slot,
             (unsigned long)plan.slots);
    set_text(objects.set_slot, buf);
    snprintf(buf, sizeof(buf), "%u", s_radio.hop_limit);
    set_text(objects.set_hops, buf);
    if (s_radio.tx_power <= 0 || s_radio.tx_power >= plan.max_tx_power)
        snprintf(buf, sizeof(buf), "Max (%d dBm)", plan.tx_power);
    else
        snprintf(buf, sizeof(buf), "%d dBm", plan.tx_power);
    set_text(objects.set_power, buf);
    snprintf(buf, sizeof(buf), "%.3f MHz", plan.freq_hz / 1e6);
    set_text(objects.set_freq, buf);
}

static void load_radio_settings(void)
{
    char key[48];
    meshtastic_node_get_radio_config(&s_radio);
    meshtastic_psk_to_base64(s_radio.psk, s_radio.psk_len, key, sizeof(key));
    lv_textarea_set_text(objects.set_ch_name, s_radio.channel_name);
    lv_textarea_set_text(objects.set_ch_key, key);
    set_text(objects.set_apply_status, "");
    paint_radio();
}

static void paint_settings(void)
{
    meshtastic_status_t st;
    meshtastic_node_get_status(&st);
    char buf[48];

    snprintf(buf, sizeof(buf), "!%08lx", (unsigned long)st.num);
    set_text(objects.set_node_id, buf);
    paint_radio();
    set_text(objects.set_ble, st.ble_name[0] ? st.ble_name : "-");
    set_text(objects.set_phone, st.phone_connected ? "Connected" : "Not connected");
    set_text(objects.set_radio, st.radio_ok ? "SX1262 detected" : "Not detected - check the module");
}

static void paint_topbars(void)
{
    meshtastic_status_t st;
    meshtastic_node_get_status(&st);
    char node[64], count[24];
    snprintf(node, sizeof(node), "%s  !%08lx", st.long_name, (unsigned long)st.num);
    snprintf(count, sizeof(count), "%u node%s", (unsigned)st.node_count, st.node_count == 1 ? "" : "s");
    for (int b = 0; b < 3; b++) {
        set_text(s_topbars[b].node, node);
        set_text(s_topbars[b].count, count);
        set_hidden(s_topbars[b].phone, !st.phone_connected);
        for (int k = 0; k < 3; k++) set_checked(s_topbars[b].nav[k], k == s_page);
    }
}

static void paint_timer_cb(lv_timer_t *t)
{
    (void)t;
    static int64_t last_slow;
    int64_t now = esp_timer_get_time();
    bool slow = now - last_slow > 1000000;      /* "ago" texts and status: once a second */
    if (slow) last_slow = now;

    if (s_dirty || slow) {
        xSemaphoreTake(s_lock, portMAX_DELAY);
        s_dirty = false;
        if (s_page == PAGE_MESSAGES) s_conv[s_current].unread = 0;
        paint_conversations();
        paint_thread();
        xSemaphoreGive(s_lock);
    }
    if (slow) {
        paint_topbars();
        if (s_page == PAGE_NODES) paint_nodes();
        if (s_page == PAGE_SETTINGS) paint_settings();
    }
}

/* --- Actions (LVGL task) --- */

static lv_obj_t *page_keyboard(int page)
{
    return page == PAGE_SETTINGS ? objects.set_kb : objects.msg_kb;
}

void mesh_ui_navigate(int page)
{
    if (page < PAGE_MESSAGES || page > PAGE_SETTINGS) return;
    set_hidden(objects.msg_kb, true);
    set_hidden(objects.set_kb, true);
    s_page = page;
    if (page == PAGE_SETTINGS) {
        meshtastic_status_t st;
        meshtastic_node_get_status(&st);
        lv_textarea_set_text(objects.set_long_name, st.long_name);
        lv_textarea_set_text(objects.set_short_name, st.short_name);
        set_text(objects.set_save_status, "");
        load_radio_settings();
        paint_settings();
    }
    if (page == PAGE_NODES) paint_nodes();
    s_dirty = true;
    paint_topbars();
    static const enum ScreensEnum ids[] = { SCREEN_ID_PAGE_MESSAGES, SCREEN_ID_PAGE_NODES, SCREEN_ID_PAGE_SETTINGS };
    loadScreen(ids[page]);
}

void mesh_ui_select_conversation(int row)
{
    if (row < 0 || row >= NUM_CONV || !s_conv[row].used) return;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_current = row;
    s_conv[row].unread = 0;
    s_dirty = true;
    xSemaphoreGive(s_lock);
    lv_obj_scroll_to_y(objects.msg_list, 0, LV_ANIM_OFF);
}

void mesh_ui_send(void)
{
    const char *text = lv_textarea_get_text(objects.msg_compose);
    if (!text || !text[0]) return;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    uint32_t to = s_current == 0 ? MESHTASTIC_BROADCAST : s_conv[s_current].peer;
    xSemaphoreGive(s_lock);

    /* on_sent() records the message (it also fires for texts sent from the phone). */
    esp_err_t err = meshtastic_node_send_text(to, 0, text, NULL);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "send failed: %s", esp_err_to_name(err));
        set_text(objects.msg_thread_sub, err == ESP_ERR_INVALID_SIZE ? "Message too long"
                                                                     : "Radio busy or not detected");
        return;
    }
    lv_textarea_set_text(objects.msg_compose, "");
    set_hidden(objects.msg_kb, true);
    lv_obj_scroll_to_y(objects.msg_list, 0, LV_ANIM_OFF);
}

void mesh_ui_show_keyboard(int field)
{
    lv_obj_t *ta = field == 1 ? objects.set_long_name
                 : field == 2 ? objects.set_short_name
                 : field == 3 ? objects.set_ch_name
                 : field == 4 ? objects.set_ch_key
                 : objects.msg_compose;
    lv_obj_t *kb = page_keyboard(field == 0 ? PAGE_MESSAGES : PAGE_SETTINGS);
    lv_keyboard_set_textarea(kb, ta);       /* Trap 26: bind on every path in */
    set_hidden(kb, false);
}

void mesh_ui_keyboard_done(int page, bool ready)
{
    if (page == 0 && ready) {
        mesh_ui_send();
        return;
    }
    set_hidden(page_keyboard(page == 0 ? PAGE_MESSAGES : PAGE_SETTINGS), true);
}

void mesh_ui_message_node(int row)
{
    if (row < 0 || row >= NUM_NODE_ROWS || !s_node_rows[row]) return;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_current = conv_for_peer(s_node_rows[row], true);
    s_conv[s_current].last_us = esp_timer_get_time();
    s_dirty = true;
    xSemaphoreGive(s_lock);
    mesh_ui_navigate(PAGE_MESSAGES);
}

void mesh_ui_save_owner(void)
{
    char long_name[40], short_name[5];
    snprintf(long_name, sizeof(long_name), "%s", lv_textarea_get_text(objects.set_long_name));
    snprintf(short_name, sizeof(short_name), "%s", lv_textarea_get_text(objects.set_short_name));
    if (!long_name[0] || !short_name[0]) {
        set_text(objects.set_save_status, "Both names are needed.");
        return;
    }
    esp_err_t err = meshtastic_node_set_owner(long_name, short_name);
    set_text(objects.set_save_status, err == ESP_OK
             ? "Saved and announced. The Bluetooth name changes after a restart."
             : "Could not save the name.");
    set_hidden(objects.set_kb, true);
    paint_topbars();
}

/* Same row order as STEPPERS in the project generator. */
enum { STEP_REGION, STEP_PRESET, STEP_SLOT, STEP_HOPS, STEP_POWER, NUM_STEPPERS };

void mesh_ui_step_setting(int step)
{
    int row = step / 2, dir = step % 2 ? 1 : -1;
    if (row < 0 || row >= NUM_STEPPERS) return;
    meshtastic_radio_plan_t plan;
    meshtastic_radio_plan(&s_radio, &plan);
    size_t n, i;

    switch (row) {
    case STEP_REGION: {
        const meshtastic_region_t *r = meshtastic_regions(&n);
        for (i = 0; i < n && r[i].code != s_radio.region; i++) {}
        s_radio.region = r[i < n ? (i + n + dir) % n : 0].code;
        break;
    }
    case STEP_PRESET: {
        const meshtastic_preset_t *p = meshtastic_presets(&n);
        for (i = 0; i < n && p[i].code != s_radio.preset; i++) {}
        s_radio.preset = p[i < n ? (i + n + dir) % n : 0].code;
        break;
    }
    case STEP_SLOT: {
        /* 0 (Auto) and then 1..slots, wrapping, so holding a button cycles through them all. */
        uint32_t cur = s_radio.slot <= plan.slots ? s_radio.slot : 0;
        s_radio.slot = (cur + plan.slots + 1 + dir) % (plan.slots + 1);
        break;
    }
    case STEP_HOPS: {
        int h = s_radio.hop_limit + dir;
        s_radio.hop_limit = h < 1 ? 1 : h > 7 ? 7 : h;
        break;
    }
    case STEP_POWER: {
        /* 0 means "the most allowed", which sits at the top of the range. */
        int cur = (s_radio.tx_power <= 0 || s_radio.tx_power >= plan.max_tx_power) ? plan.max_tx_power
                                                                                    : s_radio.tx_power;
        int p = cur + dir;
        s_radio.tx_power = p >= plan.max_tx_power ? 0 : p < 1 ? 1 : p;
        break;
    }
    }

    /* A new region or preset changes the slot count and power limit. */
    meshtastic_radio_plan(&s_radio, &plan);
    if (s_radio.slot > plan.slots) s_radio.slot = 0;
    if (s_radio.tx_power > plan.max_tx_power) s_radio.tx_power = 0;
    paint_radio();
    set_text(objects.set_apply_status, "Not applied yet. Press Apply radio settings to use them.");
}

void mesh_ui_apply_radio(void)
{
    meshtastic_radio_config_t cfg = s_radio;
    snprintf(cfg.channel_name, sizeof(cfg.channel_name), "%s", lv_textarea_get_text(objects.set_ch_name));
    if (meshtastic_psk_from_base64(lv_textarea_get_text(objects.set_ch_key), cfg.psk, &cfg.psk_len) != ESP_OK) {
        set_text(objects.set_apply_status,
                 "The channel key must be base64 of 1, 16 or 32 bytes. AQ== is the default key.");
        return;
    }
    if (meshtastic_node_set_radio_config(&cfg) != ESP_OK) {
        set_text(objects.set_apply_status, "Could not apply these settings.");
        return;
    }
    s_radio = cfg;
    meshtastic_radio_plan_t plan;
    meshtastic_radio_plan(&cfg, &plan);
    char buf[96];
    snprintf(buf, sizeof(buf), "Applied. Now on %.3f MHz%s", plan.freq_hz / 1e6,
             cfg.psk_len ? "." : " with no encryption: anyone can read this channel.");
    set_text(objects.set_apply_status, buf);
    set_hidden(objects.set_kb, true);
    paint_radio();
    paint_topbars();
}

/* --- Init --- */

void mesh_ui_init(void)
{
    s_lock = xSemaphoreCreateMutex();
    s_conv = heap_caps_calloc(NUM_CONV, sizeof(conv_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!s_conv) s_conv = calloc(NUM_CONV, sizeof(conv_t));
    s_conv[0].used = true;
    s_conv[0].peer = MESHTASTIC_BROADCAST;

    bind_widgets();
    meshtastic_node_set_text_callback(on_text);
    meshtastic_node_set_sent_callback(on_sent);
    meshtastic_node_set_ack_callback(on_ack);

    lv_timer_create(paint_timer_cb, 200, NULL);
    paint_topbars();
}

#else
void mesh_ui_init(void) {}
void mesh_ui_navigate(int page) { (void)page; }
void mesh_ui_select_conversation(int row) { (void)row; }
void mesh_ui_send(void) {}
void mesh_ui_show_keyboard(int field) { (void)field; }
void mesh_ui_keyboard_done(int page, bool ready) { (void)page; (void)ready; }
void mesh_ui_message_node(int row) { (void)row; }
void mesh_ui_save_owner(void) {}
void mesh_ui_step_setting(int step) { (void)step; }
void mesh_ui_apply_radio(void) {}
#endif
