/*
 * dash_data — the dashboard's data model.
 *
 * This file lives in main/UI/ on purpose. EEZ Studio's full simulator copies
 * ONLY the export destination folder into its build container
 * (docker cp <destinationFolder>/. -> /project/src/) and then compiles every
 * .c it finds there, linking with -sLLD_REPORT_UNDEFINED. So anything the UI
 * references — action_*(), get_var_*() and the code behind them — has to be
 * inside this folder or the simulator will not link.
 *
 * Nothing here is overwritten by an EEZ Studio export: EEZ Studio only
 * rewrites the files it generates (screens.*, ui.*, styles.*, fonts.h,
 * images.*, actions.h, vars.h, ui_font_*.c).
 *
 * This translation unit is deliberately free of both ESP-IDF and LVGL
 * headers — it is plain C, so it compiles unchanged for xtensa/riscv and for
 * Emscripten. The LVGL-facing glue is in vars.c.
 */

#ifndef DASH_DATA_H
#define DASH_DATA_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Gauge domain. Battery is held in millivolts and exposed as the decivolts
 * the Scale/Arc widgets use, because LVGL 9's Scale range is integer-only. */
#define DASH_BATT_MV_MIN     10000   /* 10.0 V — scale floor   */
#define DASH_BATT_MV_MAX     14000   /* 14.0 V — scale ceiling */
#define DASH_SOLAR_W_MAX      1200   /* full array output      */

/* Arc sweep: LVGL angles, 0 = 3 o'clock, increasing clockwise. The gauge
 * starts bottom-left and sweeps 270 degrees through the top. */
#define DASH_ARC_START_ANGLE   135
#define DASH_ARC_SWEEP         270

/* Everything the UI reads, recomputed by dash_data_tick(). The char arrays
 * are handed to LVGL as const char * and must outlive the call, which is why
 * they are inline rather than pointers.
 *
 * The sizes are the worst case of the snprintf format, not the display width
 * of the value — "%ld" can emit 11 digits and a sign even though the gauge
 * will only ever show two. The comments give the hero string each field is
 * laid out for; the .eez-project owns the on-screen width. */
typedef struct {
    int32_t batt_decivolts;
    int32_t solar_sweep_end;
    int32_t fresh_percent;
    int32_t grey_percent;
    char    batt_percent_text[12];   /* hero "85"    -> placeholder "--"   */
    char    batt_volts_text[16];     /* hero "12.6"  -> placeholder "--.-" */
    char    batt_state_text[40];
    char    solar_watts_text[12];    /* hero "250"   -> placeholder "---"  */
    char    solar_today_text[48];    /* "SINCE BOOT  1.8 kWh" */
    char    fresh_percent_text[12];
    char    grey_percent_text[12];
    char    link_text[40];
    char    uptime_text[24];
} dash_view_t;

/* The current view. Read-only for callers; dash_data_tick() owns it. */
const dash_view_t *dash_view(void);

/* ---- inputs: called by the firmware when a reading arrives ----
 *
 * The shapes here mirror what Headwaters actually publishes on
 * local/energy/status and local/water/status, so nothing displayed on the
 * panel has to be inferred from something else:
 *
 *   battery_voltage   -> millivolts
 *   battery_percent   -> soc_tenths        (-1 when the field is absent)
 *   consumption_watts -> load_watts        (0 when not discharging)
 *   charge_type       -> charge_state      (NULL / "" when not charging)
 *   solar_watts       -> dash_data_set_solar()
 *   fresh / grey      -> dash_data_set_tanks()
 *
 * Current in amps is deliberately absent: the rig publishes watts, there is
 * no current sensor behind it, and deriving A = W / V would put a number on
 * screen that nothing measured. */
void dash_data_set_battery(int32_t millivolts, int32_t soc_tenths,
                           int32_t load_watts, const char *charge_state);
void dash_data_set_solar(int32_t watts);
void dash_data_set_tanks(int32_t fresh_pct, int32_t grey_pct);
void dash_data_set_link(bool up, const char *detail);

/* ---- lifecycle ---- */
void dash_data_init(void);

/* Recompute the view. now_ms is any monotonic millisecond clock. In the
 * simulator this also synthesises the readings. Safe to call at any rate. */
void dash_data_tick(uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif /* DASH_DATA_H */
