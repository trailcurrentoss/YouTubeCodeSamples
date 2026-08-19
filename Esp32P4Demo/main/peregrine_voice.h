/*
 * peregrine_voice — push-to-talk client for the Peregrine voice assistant.
 *
 * Holding the button on the header captures speech; releasing it uploads the
 * capture and plays the spoken reply through the speaker.
 *
 *   1. Button PRESSED   -> peregrine_voice_press()
 *        starts capturing into PSRAM
 *
 *   2. Button RELEASED  -> peregrine_voice_release()
 *        stops the capture and hands it to a worker task, which:
 *          a. wraps the samples in a WAV header
 *          b. POSTs them to ${PEREGRINE_URL}/api/voice
 *             with "Authorization: Bearer ${PEREGRINE_VOICE_TOKEN}"
 *          c. streams the reply body straight to the speaker as it arrives
 *
 *   3. Reply finishes, or fails -> back to idle
 *
 * The URL and token normally come from /sdcard/environment.conf via
 * sd_config.c; the Kconfig values are only fallbacks. With no token the
 * button is inert and says so, rather than failing on every press.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PRG_VOICE_IDLE = 0,
    PRG_VOICE_LISTENING,    /**< button held, capturing */
    PRG_VOICE_UPLOADING,    /**< POST in flight */
    PRG_VOICE_PLAYING,      /**< reply streaming to the speaker */
    PRG_VOICE_ERROR,        /**< last exchange failed; clears on next press */
} peregrine_voice_state_t;

/**
 * @brief Start the worker task. Idempotent.
 *
 * Call after audio_recorder_init() so the microphone and speaker exist, and
 * before sd_config_load() so the setters are ready for the card's values.
 */
void peregrine_voice_init(void);

/** @brief Button went down: start capturing. */
void peregrine_voice_press(void);

/** @brief Button came up: send what was captured and play the reply. */
void peregrine_voice_release(void);

/** @brief Where the exchange has got to. Cheap enough to poll. */
peregrine_voice_state_t peregrine_voice_get_state(void);

/** @brief True once a URL and a token are both set. */
bool peregrine_voice_configured(void);

/** @brief Override the base URL. NULL or "" restores the Kconfig default. */
void peregrine_voice_set_url(const char *url);

/** @brief Override the bearer token. NULL or "" disables push-to-talk. */
void peregrine_voice_set_token(const char *token);

#ifdef __cplusplus
}
#endif
