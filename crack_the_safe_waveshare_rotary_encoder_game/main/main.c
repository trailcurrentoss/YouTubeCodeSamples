/*
 * Crack the Safe -- Waveshare ESP32-S3-Knob-Touch-LCD-1.8.
 *
 * Turn the knob right until it clicks, reverse right there to lock the
 * number in, find the next one turning left, the last turning right -- and
 * the vault door swings open on a pile of gold. Turn past a number, or back
 * off anywhere else, and the safe re-locks with a new code.
 *
 * Boot order:
 *   0. NVS (the saved volume).
 *   1. knob_board_init() -- panel, touch, knob, haptics, LVGL. Backlight off.
 *   2. ui_init(), the dark theme, the game and the ui_tick() timer, under the
 *      lock.
 *   3. Backlight on, so the panel never shows an unpainted frame.
 *
 * The GUI is authored in GUI/CrackTheSafe.eez-project and exported to
 * main/ui/, which is disposable.
 */

#include "esp_log.h"
#include "esp_random.h"
#include "nvs_flash.h"

#include "knob_board.h"

#ifndef APP_HAVE_UI
#  error "APP_HAVE_UI is not defined -- main/CMakeLists.txt must set it"
#endif

#if APP_HAVE_UI
#  include "app.h"
#  include "screens.h"
#  include "ui.h"
#endif

static const char *TAG = "main";

#define MENU_HOLD_MS 1000

#if APP_HAVE_UI
/* Board rotate callback: LVGL task, lock already held. */
static void on_rotate(int diff, void *ctx)
{
    (void)ctx;
    app_rotate(diff);
}

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
    /* NVS holds the volume setting. */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(knob_board_init());

    if (knob_board_lock(0)) {
#if APP_HAVE_UI
        ui_init();
        /* The game is designed dark; EEZ starts on theme 0 ("Default"). */
        change_color_theme(THEME_ID_DARK);
        app_init(esp_random());
        lv_timer_create(ui_tick_timer_cb, 20, NULL);
        knob_board_set_rotate_callback(on_rotate, NULL);
        /* Touch and hold for the menu: a deliberate hold, not LVGL's 400 ms
         * default, which a resting finger trips by accident. */
        if (knob_board_touch_indev()) {
            lv_indev_set_long_press_time(knob_board_touch_indev(), MENU_HOLD_MS);
        }
#else
        /* Placeholder until the first EEZ Studio export. Built in C because
         * there is no .eez-project widget to author it on yet. */
        lv_obj_t *scr = lv_screen_active();
        lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
        lv_obj_t *msg = lv_label_create(scr);
        lv_obj_set_style_text_color(msg, lv_color_white(), 0);
        lv_label_set_text(msg, "No UI exported yet.\n"
                               "Open GUI/CrackTheSafe.eez-project\n"
                               "in EEZ Studio and press Ctrl+B.");
        lv_obj_set_style_text_align(msg, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(msg);
#endif
        knob_board_unlock();
    }

    ESP_ERROR_CHECK(knob_board_backlight_set(100));
    ESP_LOGI(TAG, "up");
}
