/*
 * microSD card bring-up for the Waveshare ESP32-P4-WIFI6.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>

#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"

#include "board_esp32p4_wifi6.h"
#include "sd_card.h"

static const char *TAG = "sd_card";

#define SD_IS_UHS1 (CONFIG_DEMO_SD_SPEED_UHS_SDR50 || CONFIG_DEMO_SD_SPEED_UHS_DDR50)

/* Held so unmount can tear the LDO back down in the right order. */
static sd_pwr_ctrl_handle_t s_pwr_ctrl_handle = NULL;

/* The mounted card, so other modules can query it without holding the handle. */
static sdmmc_card_t *s_card = NULL;

esp_err_t sd_card_mount(sdmmc_card_t **out_card, bool format_if_needed,
                        size_t alloc_unit_size, bool *out_formatted)
{
    esp_err_t ret;

    if (out_formatted) {
        *out_formatted = false;
    }

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    /* The card MUST be on slot 0.
     *
     * SDMMC_HOST_DEFAULT() selects slot 1, and the ESP32-P4 has two slots — but
     * on this board slot 1 belongs to the onboard ESP32-C6 radio: esp_hosted
     * hardcodes H_SDMMC_HOST_SLOT to SDMMC_HOST_SLOT_1. Leaving the card on the
     * default means whichever comes up first wins, and since the card mounts
     * before Wi-Fi, ESP-Hosted then re-initialises a slot already in use. Its
     * SDIO probe finds no SDIO device, falls through to the SD-memory path and
     * dies with "sdmmc_init_sd_scr: send_scr (1) returned 0xffffffff" — which
     * reads like a broken C6 but is really a slot collision.
     *
     * Slot 0 is where the card belongs anyway: it owns the dedicated
     * high-speed pins (43/44/39-42) and is the only slot supporting UHS-I. */
    host.slot = SDMMC_HOST_SLOT_0;

#if CONFIG_DEMO_SD_SPEED_HS
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
#elif CONFIG_DEMO_SD_SPEED_UHS_SDR50
    host.max_freq_khz = SDMMC_FREQ_SDR50;
    host.flags &= ~SDMMC_HOST_FLAG_DDR;
#elif CONFIG_DEMO_SD_SPEED_UHS_DDR50
    host.max_freq_khz = SDMMC_FREQ_DDR50;
#endif

    /* The SD IO rail on this board is fed by the SoC's internal LDO channel 4.
     * It has to be running before the card is clocked, otherwise probing fails
     * with ESP_ERR_TIMEOUT and looks exactly like a wiring fault. */
    const sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = BSP_SD_PWR_LDO_CHAN_ID,
    };
    ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &s_pwr_ctrl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start on-chip LDO %d for SD power: %s",
                 BSP_SD_PWR_LDO_CHAN_ID, esp_err_to_name(ret));
        return ret;
    }
    host.pwr_ctrl_handle = s_pwr_ctrl_handle;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
#if SD_IS_UHS1
    slot_config.flags |= SDMMC_SLOT_FLAG_UHS1;
#endif
#if CONFIG_DEMO_SD_BUS_WIDTH_4
    slot_config.width = 4;
#else
    slot_config.width = 1;
#endif
    slot_config.clk = BSP_SD_CLK;
    slot_config.cmd = BSP_SD_CMD;
    slot_config.d0  = BSP_SD_D0;
#if CONFIG_DEMO_SD_BUS_WIDTH_4
    slot_config.d1  = BSP_SD_D1;
    slot_config.d2  = BSP_SD_D2;
    slot_config.d3  = BSP_SD_D3;
#endif
    /* The board carries external pull-ups on the SD bus; the internal ones are
     * enabled as belt-and-braces and cost nothing. */
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 8,
        .allocation_unit_size = alloc_unit_size,
        .disk_status_check_enable = false,
        .use_one_fat = false,
    };

    ESP_LOGI(TAG, "Probing SD card on SDMMC slot %d (%d-bit, %d kHz)",
             host.slot, slot_config.width, host.max_freq_khz);

    /* First attempt without formatting, purely so we can tell an existing
     * filesystem apart from a blank card. The distinction matters: a blank card
     * has to be formatted by the mount itself (on failure ESP-IDF frees the card
     * and never hands back a handle), and knowing it already happened saves the
     * caller a redundant second format — ~40 s on a 32 GB card. */
    sdmmc_card_t *card = NULL;
    ret = esp_vfs_fat_sdmmc_mount(BSP_SD_MOUNT_POINT, &host, &slot_config,
                                  &mount_config, &card);

    if (ret == ESP_FAIL && format_if_needed) {
        ESP_LOGW(TAG, "No filesystem found — formatting the card "
                      "(this can take a minute on a large card)");
        mount_config.format_if_mount_failed = true;
        card = NULL;
        ret = esp_vfs_fat_sdmmc_mount(BSP_SD_MOUNT_POINT, &host, &slot_config,
                                      &mount_config, &card);
        if (ret == ESP_OK && out_formatted) {
            *out_formatted = true;
        }
    }

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Card has no mountable filesystem and formatting was "
                          "not permitted — enable CONFIG_DEMO_SD_FORMAT_ON_BOOT");
        } else {
            ESP_LOGE(TAG, "Card init failed: %s", esp_err_to_name(ret));
        }
        sd_pwr_ctrl_del_on_chip_ldo(s_pwr_ctrl_handle);
        s_pwr_ctrl_handle = NULL;
        return ret;
    }

    ESP_LOGI(TAG, "Mounted at %s", BSP_SD_MOUNT_POINT);
    s_card = card;
    if (out_card) {
        *out_card = card;
    }
    return ESP_OK;
}

const sdmmc_card_t *sd_card_get(void)
{
    return s_card;
}

esp_err_t sd_card_get_fs_usage(uint64_t *out_total, uint64_t *out_free)
{
    if (s_card == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_vfs_fat_info(BSP_SD_MOUNT_POINT, out_total, out_free);
}

esp_err_t sd_card_format(sdmmc_card_t *card, size_t alloc_unit_size)
{
    if (card == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGW(TAG, "Formatting card — all existing data will be destroyed");
    if (alloc_unit_size) {
        ESP_LOGI(TAG, "Allocation unit size: %u bytes", (unsigned)alloc_unit_size);
    }

    esp_vfs_fat_mount_config_t fmt_config = {
        .format_if_mount_failed = true,
        .max_files = 8,
        .allocation_unit_size = alloc_unit_size,
        .disk_status_check_enable = false,
        .use_one_fat = false,
    };

    esp_err_t ret = esp_vfs_fat_sdcard_format_cfg(BSP_SD_MOUNT_POINT, card, &fmt_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Format failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Format complete, filesystem re-mounted empty");
    return ESP_OK;
}

esp_err_t sd_card_unmount(sdmmc_card_t *card)
{
    s_card = NULL;
    esp_err_t ret = esp_vfs_fat_sdcard_unmount(BSP_SD_MOUNT_POINT, card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Unmount failed: %s", esp_err_to_name(ret));
    }

    if (s_pwr_ctrl_handle) {
        esp_err_t ldo_ret = sd_pwr_ctrl_del_on_chip_ldo(s_pwr_ctrl_handle);
        s_pwr_ctrl_handle = NULL;
        if (ldo_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to release SD LDO: %s", esp_err_to_name(ldo_ret));
            if (ret == ESP_OK) {
                ret = ldo_ret;
            }
        }
    }

    return ret;
}

void sd_card_print_info(const sdmmc_card_t *card)
{
    if (card == NULL) {
        return;
    }

    /* sdmmc_card_print_info() takes a non-const pointer but only reads. */
    printf("\n");
    sdmmc_card_print_info(stdout, (sdmmc_card_t *)card);

    const uint64_t capacity_bytes = (uint64_t)card->csd.capacity * card->csd.sector_size;
    printf("Capacity : %llu bytes (%.2f GB)\n",
           (unsigned long long)capacity_bytes,
           (double)capacity_bytes / (1000.0 * 1000.0 * 1000.0));
    printf("Sectors  : %d x %d bytes\n", card->csd.capacity, card->csd.sector_size);
    printf("Bus width: %d-bit\n", card->log_bus_width ? (1 << card->log_bus_width) : 1);
    printf("\n");
}

void sd_card_print_fs_usage(void)
{
    uint64_t total = 0;
    uint64_t used_free = 0;

    esp_err_t ret = esp_vfs_fat_info(BSP_SD_MOUNT_POINT, &total, &used_free);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Could not read filesystem usage: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "Filesystem: %.2f MB total, %.2f MB free, %.2f MB used",
             (double)total / (1024.0 * 1024.0),
             (double)used_free / (1024.0 * 1024.0),
             (double)(total - used_free) / (1024.0 * 1024.0));
}
