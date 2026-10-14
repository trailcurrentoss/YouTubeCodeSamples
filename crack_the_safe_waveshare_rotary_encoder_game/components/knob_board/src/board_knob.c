/*
 * The knob: an Alps SSCM110100 bidirectional detector switch.
 *
 * It is NOT a quadrature encoder, so PCNT does not apply. Each detent closes
 * one of two contacts to ground -- A turning clockwise, B anticlockwise --
 * and opens it again as the detent settles. Waveshare's Demo 04
 * (bidi_switch_knob.c) counts a detent on the RISING edge of a contact that
 * has been seen low for at least DEBOUNCE_TICKS polls; this is the same rule.
 *
 * Polled from an esp_timer every 3 ms, as the demo does, rather than from GPIO
 * interrupts: contact bounce on a mechanical switch produces bursts of edges,
 * and a debounced poll at 3 ms is both simpler and cheaper than an ISR that
 * has to reject most of what it sees.
 *
 * Detents accumulate in an atomic counter and are delivered to the UI from an
 * lv_timer, so the rotate callback always runs in the LVGL task with the
 * display lock held.
 */

#include <stdatomic.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "board_internal.h"
#include "board_pins.h"
#include "knob_board.h"

static const char *TAG = "board.knob";

#define POLL_US         3000
#define DEBOUNCE_TICKS  2
#define DELIVER_MS      10

typedef struct {
    gpio_num_t pin;
    int        dir;          /* +1 clockwise, -1 anticlockwise */
    uint8_t    prev;
    uint8_t    low_ticks;
} contact_t;

static contact_t s_contacts[2] = {
    { .pin = BOARD_KNOB_A, .dir = +1 },
    { .pin = BOARD_KNOB_B, .dir = -1 },
};

#ifdef CONFIG_KNOB_BOARD_INVERT
#  define KNOB_SIGN (-1)
#else
#  define KNOB_SIGN (+1)
#endif

static atomic_int                 s_pending;
static esp_timer_handle_t         s_poll;
static knob_board_rotate_cb_t  s_cb;
static void                      *s_cb_ctx;

static void poll_cb(void *arg)
{
    (void)arg;
    for (int i = 0; i < 2; i++) {
        contact_t *c = &s_contacts[i];
        const uint8_t level = (uint8_t)gpio_get_level(c->pin);
        if (level == 0) {
            if (c->low_ticks < 255) {
                c->low_ticks++;
            }
        } else if (c->prev == 0) {
            /* Released. A contact that was low for only one poll is bounce
             * from the neighbouring detent, not a detent of its own. */
            if (c->low_ticks >= DEBOUNCE_TICKS) {
                atomic_fetch_add(&s_pending, c->dir * KNOB_SIGN);
            }
            c->low_ticks = 0;
        }
        c->prev = level;
    }
}

static void deliver_cb(lv_timer_t *t)
{
    (void)t;
    const int diff = atomic_exchange(&s_pending, 0);
    if (diff == 0) {
        return;
    }
    board_note_input();
    if (s_cb) {
        s_cb(diff, s_cb_ctx);
    }
}

void knob_board_set_rotate_callback(knob_board_rotate_cb_t cb, void *ctx)
{
    s_cb     = cb;
    s_cb_ctx = ctx;
}

esp_err_t board_knob_init(void)
{
    const gpio_config_t io = {
        .pin_bit_mask = (1ULL << BOARD_KNOB_A) | (1ULL << BOARD_KNOB_B),
        .mode         = GPIO_MODE_INPUT,
        /* 10k external pull-ups are fitted; the internal one is belt and
         * braces against a missing resistor reading as a held contact. */
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io), TAG, "gpio");
    for (int i = 0; i < 2; i++) {
        s_contacts[i].prev = (uint8_t)gpio_get_level(s_contacts[i].pin);
    }

    const esp_timer_create_args_t args = {
        .callback = poll_cb,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "knob",
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&args, &s_poll), TAG, "timer");
    ESP_RETURN_ON_ERROR(esp_timer_start_periodic(s_poll, POLL_US), TAG, "start");

    /* Created under the LVGL lock by knob_board_init(). */
    lv_timer_create(deliver_cb, DELIVER_MS, NULL);

    ESP_LOGI(TAG, "knob on A=%d (cw) B=%d (ccw), polled every %d us",
             BOARD_KNOB_A, BOARD_KNOB_B, POLL_US);
    return ESP_OK;
}
