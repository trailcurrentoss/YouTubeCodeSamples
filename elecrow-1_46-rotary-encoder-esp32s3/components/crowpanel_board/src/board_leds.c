/*
 * The addressable LED ring: 8 WS2812s around the panel.
 *
 * Driven by the RMT peripheral with ESP-IDF's own bytes encoder: WS2812 bits
 * are one high/low pulse pair each, which is exactly what a bytes encoder
 * emits, so no extra component is needed. Colour order on the wire is GRB.
 * Ported from the TrailCurrent Capstan board layer.
 *
 * Every write is scaled by CONFIG_CROWPANEL_LEDS_MAX_BRIGHTNESS, which
 * defaults to full scale.
 *
 * TURN FEEDBACK
 *
 * While the ring turns, the half of the LED ring on the side it is turning
 * towards goes white: clockwise lights the right half, counter-clockwise the
 * left. Once the detents stop for LED_TURN_HOLD_US the ring goes back to the
 * colour set with crowpanel_board_leds_set_all(). A reversal mid-turn swaps
 * sides on the next detent. The bottom pair sit on neither side and keep the
 * base colour throughout (BOARD_WS2812_SIDE in board_pins.h).
 */

#include <string.h>

#include "driver/gpio.h"
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "board_internal.h"
#include "board_pins.h"
#include "crowpanel_board.h"

static const char *TAG = "board.leds";

/* 10 MHz: one tick is 100 ns. WS2812: a 0 is ~0.3 us high then ~0.9 us low,
 * a 1 is ~0.9 us high then ~0.3 us low; >50 us low latches the frame. */
#define LED_RMT_HZ        10000000

/* How long after the last detent the turn highlight stays up. Longer than
 * the gap between detents on a slow turn, or the ring flickers back to the
 * base colour mid-turn: LVGL reads the encoder every ~33 ms, and a slow
 * hand puts a few reads between detents. */
#define LED_TURN_HOLD_US  250000

static rmt_channel_handle_t s_chan;
static rmt_encoder_handle_t s_enc;
static uint8_t              s_base[3];                  /* RGB, as set */
static uint8_t              s_grb[BOARD_WS2812_COUNT * 3];
static uint8_t              s_sent[BOARD_WS2812_COUNT * 3];
static bool                 s_ready;
static bool                 s_sent_valid;               /* force first write */

/* Colours are set from the app, turns arrive from the LVGL task and the
 * highlight is cleared from the esp_timer task. */
static SemaphoreHandle_t    s_lock;
static esp_timer_handle_t   s_turn_timer;
static int                  s_turn_side;                /* -1, 0 (none), +1 */

/* Scale one channel by the brightness ceiling. */
static uint8_t scale(uint8_t v)
{
    return (uint8_t)(v * (unsigned)CONFIG_CROWPANEL_LEDS_MAX_BRIGHTNESS / 100u);
}

static void put(int i, const uint8_t rgb[3])
{
    s_grb[i * 3 + 0] = scale(rgb[1]);   /* GRB on the wire */
    s_grb[i * 3 + 1] = scale(rgb[0]);
    s_grb[i * 3 + 2] = scale(rgb[2]);
}

/* Send s_grb unless it is what the ring already shows. */
static void send(void)
{
    if (s_sent_valid && memcmp(s_grb, s_sent, sizeof(s_grb)) == 0) {
        return;
    }
    const rmt_transmit_config_t tx = { .loop_count = 0 };
    if (rmt_transmit(s_chan, s_enc, s_grb, sizeof(s_grb), &tx) != ESP_OK) {
        ESP_LOGW(TAG, "LED write failed");
        s_sent_valid = false;              /* retry on the next call */
        return;
    }
    memcpy(s_sent, s_grb, sizeof(s_grb));
    s_sent_valid = true;
}

/* Base colour with the turn highlight on top, sent. Call with s_lock held. */
static void render(void)
{
    static const int8_t  side[BOARD_WS2812_COUNT] = BOARD_WS2812_SIDE;
    static const uint8_t white[3] = { 255, 255, 255 };
    for (int i = 0; i < BOARD_WS2812_COUNT; i++) {
        put(i, (s_turn_side != 0 && side[i] == s_turn_side) ? white : s_base);
    }
    send();
}

static void turn_timer_cb(void *arg)
{
    (void)arg;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_turn_side = 0;
    render();
    xSemaphoreGive(s_lock);
}

esp_err_t board_leds_init(void)
{
    /* The ring's supply is gated; it stays dark without this high. */
    const gpio_config_t en = {
        .pin_bit_mask = 1ULL << BOARD_WS2812_EN,
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&en), TAG, "enable pin");
    gpio_set_level(BOARD_WS2812_EN, 1);

    const rmt_tx_channel_config_t ch = {
        .gpio_num = BOARD_WS2812_GPIO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_RMT_HZ,
        .mem_block_symbols = 48,
        .trans_queue_depth = 2,
    };
    ESP_RETURN_ON_ERROR(rmt_new_tx_channel(&ch, &s_chan), TAG, "rmt channel");

    const rmt_bytes_encoder_config_t enc = {
        .bit0 = { .level0 = 1, .duration0 = 3, .level1 = 0, .duration1 = 9 },
        .bit1 = { .level0 = 1, .duration0 = 9, .level1 = 0, .duration1 = 3 },
        .flags.msb_first = 1,
    };
    ESP_RETURN_ON_ERROR(rmt_new_bytes_encoder(&enc, &s_enc), TAG, "encoder");
    ESP_RETURN_ON_ERROR(rmt_enable(s_chan), TAG, "rmt enable");

    s_lock = xSemaphoreCreateMutex();
    ESP_RETURN_ON_FALSE(s_lock, ESP_ERR_NO_MEM, TAG, "lock");
    const esp_timer_create_args_t turn = {
        .callback = turn_timer_cb,
        .name = "led_turn",
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&turn, &s_turn_timer), TAG,
                        "turn timer");

    s_ready = true;
    ESP_LOGI(TAG, "%d LEDs on GPIO%d, max %d%%", BOARD_WS2812_COUNT,
             BOARD_WS2812_GPIO, CONFIG_CROWPANEL_LEDS_MAX_BRIGHTNESS);
    crowpanel_board_leds_set_all(0, 0, 0);  /* known state, not power-on noise */
    return ESP_OK;
}

void crowpanel_board_leds_set_all(uint8_t r, uint8_t g, uint8_t b)
{
    if (!s_ready) {
        return;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_base[0] = r;
    s_base[1] = g;
    s_base[2] = b;
    render();
    xSemaphoreGive(s_lock);
}

void board_leds_turn(int detents)
{
    if (!s_ready || detents == 0) {
        return;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    s_turn_side = detents > 0 ? 1 : -1;    /* clockwise: right half */
    render();
    esp_timer_stop(s_turn_timer);          /* not running is fine */
    esp_timer_start_once(s_turn_timer, LED_TURN_HOLD_US);
    xSemaphoreGive(s_lock);
}
