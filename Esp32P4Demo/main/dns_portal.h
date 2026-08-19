/*
 * Minimal captive-portal DNS responder.
 *
 * Answers every A query with the SoftAP address, which is what makes phones
 * pop the setup page up on their own instead of the user having to discover
 * and type an IP address.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Start the responder. Only meaningful while the SoftAP is up. */
esp_err_t dns_portal_start(void);

/** @brief Stop the responder and release the socket. */
void dns_portal_stop(void);

#ifdef __cplusplus
}
#endif
