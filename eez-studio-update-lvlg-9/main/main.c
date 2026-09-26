/*
 * TrailCurrent Power Dashboard — Waveshare ESP32-P4-WIFI6-Touch-LCD-7B.
 *
 * 7" 1024x600 IPS (EK79007 over MIPI-DSI) + GT911 capacitive touch, both
 * brought up by Waveshare's own BSP component.
 *
 * Boot order, and why it is this order:
 *   1. nvs_flash_init()              — everything below may want NVS
 *   2. esp_event_loop_create_default()
 *   3. bsp_i2c_init()                — shared I2C: GT911 touch lives here
 *   4. bsp_display_start_with_config() — display + LVGL + touch in one call
 *   5. bsp_display_backlight / brightness
 *   6. ui_init() under bsp_display_lock()
 *   7. data source LAST
 *
 * Step 7 is last deliberately. Bringing WiFi up before touch is initialised
 * has been observed to leave LVGL's input device polling dead on P4 boards, so
 * nothing that touches the radio runs until the UI is up and taking taps.
 *
 * Everything under main/UI/ is the GUI: the files EEZ Studio generates plus
 * the three it does not (actions.c, vars.c, dash_data.c). This file never
 * positions, sizes, aligns or re-fonts an EEZ-authored widget — all of that
 * stays in the .eez-project so the EEZ Studio canvas and the panel agree.
 */

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_timer.h"
/* MALLOC_CAP_* and heap_caps_get_free_size() — reachable transitively through
 * the BSP today, but named here so a BSP reshuffle does not break the build. */
#include "esp_heap_caps.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "bsp/esp32_p4_wifi6_touch_lcd_7b.h"

#include "UI/dash_data.h"
#include "UI/broker_cfg.h"
#include "UI/wifi_port.h"

/* Implemented in mqtt_source.c; device-only, so no header of its own. */
void mqtt_source_start(void);

/* The EEZ Studio export may not exist yet on a fresh clone. Guarding on it
 * keeps `idf.py build` working before the first Ctrl+B, instead of failing on
 * a missing objects struct. */
#if __has_include("UI/ui.h")
#define UI_EXPORT_PRESENT 1
#include "UI/screens.h"
#include "UI/ui.h"
#else
#define UI_EXPORT_PRESENT 0
#endif

/*
 * Where the numbers come from on the board.
 *
 * 1 = feed the gauges a synthetic sweep so a bare board is demonstrable with
 *     nothing else on the network. This is the device-side equivalent of what
 *     dash_data.c does under EEZ_LVGL_SIMULATOR. Build with
 *     -DDASH_DEMO_SOURCE=1 when filming a board that is not in the rig.
 * 0 = the real thing: bring up Wi-Fi, connect to the Headwaters broker and
 *     let MQTT drive the gauges. Until Wi-Fi is configured from the setup
 *     screen the panel holds its placeholders, which is the honest state.
 */
#ifndef DASH_DEMO_SOURCE
#define DASH_DEMO_SOURCE 0
#endif

static const char *TAG = "powerdash";

/* --------------------------------------------------------------- data source */

#if DASH_DEMO_SOURCE
/* Same triangle sweep the simulator uses, on the same periods, so what you
 * film on the bench matches what you saw on the laptop. */
static int32_t tri(uint32_t now_ms, uint32_t period_ms, int32_t lo, int32_t hi)
{
    const uint32_t half = period_ms / 2u;
    const uint32_t t    = now_ms % period_ms;
    const uint32_t up   = (t < half) ? t : (period_ms - t);
    return lo + (int32_t)((uint64_t)up * (uint64_t)(uint32_t)(hi - lo) / half);
}

static void demo_source_task(void *arg)
{
    (void)arg;
    ESP_LOGW(TAG, "DASH_DEMO_SOURCE=1 — gauges are running synthetic data");
    for (;;) {
        const uint32_t now = (uint32_t)(esp_timer_get_time() / 1000);
        const int32_t mv    = tri(now, 24000u, 11400, 13900);
        const int32_t solar = tri(now, 17000u, 0, 1150);
        const bool    charging   = solar > 575;
        const int32_t load_watts = charging ? 0 : tri(now, 24000u, 10, 240);

        dash_data_set_battery(mv,
                              tri(now, 24000u, 180, 990),   /* SOC x10 */
                              load_watts,
                              charging ? "bulk" : NULL);
        dash_data_set_solar(solar);
        dash_data_set_tanks(tri(now, 31000u, 8, 96),
                            tri(now, 43000u, 4, 88));
        dash_data_set_link(true, "DEMO DATA");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif /* DASH_DEMO_SOURCE */

static void dash_source_start(void)
{
#if DASH_DEMO_SOURCE
    xTaskCreatePinnedToCore(demo_source_task, "dash_demo", 3072, NULL,
                            tskIDLE_PRIORITY + 2, NULL, 0);
#else
    /*
     * The real source. The seam is deliberately narrow: the transport only has
     * to call the four dash_data_set_*() functions, and neither the UI nor
     * dash_data.c knows anything about Wi-Fi or MQTT.
     *
     * Order matters, and not for the reason you would guess. This runs last,
     * after the display and the touch controller are up, because bringing
     * Wi-Fi up first has been seen to stop LVGL's input device polling
     * outright on a P4 panel -- the screen draws, nothing responds to touch,
     * and it presents as a digitiser fault.
     *
     * wifi_port_init() rejoins the saved network by itself if NVS has one, so
     * a configured panel needs nobody to touch it after a power cut. If it
     * does not, the gauges hold their placeholders until someone uses the
     * Wi-Fi setup screen.
     */
    dash_data_set_link(false, NULL);
    broker_cfg_init();
    wifi_port_init();
    /* Returns immediately and does nothing if the broker has not been
     * configured on the panel yet; the Headwaters screen is how that happens,
     * and saving there restarts the client. */
    mqtt_source_start();
#endif
}

#if UI_EXPORT_PRESENT
/* Runs in the LVGL task; the display lock is already held here. */
static void ui_tick_timer_cb(lv_timer_t *t)
{
    (void)t;
    ui_tick();
}
#endif

/* ------------------------------------------------------------------ app_main */

static void init_fail(const char *what, esp_err_t err)
{
    ESP_LOGE(TAG, "init failed at %s: %s", what, esp_err_to_name(err));
    vTaskDelay(pdMS_TO_TICKS(2000));
    abort();
}

void app_main(void)
{
    ESP_LOGI(TAG, "TrailCurrent Power Dashboard — LVGL %d.%d.%d",
             LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    /* 1. NVS. */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* 2. Default event loop — the BSP does not create one. */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* 3. I2C — GT911 touch shares this bus. Idempotent; the BSP's own touch
     *    init calls it too. */
    ESP_ERROR_CHECK(bsp_i2c_init());

    /* 4. Display + LVGL + touch.
     *
     *    task_affinity 1 pins the LVGL port task to core 1, keeping the panel
     *    refresh cadence off core 0 where the network stack runs. The draw
     *    buffer is 100 lines x 1024 px, double buffered, out of PSRAM — the
     *    full framebuffer at 1024x600x2 is ~1.2 MB and will not fit in
     *    internal RAM. */
    bsp_display_cfg_t disp_cfg = {
        .lvgl_port_cfg = {
            .task_priority     = configMAX_PRIORITIES - 4,
            .task_stack        = 8192 * 2,
            .task_affinity     = 1,
            .task_max_sleep_ms = 10,
            .task_stack_caps   = MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT,
            .timer_period_ms   = 5,
        },
        .buffer_size   = BSP_LCD_H_RES * 100,
        .double_buffer = true,
        .flags = {
            .buff_dma    = false,
            .buff_spiram = true,
            .sw_rotate   = true,
        },
    };
    lv_display_t *disp = bsp_display_start_with_config(&disp_cfg);
    if (!disp) init_fail("display", ESP_FAIL);

    /* The Waveshare 7B ships with the display ribbon on the top edge, so with
     * the board the way up you actually mount it the panel scans out
     * upside-down and the UI has to compensate. Confirmed on the bench: at
     * ROTATION_0 the dashboard renders inverted. `sw_rotate = true` in
     * disp_cfg above is the prerequisite — the BSP then has LVGL rotate on
     * each flush.
     *
     * Bare-board with the ribbon at the bottom instead? Use
     * LV_DISPLAY_ROTATION_0. */
    bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);

    bsp_display_backlight_on();

    /* 5. Model before the UI, so the first tick_screen() reads initialised
     *    placeholders rather than zeroes. */
    dash_data_init();

    /* 6. Build the screens under the LVGL lock. */
#if UI_EXPORT_PRESENT
    if (bsp_display_lock(0)) {
        ui_init();

        /*
         * Drive ui_tick().
         *
         * EEZ Studio's ui_tick() calls tick_screen(), which is what reads every
         * expression-bound property through its get_var_*() accessor. Those
         * accessors are also what pump the data model (see vars.c). So without
         * this timer nothing on the panel ever updates: no gauge moves, no
         * label changes, and the Wi-Fi scan list stays empty however well the
         * radio works.
         *
         * It is easy to miss because the SIMULATOR does not need it -- the
         * eez-open simulator's own main.c calls ui_init() and then ui_tick()
         * in its loop, so the whole UI ticks there for free. On the device it
         * is the application's job, and the symptom of forgetting is a screen
         * that draws perfectly and then never changes.
         *
         * An lv_timer runs in the LVGL task, so the display lock is already
         * held when the callback fires and must not be taken again. 50 ms is
         * twice the model's own 100 ms pump rate, which is enough to never be
         * the limiting factor.
         */
        lv_timer_create(ui_tick_timer_cb, 50, NULL);

        bsp_display_unlock();
        ESP_LOGI(TAG, "UI ready");
    } else {
        init_fail("bsp_display_lock", ESP_FAIL);
    }
#else
    ESP_LOGE(TAG, "main/UI/ has no EEZ Studio export yet.");
    ESP_LOGE(TAG, "Open GUI/TrailCurrentPowerDash.eez-project and press "
                  "Ctrl+B, then rebuild.");
    if (bsp_display_lock(0)) {
        lv_obj_t *msg = lv_label_create(lv_screen_active());
        lv_label_set_text(msg, "Run EEZ Studio -> Build (Ctrl+B)");
        lv_obj_center(msg);
        bsp_display_unlock();
    }
#endif

    /* 7. Data source last — see the note at the top of this file. */
    dash_source_start();

    ESP_LOGI(TAG, "free heap %u, PSRAM %u",
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
}
