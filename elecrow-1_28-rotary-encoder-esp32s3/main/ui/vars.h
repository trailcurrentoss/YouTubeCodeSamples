#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations

// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_USB_STATUS_TEXT = 0,
    FLOW_GLOBAL_VARIABLE_AXIS_MODE_TEXT = 1,
    FLOW_GLOBAL_VARIABLE_ANGLE_TEXT = 2,
    FLOW_GLOBAL_VARIABLE_TIMECODE_TEXT = 3,
    FLOW_GLOBAL_VARIABLE_STEP_TEXT = 4,
    FLOW_GLOBAL_VARIABLE_SCROLL_STEP_TEXT = 5,
    FLOW_GLOBAL_VARIABLE_CLOCK_HOUR = 6,
    FLOW_GLOBAL_VARIABLE_CLOCK_MINUTE = 7,
    FLOW_GLOBAL_VARIABLE_CLOCK_SECOND = 8
};

// Native global variables

extern const char *get_var_usb_status_text();
extern void set_var_usb_status_text(const char *value);
extern const char *get_var_axis_mode_text();
extern void set_var_axis_mode_text(const char *value);
extern const char *get_var_angle_text();
extern void set_var_angle_text(const char *value);
extern const char *get_var_timecode_text();
extern void set_var_timecode_text(const char *value);
extern const char *get_var_step_text();
extern void set_var_step_text(const char *value);
extern const char *get_var_scroll_step_text();
extern void set_var_scroll_step_text(const char *value);
extern int32_t get_var_clock_hour();
extern void set_var_clock_hour(int32_t value);
extern int32_t get_var_clock_minute();
extern void set_var_clock_minute(int32_t value);
extern int32_t get_var_clock_second();
extern void set_var_clock_second(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/