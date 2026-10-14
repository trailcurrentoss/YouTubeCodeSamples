/*
 * knob_board -- the only board-aware code in the firmware.
 *
 * One board sits behind this header: the Waveshare ESP32-S3-Knob-Touch-LCD-1.8
 * (360x360 ST77916 on QSPI, CST816 touch, a detent knob with no push, and a
 * DRV2605 haptic driver). The game above it never touches a pin.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KNOB_BOARD_H_RES 360
#define KNOB_BOARD_V_RES 360

/**
 * Bring up the panel, touch, knob, haptics and LVGL.
 *
 * On return LVGL runs on its own task and the panel is dark -- call
 * knob_board_backlight_set() once the first screen is drawn, so an
 * unpainted frame is never shown.
 */
esp_err_t knob_board_init(void);

/**
 * The LVGL mutex. Wrap every lv_* call made from a task other than LVGL's.
 * LVGL's own callbacks (timers, events, the rotate callback below) already
 * hold it -- taking it again there deadlocks.
 *
 * @param timeout_ms  0 waits forever.
 */
bool knob_board_lock(uint32_t timeout_ms);
void knob_board_unlock(void);

/** Backlight 0-100, LEDC PWM. */
esp_err_t knob_board_backlight_set(uint8_t percent);

/** The touch input device. NULL if the CST816 did not start. */
lv_indev_t *knob_board_touch_indev(void);

/**
 * Knob rotation, as a signed number of detents (+ is clockwise).
 *
 * Delivered from an lv_timer, so it runs in the LVGL task with the display
 * lock ALREADY HELD: call lv_* freely, never knob_board_lock() inside.
 * Detents are relative; the board keeps no running total.
 */
typedef void (*knob_board_rotate_cb_t)(int diff, void *ctx);
void knob_board_set_rotate_callback(knob_board_rotate_cb_t cb, void *ctx);

/* ----------------------------------------------------------------------
 * Haptics: DRV2605, internal trigger, ROM waveform library.
 *
 * Effects are ids from the DRV2605 ROM library table (datasheet section
 * 11.2) -- 1 is "Strong Click 100%", 10 "Double Click 100%", 14 "Strong Buzz
 * 100%", 82 "Transition Ramp Up Long Smooth 1".
 *
 * Fire-and-forget: the call writes the waveform slots and sets GO, and the
 * driver plays from its own ROM. A new call interrupts whatever is still
 * playing, which is what a game wants -- the latest event is the one that
 * matters. Both are no-ops when the DRV2605 did not answer at boot.
 * ---------------------------------------------------------------------- */

/** True if the DRV2605 answered at boot. */
bool knob_board_has_haptics(void);

/** Play one ROM effect. */
void knob_board_haptic(uint8_t effect);

/**
 * Play up to 8 ROM effects back to back. An entry with bit 7 set is a pause
 * of (entry & 0x7F) x 10 ms rather than an effect -- the DRV2605's own
 * waveform-sequencer encoding.
 */
void knob_board_haptic_sequence(const uint8_t *effects, size_t count);

/**
 * A pulse at a chosen strength, for feedback that has to be graded rather
 * than picked from the ROM table: the DRV2605's real-time playback mode, the
 * motor driven at `level` (0..127, 127 = full) for `ms` milliseconds, then
 * back to the ROM mode the other calls use. A new pulse or effect replaces
 * one still running.
 */
void knob_board_haptic_pulse(uint8_t level, uint16_t ms);

/* ----------------------------------------------------------------------
 * Sound: short effects through the PCM5100A DAC, out of the 3.5 mm jack
 * (line level -- no amplifier on the board).
 * ---------------------------------------------------------------------- */

/** Sample rate knob_board_audio_play() expects. */
#define KNOB_BOARD_AUDIO_RATE 22050

/**
 * Play a sound: signed 16-bit mono at KNOB_BOARD_AUDIO_RATE, any length.
 * The samples are NOT copied -- they are read while the sound plays, so the
 * buffer must stay valid and unchanged (a static or a long-lived heap
 * buffer; PSRAM is fine). Returns at once; playback starts within ~12 ms
 * and replaces whatever is playing.
 */
void knob_board_audio_play(const int16_t *pcm, size_t samples);

/** Cut the current sound off (within ~12 ms). */
void knob_board_audio_stop(void);

/**
 * Volume, as a gain applied to every sample: 256 plays sounds as they are,
 * 0 is silent, above 256 is louder (clipped at full scale).
 */
void knob_board_audio_set_gain(uint16_t gain_q8);

/**
 * A tap on the output: `cb` receives every block of sound as it goes to the
 * DAC (gain applied, KNOB_BOARD_AUDIO_RATE mono; silence between sounds is
 * not passed). It runs on the audio task, so it must be quick -- the next
 * block is due ~12 ms later. NULL removes it.
 */
typedef void (*knob_board_audio_tap_t)(const int16_t *pcm, size_t samples, void *ctx);
void knob_board_audio_set_tap(knob_board_audio_tap_t cb, void *ctx);

/* ----------------------------------------------------------------------
 * The ESP32 co-processor: a text-line UART link (115200 8N1).
 *
 * What the lines mean is up to the firmware on each side; the board only
 * moves them. Lines are delivered without their newline, on the board's own
 * reader task -- NOT the LVGL task, so take knob_board_lock() before any lv_*
 * call there.
 * ---------------------------------------------------------------------- */

typedef void (*knob_board_coproc_line_cb_t)(const char *line, void *ctx);

/** Open the link. Call once; `cb` receives every line the ESP32 sends. */
esp_err_t knob_board_coproc_start(knob_board_coproc_line_cb_t cb, void *ctx);

/** Send `len` bytes as one line (a newline is appended). Thread-safe; returns
 *  once the bytes are queued. A no-op before knob_board_coproc_start(). */
void knob_board_coproc_send(const char *line, size_t len);

#ifdef __cplusplus
}
#endif
