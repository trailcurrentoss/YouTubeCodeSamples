/*
 * The Play and Unlocked screens, painted from g_game through ui_lv.h's
 * change-only setters -- an unchanged value is a no-op, so a screen redraws
 * only when something on it actually changes.
 *
 * The dial's rotation is set here too, like Tiercel's clock hands: a live
 * value on a widget whose authored rotation (270, the dial at 0) is what the
 * canvas shows. It is not an EEZ expression binding because EEZ 0.29 reads a
 * bound Scale rotation back with lv_scale_get_rotation(), which LVGL 9.2.2
 * does not have.
 */

#include "ui_game.h"

#include <stdint.h>
#include <stdio.h>

#include "app.h"
#include "bt_link.h"
#include "screens.h"
#include "sound.h"
#include "ui_lv.h"

static const lv_state_t DONE  = LV_STATE_CHECKED;
static const lv_state_t WRONG = LV_STATE_DISABLED;
static const lv_state_t CURRENT = LV_STATE_PRESSED;

/* ---- Play ------------------------------------------------------------------ */

#define POLICE_SWAP_MS 350

/* Turn the numbered ring and the knob's grip ring with the knob. A Scale's
 * rotation is where its first tick (0) sits; 270 = 12 o'clock, and each
 * detent is one position, 9 degrees. */
static void set_dial_rotation(void)
{
    static int32_t s_shown = -1;
    int32_t r = (270 + g_game.ticks * GAME_DEG_PER_POS) % 360;
    if (r < 0) {
        r += 360;
    }
    if (r != s_shown) {
        lv_scale_set_rotation(objects.dial_ticks, r);
        lv_scale_set_rotation(objects.dial_grip, r);
        s_shown = r;
    }
}

static void refresh_play(void)
{
    const bool wrong = g_game.mode == GAME_WRONG;
    const int  dir = game_stage_dir(&g_game);
    char buf[16];

    set_dial_rotation();

    /* The countdown, rounded UP so "0:00" only shows once time is out, and
     * red for the last ten seconds as well as in the red hold. */
    const uint32_t left_ms = game_time_left_ms(&g_game, app_now_ms());
    const uint32_t left_s = (left_ms + 999) / 1000;
    snprintf(buf, sizeof(buf), "%lu:%02lu", (unsigned long)(left_s / 60),
             (unsigned long)(left_s % 60));
    ui_lv_set_text(objects.timer_text, buf);
    ui_lv_set_state_in(objects.timer_text, WRONG, (wrong || left_s <= 10) ? WRONG : 0);

    snprintf(buf, sizeof(buf), "Level %u", g_game.level ? g_game.level : 1);
    ui_lv_set_text(objects.level_text, buf);
    ui_lv_set_text(objects.hint_text,
                   wrong ? (g_game.timed_out ? "Time's Up" : "Wrong")
                         : dir > 0 ? "Turn Right" : "Turn Left");
    ui_lv_set_hidden(objects.hint_arrow_cw, wrong || dir < 0);
    ui_lv_set_hidden(objects.hint_arrow_ccw, wrong || dir > 0);

    ui_lv_set_state_in(objects.hint_text, WRONG, wrong ? WRONG : 0);
    ui_lv_set_state_in(objects.dial_index, WRONG, wrong ? WRONG : 0);

    lv_obj_t *const seg[3]     = { objects.seg1, objects.seg2, objects.seg3 };
    lv_obj_t *const seg_cur[3] = { objects.seg1_cur, objects.seg2_cur, objects.seg3_cur };
    lv_obj_t *const slot[3]    = { objects.slot1, objects.slot2, objects.slot3 };
    lv_obj_t *const line[3]    = { objects.slot1_line, objects.slot2_line, objects.slot3_line };
    lv_obj_t *const cur[3]     = { objects.slot1_cur, objects.slot2_cur, objects.slot3_cur };
    for (int k = 0; k < 3; k++) {
        const bool done = k < g_game.locked;
        const bool current = g_game.mode == GAME_PLAY && k == g_game.stage;
        const lv_state_t st = wrong ? WRONG : done ? DONE : 0;
        ui_lv_set_state_in(seg[k], DONE | WRONG, st);
        /* The slot being searched is white in the design: PRESSED, borrowed
         * as a selector on an inert label (SlotText). */
        ui_lv_set_state_in(slot[k], DONE | WRONG | CURRENT,
                           st ? st : current ? CURRENT : 0);
        ui_lv_set_state_in(line[k], DONE | WRONG, st);
        ui_lv_set_hidden(seg_cur[k], !current);
        ui_lv_set_hidden(cur[k], !current);
        if (done) {
            snprintf(buf, sizeof(buf), "%02u", g_game.code[k]);
            ui_lv_set_text(slot[k], buf);
        } else {
            ui_lv_set_text(slot[k], "--");
        }
    }

    /* The police are coming: red and blue light swapping over the dial for
     * the whole "wrong" hold. It swaps, never blinks to dark, and stays under
     * three changes a second -- big saturated flashes faster than that are a
     * photosensitivity risk. */
    const uint32_t t = app_now_ms() - g_game.mode_since_ms;
    const bool red = (t / POLICE_SWAP_MS) % 2 == 0;
    ui_lv_set_hidden(objects.police_red, !(wrong && red));
    ui_lv_set_hidden(objects.police_blue, !(wrong && !red));
}

/* ---- Unlocked ---------------------------------------------------------------- */

static lv_obj_t *door_frame(int i)
{
    lv_obj_t *const frames[UNLOCK_DOOR_FRAMES] = {
        objects.door_00, objects.door_01, objects.door_02, objects.door_03,
        objects.door_04, objects.door_05, objects.door_06, objects.door_07,
        objects.door_08, objects.door_09, objects.door_10, objects.door_11,
        objects.door_12, objects.door_13, objects.door_14, objects.door_15,
    };
    return frames[i];
}

/* The glow's brightness steps, as GoldGlow style states: DISABLED (dim),
 * CHECKED (mid), DEFAULT (full). It fades in dim-mid-full, then breathes
 * full-mid-dim-mid every GLOW_STEP_MS. */
#define GLOW_STEP_MS 260

static lv_state_t glow_state(uint32_t since_glow)
{
    const uint32_t step = since_glow / GLOW_STEP_MS;
    if (step < 2) {
        return step == 0 ? LV_STATE_DISABLED : LV_STATE_CHECKED;
    }
    static const lv_state_t breathe[4] = { 0, LV_STATE_CHECKED, LV_STATE_DISABLED,
                                           LV_STATE_CHECKED };
    return breathe[(step - 2) % 4];
}

static lv_obj_t *prize(int level)
{
    lv_obj_t *const prizes[GAME_LEVELS] = {
        objects.prize_1, objects.prize_2, objects.prize_3, objects.prize_4,
        objects.prize_5, objects.prize_6, objects.prize_7, objects.prize_8,
    };
    return prizes[level - 1];
}

static void refresh_unlocked(void)
{
    const bool open = g_game.mode == GAME_UNLOCKED;
    const uint32_t t = open ? app_unlock_ms() : 0;

    /* The prize behind the door is the level's own. */
    const int lv = g_game.level ? g_game.level : 1;
    for (int n = 1; n <= GAME_LEVELS; n++) {
        ui_lv_set_hidden(prize(n), n != lv);
    }
    const int frame = app_door_frame_at(t);
    for (int i = 0; i < UNLOCK_DOOR_FRAMES; i++) {
        ui_lv_set_hidden(door_frame(i), i != frame);
    }

    const uint32_t glow_at = UNLOCK_DOOR_START_MS + (UNLOCK_GLOW_FRAME - 1) * UNLOCK_FRAME_MS;
    const bool glow = open && t >= glow_at;
    ui_lv_set_hidden(objects.gold_glow, !glow);
    if (glow) {
        ui_lv_set_state_in(objects.gold_glow, LV_STATE_CHECKED | LV_STATE_DISABLED,
                           glow_state(t - glow_at));
    }

    ui_lv_set_hidden(objects.reward_pill, !(open && t >= UNLOCK_REWARD_MS));
    ui_lv_set_hidden(objects.reward_again, !(open && t >= GAME_REPLAY_LOCKOUT_MS));

    char buf[40];
    /* U+00B7 middle dot is in the house Roboto range. */
    snprintf(buf, sizeof(buf), "%02u \xC2\xB7 %02u \xC2\xB7 %02u",
             g_game.code[0], g_game.code[1], g_game.code[2]);
    ui_lv_set_text(objects.reward_code, buf);
    const uint32_t s = (game_time_left_ms(&g_game, app_now_ms()) + 999) / 1000;
    snprintf(buf, sizeof(buf), "Level %u \xC2\xB7 %lu:%02lu left", g_game.level,
             (unsigned long)(s / 60), (unsigned long)(s % 60));
    ui_lv_set_text(objects.reward_meta, buf);
    const int next = game_next_level(&g_game);
    if (next == 1) {
        ui_lv_set_text(objects.reward_again, "Turn to Play Again");
    } else {
        snprintf(buf, sizeof(buf), "Turn for Level %d", next);
        ui_lv_set_text(objects.reward_again, buf);
    }
}

/* ---- Instructions and Headphones ------------------------------------------- */

/* One list row's pitch: 34 px row + 4 px gap (screens_layout.py). */
#define HP_ROW_PITCH 38

static bool s_info_open;     /* the (i) panel: Forget Device / Cancel */
static bool s_info_dirty;

void ui_game_headphones_info(bool open)
{
    s_info_open = open;
    s_info_dirty = true;
}

static void refresh_headphones(void)
{
    static uint32_t s_gen = UINT32_MAX;
    bt_link_state_t s;
    bt_link_get(&s);
    if (s.gen == s_gen && !s_info_dirty) {
        return;
    }
    s_gen = s.gen;
    s_info_dirty = false;

    const bool connected = s.present && s.conn == BT_CONN_CONNECTED;
    bt_dev_t home;
    const bool has_home = s.present && bt_link_home(&s, &home);

    /* The Instructions page's button goes green while connected. */
    const lv_state_t on = connected ? DONE : 0;
    ui_lv_set_state_in(objects.hp_button, DONE, on);
    ui_lv_set_state_in(objects.hp_button_icon, DONE, on);
    ui_lv_set_state_in(objects.hp_button_text, DONE, on);

    /* Settings: the Headphones row's value. */
    ui_lv_set_text(objects.settings_hp_value,
                   !s.present ? "Unavailable" : connected ? "Connected" : "Not connected");
    ui_lv_set_state_in(objects.settings_hp_value, DONE | WRONG,
                       !s.present ? WRONG : connected ? DONE : 0);

    const char *status;
    bool bad = false;
    if (!s.present) {
        status = "Bluetooth is not available";
        bad = true;
    } else if (s.conn == BT_CONN_CONNECTING) {
        status = "Connecting...";
    } else if (connected) {
        status = "Connected";
    } else if (s.failed) {
        status = "Couldn't connect. Try again.";
        bad = true;
    } else if (has_home || s.ndev > 0) {
        status = "Tap your headphones";
    } else if (s.searching) {
        status = "Searching...";
    } else {
        status = "Put headphones in pairing mode";
    }
    ui_lv_set_text(objects.hp_status, status);
    ui_lv_set_state_in(objects.hp_status, WRONG, bad ? WRONG : 0);

    lv_obj_t *const rows[BT_LINK_ROWS] = {
        objects.hp_row0, objects.hp_row1, objects.hp_row2, objects.hp_row3,
        objects.hp_row4, objects.hp_row5, objects.hp_row6, objects.hp_row7,
    };
    lv_obj_t *const texts[BT_LINK_ROWS] = {
        objects.hp_row0_text, objects.hp_row1_text, objects.hp_row2_text, objects.hp_row3_text,
        objects.hp_row4_text, objects.hp_row5_text, objects.hp_row6_text, objects.hp_row7_text,
    };
    for (int i = 0; i < BT_LINK_ROWS; i++) {
        bt_dev_t d;
        const bool shown = s.present && bt_link_row(&s, i, &d);
        ui_lv_set_hidden(rows[i], !shown);
        if (shown) {
            ui_lv_set_text(texts[i], d.name);
            ui_lv_set_state_in(texts[i], DONE, i == 0 && connected ? DONE : 0);
        }
    }
    /* (i) only on the remembered headphones, which are always row 0. */
    ui_lv_set_hidden(objects.hp_row0_info, !has_home);

    const bool info = s_info_open && has_home;
    ui_lv_set_hidden(objects.hp_info, !info);
    if (info) {
        ui_lv_set_text(objects.hp_info_name, home.name);
    }
}

void ui_game_scroll_headphones(int detents)
{
    lv_obj_scroll_by_bounded(objects.hp_list, 0, -detents * HP_ROW_PITCH, LV_ANIM_ON);
}

/* ---- Settings -------------------------------------------------------------- */

/* The Volume row: the level, and - / + greyed out at the ends of the range
 * (DISABLED also stops them taking taps). */
static void refresh_settings(void)
{
    const int v = sound_volume();
    char buf[8];
    if (v == 0) {
        snprintf(buf, sizeof(buf), "Off");
    } else {
        snprintf(buf, sizeof(buf), "%d", v);
    }
    ui_lv_set_text(objects.settings_vol_value, buf);
    const lv_state_t at_min = v == 0 ? WRONG : 0;
    const lv_state_t at_max = v == SOUND_VOLUME_MAX ? WRONG : 0;
    ui_lv_set_state_in(objects.settings_vol_down, WRONG, at_min);
    ui_lv_set_state_in(objects.settings_vol_down_icon, WRONG, at_min);
    ui_lv_set_state_in(objects.settings_vol_up, WRONG, at_max);
    ui_lv_set_state_in(objects.settings_vol_up_icon, WRONG, at_max);
}

void ui_game_refresh(void)
{
    refresh_play();
    refresh_unlocked();
    refresh_headphones();
    refresh_settings();
}
