/*
 * Battery: what the S3 can see of the power supply, which is one ADC pin.
 *
 * BATT_ADC (GPIO1, ADC1 channel 0) is the midpoint of a 10k/10k divider
 * (R62/R63) on the net the schematic calls "5V" -- the input of the 3.3 V
 * buck (Sch p.4, p.1). Waveshare's Demo 01 reads it the same way, x2, as the
 * "system voltage". With USB in, that net is the USB supply, about 5 V; on
 * the battery alone it follows the cell, 4.2 V full down to about 3.3 V.
 *
 * The charger has no status line to the S3, so "charging" is read off that
 * same voltage: well above a full cell means USB power is in. While it is,
 * the cell itself is out of sight, so there is no level to report -- only
 * that it is on the charger.
 *
 * Sampled on an esp_timer and smoothed, so a reading is a cached copy and
 * cheap to take from any task. The level is the voltage under whatever the
 * board is drawing at the time, read against a resting Li-ion curve, so it
 * reads a little low while the backlight and haptics run. Good enough to
 * warn that the battery is getting low, which is all it is for.
 */

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "board_internal.h"
#include "board_pins.h"
#include "knob_board.h"

static const char *TAG = "board.battery";

#define SAMPLE_PERIOD_US  (500 * 1000)
#define SAMPLES_PER_READ  8
#define LOG_EVERY_READS   60       /* the rail voltage to the log every 30 s */

/* USB in: the rail sits near 5 V, a full cell at 4.2 V. Apart, with a gap
 * so a rail sagging under load does not flicker between the two. */
#define EXT_ON_MV   4450
#define EXT_OFF_MV  4350

static adc_oneshot_unit_handle_t s_adc;
static adc_cali_handle_t         s_cali;
static esp_timer_handle_t        s_timer;
static bool                      s_present;

/* Written by the timer task, read by anyone. Each is a single aligned word,
 * and a reader that sees a mix of two consecutive samples is harmless. */
static volatile uint32_t s_rail_mv;      /* smoothed, 0 until the first read */
static volatile bool     s_external;
static uint32_t          s_reads;

/* Resting Li-ion voltage to percent. Steps in between are linear. */
static const struct { uint16_t mv; uint8_t pct; } CURVE[] = {
    { 4200, 100 }, { 4110, 90 }, { 4020, 80 }, { 3950, 70 }, { 3870, 60 },
    { 3840, 50 },  { 3800, 40 }, { 3770, 30 }, { 3730, 20 }, { 3690, 10 },
    { 3610, 5 },   { 3300, 0 },
};

static uint8_t percent_for(uint32_t mv)
{
    const size_t n = sizeof(CURVE) / sizeof(CURVE[0]);
    if (mv >= CURVE[0].mv) {
        return 100;
    }
    for (size_t i = 1; i < n; i++) {
        if (mv >= CURVE[i].mv) {
            const uint32_t span_mv = CURVE[i - 1].mv - CURVE[i].mv;
            const uint32_t span_pct = CURVE[i - 1].pct - CURVE[i].pct;
            return (uint8_t)(CURVE[i].pct + (mv - CURVE[i].mv) * span_pct / span_mv);
        }
    }
    return 0;
}

/* One reading of the rail in mV: the mean of a few samples, through the
 * eFuse calibration, times the divider. 0 if the ADC failed. */
static uint32_t read_rail_mv(void)
{
    int sum = 0;
    int got = 0;
    for (int i = 0; i < SAMPLES_PER_READ; i++) {
        int raw;
        int mv;
        if (adc_oneshot_read(s_adc, BOARD_BATT_ADC_CHANNEL, &raw) == ESP_OK &&
            adc_cali_raw_to_voltage(s_cali, raw, &mv) == ESP_OK) {
            sum += mv;
            got++;
        }
    }
    return got ? (uint32_t)(sum / got) * BOARD_BATT_DIVIDER : 0;
}

static void sample_cb(void *arg)
{
    (void)arg;
    const uint32_t mv = read_rail_mv();
    if (mv == 0) {
        return;
    }

    bool ext = s_external;
    if (!ext && mv >= EXT_ON_MV) {
        ext = true;
    } else if (ext && mv <= EXT_OFF_MV) {
        ext = false;
    }

    /* Smooth over about two seconds, except across a plug or unplug: start
     * again from the new reading, or a pulled cable would show the battery
     * as full for the seconds the average takes to come down from 5 V. */
    const uint32_t prev = s_rail_mv;
    s_rail_mv = (prev == 0 || ext != s_external) ? mv : (prev * 3 + mv) / 4;

    if (ext != s_external) {
        ESP_LOGI(TAG, "%s (rail %lu mV)", ext ? "USB power in" : "on battery",
                 (unsigned long)mv);
        s_external = ext;
    }
    if (++s_reads % LOG_EVERY_READS == 0) {
        if (s_external) {
            ESP_LOGI(TAG, "rail %lu mV, USB", (unsigned long)s_rail_mv);
        } else {
            ESP_LOGI(TAG, "rail %lu mV, battery %u%%", (unsigned long)s_rail_mv,
                     percent_for(s_rail_mv));
        }
    }
}

esp_err_t board_battery_init(void)
{
    const adc_oneshot_unit_init_cfg_t unit = { .unit_id = ADC_UNIT_1 };
    ESP_RETURN_ON_ERROR(adc_oneshot_new_unit(&unit, &s_adc), TAG, "adc unit");

    /* 12 dB: up to ~3.1 V at the pin, which covers 2.5 V (5 V / 2). */
    const adc_oneshot_chan_cfg_t chan = {
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_RETURN_ON_ERROR(adc_oneshot_config_channel(s_adc, BOARD_BATT_ADC_CHANNEL, &chan),
                        TAG, "adc channel");

    const adc_cali_curve_fitting_config_t cali = {
        .unit_id  = ADC_UNIT_1,
        .chan     = BOARD_BATT_ADC_CHANNEL,
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_RETURN_ON_ERROR(adc_cali_create_scheme_curve_fitting(&cali, &s_cali),
                        TAG, "adc calibration");

    sample_cb(NULL);       /* a reading is there before the first screen */

    const esp_timer_create_args_t args = {
        .callback = sample_cb,
        .name     = "battery",
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&args, &s_timer), TAG, "timer");
    ESP_RETURN_ON_ERROR(esp_timer_start_periodic(s_timer, SAMPLE_PERIOD_US), TAG, "start");

    s_present = true;
    ESP_LOGI(TAG, "rail %lu mV, %s", (unsigned long)s_rail_mv,
             s_external ? "USB power in" : "on battery");
    return ESP_OK;
}

bool knob_board_battery_get(knob_board_battery_t *out)
{
    const uint32_t mv = s_rail_mv;
    if (!s_present || mv == 0 || !out) {
        return false;
    }
    out->rail_mv = (uint16_t)mv;
    out->charging = s_external;
    out->percent = s_external ? 0 : percent_for(mv);   /* the cell is out of sight */
    return true;
}
