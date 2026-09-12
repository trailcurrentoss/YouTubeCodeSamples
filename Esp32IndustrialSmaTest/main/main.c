/*
 * Antenna A/B test for the Waveshare ESP32-S3-RS485-CAN industrial board.
 *
 * The board carries a patch ceramic antenna AND an IPEX Gen1 connector, but
 * only one of them is connected to the ESP32-S3 at a time: a 0R link near the
 * IPEX socket selects the path, and Waveshare's own FAQ says to switch it by
 * rotating that resistor from vertical to horizontal soldering. There is no RF
 * switch and no GPIO involved, so the two configurations cannot be compared
 * within a single power cycle.
 *
 * This firmware works around that by persisting each run to NVS. Measure with
 * the factory ceramic antenna, power down, rework the link, fit the SMA
 * pigtail, measure again, then `report` to see the delta.
 */

#include <inttypes.h>
#include <stdio.h>

#include "console_cmds.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "results.h"
#include "sdkconfig.h"
#include "wifi_link.h"

static const char *TAG = "main";

static void print_banner(void)
{
    printf("\n");
    printf("=====================================================\n");
    printf(" ESP32-S3-RS485-CAN antenna A/B test\n");
    printf("=====================================================\n");
    printf(" Reference AP : %s\n", CONFIG_AB_WIFI_SSID);
    printf(" Run length   : %d samples @ %d ms (%.0f s)\n",
           CONFIG_AB_RSSI_SAMPLES, CONFIG_AB_RSSI_INTERVAL_MS,
           CONFIG_AB_RSSI_SAMPLES * CONFIG_AB_RSSI_INTERVAL_MS / 1000.0f);
    printf(" ICMP echoes  : %d @ %d ms\n", CONFIG_AB_PING_COUNT,
           CONFIG_AB_PING_INTERVAL_MS);
    printf("-----------------------------------------------------\n");
    printf(" Commands:\n");
    printf("   measure onboard    run with the ceramic patch antenna\n");
    printf("   measure external   run with the SMA antenna on IPEX\n");
    printf("   report             both runs side by side, with deltas\n");
    printf("   show <slot>        one stored run\n");
    printf("   scan               quick AP census\n");
    printf("   monitor -t 30      live RSSI stream\n");
    printf("   clear [slot]       erase stored results\n");
    printf("   help               full command list\n");
    printf("-----------------------------------------------------\n");
}

static void print_stored_state(void)
{
    rf_result_t r;
    for (int i = 0; i < RF_SLOT_COUNT; i++) {
        const rf_slot_t slot = (rf_slot_t)i;
        if (results_load(slot, &r) == ESP_OK) {
            printf(" Slot %-9s: stored, RSSI mean %.2f dBm over %" PRIu32
                   " samples\n",
                   rf_slot_name(slot), r.rssi_mean, r.rssi_samples);
        } else {
            printf(" Slot %-9s: empty\n", rf_slot_name(slot));
        }
    }
    printf("=====================================================\n\n");
}

static void start_console(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "antenna>";
    repl_config.max_cmdline_length = 128;

    /* No filesystem is mounted, so command history lives in RAM only. */
    repl_config.history_save_path = NULL;

#if defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config =
        ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(
        esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));
#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config =
        ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(
        esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));
#else
    esp_console_dev_uart_config_t hw_config =
        ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
#endif

    ESP_ERROR_CHECK(esp_console_register_help_command());
    console_cmds_register();

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(results_init());
    ESP_ERROR_CHECK(wifi_link_init());

    print_banner();
    print_stored_state();

    ESP_LOGI(TAG, "console ready");
    start_console();
}
