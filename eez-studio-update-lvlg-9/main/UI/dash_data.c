/*
 * dash_data — data model shared by the firmware and the EEZ Studio simulator.
 *
 * The only difference between the two builds is where the numbers come from,
 * and that difference is one #ifdef on EEZ_LVGL_SIMULATOR — the symbol the
 * simulator's CMakeLists.txt defines (add_definitions(-DEEZ_LVGL_SIMULATOR)).
 *
 *   device     : main.c calls dash_data_set_*() as readings arrive.
 *   simulator  : sim_feed() below sweeps every gauge through its range so the
 *                layout can be checked against real-looking values.
 */

#include "dash_data.h"

#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ state */

typedef struct {
    int32_t  batt_mv;        /* millivolts                             */
    int32_t  soc_tenths;     /* published SOC x10, -1 when absent      */
    int32_t  load_watts;     /* consumption_watts, 0 = not discharging */
    char     charge_state[16];
    int32_t  solar_w;
    /* Wh since boot, accumulated in milliwatt-hours so a 250 W array still
     * moves the counter between ticks. Headwaters publishes instantaneous
     * solar_watts and no daily total, and the panel has no wall clock, so
     * "today" is not a thing this device can honestly claim. */
    int64_t  solar_mwh;
    uint32_t last_tick_ms;
    bool     have_tick;
    int32_t  fresh_pct;
    int32_t  grey_pct;
    bool     link_up;
    bool     have_battery;
    bool     have_solar;
    bool     have_tanks;
    char     link_detail[24];
    uint32_t boot_ms;
    bool     boot_set;
} dash_state_t;

static dash_state_t s;

/* Boot placeholders. Each one has the same character count as the hero value
 * it stands in for, so the centred value box does not jump when real data
 * lands. */
static dash_view_t v = {
    .batt_decivolts   = (DASH_BATT_MV_MIN + DASH_BATT_MV_MAX) / 2 / 100,
    .solar_sweep_end  = DASH_ARC_START_ANGLE,
    .batt_percent_text = "--",
    .batt_volts_text   = "--.-",
    .batt_state_text   = "WAITING FOR DATA",
    .solar_watts_text  = "---",
    .solar_today_text  = "SINCE BOOT  --.- kWh",
    .fresh_percent_text = "--",
    .grey_percent_text  = "--",
    .link_text          = "  LINK DOWN",
    .uptime_text        = "UP 00:00:00",
};

const dash_view_t *dash_view(void) { return &v; }

/* ------------------------------------------------------------------ helpers */

static int32_t clampi(int32_t x, int32_t lo, int32_t hi)
{
    return x < lo ? lo : (x > hi ? hi : x);
}

/* Resting-voltage state of charge for a 12 V LiFePO4 bank. Deliberately
 * coarse — this is a display hint, not a fuel gauge. */
static int32_t soc_from_mv(int32_t mv)
{
    static const int32_t curve[][2] = {
        { 10000,   0 }, { 12000,  10 }, { 12500,  20 }, { 12800,  40 },
        { 13000,  60 }, { 13200,  80 }, { 13400,  95 }, { 14000, 100 },
    };
    const int n = (int)(sizeof(curve) / sizeof(curve[0]));

    if (mv <= curve[0][0])     return curve[0][1];
    if (mv >= curve[n - 1][0]) return curve[n - 1][1];

    for (int i = 1; i < n; i++) {
        if (mv <= curve[i][0]) {
            const int32_t mv0 = curve[i - 1][0], mv1 = curve[i][0];
            const int32_t p0  = curve[i - 1][1], p1  = curve[i][1];
            return p0 + (mv - mv0) * (p1 - p0) / (mv1 - mv0);
        }
    }
    return 100;
}

/* Map 0..DASH_SOLAR_W_MAX onto the arc's 270-degree sweep. This is the value
 * the solar Arc widget's "End angle" property reads as an expression, which is
 * the LVGL-9 / EEZ 0.26 feature the arc is there to show. LVGL wants the angle
 * in 0..359, so the result wraps. */
static int32_t sweep_end_from_watts(int32_t watts)
{
    const int32_t w = clampi(watts, 0, DASH_SOLAR_W_MAX);
    int32_t end = DASH_ARC_START_ANGLE + (w * DASH_ARC_SWEEP) / DASH_SOLAR_W_MAX;
    while (end >= 360) end -= 360;
    return end;
}

/* ------------------------------------------------------------------ inputs */

void dash_data_set_battery(int32_t millivolts, int32_t soc_tenths,
                           int32_t load_watts, const char *charge_state)
{
    s.batt_mv     = millivolts;
    s.soc_tenths  = soc_tenths;
    s.load_watts  = load_watts;
    s.charge_state[0] = '\0';
    if (charge_state) {
        strncpy(s.charge_state, charge_state, sizeof(s.charge_state) - 1);
        s.charge_state[sizeof(s.charge_state) - 1] = '\0';
    }
    s.have_battery = true;
}

void dash_data_set_solar(int32_t watts)
{
    s.solar_w = watts;
    s.have_solar = true;
}

void dash_data_set_tanks(int32_t fresh_pct, int32_t grey_pct)
{
    s.fresh_pct = clampi(fresh_pct, 0, 100);
    s.grey_pct  = clampi(grey_pct, 0, 100);
    s.have_tanks = true;
}

void dash_data_set_link(bool up, const char *detail)
{
    s.link_up = up;
    s.link_detail[0] = '\0';
    if (detail) {
        strncpy(s.link_detail, detail, sizeof(s.link_detail) - 1);
        s.link_detail[sizeof(s.link_detail) - 1] = '\0';
    }
}

void dash_data_init(void)
{
    memset(&s, 0, sizeof(s));
    s.batt_mv    = (DASH_BATT_MV_MIN + DASH_BATT_MV_MAX) / 2;
    s.soc_tenths = -1;          /* no published SOC yet */
}

/* ------------------------------------------------- simulated readings only */
#ifdef EEZ_LVGL_SIMULATOR
/*
 * The laptop is not in the rig, so there is no real data here. This sweeps
 * every gauge through its range on deliberately different periods so nothing
 * pulses in lockstep — which makes a mis-sized value box obvious on screen.
 *
 * Integer triangle wave: lo -> hi -> lo over period_ms.
 */
static int32_t tri(uint32_t now_ms, uint32_t period_ms, int32_t lo, int32_t hi)
{
    const uint32_t half = period_ms / 2u;
    const uint32_t t    = now_ms % period_ms;
    const uint32_t up   = (t < half) ? t : (period_ms - t);
    return lo + (int32_t)((uint64_t)up * (uint64_t)(uint32_t)(hi - lo) / half);
}

static void sim_feed(uint32_t now_ms)
{
    /* 11.4 V .. 13.9 V over 24 s — crosses both warning sections. */
    const int32_t mv    = tri(now_ms, 24000u, 11400, 13900);
    const int32_t solar = tri(now_ms, 17000u, 0, 1150);
    /* Below half the sweep the rig is drawing from the bank; above it the
     * array is charging. Same shape the real fields produce, so the layout
     * is exercised in both states. */
    const bool    charging   = solar > 575;
    const int32_t load_watts = charging ? 0 : tri(now_ms, 24000u, 10, 240);

    dash_data_set_battery(mv,
                          tri(now_ms, 24000u, 180, 990),   /* SOC x10 */
                          load_watts,
                          charging ? "bulk" : NULL);
    dash_data_set_solar(solar);
    dash_data_set_tanks(tri(now_ms, 31000u, 8, 96),
                        tri(now_ms, 43000u, 4, 88));
    dash_data_set_link(true, "SIMULATOR");
}
#endif /* EEZ_LVGL_SIMULATOR */

/* ------------------------------------------------------------------- tick */

void dash_data_tick(uint32_t now_ms)
{
    if (!s.boot_set) {
        s.boot_ms  = now_ms;
        s.boot_set = true;
    }

#ifdef EEZ_LVGL_SIMULATOR
    sim_feed(now_ms);
#endif

    /* Integrate solar production. W * ms / 3600 = mWh. Guarded on have_tick
     * so the first call contributes nothing, and on a plausible dt so a
     * clock step cannot inject a spike. */
    if (s.have_tick && s.have_solar) {
        const uint32_t dt = now_ms - s.last_tick_ms;
        if (dt > 0u && dt < 10000u) {
            s.solar_mwh += ((int64_t)s.solar_w * (int64_t)dt) / 3600;
        }
    }
    s.last_tick_ms = now_ms;
    s.have_tick    = true;

    /* --- battery --- */
    if (s.have_battery) {
        const int32_t mv = clampi(s.batt_mv, DASH_BATT_MV_MIN, DASH_BATT_MV_MAX);
        v.batt_decivolts = mv / 100;

        /* Prefer the published SOC (it comes off the shunt); fall back to
         * the voltage curve only when the field is absent. */
        const int32_t pct = (s.soc_tenths >= 0) ? (s.soc_tenths + 5) / 10
                                                : soc_from_mv(s.batt_mv);
        snprintf(v.batt_percent_text, sizeof(v.batt_percent_text), "%ld",
                 (long)clampi(pct, 0, 100));
        snprintf(v.batt_volts_text, sizeof(v.batt_volts_text), "%ld.%01ld",
                 (long)(mv / 1000), (long)((mv % 1000) / 100));

        /* Watts, not amps — see the note in dash_data.h. The charge state
         * is the rig's own charge_type string rather than a number we would
         * have had to infer. */
        if (s.load_watts > 0) {
            snprintf(v.batt_state_text, sizeof(v.batt_state_text),
                     "DISCHARGING  %ld W", (long)s.load_watts);
        } else if (s.charge_state[0]) {
            char up[sizeof(s.charge_state)];
            size_t i = 0;
            for (; s.charge_state[i] && i < sizeof(up) - 1; i++) {
                const char c = s.charge_state[i];
                up[i] = (c >= 'a' && c <= 'z') ? (char)(c - 'a' + 'A') : c;
            }
            up[i] = '\0';
            snprintf(v.batt_state_text, sizeof(v.batt_state_text),
                     "CHARGING  %s", up);
        } else {
            snprintf(v.batt_state_text, sizeof(v.batt_state_text), "AT REST");
        }
    }

    /* --- solar --- */
    if (s.have_solar) {
        const int32_t w = clampi(s.solar_w, 0, DASH_SOLAR_W_MAX);
        v.solar_sweep_end = sweep_end_from_watts(w);
        snprintf(v.solar_watts_text, sizeof(v.solar_watts_text), "%ld", (long)w);
        /* Scale the unit rather than the value. A 500 W array needs two hours
         * to reach 1 kWh, so a fixed kWh format reads "0.0" for most of a
         * session and looks like a dead field. Wh until it earns the k. */
        const long wh = (long)(s.solar_mwh / 1000);
        if (wh < 1000) {
            snprintf(v.solar_today_text, sizeof(v.solar_today_text),
                     "SINCE BOOT  %ld Wh", wh);
        } else {
            snprintf(v.solar_today_text, sizeof(v.solar_today_text),
                     "SINCE BOOT  %ld.%01ld kWh", wh / 1000, (wh % 1000) / 100);
        }
    }

    /* --- tanks --- */
    if (s.have_tanks) {
        v.fresh_percent = s.fresh_pct;
        v.grey_percent  = s.grey_pct;
        snprintf(v.fresh_percent_text, sizeof(v.fresh_percent_text), "%ld",
                 (long)s.fresh_pct);
        snprintf(v.grey_percent_text, sizeof(v.grey_percent_text), "%ld",
                 (long)s.grey_pct);
    }

    /* --- link. The icon is folded into the merged "dashicons" font, so one
     *     label carries glyph + text in a single string. --- */
    if (s.link_up) {
        snprintf(v.link_text, sizeof(v.link_text), "  %s",
                 s.link_detail[0] ? s.link_detail : "LINK OK");
    } else {
        snprintf(v.link_text, sizeof(v.link_text), "  LINK DOWN");
    }

    /* --- uptime --- */
    const uint32_t up_s = (now_ms - s.boot_ms) / 1000u;
    snprintf(v.uptime_text, sizeof(v.uptime_text), "UP %02lu:%02lu:%02lu",
             (unsigned long)(up_s / 3600u),
             (unsigned long)((up_s / 60u) % 60u),
             (unsigned long)(up_s % 60u));
}
