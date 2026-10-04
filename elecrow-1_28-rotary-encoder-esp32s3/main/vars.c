/*
 * vars.c — HAND-WRITTEN. Implements the variable accessors EEZ Studio
 * declares in vars.h from the project's global variables.
 *
 * Lives in main/, NOT in main/ui/: main/ui/ is EEZ Studio's export and is
 * disposable -- it can be deleted and re-exported at any time, so nothing
 * hand-written goes there. Compiled only once an export exists (see
 * main/CMakeLists.txt).
 *
 * The getters are read every ui_tick() and hand their pointer straight to
 * LVGL, so each returns storage that outlives the call (app_model's static
 * buffers). The setters exist because EEZ declares them; the device never
 * writes these from the UI side.
 */

#include "vars.h"

#include "app_model.h"
#include "clock.h"
#include "ui_glue.h"

const char *get_var_usb_status_text(void) { return ui_glue_usb_status_text(); }
const char *get_var_axis_mode_text(void)  { return app_model_axis_mode_text(); }
const char *get_var_angle_text(void)      { return app_model_angle_text(); }
const char *get_var_timecode_text(void)   { return app_model_timecode_text(); }
const char *get_var_step_text(void)       { return app_model_step_text(); }
const char *get_var_scroll_step_text(void) { return app_model_scroll_step_text(); }

/* Idle-page clock hands, 0..3600 per revolution (clock.h). */
int32_t get_var_clock_hour(void)   { return clock_hour_hand(); }
int32_t get_var_clock_minute(void) { return clock_minute_hand(); }
int32_t get_var_clock_second(void) { return clock_second_hand(); }
void set_var_clock_hour(int32_t value)   { (void)value; }
void set_var_clock_minute(int32_t value) { (void)value; }
void set_var_clock_second(int32_t value) { (void)value; }

void set_var_usb_status_text(const char *value) { (void)value; }
void set_var_axis_mode_text(const char *value)  { (void)value; }
void set_var_angle_text(const char *value)      { (void)value; }
void set_var_timecode_text(const char *value)   { (void)value; }
void set_var_step_text(const char *value)       { (void)value; }
void set_var_scroll_step_text(const char *value) { (void)value; }
