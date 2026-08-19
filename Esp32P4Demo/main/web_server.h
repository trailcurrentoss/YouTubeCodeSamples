/*
 * HTTP server serving the TrailCurrent-branded web UI and its REST API.
 *
 * Two personalities, chosen at start:
 *   - Setup:     Wi-Fi provisioning — scan, pick, enter password, verify.
 *   - Dashboard: board details, microSD capacity, file management.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WEB_UI_SETUP = 0,   /**< Wi-Fi provisioning UI */
    WEB_UI_DASHBOARD,   /**< Normal operation UI */
} web_ui_mode_t;

/** @brief Start the HTTP server in the given mode. */
esp_err_t web_server_start(web_ui_mode_t mode);

/** @brief Stop the HTTP server. */
void web_server_stop(void);

#ifdef __cplusplus
}
#endif
