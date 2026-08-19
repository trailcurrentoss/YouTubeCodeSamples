/*
 * Microphone capture for the Waveshare ESP32-P4-WIFI6.
 *
 * The onboard analog microphone feeds an ES8311 codec; the P4 reads the
 * digitised audio over I2S and writes it to the microSD card as a WAV file.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Result of one recording, including whether anything was heard. */
typedef struct {
    char     path[128];     /**< Path relative to the mount point */
    uint32_t bytes;         /**< Size of the finished WAV file */
    uint32_t samples;       /**< PCM frames captured */
    uint32_t sample_rate;   /**< Hz */
    float    seconds;       /**< Actual captured duration */
    int16_t  peak;          /**< Largest absolute sample value */
    float    peak_dbfs;     /**< Peak expressed in dBFS (-inf for pure silence) */
    float    rms_dbfs;      /**< RMS level in dBFS */
    bool     silent;        /**< True if the level never rose above the noise floor */
} audio_recording_t;

/**
 * @brief Bring up the I2C bus, the ES8311 codec and the I2S receive channel.
 *
 * Safe to call when no microphone work follows — recording endpoints report a
 * clear error if this failed rather than the whole board refusing to boot.
 */
esp_err_t audio_recorder_init(void);

/** @brief True once the codec and I2S channel are ready. */
bool audio_recorder_ready(void);

typedef enum {
    AUDIO_STATE_IDLE = 0,
    AUDIO_STATE_RECORDING,
} audio_state_t;

/**
 * @brief Begin recording, returning immediately.
 *
 * Capture runs on its own task, streaming to the card through a PSRAM ring
 * buffer, so the caller (an HTTP handler) is not tied up and recording length
 * is bounded by card space rather than RAM. It stops on its own only when the
 * card fills, the WAV 32-bit size limit approaches, or the optional
 * CONFIG_DEMO_AUDIO_MAX_SECONDS limit (0 = disabled, the default) is reached.
 */
esp_err_t audio_recorder_start(void);

/**
 * @brief Stop the running recording and write it to the card.
 *
 * Blocks briefly while the capture task finishes and the WAV file is written.
 *
 * @param[out] out Details of the finished recording, including signal level.
 */
esp_err_t audio_recorder_stop(audio_recording_t *out);

/** @brief Whether a capture is currently running. */
audio_state_t audio_recorder_state(void);

/** @brief Seconds captured so far, for the live elapsed display. */
float audio_recorder_elapsed(void);

/* -------------------------------------------------------------------------
 * Push-to-talk capture — to memory, not to the card
 *
 * The card path above is built for long recordings; a voice query is a few
 * seconds long and is uploaded rather than kept, so it is captured straight
 * into a PSRAM buffer. That skips the file, the writer task, and the SD write
 * bursts that couple into the microphone's analog path.
 *
 * Shares the codec and the I2S port with the recorder and the player, so only
 * one of the three may run at a time.
 * ------------------------------------------------------------------------- */

/**
 * @brief Begin capturing to memory. Returns immediately.
 *
 * Capture stops on its own at CONFIG_DEMO_PTT_MAX_SECONDS so a stuck button
 * cannot exhaust PSRAM.
 */
esp_err_t audio_recorder_capture_start(void);

/**
 * @brief Stop the memory capture and hand back what was recorded.
 *
 * Blocks briefly while the capture task exits. The buffer stays owned by this
 * module and stays valid until the next audio_recorder_capture_start(), so the
 * caller may upload from it without copying.
 *
 * @param[out] out_pcm   16-bit mono samples at CONFIG_DEMO_AUDIO_SAMPLE_RATE.
 * @param[out] out_bytes Length in bytes; 0 if nothing was captured.
 */
void audio_recorder_capture_stop(const int16_t **out_pcm, size_t *out_bytes);

/* -------------------------------------------------------------------------
 * Local playback — through the ES8311 DAC to a speaker on the board header
 * ------------------------------------------------------------------------- */

/**
 * @brief Play a WAV file from the card through the onboard speaker output.
 *
 * Returns as soon as playback starts; the file is streamed by a background
 * task. 16-bit PCM only, mono or stereo. If the file's sample rate differs
 * from the recording rate, the I2S clock and the codec are retuned for the
 * duration and restored afterwards.
 *
 * Recording and playback share one I2S port and one codec, so this fails with
 * ESP_ERR_INVALID_STATE while a recording is running.
 *
 * @param rel_path Path relative to the card mount point, e.g. "/recordings/rec-0001.wav".
 */
esp_err_t audio_player_start(const char *rel_path);

/** @brief Stop playback early. Harmless if nothing is playing. */
void audio_player_stop(void);

/** @brief True while a file is playing through the speaker. */
bool audio_player_is_playing(void);

/** @brief Path currently playing, or "" when idle. */
const char *audio_player_path(void);

/** @brief Playback position and length in seconds, for the UI. */
void audio_player_progress(float *out_position, float *out_duration);

/* -------------------------------------------------------------------------
 * Streaming playback — for audio that arrives a chunk at a time
 *
 * audio_player_start() above needs a finished file on the card. A reply from
 * the voice assistant arrives over HTTP while it is still being synthesised,
 * so it is pushed to the speaker as it lands instead.
 *
 * Unlike the file player these run on the CALLING task: _write blocks until
 * the samples are handed to the I2S DMA, which is what paces the download.
 * ------------------------------------------------------------------------- */

/**
 * @brief Open the speaker for streamed audio at the given rate.
 *
 * Retunes the shared I2S clock and codec if the rate differs from the capture
 * rate, and switches the amplifier on.
 */
esp_err_t audio_player_stream_start(uint32_t sample_rate);

/**
 * @brief Push 16-bit mono PCM to the speaker, blocking until it is queued.
 *
 * A partial trailing sample is buffered until the next call, so the caller can
 * pass arbitrary byte counts straight from a socket read.
 */
esp_err_t audio_player_stream_write(const void *pcm, size_t bytes);

/** @brief Let the queued audio drain, then mute and release the speaker. */
void audio_player_stream_end(void);

/** @brief Folder on the card where recordings are written. */
const char *audio_recorder_dir(void);

#ifdef __cplusplus
}
#endif
