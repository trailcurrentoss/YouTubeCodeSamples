/*
 * ui_glue — drives the EEZ Studio screens from app_model.
 *
 * Everything EEZ-authored is only ever given state, flags, content or a
 * screen load from here -- never a position, size or font. The one
 * exception to "no geometry from C" is the Kdenlive cut marks: those are
 * objects CREATED here, at runtime, inside the EEZ-authored strip, because
 * where they sit is data (where the user cut), not layout. They take their
 * colour from the EEZ "CutMark" style, so the theme still owns it.
 *
 * Runs in the LVGL task with the display lock held.
 */

#include "ui.h"
#include "screens.h"
#include "styles.h"

#include "app_model.h"
#include "clock.h"
#include "rotary_usb.h"
#include "ui_glue.h"

/* ------------------------------------------------------------------------ */

static void set_checked(lv_obj_t *obj, bool on)
{
    if (!obj) {
        return;
    }
    if (on) {
        lv_obj_add_state(obj, LV_STATE_CHECKED);
    } else {
        lv_obj_remove_state(obj, LV_STATE_CHECKED);
    }
}

static void update_axis_chips(void)
{
    const app_axis_t a = app_model_axis();
    lv_obj_t *fc[AXIS_COUNT] = { objects.fc_axis_x, objects.fc_axis_y, objects.fc_axis_z };
    lv_obj_t *bl[AXIS_COUNT] = { objects.bl_axis_x, objects.bl_axis_y, objects.bl_axis_z };
    for (int i = 0; i < AXIS_COUNT; i++) {
        set_checked(fc[i], i == (int)a && app_model_app() == ROTARY_APP_FREECAD);
        set_checked(bl[i], i == (int)a && app_model_app() == ROTARY_APP_BLENDER);
    }
}

/* ---- Kdenlive scrub strip ----------------------------------------------
 *
 * The strip's content area is 124x14 px (126x16 less its 1 px border) and
 * shows 6 seconds, with the playhead fixed in the middle (x = 61 in
 * kd_playhead's authored position). Keep these in step with STRIP_W,
 * STRIP_H and PLAYHEAD_X in GUI/tmp/gen_eez_project.py.
 */
#define STRIP_W        124
#define STRIP_H        14
#define PLAYHEAD_X     61
#define STRIP_FRAMES   (6 * CONFIG_ROTARY_KDENLIVE_FPS)

static lv_obj_t *s_marks[APP_MAX_CUTS];

static void update_cut_marks(void)
{
    if (!objects.kd_strip) {
        return;
    }
    const int32_t *cuts;
    const int n = app_model_cuts(&cuts);
    const int32_t frame = app_model_frame();

    for (int i = 0; i < APP_MAX_CUTS; i++) {
        if (i >= n) {
            if (s_marks[i]) {
                lv_obj_add_flag(s_marks[i], LV_OBJ_FLAG_HIDDEN);
            }
            continue;
        }
        const int x = PLAYHEAD_X + (int)((cuts[i] - frame) * STRIP_W / STRIP_FRAMES);
        if (x < 0 || x > STRIP_W - 2) {
            if (s_marks[i]) {
                lv_obj_add_flag(s_marks[i], LV_OBJ_FLAG_HIDDEN);
            }
            continue;
        }
        if (!s_marks[i]) {
            lv_obj_t *m = lv_obj_create(objects.kd_strip);
            lv_obj_remove_style_all(m);
            lv_obj_add_style(m, get_style_cut_mark_MAIN_DEFAULT(),
                             LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(m, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_size(m, 2, STRIP_H);
            /* Behind the playhead, as in the design. */
            lv_obj_move_to_index(m, 0);
            s_marks[i] = m;
        }
        lv_obj_set_pos(s_marks[i], x, 0);
        lv_obj_remove_flag(s_marks[i], LV_OBJ_FLAG_HIDDEN);
    }
}

/* ---- Screens ----------------------------------------------------------- */

static enum ScreensEnum screen_for(rotary_app_t app)
{
    switch (app) {
    case ROTARY_APP_FREECAD:  return SCREEN_ID_PAGE_FREECAD;
    case ROTARY_APP_BLENDER:  return SCREEN_ID_PAGE_BLENDER;
    case ROTARY_APP_KDENLIVE: return SCREEN_ID_PAGE_KDENLIVE;
    case ROTARY_APP_VSCODIUM: return SCREEN_ID_PAGE_VSCODIUM;
    case ROTARY_APP_GIMP:     return SCREEN_ID_PAGE_GIMP;
    case ROTARY_APP_INKSCAPE: return SCREEN_ID_PAGE_INKSCAPE;
    case ROTARY_APP_LIBREOFFICE: return SCREEN_ID_PAGE_LIBREOFFICE;
    case ROTARY_APP_FIREFOX:  return SCREEN_ID_PAGE_FIREFOX;
    case ROTARY_APP_CHROMIUM: return SCREEN_ID_PAGE_CHROMIUM;
    default:                  return SCREEN_ID_PAGE_IDLE;
    }
}

static enum ScreensEnum s_shown;

static void on_model_changed(void)
{
    const enum ScreensEnum want = screen_for(app_model_app());
    if (want != s_shown) {
        loadScreen(want);
        s_shown = want;
    }
    update_axis_chips();
    update_cut_marks();
}

/* ---- USB status pill (idle page) --------------------------------------- */

static const char *s_usb_text = "USB not connected";

const char *ui_glue_usb_status_text(void)
{
    return s_usb_text;
}

static void status_timer_cb(lv_timer_t *t)
{
    (void)t;
    const bool mounted   = rotary_usb_mounted();
    const bool companion = rotary_usb_companion_connected();

    s_usb_text = !mounted  ? "USB not connected"
               : companion ? "USB HID connected"
                           : "Companion not running";
    set_checked(objects.idle_dot, mounted && companion);

    /* No time yet (no companion since power-up): show the dial without
     * hands rather than a wrong time. */
    lv_obj_t *hands[] = { objects.idle_hand_hour, objects.idle_hand_minute,
                          objects.idle_hand_second };
    for (size_t i = 0; i < sizeof(hands) / sizeof(hands[0]); i++) {
        if (!hands[i]) {
            continue;
        }
        if (clock_valid()) {
            lv_obj_remove_flag(hands[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(hands[i], LV_OBJ_FLAG_HIDDEN);
        }
    }

    /* No companion means nobody is telling us what has focus; stop sending
     * app-specific keys into whatever that is. */
    if (!companion && app_model_app() != ROTARY_APP_OTHER) {
        app_model_set_app(ROTARY_APP_OTHER);
    }
}

/* ------------------------------------------------------------------------ */

void ui_glue_init(void)
{
    /* The design is dark; the Default (light) theme stays available. */
    change_color_theme(THEME_ID_DARK);

    app_model_set_changed_callback(on_model_changed);
    s_shown = SCREEN_ID_PAGE_IDLE;
    loadScreen(SCREEN_ID_PAGE_IDLE);
    on_model_changed();

    lv_timer_create(status_timer_cb, 250, NULL);
    status_timer_cb(NULL);
}
