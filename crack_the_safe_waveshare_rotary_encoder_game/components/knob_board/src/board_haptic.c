/*
 * DRV2605 haptic driver, internal trigger, ROM effect library.
 *
 * Bring-up mirrors what Waveshare's Demo 03 does through SensorLib's
 * SensorDRV2605::begin(): out of standby, clear the waveform slots and the
 * overdrive/sustain/brake offsets, ERM open-loop, then select a library and
 * internal-trigger mode -- EXCEPT the motor type. The demo drives it as an
 * ERM, but OUT+/OUT- go to an LRA (schematic: LRA_P / LRA_N), and an LRA
 * driven as an ERM barely moves. So it is driven as an LRA, open loop at
 * OL_LRA_PERIOD, with the LRA library (Kconfig: Knob board > Haptic motor).
 *
 * Playing is "write the effect ids into the waveform slots, terminate with 0,
 * set GO". The driver plays from its own ROM and the I2C bus is free again in
 * well under a millisecond, so this runs inline on the caller's task.
 */

#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "board_internal.h"
#include "board_pins.h"
#include "knob_board.h"

static const char *TAG = "board.haptic";

#define MODE_INTTRIG   0x00   /* ROM effects, started by GO */
#define MODE_RTP       0x05   /* real-time playback: RTP_IN is the drive level */

#define REG_STATUS     0x00
#define REG_MODE       0x01
#define REG_RTP        0x02
#define REG_LIBRARY    0x03
#define REG_WAVESEQ1   0x04   /* ... REG_WAVESEQ8 = 0x0B */
#define REG_GO         0x0C
#define REG_OVERDRIVE  0x0D
#define REG_SUSTAINPOS 0x0E
#define REG_SUSTAINNEG 0x0F
#define REG_BREAK      0x10
#define REG_AUDIOMAX   0x13
#define REG_FEEDBACK   0x1A
#define REG_CONTROL3   0x1D
#define REG_OL_LRA_PER 0x20

#define WAVESEQ_SLOTS  8

static i2c_master_dev_handle_t s_dev;
static bool                    s_present;
static esp_timer_handle_t      s_rtp_timer;
static volatile bool           s_rtp_on;

static esp_err_t wr(uint8_t reg, uint8_t val)
{
    const uint8_t buf[2] = { reg, val };
    return i2c_master_transmit(s_dev, buf, sizeof(buf), 50);
}

static esp_err_t rd(uint8_t reg, uint8_t *val)
{
    return i2c_master_transmit_receive(s_dev, &reg, 1, val, 1, 50);
}

esp_err_t board_haptic_init(i2c_master_bus_handle_t bus, bool *present)
{
    *present = false;
    const i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = BOARD_HAPTIC_ADDR,
        .scl_speed_hz    = BOARD_I2C_HZ,
    };
    ESP_RETURN_ON_ERROR(i2c_master_bus_add_device(bus, &cfg, &s_dev), TAG,
                        "add device");

    uint8_t status = 0;
    if (rd(REG_STATUS, &status) != ESP_OK) {
        /* Not fatal: the game still runs, it just cannot be felt. */
        ESP_LOGW(TAG, "DRV2605 did not answer at 0x%02X -- haptics off",
                 BOARD_HAPTIC_ADDR);
        return ESP_OK;
    }
    const uint8_t id = status >> 5;   /* 3 = DRV2605, 7 = DRV2605L */

    /* Each step checked on its own: OR-ing esp_err_t codes together would
     * report a nonsense code on failure. */
    uint8_t fb = 0, c3 = 0;
    ESP_RETURN_ON_ERROR(wr(REG_MODE, 0x00), TAG, "standby off");
    ESP_RETURN_ON_ERROR(wr(REG_RTP, 0x00), TAG, "rtp");
    ESP_RETURN_ON_ERROR(wr(REG_WAVESEQ1, 1), TAG, "seq1");
    ESP_RETURN_ON_ERROR(wr(REG_WAVESEQ1 + 1, 0), TAG, "seq2");
    ESP_RETURN_ON_ERROR(wr(REG_OVERDRIVE, 0), TAG, "overdrive");
    ESP_RETURN_ON_ERROR(wr(REG_SUSTAINPOS, 0), TAG, "sustain+");
    ESP_RETURN_ON_ERROR(wr(REG_SUSTAINNEG, 0), TAG, "sustain-");
    ESP_RETURN_ON_ERROR(wr(REG_BREAK, 0), TAG, "brake");
    ESP_RETURN_ON_ERROR(wr(REG_AUDIOMAX, 0x64), TAG, "audiomax");
    ESP_RETURN_ON_ERROR(rd(REG_FEEDBACK, &fb), TAG, "read feedback");
    ESP_RETURN_ON_ERROR(rd(REG_CONTROL3, &c3), TAG, "read control3");
#ifdef CONFIG_KNOB_BOARD_HAPTIC_MOTOR_LRA
    ESP_RETURN_ON_ERROR(wr(REG_FEEDBACK, fb | 0x80), TAG, "lra");   /* N_ERM_LRA = LRA */
    /* Open loop at a fixed period: closed-loop LRA would need an
     * auto-calibration run per motor. */
    ESP_RETURN_ON_ERROR(wr(REG_OL_LRA_PER, CONFIG_KNOB_BOARD_HAPTIC_LRA_PERIOD), TAG,
                        "lra period");
    ESP_RETURN_ON_ERROR(wr(REG_CONTROL3, c3 | 0x01), TAG, "lra open loop");
#else
    ESP_RETURN_ON_ERROR(wr(REG_FEEDBACK, fb & 0x7F), TAG, "erm");   /* N_ERM_LRA = ERM */
    ESP_RETURN_ON_ERROR(wr(REG_CONTROL3, c3 | 0x20), TAG, "erm open loop");
#endif
    ESP_RETURN_ON_ERROR(wr(REG_LIBRARY, CONFIG_KNOB_BOARD_HAPTIC_ROM_LIBRARY), TAG,
                        "library");
    ESP_RETURN_ON_ERROR(wr(REG_MODE, 0x00), TAG, "internal trigger");

    s_present = true;
    *present = true;
    ESP_LOGI(TAG, "DRV2605%s (id %u) ready, library %d, %s",
             id == 7 ? "L" : "", (unsigned)id, CONFIG_KNOB_BOARD_HAPTIC_ROM_LIBRARY,
#ifdef CONFIG_KNOB_BOARD_HAPTIC_MOTOR_LRA
             "LRA open loop"
#else
             "ERM"
#endif
             );
    return ESP_OK;
}

/* Leave real-time mode: drive to zero first so the motor stops cleanly,
 * then back to internal trigger for the ROM effects. */
static void rtp_off(void)
{
    if (s_rtp_on) {
        wr(REG_RTP, 0);
        wr(REG_MODE, MODE_INTTRIG);
        s_rtp_on = false;
    }
}

static void rtp_timer_cb(void *arg)
{
    (void)arg;
    rtp_off();
}

void knob_board_haptic_pulse(uint8_t level, uint16_t ms)
{
    if (!s_present) {
        return;
    }
    if (!s_rtp_timer) {
        const esp_timer_create_args_t args = {
            .callback = rtp_timer_cb,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "haptic_rtp",
        };
        if (esp_timer_create(&args, &s_rtp_timer) != ESP_OK) {
            return;
        }
    }
    esp_timer_stop(s_rtp_timer);          /* fine if it was not running */
    if (level > 127) {
        level = 127;                      /* RTP_IN is signed by default */
    }
    if (level == 0) {
        rtp_off();
        return;
    }
    if (!s_rtp_on) {
        wr(REG_GO, 0);                    /* cut any ROM effect still playing */
        if (wr(REG_MODE, MODE_RTP) != ESP_OK) {
            return;
        }
        s_rtp_on = true;
    }
    wr(REG_RTP, level);
    esp_timer_start_once(s_rtp_timer, (uint64_t)ms * 1000u);
}

void knob_board_haptic_sequence(const uint8_t *effects, size_t count)
{
    if (!s_present || count == 0) {
        return;
    }
    if (s_rtp_timer) {
        esp_timer_stop(s_rtp_timer);
    }
    rtp_off();
    if (count > WAVESEQ_SLOTS) {
        count = WAVESEQ_SLOTS;
    }
    /* Stop anything still playing so the new sequence starts now, not after
     * the old one drains. */
    esp_err_t err = wr(REG_GO, 0);
    for (size_t i = 0; err == ESP_OK && i < count; i++) {
        err = wr(REG_WAVESEQ1 + i, effects[i]);
    }
    if (err == ESP_OK && count < WAVESEQ_SLOTS) {
        err = wr(REG_WAVESEQ1 + count, 0);   /* end of sequence */
    }
    if (err == ESP_OK) {
        err = wr(REG_GO, 1);
    }
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "effect %u not sent: %s", (unsigned)effects[0],
                 esp_err_to_name(err));
    }
}

void knob_board_haptic(uint8_t effect)
{
    knob_board_haptic_sequence(&effect, 1);
}
