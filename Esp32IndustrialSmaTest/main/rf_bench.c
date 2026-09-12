#include "rf_bench.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "lwip/netdb.h"
#include "ping/ping_sock.h"
#include "sdkconfig.h"
#include "wifi_link.h"

static const char *TAG = "rf_bench";

static volatile bool s_monitor_abort;

/* ------------------------------------------------------------------ stats -- */

static int cmp_int8(const void *a, const void *b)
{
    return (int)(*(const int8_t *)a) - (int)(*(const int8_t *)b);
}

/* Percentile on the already-sorted series, nearest-rank. */
static int8_t percentile_sorted(const int8_t *sorted, uint32_t n, float pct)
{
    if (n == 0) {
        return 0;
    }
    int32_t rank = (int32_t)ceilf(pct / 100.0f * (float)n) - 1;
    if (rank < 0) {
        rank = 0;
    }
    if (rank >= (int32_t)n) {
        rank = (int32_t)n - 1;
    }
    return sorted[rank];
}

static void summarise_rssi(int8_t *samples, uint32_t n, rf_result_t *out)
{
    if (n == 0) {
        return;
    }

    double sum = 0.0;
    for (uint32_t i = 0; i < n; i++) {
        sum += samples[i];
    }
    const double mean = sum / (double)n;

    double sq = 0.0;
    for (uint32_t i = 0; i < n; i++) {
        const double d = (double)samples[i] - mean;
        sq += d * d;
    }
    /* Sample standard deviation (n-1); with n == 1 there is no spread. */
    const double stddev = (n > 1) ? sqrt(sq / (double)(n - 1)) : 0.0;

    qsort(samples, n, sizeof(samples[0]), cmp_int8);

    out->rssi_samples = n;
    out->rssi_mean    = (float)mean;
    out->rssi_stddev  = (float)stddev;
    out->rssi_min     = samples[0];
    out->rssi_max     = samples[n - 1];
    out->rssi_median  = percentile_sorted(samples, n, 50.0f);
    out->rssi_p10     = percentile_sorted(samples, n, 10.0f);
}

/* ------------------------------------------------------------------- ping -- */

typedef struct {
    SemaphoreHandle_t done;
    uint32_t          rtt_sum_ms;
    uint32_t          rtt_max_ms;
    uint32_t          transmitted;
    uint32_t          received;
} ping_ctx_t;

static void on_ping_success(esp_ping_handle_t hdl, void *args)
{
    ping_ctx_t *ctx = args;
    uint32_t elapsed = 0;
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed, sizeof(elapsed));

    ctx->rtt_sum_ms += elapsed;
    if (elapsed > ctx->rtt_max_ms) {
        ctx->rtt_max_ms = elapsed;
    }
}

static void on_ping_end(esp_ping_handle_t hdl, void *args)
{
    ping_ctx_t *ctx = args;
    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &ctx->transmitted,
                         sizeof(ctx->transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &ctx->received,
                         sizeof(ctx->received));
    xSemaphoreGive(ctx->done);
}

static esp_err_t resolve_target(ip_addr_t *addr)
{
    char target[64];

    if (strlen(CONFIG_AB_PING_TARGET) > 0) {
        strlcpy(target, CONFIG_AB_PING_TARGET, sizeof(target));
    } else {
        esp_err_t err = wifi_link_gateway_str(target, sizeof(target));
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "no default gateway to ping");
            return err;
        }
    }

    /* Dotted quad first -- avoids a DNS round trip for the common case. */
    if (ipaddr_aton(target, addr)) {
        ESP_LOGI(TAG, "ping target %s", target);
        return ESP_OK;
    }

    struct addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_RAW };
    struct addrinfo *res = NULL;
    if (getaddrinfo(target, NULL, &hints, &res) != 0 || res == NULL) {
        ESP_LOGW(TAG, "cannot resolve ping target '%s'", target);
        return ESP_ERR_NOT_FOUND;
    }

    struct in_addr v4 = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    inet_addr_to_ip4addr(ip_2_ip4(addr), &v4);
    addr->type = IPADDR_TYPE_V4;
    freeaddrinfo(res);

    ESP_LOGI(TAG, "ping target %s -> " IPSTR, target, IP2STR(ip_2_ip4(addr)));
    return ESP_OK;
}

static esp_err_t run_ping(rf_result_t *out)
{
    if (CONFIG_AB_PING_COUNT == 0) {
        return ESP_OK;
    }

    ip_addr_t target;
    esp_err_t err = resolve_target(&target);
    if (err != ESP_OK) {
        return err;
    }

    ping_ctx_t ctx = { 0 };
    ctx.done = xSemaphoreCreateBinary();
    if (ctx.done == NULL) {
        return ESP_ERR_NO_MEM;
    }

    esp_ping_config_t cfg = ESP_PING_DEFAULT_CONFIG();
    cfg.target_addr = target;
    cfg.count       = CONFIG_AB_PING_COUNT;
    cfg.interval_ms = CONFIG_AB_PING_INTERVAL_MS;
    cfg.timeout_ms  = 1000;
    cfg.task_stack_size = 4096;

    esp_ping_callbacks_t cbs = {
        .on_ping_success = on_ping_success,
        .on_ping_timeout = NULL,
        .on_ping_end     = on_ping_end,
        .cb_args         = &ctx,
    };

    esp_ping_handle_t ping = NULL;
    err = esp_ping_new_session(&cfg, &cbs, &ping);
    if (err != ESP_OK) {
        vSemaphoreDelete(ctx.done);
        return err;
    }

    err = esp_ping_start(ping);
    if (err == ESP_OK) {
        /* Worst case is every echo timing out, plus a little slack. */
        const TickType_t budget = pdMS_TO_TICKS(
            (uint32_t)CONFIG_AB_PING_COUNT *
                ((uint32_t)CONFIG_AB_PING_INTERVAL_MS + 1000) + 5000);
        if (xSemaphoreTake(ctx.done, budget) != pdTRUE) {
            ESP_LOGW(TAG, "ping session did not finish in time");
            esp_ping_stop(ping);
            err = ESP_ERR_TIMEOUT;
        }
    }

    esp_ping_delete_session(ping);
    vSemaphoreDelete(ctx.done);

    out->ping_sent = ctx.transmitted;
    out->ping_recv = ctx.received;
    out->ping_rtt_avg_ms =
        (ctx.received > 0) ? (ctx.rtt_sum_ms / ctx.received) : 0;
    out->ping_rtt_max_ms = ctx.rtt_max_ms;

    return err;
}

/* -------------------------------------------------------------- campaign -- */

esp_err_t rf_bench_run(rf_result_t *out)
{
    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(out, 0, sizeof(*out));
    out->magic   = RF_RESULT_MAGIC;
    out->version = RF_RESULT_VERSION;
    strlcpy(out->ssid, CONFIG_AB_WIFI_SSID, sizeof(out->ssid));
    out->uptime_us_at_run = (uint64_t)esp_timer_get_time();

    /* Stage 1: AP census, taken while disconnected so the scan is not fighting
     * an active association for airtime. */
    ESP_LOGI(TAG, "stage 1/4: scanning all channels");
    wifi_link_disconnect();

    wifi_scan_census_t census;
    esp_err_t err = wifi_link_scan(&census);
    if (err != ESP_OK) {
        return err;
    }
    out->scan_ap_count    = census.ap_count;
    out->scan_rssi_mean   = census.rssi_mean;
    out->scan_rssi_best   = census.rssi_best;
    out->scan_target_rssi = census.target_found ? census.target_rssi : 0;
    out->census_n         = census.top_n;
    memcpy(out->census, census.top, sizeof(out->census));

    ESP_LOGI(TAG, "  heard %u AP(s), mean %.1f dBm, best %d dBm",
             (unsigned)census.ap_count, census.rssi_mean, census.rssi_best);
    if (!census.target_found) {
        ESP_LOGE(TAG, "  reference AP '%s' was NOT heard in the scan",
                 CONFIG_AB_WIFI_SSID);
        if (strcmp(CONFIG_AB_WIFI_SSID, "myssid") == 0) {
            ESP_LOGE(TAG, "  that is the placeholder SSID -- this build has no "
                          "AP configured.");
            ESP_LOGE(TAG, "  Run 'idf.py menuconfig' -> Antenna A/B Test "
                          "Configuration. Note that");
            ESP_LOGE(TAG, "  editing sdkconfig.defaults will NOT change an "
                          "existing sdkconfig.");
        }
        ESP_LOGE(TAG, "  Stages 2-4 will fail and only the AP census will hold "
                      "real data.");
    }

    /* Stage 2: associate. */
    ESP_LOGI(TAG, "stage 2/4: associating with '%s'", CONFIG_AB_WIFI_SSID);
    uint32_t connect_ms = 0;
    err = wifi_link_connect(&connect_ms);
    if (err != ESP_OK) {
        /* A run that cannot associate is still a data point -- record it. */
        ESP_LOGE(TAG, "connect failed: %s", esp_err_to_name(err));
        out->connect_ok = false;
        out->tx_power_qdbm = wifi_link_tx_power_qdbm();
        return ESP_OK;
    }
    out->connect_ok = true;
    out->connect_ms = connect_ms;
    ESP_LOGI(TAG, "  associated in %u ms", (unsigned)connect_ms);

    wifi_ap_record_t ap;
    if (wifi_link_ap_info(&ap) == ESP_OK) {
        memcpy(out->bssid, ap.bssid, sizeof(out->bssid));
        out->channel = ap.primary;
    }
    out->tx_power_qdbm = wifi_link_tx_power_qdbm();

    /* Stage 3: RSSI series. */
    const uint32_t n_target = CONFIG_AB_RSSI_SAMPLES;
    ESP_LOGI(TAG, "stage 3/4: sampling RSSI %ux at %d ms (%.1f s)",
             (unsigned)n_target, CONFIG_AB_RSSI_INTERVAL_MS,
             n_target * CONFIG_AB_RSSI_INTERVAL_MS / 1000.0f);

    int8_t *samples = malloc(n_target * sizeof(int8_t));
    if (samples == NULL) {
        return ESP_ERR_NO_MEM;
    }

    uint32_t taken = 0;
    for (uint32_t i = 0; i < n_target; i++) {
        wifi_ap_record_t rec;
        if (wifi_link_ap_info(&rec) == ESP_OK) {
            samples[taken++] = rec.rssi;
        }
        vTaskDelay(pdMS_TO_TICKS(CONFIG_AB_RSSI_INTERVAL_MS));
    }

    if (taken == 0) {
        ESP_LOGE(TAG, "  link dropped before a single sample landed");
        free(samples);
        return ESP_ERR_INVALID_STATE;
    }
    if (taken < n_target) {
        ESP_LOGW(TAG, "  only %u of %u samples captured (link dropped)",
                 (unsigned)taken, (unsigned)n_target);
    }

    summarise_rssi(samples, taken, out);
    free(samples);

    ESP_LOGI(TAG, "  mean %.2f dBm, sd %.2f, min %d, max %d",
             out->rssi_mean, out->rssi_stddev, out->rssi_min, out->rssi_max);

    /* Stage 4: round-trip link quality. */
    if (CONFIG_AB_PING_COUNT > 0) {
        ESP_LOGI(TAG, "stage 4/4: %d ICMP echoes", CONFIG_AB_PING_COUNT);
        err = run_ping(out);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "  ping stage incomplete: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "  %u/%u replies, avg %u ms",
                     (unsigned)out->ping_recv, (unsigned)out->ping_sent,
                     (unsigned)out->ping_rtt_avg_ms);
        }
    } else {
        ESP_LOGI(TAG, "stage 4/4: ping disabled in config, skipping");
    }

    return ESP_OK;
}

/* --------------------------------------------------------------- monitor -- */

void rf_bench_monitor_stop(void)
{
    s_monitor_abort = true;
}

esp_err_t rf_bench_monitor(uint32_t duration_s, uint32_t interval_ms)
{
    if (interval_ms == 0) {
        interval_ms = 500;
    }
    s_monitor_abort = false;

    if (!wifi_link_is_connected()) {
        ESP_LOGI(TAG, "not associated, connecting first");
        esp_err_t err = wifi_link_connect(NULL);
        if (err != ESP_OK) {
            return err;
        }
    }

    const int64_t deadline_us =
        (duration_s > 0)
            ? esp_timer_get_time() + (int64_t)duration_s * 1000000
            : INT64_MAX;

    printf("\n  elapsed_s  rssi_dbm  channel\n");
    printf("  ---------  --------  -------\n");

    const int64_t started = esp_timer_get_time();
    while (!s_monitor_abort && esp_timer_get_time() < deadline_us) {
        wifi_ap_record_t rec;
        const float elapsed = (esp_timer_get_time() - started) / 1000000.0f;

        if (wifi_link_ap_info(&rec) == ESP_OK) {
            printf("  %9.1f  %8d  %7u\n", elapsed, rec.rssi, rec.primary);
        } else {
            printf("  %9.1f  %8s  %7s\n", elapsed, "--", "--");
        }
        vTaskDelay(pdMS_TO_TICKS(interval_ms));
    }

    printf("\n");
    return ESP_OK;
}
