/*
 * crowpanel_board — Elecrow CrowPanel 1.46" Rotary (360x360, ESP32-S3).
 *
 * The only hardware-aware code in the firmware: the JD9855 SPI panel, the
 * CST816D/T capacitive touch controller, the rotary ring and its push button,
 * the WS2812 LED ring, and LVGL on top of them via esp_lvgl_port.
 *
 * Ported from the TrailCurrent Capstan board layer, trimmed to this one
 * board. Unlike Capstan, TOUCH IS ON: the touch indev is live from boot.
 *
 * Note that on this unit the whole panel is the ring's push button --
 * pressing the ring means pressing the glass. So one physical press can
 * produce a touch event AND a ring press. Screens that care must decide which
 * of the two they act on.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CROWPANEL_LCD_H_RES 360
#define CROWPANEL_LCD_V_RES 360

/**
 * Bring up the panel, touch, the rotary ring and LVGL.
 *
 * On return LVGL is running on its own task and the backlight is still OFF.
 * Draw the first screen, then call crowpanel_board_backlight_set() so the
 * panel never shows an unpainted frame.
 */
esp_err_t crowpanel_board_init(void);

/**
 * Take the LVGL mutex. Every lv_* call from a task other than LVGL's own must
 * be wrapped in this. LVGL callbacks (timers, event handlers, and the ring
 * callbacks below) already hold it -- taking it again there deadlocks.
 *
 * @param timeout_ms  0 waits forever.
 */
bool crowpanel_board_lock(uint32_t timeout_ms);
void crowpanel_board_unlock(void);

/** Backlight, 0-100 percent (LEDC PWM). */
esp_err_t crowpanel_board_backlight_set(uint8_t percent);

/**
 * Set every LED in the ring to one colour (0-255 per channel, scaled by
 * CONFIG_CROWPANEL_LEDS_MAX_BRIGHTNESS). Repeating the current colour costs
 * nothing -- it is not re-sent. Safe from any task.
 *
 * This is the ring's resting colour. While the ring turns, the half on the
 * side it is turning towards shows white on top of it, then returns to this.
 */
void crowpanel_board_leds_set_all(uint8_t r, uint8_t g, uint8_t b);

/** The touch indev (LV_INDEV_TYPE_POINTER). NULL if touch failed to start. */
lv_indev_t *crowpanel_board_touch_indev(void);

/** The ring as an LV_INDEV_TYPE_ENCODER, for attaching lv_group_t objects. */
lv_indev_t *crowpanel_board_encoder_indev(void);

/**
 * Ring rotation as a signed number of detents. RELATIVE ONLY -- the ring has
 * no end stops, so there is no absolute position to read, and nothing above
 * this layer should keep a running total and derive state from it.
 *
 * Runs in the LVGL task with the display lock already held. While a callback
 * is registered, rotation is NOT also delivered to LVGL as enc_diff. Pass
 * NULL to hand rotation back to LVGL's focus handling.
 */
typedef void (*crowpanel_rotate_cb_t)(int detents, void *ctx);
void crowpanel_board_set_rotate_callback(crowpanel_rotate_cb_t cb, void *ctx);

/**
 * Short ring press. Same contract as the rotate callback: LVGL task, lock
 * held, and while registered the press is NOT also sent to LVGL as
 * LV_KEY_ENTER.
 */
typedef void (*crowpanel_press_cb_t)(void *ctx);
void crowpanel_board_set_press_callback(crowpanel_press_cb_t cb, void *ctx);

/**
 * Long ring press (CONFIG_CROWPANEL_LONG_PRESS_MS). Never reaches LVGL, and a
 * long press never also fires the short-press callback.
 */
void crowpanel_board_set_long_press_callback(crowpanel_press_cb_t cb, void *ctx);

/**
 * True if the ring's push switch closed during the current (or most recent)
 * finger contact on the glass.
 *
 * On this unit the whole panel is the ring's button, so a firm press that
 * is meant as a ring push also lands on the glass as a tap. Touch handlers
 * call this and ignore the tap when it returns true -- the push has already
 * been handled as a push.
 */
bool crowpanel_board_touch_contact_pushed(void);

/** Milliseconds since the last touch, rotation or press. */
uint32_t crowpanel_board_ms_since_input(void);

#ifdef __cplusplus
}
#endif
