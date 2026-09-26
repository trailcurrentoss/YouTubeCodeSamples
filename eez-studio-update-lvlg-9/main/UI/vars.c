/*
 * vars.c — implements the native variable accessors EEZ Studio declares in
 * UI/vars.h from the project's global variables.
 *
 * Lives in main/UI/ because the full simulator only compiles what is in the
 * export folder (see dash_data.h for the detail).
 *
 * EEZ Studio's generated tick_screen() calls these getters every frame. Two
 * consequences drive the shape of this file:
 *
 *  1. String getters return a pointer straight to LVGL, so every one must
 *     point at storage that outlives the call. They all return into the
 *     dash_view_t, which is static.
 *
 *  2. Something has to advance the model, and it has to work in both builds.
 *     On the device main.c could call dash_data_tick() itself, but the
 *     simulator's main.c belongs to the eez-open simulator repo and only ever
 *     calls ui_init() and ui_tick(). So the pump is driven from the getters
 *     instead: whichever getter LVGL reaches first in a frame advances the
 *     model, throttled to DASH_PUMP_MS. That needs no cooperation from either
 *     main() and behaves identically in both.
 */

#include <stdint.h>
#include <stdio.h>   /* snprintf -- see the note in actions.c */

#include <lvgl.h>

#include "dash_data.h"
#include "wifi_ui.h"

#if __has_include("vars.h")
#include "vars.h"
#endif

/* 10 Hz is plenty for a power dashboard and keeps the snprintf work off the
 * render path. Raise it if a gauge ever looks steppy. */
#define DASH_PUMP_MS 100

static void pump(void)
{
    static uint32_t last;
    static bool started;

    const uint32_t now = lv_tick_get();

    if (!started) {
        started = true;
        last = now;
        dash_data_tick(now);
        wifi_ui_tick(now);
        return;
    }
    if ((uint32_t)(now - last) >= (uint32_t)DASH_PUMP_MS) {
        last = now;
        dash_data_tick(now);
        /* Same pump, so the Wi-Fi screen's async work advances whether or not
         * that screen is the one on display. */
        wifi_ui_tick(now);
    }
}

/* ---------------------------------------------------------------- battery */

int32_t get_var_batt_decivolts(void)
{
    pump();
    return dash_view()->batt_decivolts;
}
void set_var_batt_decivolts(int32_t value) { (void)value; }

const char *get_var_batt_percent_text(void)
{
    pump();
    return dash_view()->batt_percent_text;
}
void set_var_batt_percent_text(const char *value) { (void)value; }

const char *get_var_batt_volts_text(void)
{
    pump();
    return dash_view()->batt_volts_text;
}
void set_var_batt_volts_text(const char *value) { (void)value; }

const char *get_var_batt_state_text(void)
{
    pump();
    return dash_view()->batt_state_text;
}
void set_var_batt_state_text(const char *value) { (void)value; }

/* ------------------------------------------------------------------ solar */

int32_t get_var_solar_sweep_end(void)
{
    pump();
    return dash_view()->solar_sweep_end;
}
void set_var_solar_sweep_end(int32_t value) { (void)value; }

const char *get_var_solar_watts_text(void)
{
    pump();
    return dash_view()->solar_watts_text;
}
void set_var_solar_watts_text(const char *value) { (void)value; }

const char *get_var_solar_today_text(void)
{
    pump();
    return dash_view()->solar_today_text;
}
void set_var_solar_today_text(const char *value) { (void)value; }

/* ------------------------------------------------------------------ tanks */

int32_t get_var_fresh_percent(void)
{
    pump();
    return dash_view()->fresh_percent;
}
void set_var_fresh_percent(int32_t value) { (void)value; }

const char *get_var_fresh_percent_text(void)
{
    pump();
    return dash_view()->fresh_percent_text;
}
void set_var_fresh_percent_text(const char *value) { (void)value; }

int32_t get_var_grey_percent(void)
{
    pump();
    return dash_view()->grey_percent;
}
void set_var_grey_percent(int32_t value) { (void)value; }

const char *get_var_grey_percent_text(void)
{
    pump();
    return dash_view()->grey_percent_text;
}
void set_var_grey_percent_text(const char *value) { (void)value; }

/* ----------------------------------------------------------------- chrome */

const char *get_var_link_text(void)
{
    pump();
    return dash_view()->link_text;
}
void set_var_link_text(const char *value) { (void)value; }

const char *get_var_uptime_text(void)
{
    pump();
    return dash_view()->uptime_text;
}
void set_var_uptime_text(const char *value) { (void)value; }

/* ------------------------------------------------------------------- wi-fi */

const char *get_var_wifi_status_text(void)
{
    pump();
    return wifi_ui_status_text();
}
void set_var_wifi_status_text(const char *value) { (void)value; }

const char *get_var_wifi_selected_text(void)
{
    pump();
    return wifi_ui_selected_text();
}
void set_var_wifi_selected_text(const char *value) { (void)value; }

const char *get_var_broker_status_text(void)
{
    pump();
    return broker_ui_status_text();
}
void set_var_broker_status_text(const char *value) { (void)value; }
