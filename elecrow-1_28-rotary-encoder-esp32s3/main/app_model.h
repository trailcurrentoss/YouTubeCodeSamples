/*
 * app_model — what the macro pad is doing, independent of how it is drawn.
 *
 * Holds the focused app (from the companion), the per-app rotate axis and
 * the running angle / playhead estimates, and turns ring and key input into
 * HID keystrokes. Everything here runs in the LVGL task with the display
 * lock held (ring callbacks, LVGL events), or takes the lock first (the USB
 * focus callback in main.c).
 *
 * The angle and timecode are DISPLAY TALLIES of what this device has sent,
 * not readings from the app. Use the mouse in Blender or click the Kdenlive
 * timeline and they no longer match -- nothing here can see that.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "rotary_usb.h"

typedef enum { AXIS_X = 0, AXIS_Y, AXIS_Z, AXIS_COUNT } app_axis_t;

#define APP_MAX_CUTS 32

void         app_model_set_app(rotary_app_t app);
rotary_app_t app_model_app(void);

/* Input */
void app_model_rotate(int detents);
void app_model_push(void);
void app_model_key_tap(int index);      /* 0..7, clockwise from 12 o'clock */
void app_model_toggle_step(void);       /* Kdenlive centre tap */

/* State for the UI */
app_axis_t app_model_axis(void);        /* active axis of the current app */
int32_t    app_model_frame(void);
bool       app_model_step_is_second(void);
int        app_model_cuts(const int32_t **out);

/* Formatted strings; pointers stay valid until the next model change. */
const char *app_model_axis_mode_text(void);
const char *app_model_angle_text(void);
const char *app_model_timecode_text(void);
const char *app_model_step_text(void);
const char *app_model_scroll_step_text(void);

/* Set by ui_glue: called after every model change, in the LVGL task. */
typedef void (*app_model_changed_cb_t)(void);
void app_model_set_changed_callback(app_model_changed_cb_t cb);
