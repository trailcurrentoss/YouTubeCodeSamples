/*
 * CST816 capacitive touch on the shared I2C0 bus.
 *
 * Touch is always on: the knob has no push, so a tap on the glass is the
 * only way to select anything. There is no calibration either --
 * the CST816 on this module reports panel coordinates directly (Waveshare's
 * demo passes them straight to LVGL), and a per-unit calibration screen for a
 * finger-sized UI would cost more than it fixes.
 */

#include "esp_check.h"
#include "esp_lcd_touch_cst816s.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "board_internal.h"
#include "board_pins.h"
#include "knob_board.h"

static const char *TAG = "board.touch";

static esp_lcd_touch_handle_t s_touch;
static lv_indev_t            *s_indev;
static lv_indev_read_cb_t     s_port_read_cb;

/* In front of esp_lvgl_port's own read callback: note activity for the idle
 * timeout, then let the port do the work. */
static void touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (s_port_read_cb) {
        s_port_read_cb(indev, data);
    }
    if (data->state == LV_INDEV_STATE_PRESSED) {
        board_note_input();
    }
}

esp_err_t board_touch_init(i2c_master_bus_handle_t bus, lv_display_t *disp,
                           lv_indev_t **out_indev)
{
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_io_i2c_config_t io_cfg = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    io_cfg.dev_addr     = BOARD_TOUCH_ADDR;
    io_cfg.scl_speed_hz = BOARD_I2C_HZ;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(bus, &io_cfg, &io), TAG,
                        "touch panel io");

    /*
     * The INT line is given to the driver. CST816 parts drop into a low-power
     * mode between touches and stop answering I2C; with INT wired, the driver
     * reads only after the controller has signalled a touch, instead of
     * NACKing its way through every LVGL read period.
     */
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max        = BOARD_LCD_H_RES,
        .y_max        = BOARD_LCD_V_RES,
        .rst_gpio_num = BOARD_TOUCH_RST,
        .int_gpio_num = BOARD_TOUCH_INT,
        .levels       = { .reset = 0, .interrupt = 0 },
        .flags        = { .swap_xy = 0, .mirror_x = 0, .mirror_y = 0 },
    };
    ESP_RETURN_ON_ERROR(esp_lcd_touch_new_i2c_cst816s(io, &tp_cfg, &s_touch),
                        TAG, "cst816");

    const lvgl_port_touch_cfg_t lv_cfg = { .disp = disp, .handle = s_touch };
    s_indev = lvgl_port_add_touch(&lv_cfg);
    ESP_RETURN_ON_FALSE(s_indev, ESP_FAIL, TAG, "lvgl_port_add_touch failed");

    s_port_read_cb = lv_indev_get_read_cb(s_indev);
    lv_indev_set_read_cb(s_indev, touch_read_cb);

    ESP_LOGI(TAG, "CST816 @0x%02X, INT %d RST %d", BOARD_TOUCH_ADDR,
             BOARD_TOUCH_INT, BOARD_TOUCH_RST);
    *out_indev = s_indev;
    return ESP_OK;
}

lv_indev_t *knob_board_touch_indev(void) { return s_indev; }
