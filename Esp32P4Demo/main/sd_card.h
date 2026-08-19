/*
 * microSD card bring-up for the Waveshare ESP32-P4-WIFI6.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"
#include "sdmmc_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Power up the SD rail, probe the card, and mount FAT at BSP_SD_MOUNT_POINT.
 *
 * Brings up the on-chip LDO that feeds the SD IO rail, configures SDMMC slot 0
 * with the board's pin map, and mounts the filesystem.
 *
 * @param[out] out_card         Receives the card handle on success. May be NULL.
 * @param      format_if_needed If true, a card with no readable filesystem is
 *                              partitioned and formatted here rather than
 *                              failing. This is required for a blank card:
 *                              when the mount fails, ESP-IDF frees the card
 *                              internally and never hands back a handle, so
 *                              there is nothing left to format afterwards.
 * @param      alloc_unit_size  Cluster size to use if a format happens, or 0
 *                              for the FATFS default.
 * @param[out] out_formatted    Set true if the card was blank and got formatted
 *                              here. Lets the caller skip a redundant second
 *                              format, which on a 32 GB card costs ~40 s.
 *                              May be NULL.
 *
 * @return ESP_OK on success. ESP_FAIL means the card carries no mountable
 *         filesystem and @p format_if_needed was false.
 */
esp_err_t sd_card_mount(sdmmc_card_t **out_card, bool format_if_needed,
                        size_t alloc_unit_size, bool *out_formatted);

/**
 * @brief Erase the card and write a fresh FAT filesystem.
 *
 * The card must already be mounted. The mount survives the operation: the
 * filesystem is re-mounted empty when this returns.
 *
 * WARNING: this destroys every byte of user data on the card.
 *
 * @param card             Handle from sd_card_mount().
 * @param alloc_unit_size  Cluster size in bytes, or 0 for the FATFS default.
 */
esp_err_t sd_card_format(sdmmc_card_t *card, size_t alloc_unit_size);

/**
 * @brief Unmount the filesystem and release the SD power control driver.
 */
esp_err_t sd_card_unmount(sdmmc_card_t *card);

/**
 * @brief Log card identity, capacity, bus width, and negotiated speed.
 */
void sd_card_print_info(const sdmmc_card_t *card);

/**
 * @brief Log total and free bytes on the mounted filesystem.
 */
void sd_card_print_fs_usage(void);

/**
 * @brief The currently mounted card, or NULL if none is mounted.
 *
 * Lets other modules (the web API) report card details without having to have
 * the handle threaded through to them.
 */
const sdmmc_card_t *sd_card_get(void);

/**
 * @brief Total and free bytes on the mounted filesystem.
 *
 * @return ESP_ERR_INVALID_STATE if no card is mounted.
 */
esp_err_t sd_card_get_fs_usage(uint64_t *out_total, uint64_t *out_free);

#ifdef __cplusplus
}
#endif
