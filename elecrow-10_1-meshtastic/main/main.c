/*
 * Meshtastic messenger — Elecrow CrowPanel Advance 10.1" ESP32-P4.
 *
 * Boot order matters on this board: display + touch come up first, and the
 * ESP32-C6 link (ESP-Hosted, used here for Bluetooth) comes up LAST. Bringing
 * the coprocessor up before the GT911 touch controller stops LVGL from
 * polling touch at all.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_ldo_regulator.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "bsp_display.h"
#include "bsp_i2c.h"
#include "bsp_illuminate.h"

#include "meshtastic_node.h"
#include "mesh_ui.h"

#if __has_include("ui/ui.h")
#include "ui.h"
#define UI_EXPORT_PRESENT 1
#else
#define UI_EXPORT_PRESENT 0
#endif

static const char *TAG = "main";

static esp_ldo_channel_handle_t s_ldo3;
static esp_ldo_channel_handle_t s_ldo4;

static void init_fail(const char *module, esp_err_t err)
{
    while (1) {
        ESP_LOGE(TAG, "[%s] init failed: %s", module, esp_err_to_name(err));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#if UI_EXPORT_PRESENT
/* ui_tick() reads every bound property; nothing calls it unless we do. Runs
 * in the LVGL task, so the display lock is already held. */
static void ui_tick_cb(lv_timer_t *t)
{
    (void)t;
    ui_tick();
}

/* What ui_init() does, one page at a time. The pages hold about 540 widgets
 * and take seconds to build; doing it under one lock leaves the screen black
 * and starves the idle task long enough to trip the task watchdog. Messages
 * is built and shown first, and touch stays off until every page exists.
 * Keep the page list in step with create_screens() in ui/screens.c. */
static void build_ui(void)
{
    static void (*const later_pages[])(void) = {
        create_screen_page_nodes,
        create_screen_page_settings,
    };

    lvgl_port_lock(0);
    lv_disp_t *disp = lv_disp_get_default();
    lv_disp_set_theme(disp, lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
                                                  lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT));
    lv_indev_t *touch = lv_indev_get_next(NULL);
    if (touch) lv_indev_enable(touch, false);
    create_screen_page_messages();
    loadScreen(SCREEN_ID_PAGE_MESSAGES);
    lvgl_port_unlock();

    for (size_t i = 0; i < sizeof(later_pages) / sizeof(later_pages[0]); i++) {
        vTaskDelay(pdMS_TO_TICKS(20));
        lvgl_port_lock(0);
        later_pages[i]();
        lvgl_port_unlock();
    }

    lvgl_port_lock(0);
    lv_timer_create(ui_tick_cb, 50, NULL);
    mesh_ui_init();
    if (touch) lv_indev_enable(touch, true);
    lvgl_port_unlock();
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

    /* LDO3 powers the MIPI-DSI PHY, LDO4 the LCD. */
    esp_ldo_channel_config_t ldo3 = { .chan_id = 3, .voltage_mv = 2500 };
    if ((err = esp_ldo_acquire_channel(&ldo3, &s_ldo3)) != ESP_OK) init_fail("ldo3", err);
    esp_ldo_channel_config_t ldo4 = { .chan_id = 4, .voltage_mv = 3300 };
    if ((err = esp_ldo_acquire_channel(&ldo4, &s_ldo4)) != ESP_OK) init_fail("ldo4", err);

    if ((err = i2c_init()) != ESP_OK) init_fail("i2c", err);
    if ((err = touch_init()) != ESP_OK) init_fail("touch", err);
    if ((err = display_init()) != ESP_OK) init_fail("display", err);
    if ((err = set_lcd_blight(80)) != ESP_OK) ESP_LOGW(TAG, "backlight: %s", esp_err_to_name(err));

#if UI_EXPORT_PRESENT
    build_ui();
#else
    if (lvgl_port_lock(0)) {
        lv_obj_t *l = lv_label_create(lv_scr_act());
        lv_label_set_text(l, "UI not exported yet:\nopen GUI/elecrow-10_1-meshtastic in EEZ Studio and Build (Ctrl+B)");
        lv_obj_center(l);
        lvgl_port_unlock();
    }
#endif

    /* Last: LoRa radio + Bluetooth through the C6. The UI shows a "not
     * detected" state rather than stopping if the module is missing. */
    err = meshtastic_node_start();
    if (err != ESP_OK) ESP_LOGE(TAG, "Meshtastic node not running: %s", esp_err_to_name(err));
}
