/*
 * microSD card test suite.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run the full test suite against the mounted card.
 *
 * Runs, in order:
 *   1. Basic file write / read-back / content verify
 *   2. Metadata: stat, rename, append, delete
 *   3. Directory create / list / recursive delete
 *   4. Many-small-files stress (CONFIG_DEMO_SD_STRESS_FILE_COUNT)
 *   5. Sequential throughput benchmark with full pattern verification
 *      (CONFIG_DEMO_SD_BENCH_MB)
 *
 * Every test cleans up after itself. A summary table is printed at the end.
 *
 * @return ESP_OK if every test passed, ESP_FAIL otherwise.
 */
esp_err_t sd_test_run_all(void);

#ifdef __cplusplus
}
#endif
