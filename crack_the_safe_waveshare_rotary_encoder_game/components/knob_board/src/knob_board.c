/*
 * Board bring-up and the small shared pieces: backlight and the LVGL lock.
 */

#include "driver/ledc.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "board_internal.h"
#include "board_pins.h"
#include "knob_board.h"

static const char *TAG = "board";

static i2c_master_bus_handle_t s_i2c;
static lv_indev_t             *s_touch;
static bool                    s_haptics;

void board_note_input(void) {}

bool knob_board_lock(uint32_t timeout_ms) { return lvgl_port_lock(timeout_ms); }
void knob_board_unlock(void)              { lvgl_port_unlock(); }
bool knob_board_has_haptics(void)         { return s_haptics; }

static esp_err_t backlight_init(void)
{
    const ledc_timer_config_t timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num       = LEDC_TIMER_3,
        .freq_hz         = BOARD_LCD_BL_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "ledc timer");
    const ledc_channel_config_t ch = {
        .gpio_num   = BOARD_LCD_BL,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_1,
        .timer_sel  = LEDC_TIMER_3,
        .duty       = 0,          /* dark until the first frame is drawn */
        .hpoint     = 0,
    };
    return ledc_channel_config(&ch);
}

esp_err_t knob_board_backlight_set(uint8_t percent)
{
    if (percent > 100) {
        percent = 100;
    }
    const uint32_t duty = (255u * percent) / 100u;
    ESP_RETURN_ON_ERROR(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty),
                        TAG, "duty");
    return ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
}

esp_err_t knob_board_init(void)
{
    ESP_RETURN_ON_ERROR(backlight_init(), TAG, "backlight");

    /*
     * 6 KB of LVGL stack: EEZ's create_screens() builds every page in one
     * call from inside this task's lock, and 4 KB is too tight for that.
     */
    const lvgl_port_cfg_t lv_cfg = {
        .task_priority     = 4,
        .task_stack        = 6144,
        .task_affinity     = 1,
        .task_max_sleep_ms = 500,
        .timer_period_ms   = 5,
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lv_cfg), TAG, "lvgl port");

    lv_display_t *disp = NULL;
    ESP_RETURN_ON_ERROR(board_display_init(&disp), TAG, "display");

    const i2c_master_bus_config_t bus_cfg = {
        .i2c_port   = BOARD_I2C_PORT,
        .sda_io_num = BOARD_I2C_SDA,
        .scl_io_num = BOARD_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_cfg, &s_i2c), TAG, "i2c0");

    /* Touch failing is logged but not fatal: the knob alone plays the game. */
    if (lvgl_port_lock(0)) {
        if (board_touch_init(s_i2c, disp, &s_touch) != ESP_OK) {
            ESP_LOGE(TAG, "touch did not start -- knob only");
        }
        ESP_RETURN_ON_ERROR(board_knob_init(), TAG, "knob");
        lvgl_port_unlock();
    }

    board_haptic_init(s_i2c, &s_haptics);

    /* Sound is not fatal either: the game is played by feel. */
    if (board_audio_init() != ESP_OK) {
        ESP_LOGE(TAG, "audio did not start -- silent");
    }

    /* Nor is the battery gauge: the UI just leaves the icon off. */
    if (board_battery_init() != ESP_OK) {
        ESP_LOGE(TAG, "battery sense did not start -- no battery icon");
    }

    ESP_LOGI(TAG, "Waveshare ESP32-S3 Knob 1.8\" %ux%u, touch %s, haptics %s",
             KNOB_BOARD_H_RES, KNOB_BOARD_V_RES, s_touch ? "on" : "FAILED",
             s_haptics ? "on" : "absent");
    return ESP_OK;
}
