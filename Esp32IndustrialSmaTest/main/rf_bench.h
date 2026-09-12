/*
 * rf_bench.h -- the measurement run itself.
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "results.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Runs one full campaign (scan census -> connect -> RSSI series -> ping) and
 * writes the outcome into `out`. Does not touch NVS; the caller decides which
 * slot it belongs to. Blocks for roughly:
 *     scan (~2.5 s) + connect + SAMPLES*INTERVAL + PING_COUNT*PING_INTERVAL
 */
esp_err_t rf_bench_run(rf_result_t *out);

/* Free-running RSSI printout for walk-around testing. Connects if needed and
 * prints one line per sample until `duration_s` elapses. duration_s == 0 runs
 * until rf_bench_monitor_stop() is called from another task. */
esp_err_t rf_bench_monitor(uint32_t duration_s, uint32_t interval_ms);

void rf_bench_monitor_stop(void);

#ifdef __cplusplus
}
#endif
