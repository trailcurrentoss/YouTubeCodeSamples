/*
 * Short sound effects through the PCM5100A DAC.
 *
 * The same I2S path as TrailCurrent Tiercel's tiercel_audio (Waveshare's
 * Demo 07 underneath), trimmed to playback only:
 *
 *   - The DAC sits behind a CH445P switch whose select line is GPIO0 (also
 *     the BOOT strap). Driven HIGH, the S3 owns the DAC.
 *   - Standard I2S (Philips) framing -- the PCM5100A's FMT pin is tied low --
 *     16-bit, mono written to both slots. No MCLK: the DAC runs off BCK.
 *   - The channel runs CONTINUOUSLY, streaming silence between sounds.
 *     Enabling and disabling it around each sound put an audible pop on
 *     both edges -- louder than a quiet click.
 *
 * It is a LINE-LEVEL output (the 3.5 mm jack); there is no amplifier on the
 * board. The DAC's soft-mute line belongs to the other MCU (ESP32 IO32),
 * which its factory firmware holds unmuted.
 *
 * Playback runs on its own task so the caller (the LVGL task) never waits on
 * the DMA: knob_board_audio_play() only records which sound to play and
 * returns; it starts within one block (~12 ms). The task streams the sound
 * in blocks, scaling each by the volume as it copies it into internal RAM --
 * so a sound may be any length and may live in PSRAM.
 */

#include <string.h>

#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "board_internal.h"
#include "board_pins.h"
#include "knob_board.h"

static const char *TAG = "board.audio";

#define AUDIO_PORT       I2S_NUM_1
#define BLOCK_SAMPLES    256                             /* ~12 ms */

static i2s_chan_handle_t s_tx;
static TaskHandle_t      s_task;
static SemaphoreHandle_t s_lock;
/* The sound queued by knob_board_audio_play(), not yet picked up. */
static const int16_t    *s_next;
static size_t            s_next_len;
static bool              s_pending;
static volatile uint16_t s_gain_q8 = 256;
static knob_board_audio_tap_t s_tap;
static void *s_tap_ctx;

/* Feeds the DAC forever: the current sound a block at a time, silence
 * otherwise. A newly queued sound (or a stop) takes over at the next block.
 * i2s_channel_write() blocks on the DMA, which paces the loop. */
static void audio_task(void *arg)
{
    (void)arg;
    /* I2S DMA cannot read PSRAM: every block goes through internal RAM. */
    static int16_t block[BLOCK_SAMPLES];
    const int16_t *cur = NULL;
    size_t left = 0;
    size_t written = 0;
    for (;;) {
        xSemaphoreTake(s_lock, portMAX_DELAY);
        if (s_pending) {
            cur = s_next;
            left = s_next_len;
            s_pending = false;
        }
        xSemaphoreGive(s_lock);

        const size_t n = left < BLOCK_SAMPLES ? left : BLOCK_SAMPLES;
        const int32_t g = s_gain_q8;
        for (size_t i = 0; i < n; i++) {
            const int32_t v = (cur[i] * g) >> 8;
            block[i] = (int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : v);
        }
        memset(block + n, 0, (BLOCK_SAMPLES - n) * sizeof(int16_t));
        cur += n;
        left -= n;
        const knob_board_audio_tap_t tap = s_tap;
        if (tap && n) {
            tap(block, n, s_tap_ctx);
        }
        i2s_channel_write(s_tx, block, sizeof(block), &written, portMAX_DELAY);
    }
}

esp_err_t board_audio_init(void)
{
    /* Claim the DAC from the other MCU, for as long as this firmware runs. */
    const gpio_config_t route = {
        .pin_bit_mask = 1ULL << BOARD_AUDIO_ROUTE,
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&route), TAG, "route gpio");
    gpio_set_level(BOARD_AUDIO_ROUTE, 1);

    i2s_chan_config_t chan = I2S_CHANNEL_DEFAULT_CONFIG(AUDIO_PORT, I2S_ROLE_MASTER);
    chan.auto_clear_after_cb = true;      /* an underrun plays silence, not a loop */
    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan, &s_tx, NULL), TAG, "tx chan");
    const i2s_std_config_t std = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(KNOB_BOARD_AUDIO_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT,
                                                        I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = BOARD_I2S_BCLK,
            .ws   = BOARD_I2S_WS,
            .dout = BOARD_I2S_DOUT,
            .din  = I2S_GPIO_UNUSED,
        },
    };
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(s_tx, &std), TAG, "tx std");

    ESP_RETURN_ON_ERROR(i2s_channel_enable(s_tx), TAG, "tx enable");

    s_lock = xSemaphoreCreateMutex();
    if (!s_lock || xTaskCreate(audio_task, "audio", 3072, NULL, 3, &s_task) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "PCM5100A on I2S1 (BCK %d WS %d DOUT %d), %d Hz, line out",
             BOARD_I2S_BCLK, BOARD_I2S_WS, BOARD_I2S_DOUT, KNOB_BOARD_AUDIO_RATE);
    return ESP_OK;
}

void knob_board_audio_play(const int16_t *pcm, size_t samples)
{
    if (!s_task || !pcm || !samples) {
        return;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_next = pcm;
    s_next_len = samples;
    s_pending = true;
    xSemaphoreGive(s_lock);
}

void knob_board_audio_stop(void)
{
    if (!s_task) {
        return;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_next = NULL;
    s_next_len = 0;
    s_pending = true;
    xSemaphoreGive(s_lock);
}

void knob_board_audio_set_tap(knob_board_audio_tap_t cb, void *ctx)
{
    s_tap_ctx = ctx;
    s_tap = cb;
}

void knob_board_audio_set_gain(uint16_t gain_q8)
{
    s_gain_q8 = gain_q8;
}
