#include "results.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "results";

/* A dedicated NVS partition keeps the measurement record away from the Wi-Fi
 * and PHY calibration data in the default `nvs` partition. */
#define RESULTS_PARTITION "results"
#define RESULTS_NAMESPACE "antenna_ab"

static const char *const SLOT_KEY[RF_SLOT_COUNT] = {
    [RF_SLOT_ONBOARD]  = "slot_onboard",
    [RF_SLOT_EXTERNAL] = "slot_external",
};

static bool s_initialised;

const char *rf_slot_name(rf_slot_t slot)
{
    switch (slot) {
    case RF_SLOT_ONBOARD:  return "onboard";
    case RF_SLOT_EXTERNAL: return "external";
    default:               return "?";
    }
}

const char *rf_slot_description(rf_slot_t slot)
{
    switch (slot) {
    case RF_SLOT_ONBOARD:
        return "patch ceramic antenna (0R link vertical, factory default)";
    case RF_SLOT_EXTERNAL:
        return "IPEX Gen1 -> SMA antenna (0R link horizontal)";
    default:
        return "unknown";
    }
}

rf_slot_t rf_slot_parse(const char *text)
{
    if (text == NULL) {
        return RF_SLOT_COUNT;
    }
    if (strcasecmp(text, "onboard") == 0 || strcasecmp(text, "a") == 0 ||
        strcasecmp(text, "internal") == 0 || strcasecmp(text, "ceramic") == 0) {
        return RF_SLOT_ONBOARD;
    }
    if (strcasecmp(text, "external") == 0 || strcasecmp(text, "b") == 0 ||
        strcasecmp(text, "sma") == 0 || strcasecmp(text, "ipex") == 0) {
        return RF_SLOT_EXTERNAL;
    }
    return RF_SLOT_COUNT;
}

esp_err_t results_init(void)
{
    if (s_initialised) {
        return ESP_OK;
    }

    esp_err_t err = nvs_flash_init_partition(RESULTS_PARTITION);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "results partition needs erasing, reformatting");
        ESP_ERROR_CHECK(nvs_flash_erase_partition(RESULTS_PARTITION));
        err = nvs_flash_init_partition(RESULTS_PARTITION);
    }
    if (err != ESP_OK) {
        return err;
    }

    s_initialised = true;
    return ESP_OK;
}

esp_err_t results_save(rf_slot_t slot, const rf_result_t *result)
{
    if (slot >= RF_SLOT_COUNT || result == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(RESULTS_PARTITION,
                                           RESULTS_NAMESPACE,
                                           NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(handle, SLOT_KEY[slot], result, sizeof(*result));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

esp_err_t results_load(rf_slot_t slot, rf_result_t *out)
{
    if (slot >= RF_SLOT_COUNT || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(RESULTS_PARTITION,
                                           RESULTS_NAMESPACE,
                                           NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t len = sizeof(*out);
    err = nvs_get_blob(handle, SLOT_KEY[slot], out, &len);
    nvs_close(handle);

    if (err != ESP_OK) {
        return err;
    }
    if (len != sizeof(*out) || out->magic != RF_RESULT_MAGIC ||
        out->version != RF_RESULT_VERSION) {
        ESP_LOGW(TAG, "slot '%s' holds an incompatible record, ignoring",
                 rf_slot_name(slot));
        return ESP_ERR_INVALID_VERSION;
    }
    return ESP_OK;
}

esp_err_t results_erase(rf_slot_t slot)
{
    if (slot >= RF_SLOT_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition(RESULTS_PARTITION,
                                           RESULTS_NAMESPACE,
                                           NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_erase_key(handle, SLOT_KEY[slot]);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        err = ESP_OK; /* already empty */
    }
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

esp_err_t results_erase_all(void)
{
    esp_err_t err = ESP_OK;
    for (int i = 0; i < RF_SLOT_COUNT; i++) {
        esp_err_t one = results_erase((rf_slot_t)i);
        if (one != ESP_OK) {
            err = one;
        }
    }
    return err;
}

/* ------------------------------------------------------------- reporting -- */

static void format_bssid(const uint8_t bssid[6], char *buf, size_t len)
{
    snprintf(buf, len, "%02x:%02x:%02x:%02x:%02x:%02x",
             bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
}

void results_print_one(rf_slot_t slot, const rf_result_t *r)
{
    char bssid[18];
    format_bssid(r->bssid, bssid, sizeof(bssid));

    const float loss_pct =
        (r->ping_sent > 0)
            ? 100.0f * (float)(r->ping_sent - r->ping_recv) / (float)r->ping_sent
            : 0.0f;

    printf("\n=== %s -- %s ===\n", rf_slot_name(slot),
           rf_slot_description(slot));
    printf("  SSID / BSSID       : %s / %s (ch %u)\n", r->ssid, bssid,
           r->channel);
    printf("  Associated         : %s", r->connect_ok ? "yes" : "NO");
    if (r->connect_ok) {
        printf(" (%" PRIu32 " ms)", r->connect_ms);
    }
    printf("\n");
    printf("  Max TX power       : %.2f dBm\n", r->tx_power_qdbm / 4.0f);
    printf("  --- RSSI over %-6" PRIu32 " samples ---\n", r->rssi_samples);
    printf("    mean             : %.2f dBm\n", r->rssi_mean);
    printf("    std deviation    : %.2f dB\n", r->rssi_stddev);
    printf("    median           : %d dBm\n", r->rssi_median);
    printf("    10th percentile  : %d dBm\n", r->rssi_p10);
    printf("    min / max        : %d / %d dBm\n", r->rssi_min, r->rssi_max);
    printf("  --- ICMP echo ---\n");
    printf("    sent / received  : %" PRIu32 " / %" PRIu32 " (%.1f%% loss)\n",
           r->ping_sent, r->ping_recv, loss_pct);
    printf("    RTT avg / max    : %" PRIu32 " / %" PRIu32 " ms\n",
           r->ping_rtt_avg_ms, r->ping_rtt_max_ms);
    printf("  --- AP census (all channels) ---\n");
    printf("    APs heard        : %" PRIu32 "\n", r->scan_ap_count);
    printf("    mean RSSI        : %.2f dBm\n", r->scan_rssi_mean);
    printf("    strongest AP     : %d dBm\n", r->scan_rssi_best);
    printf("    reference AP     : %d dBm\n", r->scan_target_rssi);
    printf("\n");
}

static void row_f(const char *label, float a, float b, const char *unit,
                  int decimals)
{
    printf("  %-22s %10.*f %10.*f %+10.*f %s\n", label, decimals, a, decimals,
           b, decimals, b - a, unit);
}

static void row_u(const char *label, uint32_t a, uint32_t b, const char *unit)
{
    printf("  %-22s %10" PRIu32 " %10" PRIu32 " %+10d %s\n", label, a, b,
           (int)b - (int)a, unit);
}

/* Paired per-BSSID comparison.
 *
 * Comparing two scan means is only meaningful if both scans heard the same set
 * of APs. They usually do not: the weak tail of a scan drops in and out between
 * consecutive scans regardless of the antenna, and losing a couple of -85 dBm
 * neighbours drags the mean several dB. Matching on BSSID and averaging the
 * per-AP differences removes that entirely -- every term in the average is the
 * same radio measured twice. */
static void print_paired_census(const rf_result_t *on, const rf_result_t *ext)
{
    int32_t sum_delta = 0;
    uint32_t matched = 0;

    printf("  --- paired per-BSSID census (same AP measured in both runs) ---\n");

    for (uint32_t i = 0; i < on->census_n && i < RF_CENSUS_TOP_N; i++) {
        for (uint32_t j = 0; j < ext->census_n && j < RF_CENSUS_TOP_N; j++) {
            if (memcmp(on->census[i].bssid, ext->census[j].bssid, 6) != 0) {
                continue;
            }
            char bssid[18];
            format_bssid(on->census[i].bssid, bssid, sizeof(bssid));
            const int delta = (int)ext->census[j].rssi - (int)on->census[i].rssi;
            printf("    %s  %4d -> %4d dBm  %+d\n", bssid, on->census[i].rssi,
                   ext->census[j].rssi, delta);
            sum_delta += delta;
            matched++;
            break;
        }
    }

    if (matched == 0) {
        printf("    No BSSIDs common to both runs -- cannot pair. Either the\n"
               "    runs are from different locations, or one predates this\n"
               "    firmware's census recording.\n\n");
        return;
    }

    const float paired = (float)sum_delta / (float)matched;
    printf("    ------------------------------------------------\n");
    printf("    %" PRIu32 " AP(s) matched, mean paired delta %+.2f dB\n",
           matched, paired);
    printf("\n");
    printf("    This is the figure to trust over 'Scan mean RSSI' above --\n");
    printf("    it is immune to the two scans hearing different AP sets.\n");

    if (matched < 4) {
        printf("    CAUTION: only %" PRIu32 " AP(s) matched. Thin basis for a\n"
               "             conclusion; prefer the connected-link RSSI series.\n",
               matched);
    }
    printf("\n");
}

esp_err_t results_print_comparison(void)
{
    rf_result_t on, ext;
    const esp_err_t err_on  = results_load(RF_SLOT_ONBOARD, &on);
    const esp_err_t err_ext = results_load(RF_SLOT_EXTERNAL, &ext);

    if (err_on != ESP_OK || err_ext != ESP_OK) {
        if (err_on != ESP_OK) {
            printf("Slot 'onboard' is empty  -- run: measure onboard\n");
        }
        if (err_ext != ESP_OK) {
            printf("Slot 'external' is empty -- run: measure external\n");
        }
        return ESP_ERR_NOT_FOUND;
    }

    results_print_one(RF_SLOT_ONBOARD, &on);
    results_print_one(RF_SLOT_EXTERNAL, &ext);

    /* A run that never associated has zero-filled RSSI and ping fields. Those
     * are absent data, not measurements of zero, and printing them in a delta
     * table invites exactly the wrong conclusion. */
    const bool rssi_valid = (on.rssi_samples > 0) && (ext.rssi_samples > 0);
    const bool ping_valid = (on.ping_sent > 0) && (ext.ping_sent > 0);

    printf("=== A/B comparison ===\n");
    printf("  %-22s %10s %10s %10s\n", "", "onboard", "external", "delta");
    printf("  %-22s %10s %10s %10s\n", "----------------------", "----------",
           "----------", "----------");

    if (rssi_valid) {
        row_f("RSSI mean", on.rssi_mean, ext.rssi_mean, "dB", 2);
        row_f("RSSI median", on.rssi_median, ext.rssi_median, "dB", 0);
        row_f("RSSI 10th pct", on.rssi_p10, ext.rssi_p10, "dB", 0);
        row_f("RSSI std deviation", on.rssi_stddev, ext.rssi_stddev, "dB", 2);
        row_u("Associate time", on.connect_ms, ext.connect_ms, "ms");
    }
    row_f("Scan mean RSSI", on.scan_rssi_mean, ext.scan_rssi_mean, "dB", 2);
    row_u("APs heard", on.scan_ap_count, ext.scan_ap_count, "");
    if (ping_valid) {
        row_u("ICMP replies", on.ping_recv, ext.ping_recv, "");
        row_u("ICMP RTT avg", on.ping_rtt_avg_ms, ext.ping_rtt_avg_ms, "ms");
    }
    printf("\n");

    print_paired_census(&on, &ext);

    if (!rssi_valid) {
        printf("  NO CONNECTED-LINK RESULT.\n");
        printf("    %s run(s) never associated with '%s', so the RSSI series,\n",
               (on.rssi_samples == 0 && ext.rssi_samples == 0) ? "Both"
                                                               : "One of the",
               on.ssid);
        printf("    associate time and ICMP figures were never measured. They\n");
        printf("    are omitted above rather than shown as zero.\n");
        if (strcmp(on.ssid, "myssid") == 0) {
            printf("    '%s' is the placeholder SSID: configure a real AP with\n",
                   on.ssid);
            printf("    'idf.py menuconfig', rebuild, reflash, and re-run both\n");
            printf("    slots. Editing sdkconfig.defaults will not change an\n");
            printf("    existing sdkconfig.\n");
        }
        printf("    The AP census above is still valid -- but read the caveat\n");
        printf("    on unpaired means in the paired-census section.\n\n");
        return ESP_OK;
    }

    const float gain = ext.rssi_mean - on.rssi_mean;
    printf("  External antenna is %+.2f dB relative to the onboard patch.\n",
           gain);

    /* Pooled spread of the two runs, as a sanity check on whether the gap is
     * bigger than the noise it was measured in. */
    const float noise = (on.rssi_stddev + ext.rssi_stddev) / 2.0f;
    if (noise > 0.0f && fabsf(gain) < noise) {
        printf("  NOTE: that gap (%.2f dB) is smaller than the mean per-run\n"
               "        standard deviation (%.2f dB). Treat it as noise, not\n"
               "        as a difference -- take longer runs or move further\n"
               "        from the AP to open up the gap.\n",
               fabsf(gain), noise);
    }

    /* Comparing two runs against different APs or channels is meaningless. */
    if (memcmp(on.bssid, ext.bssid, sizeof(on.bssid)) != 0) {
        printf("  WARNING: the two runs associated with different BSSIDs.\n"
                "           The comparison is not valid.\n");
    } else if (on.channel != ext.channel) {
        printf("  WARNING: the two runs used different channels (%u vs %u).\n"
                "           Path loss differs across the band; re-run.\n",
               on.channel, ext.channel);
    }
    if (on.tx_power_qdbm != ext.tx_power_qdbm) {
        printf("  WARNING: max TX power differed between runs (%.2f vs %.2f "
               "dBm).\n",
               on.tx_power_qdbm / 4.0f, ext.tx_power_qdbm / 4.0f);
    }
    printf("\n");

    return ESP_OK;
}
