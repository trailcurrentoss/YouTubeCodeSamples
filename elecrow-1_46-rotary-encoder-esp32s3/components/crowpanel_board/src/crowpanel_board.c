/*
 * Board bring-up and the public crowpanel_board API.
 */

#include "driver/ledc.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "board_internal.h"
#include "board_pins.h"
#include "crowpanel_board.h"

static const char *TAG = "board";

#define BL_TIMER   LEDC_TIMER_0
#define BL_CHANNEL LEDC_CHANNEL_0
#define BL_MODE    LEDC_LOW_SPEED_MODE

static esp_err_t backlight_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode      = BL_MODE,
        .timer_num       = BL_TIMER,
        .duty_resolution = BOARD_BL_PWM_BITS,
        .freq_hz         = BOARD_BL_PWM_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "bl timer failed");

    ledc_channel_config_t ch = {
        .gpio_num   = BOARD_BL_GPIO,
        .speed_mode = BL_MODE,
        .channel    = BL_CHANNEL,
        .timer_sel  = BL_TIMER,
        .duty       = 0,          /* dark until the first frame is drawn */
        .hpoint     = 0,
    };
    return ledc_channel_config(&ch);
}

esp_err_t crowpanel_board_backlight_set(uint8_t percent)
{
    if (percent > 100) {
        percent = 100;
    }
    const uint32_t max  = (1u << BOARD_BL_PWM_BITS) - 1u;
    const uint32_t duty = (max * percent) / 100u;
    ESP_RETURN_ON_ERROR(ledc_set_duty(BL_MODE, BL_CHANNEL, duty),
                        TAG, "set duty failed");
    return ledc_update_duty(BL_MODE, BL_CHANNEL);
}

bool crowpanel_board_lock(uint32_t timeout_ms)
{
    return lvgl_port_lock(timeout_ms);
}

void crowpanel_board_unlock(void)
{
    lvgl_port_unlock();
}

esp_err_t crowpanel_board_init(void)
{
    /* Not fatal: a pad whose LED ring fails to start still works. */
    if (board_leds_init() != ESP_OK) {
        ESP_LOGW(TAG, "LED ring init failed -- continuing without it");
    }

    ESP_RETURN_ON_ERROR(backlight_init(), TAG, "backlight init failed");

    /* LVGL task pinned to core 1, away from the Wi-Fi / USB stacks on core 0. */
    const lvgl_port_cfg_t lv_cfg = {
        .task_priority     = 4,
        .task_stack        = 6144,
        .task_affinity     = 1,
        .task_max_sleep_ms = 500,
        .timer_period_ms   = 5,
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lv_cfg), TAG, "lvgl_port_init failed");

    lv_display_t *disp = NULL;
    ESP_RETURN_ON_ERROR(board_display_init(&disp), TAG, "display init failed");

    lv_indev_t *touch = NULL;
    esp_err_t err = board_touch_init(disp, &touch);
    if (err != ESP_OK) {
        /* A dead touch controller should not stop the device booting -- the
         * ring can still drive it. Log loudly and continue. */
        ESP_LOGE(TAG, "touch init failed (%s) -- continuing with ring only",
                 esp_err_to_name(err));
    }

    lv_indev_t *enc = NULL;
    ESP_RETURN_ON_ERROR(board_encoder_init(&enc), TAG, "encoder init failed");

    ESP_LOGI(TAG, "CrowPanel 1.46in %dx%d ready, touch %s",
             CROWPANEL_LCD_H_RES, CROWPANEL_LCD_V_RES, touch ? "on" : "FAILED");
    return ESP_OK;
}
