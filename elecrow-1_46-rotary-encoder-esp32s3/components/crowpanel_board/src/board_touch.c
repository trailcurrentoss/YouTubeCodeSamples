/*
 * Capacitive touch: CST816D on I2C, interrupt-driven.
 *
 * The controller speaks the CST816S register map at the same 0x15 address,
 * so esp_lcd_touch_cst816s drives it. INT and RST are both wired on this
 * board; with an INT pin, esp_lvgl_port puts the indev in event mode and
 * only reads the controller when it signals a contact.
 */

#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_lcd_touch_cst816s.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "board_internal.h"
#include "board_pins.h"
#include "crowpanel_board.h"

static const char *TAG = "board.touch";

/* Touch orientation is INDEPENDENT of the display's. The touch layer is a
 * separate device whose axes are fixed by how it is bonded, so work these
 * out on the glass rather than copying the display transform. */
#ifdef CONFIG_CROWPANEL_TOUCH_MIRROR_X
#  define TOUCH_MIRROR_X 1
#else
#  define TOUCH_MIRROR_X 0
#endif
#ifdef CONFIG_CROWPANEL_TOUCH_MIRROR_Y
#  define TOUCH_MIRROR_Y 1
#else
#  define TOUCH_MIRROR_Y 0
#endif
#ifdef CONFIG_CROWPANEL_TOUCH_SWAP_XY
#  define TOUCH_SWAP_XY 1
#else
#  define TOUCH_SWAP_XY 0
#endif

static lv_indev_t        *s_indev;
static lv_indev_read_cb_t s_port_read_cb;

static bool s_touch_down;       /* finger currently on the glass */
static bool s_contact_pushed;   /* ring switch closed during this contact */

void board_touch_note_push(void)
{
    if (s_touch_down) {
        s_contact_pushed = true;
    }
}

bool crowpanel_board_touch_contact_pushed(void)
{
    return s_contact_pushed;
}

/* Chain in front of esp_lvgl_port's own read callback so a touch also counts
 * as user activity. The port's read function is not exported, so capture
 * whatever it installed and call through to it. */
static void touch_activity_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (s_port_read_cb) {
        s_port_read_cb(indev, data);
    }
    const bool down = (data->state == LV_INDEV_STATE_PRESSED);
    if (down && !s_touch_down) {
        /* New contact. The switch can close a moment before the touch
         * controller reports the finger, so a button already down counts. */
        s_contact_pushed = board_button_is_down();
    }
    s_touch_down = down;
    if (down) {
        board_note_input();
    }
}

esp_err_t board_touch_init(lv_display_t *disp, lv_indev_t **out_indev)
{
    i2c_master_bus_handle_t bus = NULL;
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port   = I2C_NUM_0,
        .sda_io_num = BOARD_TOUCH_I2C_SDA,
        .scl_io_num = BOARD_TOUCH_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_cfg, &bus), TAG,
                        "i2c bus (SDA %d SCL %d) failed",
                        BOARD_TOUCH_I2C_SDA, BOARD_TOUCH_I2C_SCL);

    esp_lcd_panel_io_handle_t tp_io = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_cfg = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    tp_io_cfg.dev_addr = BOARD_TOUCH_I2C_ADDR;
    tp_io_cfg.scl_speed_hz = 400000;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(bus, &tp_io_cfg, &tp_io),
                        TAG, "touch panel io failed");

    esp_lcd_touch_config_t tp_cfg = {
        .x_max        = CROWPANEL_LCD_H_RES,
        .y_max        = CROWPANEL_LCD_V_RES,
        .rst_gpio_num = BOARD_TOUCH_RST,
        .int_gpio_num = BOARD_TOUCH_INT,
        .levels = { .reset = 0, .interrupt = 0 },
        .flags  = {
            .swap_xy  = TOUCH_SWAP_XY,
            .mirror_x = TOUCH_MIRROR_X,
            .mirror_y = TOUCH_MIRROR_Y,
        },
    };
    esp_lcd_touch_handle_t touch = NULL;
    ESP_RETURN_ON_ERROR(esp_lcd_touch_new_i2c_cst816s(tp_io, &tp_cfg, &touch),
                        TAG, "cst816d init failed");

    const lvgl_port_touch_cfg_t lv_cfg = { .disp = disp, .handle = touch };
    s_indev = lvgl_port_add_touch(&lv_cfg);
    ESP_RETURN_ON_FALSE(s_indev, ESP_FAIL, TAG, "lvgl_port_add_touch failed");

    s_port_read_cb = lv_indev_get_read_cb(s_indev);
    lv_indev_set_read_cb(s_indev, touch_activity_cb);

    ESP_LOGI(TAG, "CST816D @0x%02X on SDA %d / SCL %d, INT %d RST %d, "
                  "mirror_x=%d mirror_y=%d swap_xy=%d",
             BOARD_TOUCH_I2C_ADDR, BOARD_TOUCH_I2C_SDA, BOARD_TOUCH_I2C_SCL,
             BOARD_TOUCH_INT, BOARD_TOUCH_RST,
             TOUCH_MIRROR_X, TOUCH_MIRROR_Y, TOUCH_SWAP_XY);

    *out_indev = s_indev;
    return ESP_OK;
}

lv_indev_t *crowpanel_board_touch_indev(void) { return s_indev; }
