/*
 * sd_config — boot-time reader for /sdcard/environment.conf.
 *
 * Settings that differ from board to board live on the card rather than in the
 * firmware image, so one build can serve every unit and a token can be rotated
 * by editing a text file.
 *
 * Format is KEY=VALUE, one per line. '#' starts a comment, blank lines are
 * ignored, and whitespace around the key and value is trimmed. Values may be
 * wrapped in single or double quotes.
 *
 *     # Peregrine voice assistant
 *     PEREGRINE_URL=http://peregrine.local:8081
 *     PEREGRINE_VOICE_TOKEN=8cb78a5aad34...
 *
 * Recognized keys:
 *   PEREGRINE_URL           -> peregrine_voice_set_url()
 *   PEREGRINE_VOICE_TOKEN   -> peregrine_voice_set_token()
 *
 * Unrecognized keys are logged and skipped, so the same file can carry Wi-Fi
 * or MQTT settings for other firmware without breaking this parser.
 *
 * The card must already be mounted — main.c does that before calling here. A
 * missing card or missing file is not an error: the compile-time Kconfig
 * defaults stay in effect and push-to-talk reports itself unconfigured.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Parse the file if it is there and hand values to their consumers. */
esp_err_t sd_config_load(void);

/**
 * @brief True only if the last load found BOTH a Peregrine URL and a token.
 *
 * Without both there is nothing for push-to-talk to talk to, so the button
 * says why it is doing nothing instead of failing on every press.
 */
bool sd_config_peregrine_present(void);

#ifdef __cplusplus
}
#endif
