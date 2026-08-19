/*
 * Wi-Fi lifecycle for the Waveshare ESP32-P4-WIFI6.
 *
 * The ESP32-P4 has no radio. Wi-Fi is served by the onboard ESP32-C6 over SDIO
 * via esp_wifi_remote + esp_hosted, which present the ordinary esp_wifi API, so
 * nothing here looks unusual — but the C6 must be running matching ESP-Hosted
 * slave firmware or every call below fails at init.
 *
 * Two modes:
 *   - Provisioning: no saved credentials. Brings up a SoftAP so a phone or
 *     laptop can connect and drive the setup UI.
 *   - Station: saved credentials found. Joins that network.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_MGR_SSID_MAX 32
#define WIFI_MGR_PASS_MAX 64

typedef enum {
    WIFI_MGR_MODE_IDLE = 0,
    WIFI_MGR_MODE_PROVISIONING,  /**< SoftAP up, serving the setup UI */
    WIFI_MGR_MODE_STATION,       /**< Joined the configured network */
} wifi_mgr_mode_t;

typedef struct {
    wifi_mgr_mode_t mode;
    bool     connected;
    char     ssid[WIFI_MGR_SSID_MAX + 1];
    char     ip[16];
    int8_t   rssi;
    uint8_t  channel;
    char     mac[18];
} wifi_mgr_status_t;

/**
 * @brief Bring up NVS, the event loop, netifs and the Wi-Fi driver.
 *
 * Must be called before anything else here. Returns an error if the ESP32-C6
 * companion radio does not respond — the usual cause is missing or mismatched
 * ESP-Hosted slave firmware on the C6.
 */
esp_err_t wifi_manager_init(void);

/** @brief True if a usable SSID is stored in NVS. */
bool wifi_manager_has_credentials(void);

/**
 * @brief Start SoftAP mode so a client can connect and run the setup UI.
 *
 * Uses APSTA rather than plain AP: the station interface has to be available
 * to scan and to test a candidate network without dropping the client that is
 * driving the UI.
 */
esp_err_t wifi_manager_start_provisioning(void);

/** @brief Join the network stored in NVS. Non-blocking; watch the status. */
esp_err_t wifi_manager_start_station(void);

/**
 * @brief Scan for networks. Caller frees @p out_records with free().
 *
 * @param[out] out_records  Array of results, strongest first, duplicates removed.
 * @param[out] out_count    Number of entries.
 */
esp_err_t wifi_manager_scan(wifi_ap_record_t **out_records, uint16_t *out_count);

/**
 * @brief Try to associate with @p ssid, blocking until it works or gives up.
 *
 * Makes several attempts within @p timeout_ms, because a single disconnect is
 * not proof of a bad password — the first association often fails transiently
 * and succeeds immediately after. Returns early only when the access point
 * specifically rejects the credentials.
 *
 * Does not persist anything — the caller decides whether to keep the
 * credentials after seeing the result.
 *
 * @return ESP_OK               associated and got an IP
 *         ESP_ERR_WIFI_PASSWORD the AP rejected the password
 *         ESP_ERR_WIFI_SSID     the network was never seen
 *         ESP_ERR_TIMEOUT       everything else, after all attempts
 */
esp_err_t wifi_manager_try_connect(const char *ssid, const char *password,
                                   uint32_t timeout_ms);

/** @brief Reason code from the most recent disconnect, for diagnostics. */
uint8_t wifi_manager_last_disconnect_reason(void);

/** @brief Persist credentials to NVS. */
esp_err_t wifi_manager_save_credentials(const char *ssid, const char *password);

/** @brief Erase stored credentials so the next boot re-enters provisioning. */
esp_err_t wifi_manager_erase_credentials(void);

/** @brief Snapshot of the current state, for the status API. */
void wifi_manager_get_status(wifi_mgr_status_t *out_status);

/** @brief SSID the SoftAP is advertising, for display on the console. */
const char *wifi_manager_ap_ssid(void);

#ifdef __cplusplus
}
#endif
