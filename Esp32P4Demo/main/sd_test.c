/*
 * microSD card test suite for the Waveshare ESP32-P4-WIFI6.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"

#include "board_esp32p4_wifi6.h"
#include "sd_card.h"
#include "sd_test.h"

static const char *TAG = "sd_test";

#define MNT             BSP_SD_MOUNT_POINT
#define MAX_TESTS       8
#define PATH_MAX_LEN    128

typedef struct {
    const char *name;
    bool        passed;
    bool        skipped;
    char        detail[64];
} test_result_t;

static test_result_t s_results[MAX_TESTS];
static int s_result_count;

static void record(const char *name, bool passed, bool skipped, const char *fmt, ...)
{
    if (s_result_count >= MAX_TESTS) {
        return;
    }
    test_result_t *r = &s_results[s_result_count++];
    r->name = name;
    r->passed = passed;
    r->skipped = skipped;
    r->detail[0] = '\0';
    if (fmt) {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(r->detail, sizeof(r->detail), fmt, ap);
        va_end(ap);
    }
}

/* Deterministic byte pattern so a read-back mismatch pinpoints the offset. */
static inline uint8_t pattern_byte(size_t offset)
{
    return (uint8_t)((offset * 31u + (offset >> 8) * 17u + 7u) & 0xFF);
}

/* -------------------------------------------------------------------------
 * Test 1 — basic write, read back, verify content
 * ------------------------------------------------------------------------- */
static bool test_basic_file_io(void)
{
    const char *path = MNT "/basic.txt";
    const char *payload = "ESP32-P4-WIFI6 microSD test payload\n";

    ESP_LOGI(TAG, "[1/5] Basic file write/read");

    FILE *f = fopen(path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "fopen(%s, w) failed: %s", path, strerror(errno));
        record("Basic file I/O", false, false, "open for write failed");
        return false;
    }
    if (fputs(payload, f) == EOF) {
        ESP_LOGE(TAG, "fputs failed: %s", strerror(errno));
        fclose(f);
        record("Basic file I/O", false, false, "write failed");
        return false;
    }
    fclose(f);

    char readback[128] = {0};
    f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "fopen(%s, r) failed: %s", path, strerror(errno));
        record("Basic file I/O", false, false, "open for read failed");
        return false;
    }
    size_t n = fread(readback, 1, sizeof(readback) - 1, f);
    fclose(f);

    if (n != strlen(payload) || strcmp(readback, payload) != 0) {
        ESP_LOGE(TAG, "Read-back mismatch (%u bytes read, expected %u)",
                 (unsigned)n, (unsigned)strlen(payload));
        record("Basic file I/O", false, false, "content mismatch");
        unlink(path);
        return false;
    }

    unlink(path);
    ESP_LOGI(TAG, "      OK — %u bytes round-tripped", (unsigned)n);
    record("Basic file I/O", true, false, "%u bytes verified", (unsigned)n);
    return true;
}

/* -------------------------------------------------------------------------
 * Test 2 — stat, append, rename, unlink
 * ------------------------------------------------------------------------- */
static bool test_file_metadata(void)
{
    const char *src = MNT "/meta_a.txt";
    const char *dst = MNT "/meta_b.txt";
    struct stat st;

    ESP_LOGI(TAG, "[2/5] File metadata: stat / append / rename / delete");

    FILE *f = fopen(src, "w");
    if (f == NULL) {
        record("File metadata", false, false, "create failed");
        return false;
    }
    fprintf(f, "first line\n");
    fclose(f);

    if (stat(src, &st) != 0) {
        ESP_LOGE(TAG, "stat failed: %s", strerror(errno));
        record("File metadata", false, false, "stat failed");
        unlink(src);
        return false;
    }
    const off_t size_after_create = st.st_size;

    /* Append and confirm the size grew. */
    f = fopen(src, "a");
    if (f == NULL) {
        record("File metadata", false, false, "open for append failed");
        unlink(src);
        return false;
    }
    fprintf(f, "second line\n");
    fclose(f);

    if (stat(src, &st) != 0 || st.st_size <= size_after_create) {
        ESP_LOGE(TAG, "Append did not grow the file (%ld -> %ld)",
                 (long)size_after_create, (long)st.st_size);
        record("File metadata", false, false, "append did not grow file");
        unlink(src);
        return false;
    }

    /* Rename over a possibly-existing destination. */
    unlink(dst);
    if (rename(src, dst) != 0) {
        ESP_LOGE(TAG, "rename failed: %s", strerror(errno));
        record("File metadata", false, false, "rename failed");
        unlink(src);
        return false;
    }
    if (stat(src, &st) == 0) {
        ESP_LOGE(TAG, "Source still present after rename");
        record("File metadata", false, false, "source survived rename");
        unlink(dst);
        return false;
    }
    if (stat(dst, &st) != 0) {
        ESP_LOGE(TAG, "Destination missing after rename");
        record("File metadata", false, false, "destination missing");
        return false;
    }

    if (unlink(dst) != 0 || stat(dst, &st) == 0) {
        ESP_LOGE(TAG, "unlink did not remove the file");
        record("File metadata", false, false, "unlink failed");
        return false;
    }

    ESP_LOGI(TAG, "      OK");
    record("File metadata", true, false, "stat/append/rename/unlink");
    return true;
}

/* -------------------------------------------------------------------------
 * Test 3 — directory create, populate, list, remove
 * ------------------------------------------------------------------------- */
static bool test_directories(void)
{
    const char *dir = MNT "/testdir";
    char path[PATH_MAX_LEN];
    const int file_count = 5;

    ESP_LOGI(TAG, "[3/5] Directory create / list / remove");

    /* Clear any leftovers from an aborted earlier run. */
    rmdir(dir);

    if (mkdir(dir, 0777) != 0 && errno != EEXIST) {
        ESP_LOGE(TAG, "mkdir failed: %s", strerror(errno));
        record("Directories", false, false, "mkdir failed");
        return false;
    }

    for (int i = 0; i < file_count; i++) {
        snprintf(path, sizeof(path), "%s/entry_%02d.txt", dir, i);
        FILE *f = fopen(path, "w");
        if (f == NULL) {
            ESP_LOGE(TAG, "Could not create %s: %s", path, strerror(errno));
            record("Directories", false, false, "create in dir failed");
            return false;
        }
        fprintf(f, "entry %d\n", i);
        fclose(f);
    }

    DIR *d = opendir(dir);
    if (d == NULL) {
        ESP_LOGE(TAG, "opendir failed: %s", strerror(errno));
        record("Directories", false, false, "opendir failed");
        return false;
    }
    int listed = 0;
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        listed++;
    }
    closedir(d);

    if (listed != file_count) {
        ESP_LOGE(TAG, "Listed %d entries, expected %d", listed, file_count);
        record("Directories", false, false, "listed %d of %d", listed, file_count);
        return false;
    }

    for (int i = 0; i < file_count; i++) {
        snprintf(path, sizeof(path), "%s/entry_%02d.txt", dir, i);
        unlink(path);
    }
    if (rmdir(dir) != 0) {
        ESP_LOGE(TAG, "rmdir failed: %s", strerror(errno));
        record("Directories", false, false, "rmdir failed");
        return false;
    }

    ESP_LOGI(TAG, "      OK — %d entries created, listed and removed", file_count);
    record("Directories", true, false, "%d entries", file_count);
    return true;
}

/* -------------------------------------------------------------------------
 * Test 4 — many small files
 * ------------------------------------------------------------------------- */
static bool test_many_small_files(void)
{
    const int count = CONFIG_DEMO_SD_STRESS_FILE_COUNT;
    char path[PATH_MAX_LEN];
    char buf[64];

    if (count == 0) {
        ESP_LOGI(TAG, "[4/5] Many-small-files test skipped (count = 0)");
        record("Many small files", true, true, "disabled");
        return true;
    }

    ESP_LOGI(TAG, "[4/5] Many small files (%d)", count);

    const int64_t t_write = esp_timer_get_time();
    for (int i = 0; i < count; i++) {
        snprintf(path, sizeof(path), MNT "/small_%04d.dat", i);
        FILE *f = fopen(path, "w");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to create %s at index %d: %s",
                     path, i, strerror(errno));
            record("Many small files", false, false, "create failed at %d", i);
            return false;
        }
        fprintf(f, "file index %d checksum %d\n", i, i * 7919);
        fclose(f);
    }
    const int64_t write_us = esp_timer_get_time() - t_write;

    /* Read each one back and confirm it holds its own index. */
    for (int i = 0; i < count; i++) {
        snprintf(path, sizeof(path), MNT "/small_%04d.dat", i);
        FILE *f = fopen(path, "r");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open %s: %s", path, strerror(errno));
            record("Many small files", false, false, "read failed at %d", i);
            return false;
        }
        memset(buf, 0, sizeof(buf));
        (void)fgets(buf, sizeof(buf), f);
        fclose(f);

        int idx = -1;
        int checksum = -1;
        if (sscanf(buf, "file index %d checksum %d", &idx, &checksum) != 2 ||
            idx != i || checksum != i * 7919) {
            ESP_LOGE(TAG, "Content mismatch in %s: got \"%s\"", path, buf);
            record("Many small files", false, false, "mismatch at %d", i);
            return false;
        }
    }

    for (int i = 0; i < count; i++) {
        snprintf(path, sizeof(path), MNT "/small_%04d.dat", i);
        unlink(path);
    }

    ESP_LOGI(TAG, "      OK — %d files written in %.0f ms (%.1f files/s), "
                  "all verified and removed",
             count, write_us / 1000.0, count * 1000000.0 / (double)write_us);
    record("Many small files", true, false, "%d files, %.0f/s",
           count, count * 1000000.0 / (double)write_us);
    return true;
}

/* -------------------------------------------------------------------------
 * Test 5 — sequential throughput with full pattern verification
 * ------------------------------------------------------------------------- */
static bool test_throughput(void)
{
    const size_t total_bytes = (size_t)CONFIG_DEMO_SD_BENCH_MB * 1024 * 1024;
    const size_t chunk_bytes = (size_t)CONFIG_DEMO_SD_BENCH_CHUNK_KB * 1024;
    const char *path = MNT "/bench.bin";

    if (CONFIG_DEMO_SD_BENCH_MB == 0) {
        ESP_LOGI(TAG, "[5/5] Throughput benchmark skipped (size = 0 MB)");
        record("Throughput", true, true, "disabled");
        return true;
    }

    ESP_LOGI(TAG, "[5/5] Sequential throughput — %d MB in %d KB chunks",
             CONFIG_DEMO_SD_BENCH_MB, CONFIG_DEMO_SD_BENCH_CHUNK_KB);

    /* DMA-capable and aligned to the cache line, so the SDMMC driver can DMA
     * straight out of it instead of staging through a bounce buffer.
     *
     * Honest note on the benefit: write throughput on this board measured
     * between 1.8 and 2.6 MB/s both with and without the alignment. Card state
     * (freshly formatted or not) moved the number more than the alignment did,
     * so treat this as correctness hygiene rather than a proven speed-up. The
     * read-side win from POSIX I/O below is real and reproducible. */
    uint8_t *buf = heap_caps_aligned_alloc(CONFIG_CACHE_L2_CACHE_LINE_SIZE,
                                           chunk_bytes,
                                           MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    if (buf == NULL) {
        ESP_LOGE(TAG, "Could not allocate a %u byte aligned DMA buffer",
                 (unsigned)chunk_bytes);
        record("Throughput", false, false, "buffer alloc failed");
        return false;
    }

    const size_t chunks = total_bytes / chunk_bytes;
    bool ok = false;

    do {
        /* Raw POSIX I/O rather than stdio. This is not a micro-optimisation:
         * newlib's FILE buffer is small, so a 64 KB fread() is serviced as a
         * long run of tiny reads, each paying full VFS + FATFS + SDMMC
         * overhead. Measured on this board, that costs 16x on reads —
         * 0.72 MB/s via fread() against 12.6 MB/s via read(). */

        /* ---- write ---- */
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd < 0) {
            ESP_LOGE(TAG, "open(%s, write) failed: %s", path, strerror(errno));
            record("Throughput", false, false, "open for write failed");
            break;
        }

        int64_t t0 = esp_timer_get_time();
        size_t written = 0;
        for (size_t c = 0; c < chunks; c++) {
            for (size_t i = 0; i < chunk_bytes; i++) {
                buf[i] = pattern_byte(written + i);
            }
            if (write(fd, buf, chunk_bytes) != (ssize_t)chunk_bytes) {
                ESP_LOGE(TAG, "Short write at chunk %u: %s",
                         (unsigned)c, strerror(errno));
                close(fd);
                record("Throughput", false, false, "short write at %uKB",
                       (unsigned)(written / 1024));
                goto cleanup;
            }
            written += chunk_bytes;
        }
        fsync(fd);
        const int64_t write_us = esp_timer_get_time() - t0;
        close(fd);

        /* ---- read back and verify every byte ---- */
        fd = open(path, O_RDONLY, 0);
        if (fd < 0) {
            ESP_LOGE(TAG, "open(%s, read) failed: %s", path, strerror(errno));
            record("Throughput", false, false, "open for read failed");
            break;
        }

        t0 = esp_timer_get_time();
        size_t read_total = 0;
        int64_t read_us = 0;
        bool mismatch = false;
        for (size_t c = 0; c < chunks; c++) {
            const int64_t tr = esp_timer_get_time();
            if (read(fd, buf, chunk_bytes) != (ssize_t)chunk_bytes) {
                ESP_LOGE(TAG, "Short read at chunk %u: %s",
                         (unsigned)c, strerror(errno));
                close(fd);
                record("Throughput", false, false, "short read at %uKB",
                       (unsigned)(read_total / 1024));
                goto cleanup;
            }
            read_us += esp_timer_get_time() - tr;

            for (size_t i = 0; i < chunk_bytes; i++) {
                if (buf[i] != pattern_byte(read_total + i)) {
                    ESP_LOGE(TAG, "Data corruption at byte offset %u: "
                                  "got 0x%02x, expected 0x%02x",
                             (unsigned)(read_total + i), buf[i],
                             pattern_byte(read_total + i));
                    mismatch = true;
                    break;
                }
            }
            if (mismatch) {
                break;
            }
            read_total += chunk_bytes;
        }
        close(fd);
        (void)t0;

        if (mismatch) {
            record("Throughput", false, false, "corruption at %uKB",
                   (unsigned)(read_total / 1024));
            break;
        }

        const double mb = (double)total_bytes / (1024.0 * 1024.0);
        const double write_mbs = mb / ((double)write_us / 1000000.0);
        const double read_mbs  = mb / ((double)read_us / 1000000.0);

        ESP_LOGI(TAG, "      Write: %.2f MB/s (%.0f ms)", write_mbs, write_us / 1000.0);
        ESP_LOGI(TAG, "      Read : %.2f MB/s (%.0f ms)", read_mbs, read_us / 1000.0);
        ESP_LOGI(TAG, "      OK — all %u bytes verified", (unsigned)total_bytes);
        record("Throughput", true, false, "W %.1f / R %.1f MB/s",
               write_mbs, read_mbs);
        ok = true;
    } while (0);

cleanup:
    heap_caps_free(buf);
    unlink(path);
    return ok;
}

/* -------------------------------------------------------------------------
 * Runner
 * ------------------------------------------------------------------------- */
esp_err_t sd_test_run_all(void)
{
    s_result_count = 0;

    printf("\n");
    ESP_LOGI(TAG, "==================== microSD test suite ====================");

    bool all_ok = true;
    all_ok &= test_basic_file_io();
    all_ok &= test_file_metadata();
    all_ok &= test_directories();
    all_ok &= test_many_small_files();
    all_ok &= test_throughput();

    static const char *rule =
        "+------------------------+--------+---------------------------+\n";

    printf("\n");
    printf("%s", rule);
    printf("| %-22s | %-6s | %-25s |\n", "Test", "Result", "Detail");
    printf("%s", rule);
    for (int i = 0; i < s_result_count; i++) {
        const test_result_t *r = &s_results[i];
        const char *verdict = r->skipped ? "SKIP" : (r->passed ? "PASS" : "FAIL");
        printf("| %-22.22s | %-6s | %-25.25s |\n", r->name, verdict, r->detail);
    }
    printf("%s\n", rule);

    if (all_ok) {
        ESP_LOGI(TAG, "All microSD tests passed");
        return ESP_OK;
    }
    ESP_LOGE(TAG, "One or more microSD tests FAILED");
    return ESP_FAIL;
}
