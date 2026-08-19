/*
 * Push-to-talk button for the Peregrine voice assistant.
 *
 * The button sits between header pin 35 (GPIO20) and ground: active-low,
 * internal pull-up. Hold it to ask a question, release it to send.
 *
 * A task polls the pin rather than taking an interrupt, because a press does
 * real work — starting capture, and on release handing off an upload — none of
 * which belongs in an ISR.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "record_button.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "board_esp32p4_wifi6.h"
#include "peregrine_voice.h"

static const char *TAG = "record_btn";

/* Poll interval and how long a level must hold to count as a real edge.
 * 3 stable samples at 10 ms rides out typical tact-switch bounce (~1-5 ms). */
#define POLL_MS          10
#define STABLE_SAMPLES   3

static void button_task(void *arg)
{
    (void)arg;

    /* Released = high (pull-up). Start from the actual level so a button held
     * across a reboot does not register as a press. */
    int level = gpio_get_level(BSP_RECORD_BUTTON);
    int candidate = level;
    int stable = 0;

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(POLL_MS));

        const int now = gpio_get_level(BSP_RECORD_BUTTON);
        if (now != candidate) {
            candidate = now;
            stable = 0;
            continue;
        }
        if (stable >= STABLE_SAMPLES || ++stable < STABLE_SAMPLES ||
            candidate == level) {
            continue;
        }
        level = candidate;

        if (!peregrine_voice_configured()) {
            if (level == 0) {
                ESP_LOGW(TAG, "Button pressed, but the voice assistant is not "
                              "configured — add PEREGRINE_URL and "
                              "PEREGRINE_VOICE_TOKEN to /sdcard/environment.conf");
            }
            continue;
        }

        if (level == 0) {
            peregrine_voice_press();
        } else {
            peregrine_voice_release();
        }
    }
}

esp_err_t record_button_init(void)
{
    const gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << BSP_RECORD_BUTTON,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    esp_err_t ret = gpio_config(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Button GPIO%d config failed (%s)",
                 BSP_RECORD_BUTTON, esp_err_to_name(ret));
        return ret;
    }

    if (xTaskCreate(button_task, "record_btn", 4096, NULL, 4, NULL) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Push-to-talk button ready on GPIO%d (header pin 35)",
             BSP_RECORD_BUTTON);
    return ESP_OK;
}
