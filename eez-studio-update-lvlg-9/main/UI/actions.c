/*
 * actions.c — implements the action handlers EEZ Studio declares in
 * UI/actions.h from the project's actions list.
 *
 * Lives in main/UI/ because the full simulator only compiles what is in the
 * export folder (see dash_data.h for the detail). That is also what makes the
 * simulator worth using: this is the real handler, so pressing the button in
 * the simulator window runs exactly the code the board runs.
 *
 * The __has_include guard keeps the project compiling before the first
 * EEZ Studio export has produced screens.h / actions.h.
 */

#include <stdint.h>
#include <stdio.h>
/* string.h is NOT optional here. On the device esp_log.h drags it in, so a
 * missing include compiles fine and only fails in the simulator, where
 * esp_log.h is excluded -- which reads as "the simulator broke" rather than
 * "this file never declared its own dependency". Include what you use. */
#include <string.h>

#include <lvgl.h>

#include "dash_data.h"
#include "wifi_port.h"
#include "broker_cfg.h"
#include "wifi_ui.h"

/* This translation unit is compiled BOTH for the device and, by the EEZ Studio
 * full simulator, under Emscripten -- the simulator copies the whole export
 * folder into its container and builds every .c it finds. ESP-IDF headers do
 * not exist there, so esp_log.h must never be included unguarded. Same rule
 * dash_data.c states in its own header comment; broken here once already while
 * chasing a device bug, which cost a simulator build. */
#ifdef EEZ_LVGL_SIMULATOR
#  define WIFI_UI_LOG(fmt, ...) ((void)0)
#else
#  include "esp_log.h"
#  define WIFI_UI_LOG(fmt, ...) ESP_LOGI("wifi_ui", fmt, ##__VA_ARGS__)
#endif

#if __has_include("actions.h")

#include "actions.h"
#include "screens.h"
#include "ui.h"

/*
 * Day / night palette swap.
 *
 * Both themes are compiled in: EEZ Studio emits theme_colors[2][N] plus
 * change_color_theme(index), and every style that references a colour token
 * is re-resolved through it. Index 0 is "Default" (daylight), 1 is "Dark".
 *
 * Note what this handler does NOT do: it never touches geometry. Position,
 * size, alignment and font all stay in the .eez-project, so the EEZ Studio
 * canvas and the panel keep showing the same thing.
 */
void action_toggle_theme(lv_event_t *e)
{
    (void)e;

    static bool dark = true;
    dark = !dark;

    change_color_theme(dark ? 1u : 0u);

    /* Swap the button glyph to match. Content, not geometry — allowed. */
    if (objects.btn_theme_icon) {
        lv_label_set_text(objects.btn_theme_icon,
                          dark ? ""    /* moon */
                               : "");  /* sun  */
    }
}


/* ============================================================== wi-fi screen
 *
 * Screen changes are the thing worth pointing at here. flowSupport is off, so
 * there is no flow engine anywhere in this project -- and EEZ Studio still
 * exports loadScreen() into ui.h along with a ScreensEnum. Changing screens is
 * therefore an ordinary C call, as below.
 *
 * What is new is not that this works on the device (it always did) but that
 * you can now watch it work on a laptop: the full simulator compiles this
 * file, so the button does the same thing there. Under the old built-in Run
 * preview none of this code ran at all, which is why screen changes appeared
 * to need flow.
 */

/* Screen state. Declared up here because action_show_wifi(), which is the
 * first handler in the file, resets it. */
static int  selected = -1;
static char selected_text[64] = "NO NETWORK SELECTED";
static int  painted_count = -1;

static void paint_rows(void);

void action_show_wifi(lv_event_t *e)
{
    (void)e;

    /* Bind the keyboard to the password field on the way in, not on row
     * select. Doing it only in action_wifi_select meant that tapping the
     * keyboard before choosing a network typed into nothing at all -- the
     * keys animated, the field stayed empty. Caught in the simulator, which
     * is the whole argument for having one: on the bench this would have
     * looked like a dead touch panel. */
    if (objects.wifi_kb && objects.wifi_password) {
        lv_keyboard_set_textarea(objects.wifi_kb, objects.wifi_password);
    }
    loadScreen(SCREEN_ID_PAGE_WIFI);

    /* Scan on the way in, so the list is filling by the time the screen has
     * finished drawing. The SCAN button re-runs it on demand; nothing scans
     * at boot, because a panel with saved credentials should go straight to
     * the dashboard. */
    selected = -1;
    snprintf(selected_text, sizeof(selected_text), "SCANNING...");
    wifi_port_scan();
    paint_rows();
}

void action_show_dashboard(lv_event_t *e)
{
    (void)e;
    loadScreen(SCREEN_ID_PAGE_DASHBOARD);
}

/* ---- scan list ---------------------------------------------------------- */

/* Row lookup.
 *
 * The .eez-project authors 20 rows; WIFI_MAX_APS in wifi_port.h is the same
 * number, and the static assert below makes a mismatch a compile error rather
 * than a list that silently stops at the wrong row. These are filled at first
 * use because `objects` is not populated until create_screens() has run.
 */
#define WIFI_UI_ROWS 20
_Static_assert(WIFI_UI_ROWS == WIFI_MAX_APS,
               "row count in the .eez-project must match WIFI_MAX_APS");

static lv_obj_t *row_obj(int i)
{
    lv_obj_t *const t[WIFI_UI_ROWS] = {
    objects.wifi_row_0,
    objects.wifi_row_1,
    objects.wifi_row_2,
    objects.wifi_row_3,
    objects.wifi_row_4,
    objects.wifi_row_5,
    objects.wifi_row_6,
    objects.wifi_row_7,
    objects.wifi_row_8,
    objects.wifi_row_9,
    objects.wifi_row_10,
    objects.wifi_row_11,
    objects.wifi_row_12,
    objects.wifi_row_13,
    objects.wifi_row_14,
    objects.wifi_row_15,
    objects.wifi_row_16,
    objects.wifi_row_17,
    objects.wifi_row_18,
    objects.wifi_row_19
    };
    return (i >= 0 && i < WIFI_UI_ROWS) ? t[i] : NULL;
}

static lv_obj_t *ssid_obj(int i)
{
    lv_obj_t *const t[WIFI_UI_ROWS] = {
    objects.wifi_ssid_0,
    objects.wifi_ssid_1,
    objects.wifi_ssid_2,
    objects.wifi_ssid_3,
    objects.wifi_ssid_4,
    objects.wifi_ssid_5,
    objects.wifi_ssid_6,
    objects.wifi_ssid_7,
    objects.wifi_ssid_8,
    objects.wifi_ssid_9,
    objects.wifi_ssid_10,
    objects.wifi_ssid_11,
    objects.wifi_ssid_12,
    objects.wifi_ssid_13,
    objects.wifi_ssid_14,
    objects.wifi_ssid_15,
    objects.wifi_ssid_16,
    objects.wifi_ssid_17,
    objects.wifi_ssid_18,
    objects.wifi_ssid_19
    };
    return (i >= 0 && i < WIFI_UI_ROWS) ? t[i] : NULL;
}

static lv_obj_t *meta_obj(int i)
{
    lv_obj_t *const t[WIFI_UI_ROWS] = {
    objects.wifi_meta_0,
    objects.wifi_meta_1,
    objects.wifi_meta_2,
    objects.wifi_meta_3,
    objects.wifi_meta_4,
    objects.wifi_meta_5,
    objects.wifi_meta_6,
    objects.wifi_meta_7,
    objects.wifi_meta_8,
    objects.wifi_meta_9,
    objects.wifi_meta_10,
    objects.wifi_meta_11,
    objects.wifi_meta_12,
    objects.wifi_meta_13,
    objects.wifi_meta_14,
    objects.wifi_meta_15,
    objects.wifi_meta_16,
    objects.wifi_meta_17,
    objects.wifi_meta_18,
    objects.wifi_meta_19
    };
    return (i >= 0 && i < WIFI_UI_ROWS) ? t[i] : NULL;
}


/* Signal strength as a coarse bar count, because an RSSI in dBm means nothing
 * to most people and the merged icon font has no bar glyphs. */
static const char *bars(int8_t rssi)
{
    if (rssi >= -55) return "....";
    if (rssi >= -67) return "...";
    if (rssi >= -78) return "..";
    return ".";
}

static void paint_rows(void)
{
    const int n = wifi_port_ap_count();

    for (int i = 0; i < WIFI_MAX_APS; i++) {
        lv_obj_t *row = row_obj(i);
        lv_obj_t *ss  = ssid_obj(i);
        lv_obj_t *mt  = meta_obj(i);
        if (!row || !ss || !mt) continue;

        wifi_ap_t ap;
        if (wifi_port_ap(i, &ap)) {
            /* Content and flags only -- never geometry, or the canvas and the
             * panel stop agreeing. */
            lv_label_set_text(ss, ap.ssid);
            char meta[32];
            snprintf(meta, sizeof(meta), "%s %s",
                     wifi_auth_name(ap.auth), bars(ap.rssi));
            lv_label_set_text(mt, meta);
            lv_obj_clear_flag(row, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_label_set_text(ss, "");
            lv_label_set_text(mt, "");
            lv_obj_add_flag(row, LV_OBJ_FLAG_HIDDEN);
        }
        if (i != selected) {
            lv_obj_clear_state(row, LV_STATE_CHECKED);
            lv_obj_clear_state(ss, LV_STATE_CHECKED);
            lv_obj_clear_state(mt, LV_STATE_CHECKED);
        }
    }
    painted_count = n;
    WIFI_UI_LOG("painted %d row(s)", n);
}

void action_wifi_scan(lv_event_t *e)
{
    (void)e;
    selected = -1;
    snprintf(selected_text, sizeof(selected_text), "NO NETWORK SELECTED");
    wifi_port_scan();
    paint_rows();                 /* clears the list while the scan runs */
}

void action_wifi_select(lv_event_t *e)
{
    const int i = (int)(intptr_t)lv_event_get_user_data(e);
    wifi_ap_t ap;
    if (!wifi_port_ap(i, &ap)) return;

    selected = i;
    snprintf(selected_text, sizeof(selected_text), "%s", ap.ssid);

    /* Highlight the chosen row. LV_STATE_CHECKED drives the CHECKED style
     * authored on ScanRow, so the selection is visible at a glance and the
     * password field is unambiguously about THAT network. Children carry the
     * state too, so their text colour flips against the accent fill. */
    for (int r = 0; r < WIFI_UI_ROWS; r++) {
        lv_obj_t *row = row_obj(r);
        lv_obj_t *ss  = ssid_obj(r);
        lv_obj_t *mt  = meta_obj(r);
        if (!row) continue;
        if (r == i) {
            lv_obj_add_state(row, LV_STATE_CHECKED);
            if (ss) lv_obj_add_state(ss, LV_STATE_CHECKED);
            if (mt) lv_obj_add_state(mt, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(row, LV_STATE_CHECKED);
            if (ss) lv_obj_clear_state(ss, LV_STATE_CHECKED);
            if (mt) lv_obj_clear_state(mt, LV_STATE_CHECKED);
        }
    }

    /* Preselect the dropdown to match what was advertised, and point the
     * keyboard at the password field. Both are content operations. */
    if (objects.wifi_security) {
        uint16_t opt = 0;         /* options order: WPA2/WPA3, WPA, WEP, OPEN */
        switch (ap.auth) {
        case TCWIFI_AUTH_WPA2: opt = 0; break;
        case TCWIFI_AUTH_WPA:  opt = 1; break;
        case TCWIFI_AUTH_WEP:  opt = 2; break;
        case TCWIFI_AUTH_OPEN: opt = 3; break;
        }
        lv_dropdown_set_selected(objects.wifi_security, opt);
    }
    if (objects.wifi_password) {
        lv_textarea_set_text(objects.wifi_password, "");
        if (objects.wifi_kb) {
            lv_keyboard_set_textarea(objects.wifi_kb, objects.wifi_password);
        }
    }
}

void action_wifi_connect(lv_event_t *e)
{
    (void)e;
    wifi_ap_t ap;
    if (!wifi_port_ap(selected, &ap)) return;

    wifi_auth_t auth = ap.auth;
    if (objects.wifi_security) {
        switch (lv_dropdown_get_selected(objects.wifi_security)) {
        case 0: auth = TCWIFI_AUTH_WPA2; break;
        case 1: auth = TCWIFI_AUTH_WPA;  break;
        case 2: auth = TCWIFI_AUTH_WEP;  break;
        case 3: auth = TCWIFI_AUTH_OPEN; break;
        default: break;
        }
    }

    const char *pass = objects.wifi_password
                     ? lv_textarea_get_text(objects.wifi_password) : "";
    wifi_port_connect(ap.ssid, auth, pass);
}

void action_wifi_cancel(lv_event_t *e)
{
    (void)e;
    selected = -1;
    snprintf(selected_text, sizeof(selected_text), "NO NETWORK SELECTED");
    if (objects.wifi_password) lv_textarea_set_text(objects.wifi_password, "");
    loadScreen(SCREEN_ID_PAGE_DASHBOARD);
}

/* ============================================================ headwaters ---
 *
 * Broker host, username and password, entered here and stored on the panel.
 * Same shape as the Wi-Fi screen: the UI only reads and writes a small config
 * struct, and knows nothing about NVS or MQTT.
 */

static lv_obj_t *brk_field(int which)
{
    switch (which) {
    case 0:  return objects.brk_host;
    case 1:  return objects.brk_user;
    case 2:  return objects.brk_pass;
    default: return NULL;
    }
}

static void brk_bind_keyboard(int which)
{
    lv_obj_t *f = brk_field(which);
    if (objects.brk_kb && f) {
        lv_keyboard_set_textarea(objects.brk_kb, f);
    }
}

void action_show_broker(lv_event_t *e)
{
    (void)e;

    broker_cfg_t c;
    broker_cfg_get(&c);           /* false just means "nothing saved yet" */
    if (objects.brk_host) lv_textarea_set_text(objects.brk_host, c.host);
    if (objects.brk_user) lv_textarea_set_text(objects.brk_user, c.user);
    /* The stored password is deliberately NOT written back into the field.
     * Showing it, even as bullets, invites someone to save the masked value
     * back over the real one. Blank means "unchanged" on save. */
    if (objects.brk_pass) lv_textarea_set_text(objects.brk_pass, "");

    brk_bind_keyboard(0);
    loadScreen(SCREEN_ID_PAGE_BROKER);
}

void action_broker_field(lv_event_t *e)
{
    brk_bind_keyboard((int)(intptr_t)lv_event_get_user_data(e));
}

void action_broker_save(lv_event_t *e)
{
    (void)e;

    broker_cfg_t cur;
    broker_cfg_get(&cur);         /* for the keep-existing-password case */

    broker_cfg_t c;
    memset(&c, 0, sizeof(c));
    if (objects.brk_host) {
        snprintf(c.host, sizeof(c.host), "%s",
                 lv_textarea_get_text(objects.brk_host));
    }
    if (objects.brk_user) {
        snprintf(c.user, sizeof(c.user), "%s",
                 lv_textarea_get_text(objects.brk_user));
    }
    const char *typed = objects.brk_pass
                      ? lv_textarea_get_text(objects.brk_pass) : "";
    /* Left blank, keep whatever is already stored -- otherwise editing just
     * the host would silently wipe the password. */
    snprintf(c.pass, sizeof(c.pass), "%s", (typed && typed[0]) ? typed : cur.pass);

    if (broker_cfg_set(&c)) {
        broker_cfg_apply();
        loadScreen(SCREEN_ID_PAGE_DASHBOARD);
    }
    /* On failure stay put; the header status says what is missing. */
}

void action_broker_cancel(lv_event_t *e)
{
    (void)e;
    if (objects.brk_pass) lv_textarea_set_text(objects.brk_pass, "");
    loadScreen(SCREEN_ID_PAGE_DASHBOARD);
}

const char *broker_ui_status_text(void)
{
    static char buf[24];
    broker_cfg_status_text(buf, sizeof(buf));
    return buf;
}

/* ---- pump-driven repaint ------------------------------------------------ */

void wifi_ui_tick(uint32_t now_ms)
{
    wifi_port_tick(now_ms);

    if (wifi_port_ap_count() != painted_count) {
        paint_rows();
    }
}

const char *wifi_ui_status_text(void)
{
    /* The port copies its status out under a lock, so keep a local buffer for
     * LVGL to point at -- handing it the port's own storage would be a
     * pointer into state another thread rewrites. */
    static char buf[24];
    wifi_port_status_text(buf, sizeof(buf));
    return buf;
}
const char *wifi_ui_selected_text(void) { return selected_text; }

#else  /* export has not run yet */

typedef int actions_placeholder;

void wifi_ui_tick(uint32_t now_ms) { (void)now_ms; }
const char *wifi_ui_status_text(void)   { return "IDLE"; }
const char *broker_ui_status_text(void) { return "NOT CONFIGURED"; }
const char *wifi_ui_selected_text(void) { return "NO NETWORK SELECTED"; }

#endif /* __has_include("actions.h") */
