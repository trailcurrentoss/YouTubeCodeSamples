/*
 * results.h -- persistent storage for antenna measurement runs.
 *
 * Two slots are kept, one per antenna configuration. Because switching this
 * board between its ceramic antenna and its IPEX/SMA path requires rotating a
 * 0R link on the PCB (see README), the two runs are necessarily separated by a
 * power cycle and a soldering iron. Storing each run in NVS is what makes the
 * A/B comparison possible at all -- slot A survives while you rework the board
 * for slot B.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RF_RESULT_MAGIC   0x414E5442u /* "ANTB" */
#define RF_RESULT_VERSION 2u

/* Per-BSSID census entries retained per run. Comparing the *same* BSSIDs across
 * two runs is far more robust than comparing means over two AP sets that may
 * not have the same membership -- the weak tail of a scan churns between
 * consecutive scans regardless of which antenna is fitted. */
#define RF_CENSUS_TOP_N 16

typedef struct {
    uint8_t bssid[6];
    int8_t  rssi;
} rf_census_entry_t;

typedef enum {
    RF_SLOT_ONBOARD = 0, /* patch ceramic antenna, 0R link vertical (factory) */
    RF_SLOT_EXTERNAL,    /* IPEX Gen1 -> SMA pigtail, 0R link horizontal      */
    RF_SLOT_COUNT
} rf_slot_t;

typedef struct {
    uint32_t magic;
    uint32_t version;

    /* Link identity -- used to reject an apples-to-oranges comparison. */
    char    ssid[33];
    uint8_t bssid[6];
    uint8_t channel;
    bool    connect_ok;
    uint32_t connect_ms; /* association + DHCP time */

    /* Connected-state RSSI series. */
    uint32_t rssi_samples;
    float    rssi_mean;
    float    rssi_stddev;
    int8_t   rssi_min;
    int8_t   rssi_max;
    int8_t   rssi_median;
    int8_t   rssi_p10; /* 10th percentile -- the deep-fade tail */

    /* Round-trip link quality. */
    uint32_t ping_sent;
    uint32_t ping_recv;
    uint32_t ping_rtt_avg_ms;
    uint32_t ping_rtt_max_ms;

    /* Scan census -- antenna sensitivity independent of any single AP. */
    uint32_t scan_ap_count;
    float    scan_rssi_mean;
    int8_t   scan_rssi_best;
    int8_t   scan_target_rssi;

    /* Strongest RF_CENSUS_TOP_N APs of this run, for the paired comparison. */
    uint32_t          census_n;
    rf_census_entry_t census[RF_CENSUS_TOP_N];

    /* Radio state, recorded so an unequal comparison is detectable. */
    int8_t   tx_power_qdbm; /* esp_wifi_get_max_tx_power() units: 0.25 dBm */

    uint64_t uptime_us_at_run;
} rf_result_t;

const char *rf_slot_name(rf_slot_t slot);
const char *rf_slot_description(rf_slot_t slot);

/* Parses "onboard"/"a" and "external"/"sma"/"b". Returns RF_SLOT_COUNT on
 * anything else. */
rf_slot_t rf_slot_parse(const char *text);

esp_err_t results_init(void);
esp_err_t results_save(rf_slot_t slot, const rf_result_t *result);
esp_err_t results_load(rf_slot_t slot, rf_result_t *out);
esp_err_t results_erase(rf_slot_t slot);
esp_err_t results_erase_all(void);

void results_print_one(rf_slot_t slot, const rf_result_t *result);

/* Prints both slots side by side plus the deltas. Returns ESP_ERR_NOT_FOUND if
 * either slot is empty. */
esp_err_t results_print_comparison(void);

#ifdef __cplusplus
}
#endif
