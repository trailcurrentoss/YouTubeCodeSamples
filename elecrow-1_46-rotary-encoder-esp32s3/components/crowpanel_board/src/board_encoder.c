/*
 * Rotary ring: quadrature rotation, short press, long press.
 *
 * Rotation is decoded in hardware by PCNT rather than a GPIO interrupt per
 * edge. The vendor demo uses an ISR, which drops counts exactly when the CPU
 * is busy -- and the CPU is busiest while the ring is turning, because LVGL
 * is redrawing. PCNT cannot miss a step.
 *
 * NO ABSOLUTE POSITION. The ring has no end stops, so this driver emits
 * direction only. The PCNT count is zeroed on every read (it is a delta, not
 * a position), and the sub-detent remainder is dropped the instant direction
 * reverses, so a reversal takes effect on the very next detent.
 *
 * Consumers must apply the delta to the DISPLAYED value and clamp the
 * result -- never keep a private running total and derive the value from it,
 * or overshooting a range leaves dead travel on the way back:
 *
 *     sel = clamp(sel + diff, 0, n - 1);      // correct
 *     raw += diff; sel = clamp(raw, 0, n-1);  // WRONG: raw keeps the overshoot
 */

#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "board_internal.h"
#include "board_pins.h"
#include "crowpanel_board.h"

static const char *TAG = "board.enc";

#ifdef CONFIG_CROWPANEL_ENCODER_INVERT
#  define ENC_INVERT 1
#else
#  define ENC_INVERT 0
#endif

/* The counter is drained every LVGL read, so these only need to cover one
 * read interval. */
#define PCNT_HIGH_LIMIT  1000
#define PCNT_LOW_LIMIT  -1000

static pcnt_unit_handle_t s_pcnt;
static lv_indev_t        *s_indev;

static int      s_residue;     /* partial detent, bounded by one detent */
static int      s_last_dir;    /* -1, 0, +1 */

static bool     s_btn_down;
static int64_t  s_btn_down_us;
static bool     s_long_fired;
static bool     s_short_pending;

static crowpanel_rotate_cb_t s_rotate_cb;
static void               *s_rotate_ctx;
static crowpanel_press_cb_t  s_press_cb;
static void               *s_press_ctx;
static crowpanel_press_cb_t  s_long_cb;
static void               *s_long_ctx;

static volatile int64_t s_last_input_us;

void board_note_input(void)
{
    s_last_input_us = esp_timer_get_time();
}

uint32_t crowpanel_board_ms_since_input(void)
{
    return (uint32_t)((esp_timer_get_time() - s_last_input_us) / 1000);
}

void crowpanel_board_set_rotate_callback(crowpanel_rotate_cb_t cb, void *ctx)
{
    s_rotate_cb  = cb;
    s_rotate_ctx = ctx;
}

void crowpanel_board_set_press_callback(crowpanel_press_cb_t cb, void *ctx)
{
    s_press_cb  = cb;
    s_press_ctx = ctx;
}

void crowpanel_board_set_long_press_callback(crowpanel_press_cb_t cb, void *ctx)
{
    s_long_cb  = cb;
    s_long_ctx = ctx;
}

bool board_button_is_down(void)
{
    return s_btn_down;
}

/* Polled from the LVGL read callback -- every few ms, which also acts as the
 * debounce for the switch's contact chatter. */
static void button_poll(void)
{
    const bool down = gpio_get_level(BOARD_ENC_BTN) == 0;
    const int64_t now = esp_timer_get_time();

    if (down && !s_btn_down) {
        s_btn_down    = true;
        s_btn_down_us = now;
        s_long_fired  = false;
        board_touch_note_push();
        board_note_input();
    } else if (down && s_btn_down && !s_long_fired) {
        if ((now - s_btn_down_us) / 1000 >= CONFIG_CROWPANEL_LONG_PRESS_MS) {
            s_long_fired = true;
            board_note_input();
            if (s_long_cb) {
                s_long_cb(s_long_ctx);
            }
        }
    } else if (!down && s_btn_down) {
        s_btn_down = false;
        board_note_input();
        /* A release that already fired the long press is consumed. */
        if (!s_long_fired) {
            s_short_pending = true;
        }
    }
}

static void encoder_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;

    int raw = 0;
    if (s_pcnt && pcnt_unit_get_count(s_pcnt, &raw) == ESP_OK && raw != 0) {
        pcnt_unit_clear_count(s_pcnt);

        const int dir = (raw > 0) ? 1 : -1;
        if (s_last_dir != 0 && dir != s_last_dir) {
            s_residue = 0;
        }
        s_last_dir = dir;
        s_residue += raw;

        const int per = CONFIG_CROWPANEL_ENCODER_STEPS_PER_DETENT;
        int detents = s_residue / per;
        s_residue  -= detents * per;

#if CONFIG_CROWPANEL_ENCODER_DEBUG
        ESP_LOGI(TAG, "raw=%+d residue=%+d detents=%+d", raw, s_residue, detents);
#endif

        if (detents != 0) {
#if ENC_INVERT
            detents = -detents;
#endif
            /* To the app if it asked, otherwise to LVGL. Never both. */
            if (s_rotate_cb) {
                s_rotate_cb(detents, s_rotate_ctx);
            } else {
                data->enc_diff = (int16_t)detents;
            }
            board_note_input();
        }
    }

    button_poll();

    if (s_short_pending) {
        s_short_pending = false;
        if (s_press_cb) {
            s_press_cb(s_press_ctx);
            data->state = LV_INDEV_STATE_RELEASED;
        } else {
            /* PRESSED for one read, released on the next: a clean ENTER. */
            data->state = LV_INDEV_STATE_PRESSED;
            data->continue_reading = true;
        }
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

esp_err_t board_encoder_init(lv_indev_t **out_indev)
{
    board_note_input();

    gpio_config_t btn = {
        .mode         = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL << BOARD_ENC_BTN,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&btn), TAG, "button gpio failed");

    pcnt_unit_config_t unit_cfg = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit  = PCNT_LOW_LIMIT,
    };
    ESP_RETURN_ON_ERROR(pcnt_new_unit(&unit_cfg, &s_pcnt), TAG, "pcnt unit failed");

    /* Kills electrical noise. Mechanical bounce nets to zero under full
     * quadrature decoding, so it needs no filter. */
    pcnt_glitch_filter_config_t filt = { .max_glitch_ns = 1000 };
    ESP_RETURN_ON_ERROR(pcnt_unit_set_glitch_filter(s_pcnt, &filt),
                        TAG, "glitch filter failed");

    pcnt_chan_config_t ch_a_cfg = { .edge_gpio_num = BOARD_ENC_A,
                                    .level_gpio_num = BOARD_ENC_B };
    pcnt_chan_config_t ch_b_cfg = { .edge_gpio_num = BOARD_ENC_B,
                                    .level_gpio_num = BOARD_ENC_A };
    pcnt_channel_handle_t ch_a = NULL, ch_b = NULL;
    ESP_RETURN_ON_ERROR(pcnt_new_channel(s_pcnt, &ch_a_cfg, &ch_a), TAG, "ch a failed");
    ESP_RETURN_ON_ERROR(pcnt_new_channel(s_pcnt, &ch_b_cfg, &ch_b), TAG, "ch b failed");

    ESP_RETURN_ON_ERROR(pcnt_channel_set_edge_action(ch_a,
        PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE),
        TAG, "ch a edge failed");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_level_action(ch_a,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE),
        TAG, "ch a level failed");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_edge_action(ch_b,
        PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE),
        TAG, "ch b edge failed");
    ESP_RETURN_ON_ERROR(pcnt_channel_set_level_action(ch_b,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE),
        TAG, "ch b level failed");

    /* No internal pull-ups on the phases: this board has external ones (the
     * vendor sketch uses bare INPUT), and adding the internal ones as well
     * would only weaken the edges. */

    ESP_RETURN_ON_ERROR(pcnt_unit_enable(s_pcnt),      TAG, "pcnt enable failed");
    ESP_RETURN_ON_ERROR(pcnt_unit_clear_count(s_pcnt), TAG, "pcnt clear failed");
    ESP_RETURN_ON_ERROR(pcnt_unit_start(s_pcnt),       TAG, "pcnt start failed");

    s_indev = lv_indev_create();
    ESP_RETURN_ON_FALSE(s_indev, ESP_FAIL, TAG, "lv_indev_create failed");
    lv_indev_set_type(s_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(s_indev, encoder_read_cb);

    ESP_LOGI(TAG, "encoder A=%d B=%d btn=%d, %d counts/detent%s",
             BOARD_ENC_A, BOARD_ENC_B, BOARD_ENC_BTN,
             CONFIG_CROWPANEL_ENCODER_STEPS_PER_DETENT,
             ENC_INVERT ? ", inverted" : "");

    *out_indev = s_indev;
    return ESP_OK;
}

lv_indev_t *crowpanel_board_encoder_indev(void) { return s_indev; }
