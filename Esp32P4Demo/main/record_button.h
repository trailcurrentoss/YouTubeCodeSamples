/*
 * Push-to-talk button for the Peregrine voice assistant.
 *
 * A user-fitted button on the 40-pin header (BSP_RECORD_BUTTON): hold it to
 * ask a question, release it to send the capture to Peregrine and hear the
 * reply through the speaker.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configure the button GPIO and start the task that watches it.
 *
 * Safe to call even when the microphone failed to come up or the card carries
 * no Peregrine settings — a press then logs why nothing happened instead of
 * failing silently.
 */
esp_err_t record_button_init(void);

#ifdef __cplusplus
}
#endif
