#include "console_cmds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "rf_bench.h"
#include "results.h"
#include "sdkconfig.h"
#include "wifi_link.h"

static const char *TAG = "console";

/* ---------------------------------------------------------------- measure -- */

static struct {
    struct arg_str *slot;
    struct arg_end *end;
} s_measure_args;

static int cmd_measure(int argc, char **argv)
{
    int errors = arg_parse(argc, argv, (void **)&s_measure_args);
    if (errors != 0) {
        arg_print_errors(stderr, s_measure_args.end, argv[0]);
        return 1;
    }

    const rf_slot_t slot = rf_slot_parse(s_measure_args.slot->sval[0]);
    if (slot == RF_SLOT_COUNT) {
        printf("Unknown slot '%s'. Use 'onboard' or 'external'.\n",
               s_measure_args.slot->sval[0]);
        return 1;
    }

    printf("\nMeasuring slot '%s'\n  %s\n", rf_slot_name(slot),
           rf_slot_description(slot));
    printf("Do not move the board until the run finishes.\n\n");

    rf_result_t result;
    esp_err_t err = rf_bench_run(&result);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "run failed: %s", esp_err_to_name(err));
        return 1;
    }

    err = results_save(slot, &result);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "could not persist result: %s", esp_err_to_name(err));
        return 1;
    }

    results_print_one(slot, &result);
    printf("Saved to slot '%s'. It survives a reboot, so you can power the\n"
           "board down, rework the antenna link, and measure the other slot.\n\n",
           rf_slot_name(slot));
    return 0;
}

/* ----------------------------------------------------------------- report -- */

static int cmd_report(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return (results_print_comparison() == ESP_OK) ? 0 : 1;
}

/* ------------------------------------------------------------------- show -- */

static struct {
    struct arg_str *slot;
    struct arg_end *end;
} s_show_args;

static int cmd_show(int argc, char **argv)
{
    int errors = arg_parse(argc, argv, (void **)&s_show_args);
    if (errors != 0) {
        arg_print_errors(stderr, s_show_args.end, argv[0]);
        return 1;
    }

    const rf_slot_t slot = rf_slot_parse(s_show_args.slot->sval[0]);
    if (slot == RF_SLOT_COUNT) {
        printf("Unknown slot '%s'. Use 'onboard' or 'external'.\n",
               s_show_args.slot->sval[0]);
        return 1;
    }

    rf_result_t result;
    esp_err_t err = results_load(slot, &result);
    if (err != ESP_OK) {
        printf("Slot '%s' is empty.\n", rf_slot_name(slot));
        return 1;
    }

    results_print_one(slot, &result);
    return 0;
}

/* ------------------------------------------------------------------- scan -- */

static int cmd_scan(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    wifi_link_disconnect();

    wifi_scan_census_t census;
    esp_err_t err = wifi_link_scan(&census);
    if (err != ESP_OK) {
        printf("Scan failed: %s\n", esp_err_to_name(err));
        return 1;
    }

    printf("\n  APs heard        : %u\n", (unsigned)census.ap_count);
    printf("  Mean RSSI        : %.2f dBm\n", census.rssi_mean);
    printf("  Strongest AP     : %d dBm\n", census.rssi_best);
    if (census.target_found) {
        printf("  '%s' : %d dBm\n", CONFIG_AB_WIFI_SSID, census.target_rssi);
    } else {
        printf("  '%s' : not heard\n", CONFIG_AB_WIFI_SSID);
    }
    printf("\n");
    return 0;
}

/* ---------------------------------------------------------------- monitor -- */

static struct {
    struct arg_int *seconds;
    struct arg_int *interval;
    struct arg_end *end;
} s_monitor_args;

static int cmd_monitor(int argc, char **argv)
{
    int errors = arg_parse(argc, argv, (void **)&s_monitor_args);
    if (errors != 0) {
        arg_print_errors(stderr, s_monitor_args.end, argv[0]);
        return 1;
    }

    const uint32_t seconds =
        (s_monitor_args.seconds->count > 0)
            ? (uint32_t)s_monitor_args.seconds->ival[0]
            : 30;
    const uint32_t interval =
        (s_monitor_args.interval->count > 0)
            ? (uint32_t)s_monitor_args.interval->ival[0]
            : 500;

    printf("Streaming RSSI for %u s. Walk the board around; the console is\n"
           "unresponsive until it finishes.\n",
           (unsigned)seconds);

    esp_err_t err = rf_bench_monitor(seconds, interval);
    if (err != ESP_OK) {
        printf("Monitor failed: %s\n", esp_err_to_name(err));
        return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------ clear -- */

static struct {
    struct arg_str *slot;
    struct arg_end *end;
} s_clear_args;

static int cmd_clear(int argc, char **argv)
{
    int errors = arg_parse(argc, argv, (void **)&s_clear_args);
    if (errors != 0) {
        arg_print_errors(stderr, s_clear_args.end, argv[0]);
        return 1;
    }

    if (s_clear_args.slot->count == 0 ||
        strcasecmp(s_clear_args.slot->sval[0], "all") == 0) {
        results_erase_all();
        printf("Both slots cleared.\n");
        return 0;
    }

    const rf_slot_t slot = rf_slot_parse(s_clear_args.slot->sval[0]);
    if (slot == RF_SLOT_COUNT) {
        printf("Unknown slot '%s'. Use 'onboard', 'external' or 'all'.\n",
               s_clear_args.slot->sval[0]);
        return 1;
    }

    results_erase(slot);
    printf("Slot '%s' cleared.\n", rf_slot_name(slot));
    return 0;
}

/* --------------------------------------------------------------- register -- */

void console_cmds_register(void)
{
    s_measure_args.slot =
        arg_str1(NULL, NULL, "<onboard|external>", "which antenna is fitted");
    s_measure_args.end = arg_end(2);
    const esp_console_cmd_t measure_cmd = {
        .command = "measure",
        .help = "Run a full measurement campaign and store it in the named slot",
        .hint = NULL,
        .func = &cmd_measure,
        .argtable = &s_measure_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&measure_cmd));

    const esp_console_cmd_t report_cmd = {
        .command = "report",
        .help = "Print both stored runs side by side with the dB delta",
        .hint = NULL,
        .func = &cmd_report,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&report_cmd));

    s_show_args.slot =
        arg_str1(NULL, NULL, "<onboard|external>", "slot to print");
    s_show_args.end = arg_end(2);
    const esp_console_cmd_t show_cmd = {
        .command = "show",
        .help = "Print one stored run",
        .hint = NULL,
        .func = &cmd_show,
        .argtable = &s_show_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&show_cmd));

    const esp_console_cmd_t scan_cmd = {
        .command = "scan",
        .help = "One-off AP census -- quick check that the RF path is alive",
        .hint = NULL,
        .func = &cmd_scan,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&scan_cmd));

    s_monitor_args.seconds =
        arg_int0("t", "seconds", "<s>", "duration, default 30");
    s_monitor_args.interval =
        arg_int0("i", "interval", "<ms>", "sample interval, default 500");
    s_monitor_args.end = arg_end(3);
    const esp_console_cmd_t monitor_cmd = {
        .command = "monitor",
        .help = "Stream live RSSI, one line per sample",
        .hint = NULL,
        .func = &cmd_monitor,
        .argtable = &s_monitor_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&monitor_cmd));

    s_clear_args.slot =
        arg_str0(NULL, NULL, "<onboard|external|all>", "default all");
    s_clear_args.end = arg_end(2);
    const esp_console_cmd_t clear_cmd = {
        .command = "clear",
        .help = "Erase stored results",
        .hint = NULL,
        .func = &cmd_clear,
        .argtable = &s_clear_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&clear_cmd));
}
