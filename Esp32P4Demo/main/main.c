/*
 * Waveshare ESP32-P4-WIFI6 (SKU 31647) demo — bare board.
 *
 * Boots one of two ways:
 *
 *   No saved Wi-Fi credentials  ->  SETUP MODE
 *       Brings up a SoftAP and serves a TrailCurrent-branded provisioning UI:
 *       scan, pick a network, enter the password. The board verifies the
 *       connection before saving it, then reboots.
 *
 *   Saved credentials present   ->  NORMAL MODE
 *       Joins the stored network and serves the dashboard: board details,
 *       microSD capacity, and file management.
 *
 * The microSD card is mounted in both modes, and the offline self-test can be
 * run at boot via CONFIG_DEMO_SD_RUN_TESTS.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "sdkconfig.h"

#include "audio_recorder.h"
#include "board_esp32p4_wifi6.h"
#include "dns_portal.h"
#include "peregrine_voice.h"
#include "record_button.h"
#include "sd_card.h"
#include "sd_config.h"
#include "sd_test.h"
#include "web_server.h"
#include "wifi_manager.h"

static const char *TAG = "main";

/* Expected hardware for this board, in bytes. */
#define EXPECTED_FLASH_BYTES  (32u * 1024 * 1024)
#define EXPECTED_PSRAM_BYTES  (32u * 1024 * 1024)

/**
 * @brief Print chip, flash and PSRAM detail, and warn on any mismatch.
 */
static void report_hardware(void)
{
    esp_chip_info_t chip = {0};
    esp_chip_info(&chip);

    printf("\n");
    printf("===========================================================\n");
    printf(" Waveshare ESP32-P4-WIFI6  —  bare board demo\n");
    printf("===========================================================\n");
    printf(" IDF version    : %s\n", esp_get_idf_version());
    printf(" Chip           : ESP32-P4, %d core(s), silicon rev v%d.%d\n",
           chip.cores, chip.revision / 100, chip.revision % 100);
    printf(" Min rev built  : v%d.%d\n",
           CONFIG_ESP_REV_MIN_FULL / 100, CONFIG_ESP_REV_MIN_FULL % 100);

    uint32_t flash_bytes = 0;
    if (esp_flash_get_physical_size(NULL, &flash_bytes) != ESP_OK) {
        flash_bytes = 0;
    }
    printf(" Flash detected : %" PRIu32 " MB\n", flash_bytes / (1024 * 1024));
    printf(" Flash config'd : %s (%s, %s)\n",
           CONFIG_ESPTOOLPY_FLASHSIZE,
#if CONFIG_ESPTOOLPY_FLASHMODE_QIO
           "QIO",
#elif CONFIG_ESPTOOLPY_FLASHMODE_DIO
           "DIO",
#else
           "other",
#endif
           CONFIG_ESPTOOLPY_FLASHFREQ);

#if CONFIG_SPIRAM
    const size_t psram_bytes = esp_psram_get_size();
    printf(" PSRAM detected : %u MB (HEX mode, %d MHz)\n",
           (unsigned)(psram_bytes / (1024 * 1024)), CONFIG_SPIRAM_SPEED);
    printf(" PSRAM free     : %u KB of %u KB\n",
           (unsigned)(heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024),
           (unsigned)(heap_caps_get_total_size(MALLOC_CAP_SPIRAM) / 1024));
#else
    const size_t psram_bytes = 0;
    printf(" PSRAM          : DISABLED in sdkconfig\n");
#endif

    printf(" Internal RAM   : %u KB free\n",
           (unsigned)(heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024));
    printf("===========================================================\n\n");

    if (flash_bytes && flash_bytes != EXPECTED_FLASH_BYTES) {
        ESP_LOGW(TAG, "Flash is %" PRIu32 " MB but this board should have 32 MB — "
                      "check CONFIG_ESPTOOLPY_FLASHSIZE_32MB",
                 flash_bytes / (1024 * 1024));
    }
    if (psram_bytes != EXPECTED_PSRAM_BYTES) {
        ESP_LOGW(TAG, "PSRAM is %u MB but this board should have 32 MB — "
                      "check CONFIG_SPIRAM / CONFIG_SPIRAM_MODE_HEX",
                 (unsigned)(psram_bytes / (1024 * 1024)));
    }
}

/**
 * @brief Mount the card. Absence is reported but is not fatal.
 *
 * The web UI has to come up either way — being able to see that no card is
 * present is more useful than the board refusing to boot.
 */
static void bring_up_sd_card(void)
{
    const size_t alloc_unit = (size_t)CONFIG_DEMO_SD_FORMAT_ALLOC_UNIT_KB * 1024;

    sdmmc_card_t *card = NULL;
    bool already_formatted = false;
    const esp_err_t ret = sd_card_mount(&card, true, alloc_unit, &already_formatted);

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "No usable microSD card (%s) — continuing without storage",
                 esp_err_to_name(ret));
        return;
    }

    sd_card_print_info(card);
    if (already_formatted) {
        ESP_LOGI(TAG, "Card had no filesystem, so one was created at mount");
    }

#if CONFIG_DEMO_SD_FORMAT_ON_BOOT
    if (!already_formatted) {
        ESP_LOGW(TAG, "About to format the card — all data on it will be lost");
        ESP_LOGW(TAG, "Disable CONFIG_DEMO_SD_FORMAT_ON_BOOT to keep the card's contents");
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (sd_card_format(card, alloc_unit) != ESP_OK) {
            ESP_LOGE(TAG, "Format failed");
        }
    }
#endif

    sd_card_print_fs_usage();

#if CONFIG_DEMO_SD_RUN_TESTS
    sd_test_run_all();
#endif
}

void app_main(void)
{
    report_hardware();
    bring_up_sd_card();

    /* Microphone. A failure here is not fatal — the rest of the board is still
     * worth reaching, and the record endpoint reports the problem. */
    if (audio_recorder_init() != ESP_OK) {
        ESP_LOGW(TAG, "Microphone unavailable — recording will be disabled");
    }

    /* Voice assistant. The worker exists before the card is read, because
     * sd_config_load() feeds it the URL and token it finds there. */
    peregrine_voice_init();
    sd_config_load();

    /* Held button = ask Peregrine a question. */
    if (record_button_init() != ESP_OK) {
        ESP_LOGW(TAG, "Push-to-talk button unavailable");
    }

    /* Wi-Fi lives on the onboard ESP32-C6 over SDIO. If this fails the board is
     * still useful over the console, so report clearly and idle rather than
     * rebooting in a loop. */
    if (wifi_manager_init() != ESP_OK) {
        ESP_LOGE(TAG, "Wi-Fi unavailable — the web UI cannot start");
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }

    if (wifi_manager_has_credentials()) {
        ESP_LOGI(TAG, "Saved network found — starting in normal operation mode");
        ESP_ERROR_CHECK(wifi_manager_start_station());
        ESP_ERROR_CHECK(web_server_start(WEB_UI_DASHBOARD));

        /* Report the address once the join completes, so the console tells the
         * user where to point a browser. */
        for (int i = 0; i < 30; i++) {
            wifi_mgr_status_t st;
            wifi_manager_get_status(&st);
            if (st.connected) {
                printf("\n");
                ESP_LOGI(TAG, "===============================================");
                ESP_LOGI(TAG, " Dashboard ready:  http://%s/", st.ip);
                ESP_LOGI(TAG, " Network        :  %s (%d dBm)", st.ssid, st.rssi);
                ESP_LOGI(TAG, "===============================================");
                printf("\n");
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    } else {
        ESP_LOGW(TAG, "No saved Wi-Fi network — starting setup mode");
        ESP_ERROR_CHECK(wifi_manager_start_provisioning());
        ESP_ERROR_CHECK(web_server_start(WEB_UI_SETUP));
#if CONFIG_DEMO_WEB_CAPTIVE_PORTAL
        dns_portal_start();
#endif

        wifi_mgr_status_t st;
        wifi_manager_get_status(&st);
        printf("\n");
        ESP_LOGI(TAG, "===============================================");
        ESP_LOGI(TAG, " SETUP MODE — no Wi-Fi network saved yet");
        ESP_LOGI(TAG, " 1. Join the Wi-Fi network:  %s", wifi_manager_ap_ssid());
        if (strlen(CONFIG_DEMO_WIFI_AP_PASSWORD) >= 8) {
            ESP_LOGI(TAG, " 2. Password              :  %s", CONFIG_DEMO_WIFI_AP_PASSWORD);
        } else {
            ESP_LOGI(TAG, "    (open network, no password)");
        }
        ESP_LOGI(TAG, " 2. Open                  :  http://%s/", st.ip);
        ESP_LOGI(TAG, "===============================================");
        printf("\n");
    }

    /* Everything from here on is driven by the HTTP server and Wi-Fi events. */
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
