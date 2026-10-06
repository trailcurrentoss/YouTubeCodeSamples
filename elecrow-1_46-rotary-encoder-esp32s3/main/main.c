/*
 * Rotary Macro Pad — Elecrow CrowPanel 1.46" Rotary (360x360).
 *
 * A USB keyboard with a ring and a round touchscreen. A host companion
 * (companion/rotary_hid_companion.py) reports which app has focus; the pad
 * shows that app's eight shortcut keys and maps the ring to it:
 *
 *   FreeCAD   turn: rotate view about the active axis   push: next axis
 *   Blender   turn: rotate selection (R <axis> n Enter)  push: next axis
 *   Kdenlive  turn: scrub by frame / second              push: cut (Shift+R)
 *   VSCodium  turn: scroll the editor (lines / pages)     push: lines <-> pages
 *   GIMP      turn: zoom in / out                         push: fit image
 *   Inkscape  turn: zoom in / out                         push: zoom page
 *   LibreOffice turn: move 3 lines / a page               push: lines <-> pages
 *   Firefox / Chromium  turn: scroll the page             push: lines <-> pages
 *
 * Boot order:
 *   1. NVS.
 *   2. crowpanel_board_init() -- panel, touch, ring, LVGL. Backlight stays off.
 *   3. ui_init(), the ui_tick() timer and the screen glue, under the lock.
 *   4. USB (HID + companion serial). The focus callback needs the UI.
 *   5. Backlight on, so the panel never shows an unpainted frame.
 *
 * The GUI is authored in GUI/RotaryHid.eez-project and exported to main/ui/,
 * which is disposable. This file never positions, sizes, aligns or re-fonts
 * an EEZ-authored widget -- that stays in the .eez-project so the EEZ Studio
 * canvas and the panel agree.
 */

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "app_model.h"
#include "clock.h"
#include "crowpanel_board.h"
#include "rotary_usb.h"

#ifndef APP_HAVE_UI
#  error "APP_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if APP_HAVE_UI
#  include "ui.h"
#  include "ui_glue.h"
#endif

static const char *TAG = "rotary_hid";

/* ---- Input ------------------------------------------------------------- *
 * The ring callbacks run in the LVGL task with the display lock held.
 */
static void on_rotate(int detents, void *ctx)
{
    (void)ctx;
    app_model_rotate(detents);
}

static void on_press(void *ctx)
{
    (void)ctx;
    app_model_push();
}

/* Focus changes arrive on the USB task, so the lock has to be taken here. */
static void on_focus(rotary_app_t app, void *ctx)
{
    (void)ctx;
    if (crowpanel_board_lock(100)) {
        app_model_set_app(app);
        crowpanel_board_unlock();
    } else {
        ESP_LOGW(TAG, "display busy -- focus change to %s dropped",
                 rotary_app_name(app));
    }
}

/* Time of day from the companion, also on the USB task. */
static void on_time(int hour, int minute, int second, void *ctx)
{
    (void)ctx;
    if (crowpanel_board_lock(100)) {
        clock_set(hour, minute, second);
        crowpanel_board_unlock();
    }
}

#if APP_HAVE_UI
/*
 * ui_tick() reads every expression-bound property through its get_var_*
 * accessor, and nothing calls it on the device. Without this timer the
 * screen draws once and never changes. Runs in the LVGL task, which already
 * holds the display lock -- do not take it again here.
 */
static void ui_tick_timer_cb(lv_timer_t *t)
{
    (void)t;
    ui_tick();
}
#endif

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(crowpanel_board_init());

    /* Resting colour for the LED ring. Turning the ring whitens the side it
     * turns towards, then falls back to this (board_leds.c). */
    crowpanel_board_leds_set_all(0, 255, 0);

    if (crowpanel_board_lock(0)) {
#if APP_HAVE_UI
        ui_init();
        ui_glue_init();
        lv_timer_create(ui_tick_timer_cb, 50, NULL);
#else
        /* Placeholder until the first EEZ Studio export. Built in C because
         * there is no .eez-project widget to author it on yet. */
        lv_obj_t *scr = lv_screen_active();
        lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
        lv_obj_t *msg = lv_label_create(scr);
        lv_obj_set_style_text_color(msg, lv_color_white(), 0);
        lv_label_set_text(msg, "No UI exported yet.\n"
                               "Open GUI/RotaryHid.eez-project\n"
                               "in EEZ Studio and press Ctrl+B.");
        lv_obj_set_style_text_align(msg, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(msg);
#endif
        crowpanel_board_set_rotate_callback(on_rotate, NULL);
        crowpanel_board_set_press_callback(on_press, NULL);
        crowpanel_board_unlock();
    }

    rotary_usb_set_time_callback(on_time, NULL);
    ESP_ERROR_CHECK(rotary_usb_init(on_focus, NULL));

    ESP_ERROR_CHECK(crowpanel_board_backlight_set(100));
    ESP_LOGI(TAG, "up");
}
