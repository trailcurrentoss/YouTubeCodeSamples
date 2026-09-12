/*
 * wifi_link.h -- thin wrapper over esp_wifi for the measurement runs.
 *
 * Deliberately synchronous: the benchmark reads better as a straight-line
 * sequence, and nothing else in this firmware competes for the radio.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "results.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t ap_count;      /* distinct BSSIDs heard                        */
    float    rssi_mean;     /* mean RSSI across every AP heard              */
    int8_t   rssi_best;     /* strongest AP in the census                   */
    int8_t   target_rssi;   /* RSSI of CONFIG_AB_WIFI_SSID, 0 if not heard  */
    bool     target_found;

    /* Strongest APs, descending. Used for the paired cross-run comparison. */
    uint32_t          top_n;
    rf_census_entry_t top[RF_CENSUS_TOP_N];
} wifi_scan_census_t;

/* Brings up netif/event loop/esp_wifi in STA mode and disables modem sleep.
 * Safe to call once from app_main. */
esp_err_t wifi_link_init(void);

/* Blocking full-channel active scan. Leaves the radio disconnected. */
esp_err_t wifi_link_scan(wifi_scan_census_t *out);

/* Associates with the configured AP and waits for an IPv4 lease.
 * elapsed_ms_out receives the association+DHCP time on success. */
esp_err_t wifi_link_connect(uint32_t *elapsed_ms_out);

esp_err_t wifi_link_disconnect(void);

bool wifi_link_is_connected(void);

/* Current AP record (RSSI, BSSID, channel). Fails if not associated. */
esp_err_t wifi_link_ap_info(wifi_ap_record_t *out);

/* Default gateway of the STA interface, as a dotted quad. */
esp_err_t wifi_link_gateway_str(char *buf, size_t len);

/* esp_wifi_get_max_tx_power() passthrough, in 0.25 dBm units. */
int8_t wifi_link_tx_power_qdbm(void);

#ifdef __cplusplus
}
#endif
