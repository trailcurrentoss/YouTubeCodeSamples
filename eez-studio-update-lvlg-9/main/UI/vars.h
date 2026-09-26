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
    FLOW_GLOBAL_VARIABLE_BATT_DECIVOLTS = 0,
    FLOW_GLOBAL_VARIABLE_BATT_PERCENT_TEXT = 1,
    FLOW_GLOBAL_VARIABLE_BATT_VOLTS_TEXT = 2,
    FLOW_GLOBAL_VARIABLE_BATT_STATE_TEXT = 3,
    FLOW_GLOBAL_VARIABLE_SOLAR_SWEEP_END = 4,
    FLOW_GLOBAL_VARIABLE_SOLAR_WATTS_TEXT = 5,
    FLOW_GLOBAL_VARIABLE_SOLAR_TODAY_TEXT = 6,
    FLOW_GLOBAL_VARIABLE_FRESH_PERCENT = 7,
    FLOW_GLOBAL_VARIABLE_FRESH_PERCENT_TEXT = 8,
    FLOW_GLOBAL_VARIABLE_GREY_PERCENT = 9,
    FLOW_GLOBAL_VARIABLE_GREY_PERCENT_TEXT = 10,
    FLOW_GLOBAL_VARIABLE_LINK_TEXT = 11,
    FLOW_GLOBAL_VARIABLE_UPTIME_TEXT = 12,
    FLOW_GLOBAL_VARIABLE_WIFI_STATUS_TEXT = 13,
    FLOW_GLOBAL_VARIABLE_WIFI_SELECTED_TEXT = 14,
    FLOW_GLOBAL_VARIABLE_BROKER_STATUS_TEXT = 15
};

// Native global variables

extern int32_t get_var_batt_decivolts();
extern void set_var_batt_decivolts(int32_t value);
extern const char *get_var_batt_percent_text();
extern void set_var_batt_percent_text(const char *value);
extern const char *get_var_batt_volts_text();
extern void set_var_batt_volts_text(const char *value);
extern const char *get_var_batt_state_text();
extern void set_var_batt_state_text(const char *value);
extern int32_t get_var_solar_sweep_end();
extern void set_var_solar_sweep_end(int32_t value);
extern const char *get_var_solar_watts_text();
extern void set_var_solar_watts_text(const char *value);
extern const char *get_var_solar_today_text();
extern void set_var_solar_today_text(const char *value);
extern int32_t get_var_fresh_percent();
extern void set_var_fresh_percent(int32_t value);
extern const char *get_var_fresh_percent_text();
extern void set_var_fresh_percent_text(const char *value);
extern int32_t get_var_grey_percent();
extern void set_var_grey_percent(int32_t value);
extern const char *get_var_grey_percent_text();
extern void set_var_grey_percent_text(const char *value);
extern const char *get_var_link_text();
extern void set_var_link_text(const char *value);
extern const char *get_var_uptime_text();
extern void set_var_uptime_text(const char *value);
extern const char *get_var_wifi_status_text();
extern void set_var_wifi_status_text(const char *value);
extern const char *get_var_wifi_selected_text();
extern void set_var_wifi_selected_text(const char *value);
extern const char *get_var_broker_status_text();
extern void set_var_broker_status_text(const char *value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/