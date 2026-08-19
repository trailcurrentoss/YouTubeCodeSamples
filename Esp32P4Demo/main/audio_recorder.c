/*
 * Microphone capture for the Waveshare ESP32-P4-WIFI6.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "es8311.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

#include "audio_recorder.h"
#include "board_esp32p4_wifi6.h"

static const char *TAG = "audio";

#define SAMPLE_RATE     CONFIG_DEMO_AUDIO_SAMPLE_RATE
#define I2C_PORT        I2C_NUM_0
#define I2C_FREQ_HZ     100000
#define REC_DIR         "/recordings"
#define CHUNK_BYTES     4096

static i2s_chan_handle_t s_rx;
static i2s_chan_handle_t s_tx;
static es8311_handle_t   s_codec;
static bool              s_ready;

/* -------------------------------------------------------------------------
 * Codec clocking
 *
 * The ES8311 derives its internal clocks from MCLK, and its driver only
 * accepts MCLK/rate pairs that appear in its coefficient table. The multiple
 * that works is NOT the same across rate families:
 *
 *   8 / 16 / 32 / 48 kHz   ->  384x   (3.072, 6.144, 12.288, 18.432 MHz)
 *   22.05 / 24 / 44.1 kHz  ->  256x   (5.6448, 6.144, 11.2896 MHz)
 *
 * 384x is simply absent from the table for the 44.1 kHz family, so a fixed
 * multiple cannot cover both. The assistant's replies are synthesised at
 * 22.05 kHz, which is exactly the case a hardcoded 384x rejects:
 * "Unable to configure sample rate 22050Hz with 8467200Hz MCLK".
 * ------------------------------------------------------------------------- */
static uint32_t mclk_multiple_for(uint32_t rate)
{
    switch (rate) {
    case 8000: case 16000: case 32000: case 48000:
        return 384;
    case 22050: case 24000: case 44100:
        return 256;
    default:
        return 0;                       /* not in the codec's table */
    }
}

static i2s_mclk_multiple_t i2s_mclk_multiple(uint32_t mult)
{
    return (mult == 256) ? I2S_MCLK_MULTIPLE_256 : I2S_MCLK_MULTIPLE_384;
}

/* -------------------------------------------------------------------------
 * WAV header
 *
 * Canonical 44-byte RIFF/WAVE header for 16-bit PCM. Sizes are patched in once
 * the capture length is known.
 * ------------------------------------------------------------------------- */
static void wav_header(uint8_t *hdr, uint32_t data_bytes, uint32_t sample_rate,
                       uint16_t channels, uint16_t bits)
{
    const uint32_t byte_rate   = sample_rate * channels * bits / 8;
    const uint16_t block_align = channels * bits / 8;

    memcpy(hdr, "RIFF", 4);
    const uint32_t riff_size = data_bytes + 36;
    memcpy(hdr + 4, &riff_size, 4);
    memcpy(hdr + 8, "WAVEfmt ", 8);

    const uint32_t fmt_size = 16;
    const uint16_t pcm_fmt  = 1;
    memcpy(hdr + 16, &fmt_size, 4);
    memcpy(hdr + 20, &pcm_fmt, 2);
    memcpy(hdr + 22, &channels, 2);
    memcpy(hdr + 24, &sample_rate, 4);
    memcpy(hdr + 28, &byte_rate, 4);
    memcpy(hdr + 32, &block_align, 2);
    memcpy(hdr + 34, &bits, 2);

    memcpy(hdr + 36, "data", 4);
    memcpy(hdr + 40, &data_bytes, 4);
}

/* -------------------------------------------------------------------------
 * Init
 * ------------------------------------------------------------------------- */
static esp_err_t i2c_init(void)
{
    /* One i2c_master bus for everything on GPIO 7/8: the ES8311 codec (vendored
     * driver, attaches via i2c_master_get_bus_handle) and the camera's SCCB
     * (esp_video receives this handle). The legacy driver cannot coexist with
     * i2c_master on the same port, which is why the codec driver was ported. */
    const i2c_master_bus_config_t cfg = {
        .i2c_port = I2C_PORT,
        .sda_io_num = BSP_I2C_SDA,
        .scl_io_num = BSP_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus = NULL;
    return i2c_new_master_bus(&cfg, &bus);
}

static esp_err_t i2s_init(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    /* The defaults give 6 x 240 frames = 90 ms of headroom, which is not enough
     * to ride out a scheduling hiccup. 8 x 512 frames buys ~256 ms for 16 KB of
     * DMA memory. Capture never touches the card (see audio_recorder_record),
     * so this is insurance rather than the main defence. */
    chan_cfg.dma_desc_num  = 8;
    chan_cfg.dma_frame_num = 512;

    /* Both directions: RX for the microphone, TX for the speaker. They share
     * one port, one clock and one codec, so only one may run at a time — the
     * state machine below enforces that. */
    ESP_RETURN_ON_ERROR(i2s_new_channel(&chan_cfg, &s_tx, &s_rx), TAG,
                        "i2s_new_channel failed");

    /* STEREO, not MONO. The ES8311 clocks out a full two-slot frame, and the
     * RX mono mode does not reliably drop the unused slot here — the capture
     * still contains both, so declaring the result mono yields a file with
     * twice the samples its header claims. That plays back at half speed and
     * an octave low: "slow and deep". Capturing the frame as it really is and
     * picking one channel in software is deterministic. */
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT,
                                                        I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = BSP_I2S_MCLK,
            .bclk = BSP_I2S_BCLK,
            .ws   = BSP_I2S_WS,
            .dout = BSP_I2S_DOUT,   /* to the codec DAC — speaker playback */
            .din  = BSP_I2S_DIN,    /* from the codec ADC — microphone */
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    /* The codec derives its internal clocks from MCLK, so the multiple here
     * must match what es8311_init() is told below. */
    std_cfg.clk_cfg.mclk_multiple = i2s_mclk_multiple(mclk_multiple_for(SAMPLE_RATE));

    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(s_rx, &std_cfg), TAG,
                        "i2s rx std init failed");
    ESP_RETURN_ON_ERROR(i2s_channel_init_std_mode(s_tx, &std_cfg), TAG,
                        "i2s tx std init failed");

    /* Only RX runs by default. TX is enabled around playback so the DAC is not
     * clocking silence into the speaker the rest of the time. Enable before
     * touching the codec: the ES8311 needs MCLK present while its clock
     * registers are configured. */
    return i2s_channel_enable(s_rx);
}

static esp_err_t codec_init(void)
{
    s_codec = es8311_create(I2C_PORT, BSP_ES8311_I2C_ADDR);
    ESP_RETURN_ON_FALSE(s_codec, ESP_FAIL, TAG,
                        "ES8311 not responding at 0x%02x", BSP_ES8311_I2C_ADDR);

    const uint32_t mclk_hz = SAMPLE_RATE * mclk_multiple_for(SAMPLE_RATE);
    const es8311_clock_config_t clk = {
        .mclk_inverted = false,
        .sclk_inverted = false,
        .mclk_from_mclk_pin = true,
        .mclk_frequency = mclk_hz,
        .sample_frequency = SAMPLE_RATE,
    };
    ESP_RETURN_ON_ERROR(es8311_init(s_codec, &clk, ES8311_RESOLUTION_16,
                                    ES8311_RESOLUTION_16),
                        TAG, "es8311_init failed");
    ESP_RETURN_ON_ERROR(es8311_sample_frequency_config(s_codec, mclk_hz, SAMPLE_RATE),
                        TAG, "sample frequency config failed");

    /* false = analog microphone. The onboard mic is analog into the codec ADC;
     * setting this true configures a PDM input and yields silence. */
    ESP_RETURN_ON_ERROR(es8311_microphone_config(s_codec, false), TAG,
                        "microphone config failed");
    ESP_RETURN_ON_ERROR(es8311_microphone_gain_set(s_codec, CONFIG_DEMO_AUDIO_MIC_GAIN),
                        TAG, "microphone gain failed");
    return ESP_OK;
}

esp_err_t audio_recorder_init(void)
{
    if (s_ready) {
        return ESP_OK;
    }
    if (mclk_multiple_for(SAMPLE_RATE) == 0) {
        ESP_LOGE(TAG, "CONFIG_DEMO_AUDIO_SAMPLE_RATE is %d Hz, which the ES8311 "
                      "has no clock setting for — use 8000, 16000, 22050, 24000, "
                      "32000, 44100 or 48000", SAMPLE_RATE);
        return ESP_ERR_INVALID_ARG;
    }

    /* Hold the speaker amplifier off so it cannot feed back into the mic. */
    const gpio_config_t pa = {
        .pin_bit_mask = 1ULL << BSP_AUDIO_PA_EN,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&pa);
    gpio_set_level(BSP_AUDIO_PA_EN, 0);

    esp_err_t ret = i2c_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C bus init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    if ((ret = i2s_init()) != ESP_OK) {
        ESP_LOGE(TAG, "I2S init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    if ((ret = codec_init()) != ESP_OK) {
        ESP_LOGE(TAG, "ES8311 init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    s_ready = true;
    ESP_LOGI(TAG, "Microphone ready — ES8311 at %d Hz, mic gain %d dB",
             SAMPLE_RATE, CONFIG_DEMO_AUDIO_MIC_GAIN * 6);
    return ESP_OK;
}

bool audio_recorder_ready(void)
{
    return s_ready;
}

const char *audio_recorder_dir(void)
{
    return REC_DIR;
}

/* -------------------------------------------------------------------------
 * Recording
 * ------------------------------------------------------------------------- */
static void next_filename(char *out, size_t out_len)
{
    /* Walk upwards until an unused name appears, so recordings accumulate
     * rather than silently overwriting each other. */
    for (int i = 1; i < 10000; i++) {
        snprintf(out, out_len, "%s%s/rec-%04d.wav", BSP_SD_MOUNT_POINT, REC_DIR, i);
        struct stat st;
        if (stat(out, &st) != 0) {
            return;
        }
    }
    snprintf(out, out_len, "%s%s/rec-overflow.wav", BSP_SD_MOUNT_POINT, REC_DIR);
}

static float to_dbfs(float linear)
{
    if (linear <= 0.0f) {
        return -INFINITY;
    }
    return 20.0f * log10f(linear / 32768.0f);
}


/* -------------------------------------------------------------------------
 * Capture pipeline: producer/consumer over a PSRAM ring buffer
 *
 * The capture task owns the I2S port and NEVER touches the card; a separate
 * writer task drains the ring to the file. The card's worst stalls (FAT
 * metadata, wear-levelling pauses) are absorbed by the ring instead of the
 * 256 ms DMA buffer, so recording length is bounded by card space, not RAM:
 * the earlier whole-capture-in-PSRAM design is what forced the auto-stop.
 *
 * Single producer, single consumer: head is written only by the capture task,
 * tail only by the writer, both monotonically increasing frame counts, so no
 * lock is needed.
 * ------------------------------------------------------------------------- */
/* 16 MB of the 32 MB PSRAM = 8.5 minutes of audio at 16 kHz. Sized so that a
 * typical recording NEVER touches the card while the microphone is live: SD
 * write current bursts couple into the mic's analog path (measured as ~1 s
 * spaced artifacts when the writer streamed 32 KB batches), so the card is
 * only written when the ring is half full — every ~4 minutes — or at stop. */
#define RING_FRAMES    (8 * 1024 * 1024)
#define SPILL_TRIGGER  (RING_FRAMES / 2)

/* WAV sizes are 32-bit; stop safely below that. ~30 h at 16 kHz mono. */
#define FILE_LIMIT_FRAMES  (UINT32_MAX / 2 - 4096)

typedef struct {
    int16_t  *ring;
    volatile uint32_t head;     /**< total frames produced (capture task) */
    volatile uint32_t tail;     /**< total frames consumed (writer task) */
    uint32_t  max_frames;       /**< optional cap; 0 = record until stopped */
    uint32_t  dropped;          /**< frames lost to a full ring (never expected) */
    int32_t   peak;
    double    sum_squares;
    char      path[160];
    FILE     *file;
    volatile bool stop_requested;
    volatile bool capture_done;
    volatile bool write_failed;
} capture_ctx_t;

static capture_ctx_t     s_cap;
static volatile audio_state_t s_state = AUDIO_STATE_IDLE;
static TaskHandle_t      s_task;
static SemaphoreHandle_t s_done;

/* Which kind of capture s_state refers to. Both write AUDIO_STATE_RECORDING,
 * but they own different buffers and different stop paths, so stopping one
 * must never reach into the other's context. */
typedef enum {
    CAP_NONE = 0,
    CAP_FILE,       /**< streaming to a WAV on the card */
    CAP_RAM,        /**< push-to-talk, straight to PSRAM */
} capture_mode_t;

static volatile capture_mode_t s_cap_mode = CAP_NONE;

/* Defined with the playback code below; the capture entry points need it to
 * refuse while the speaker owns the shared I2S port. */
static volatile bool s_playing;

static void capture_task(void *arg)
{
    int16_t *buf = heap_caps_malloc(CHUNK_BYTES, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    if (buf == NULL) {
        ESP_LOGE(TAG, "No DMA buffer for capture");
        s_cap.capture_done = true;
        vTaskDelete(NULL);
        return;
    }

    /*
     * Empty the DMA ring before starting. The I2S channel runs continuously
     * from boot, so its ring always holds the last fraction of a second of room
     * audio, captured before the button was pressed. Draining until it reports
     * empty keeps that out of the head of the recording.
     */
    for (int i = 0; i < 32; i++) {
        size_t drained = 0;
        if (i2s_channel_read(s_rx, buf, CHUNK_BYTES, &drained, 0) != ESP_OK ||
            drained == 0) {
            break;
        }
    }
    /* One more chunk for the ADC to settle, so the transient does not become
     * the reported peak. */
    size_t discard = 0;
    i2s_channel_read(s_rx, buf, CHUNK_BYTES, &discard, pdMS_TO_TICKS(500));

    while (!s_cap.stop_requested) {
        if (s_cap.max_frames && s_cap.head >= s_cap.max_frames) {
            ESP_LOGW(TAG, "Reached the configured %d s limit — stopping",
                     CONFIG_DEMO_AUDIO_MAX_SECONDS);
            break;
        }
        if (s_cap.head >= FILE_LIMIT_FRAMES) {
            ESP_LOGW(TAG, "WAV size limit reached — stopping");
            break;
        }

        size_t read_bytes = 0;
        const esp_err_t ret = i2s_channel_read(s_rx, buf, CHUNK_BYTES, &read_bytes,
                                               pdMS_TO_TICKS(1000));
        if (ret != ESP_OK || read_bytes == 0) {
            ESP_LOGE(TAG, "I2S read failed after %" PRIu32 " frames: %s",
                     s_cap.head, esp_err_to_name(ret));
            break;
        }

        /* Stereo off the wire, left slot kept — the ES8311 ADC's slot. */
        const size_t frames_in_chunk = read_bytes / 4;
        const uint32_t tail = __atomic_load_n(&s_cap.tail, __ATOMIC_ACQUIRE);
        const uint32_t free_frames = RING_FRAMES - (s_cap.head - tail);
        size_t keep = frames_in_chunk;
        if (keep > free_frames) {
            /* Writer has been starved for over a minute; keep the newest audio
             * contiguous instead of blocking the I2S read loop. */
            s_cap.dropped += keep - free_frames;
            keep = free_frames;
        }

        for (size_t i = 0; i < keep; i++) {
            const int16_t v = buf[i * 2];
            s_cap.ring[(s_cap.head + i) % RING_FRAMES] = v;

            const int32_t a = v < 0 ? -(int32_t)v : v;
            if (a > s_cap.peak) {
                s_cap.peak = a;
            }
            s_cap.sum_squares += (double)v * (double)v;
        }
        /* RELEASE: the samples written above must be visible to the writer
         * core BEFORE the new head is. Without this, RISC-V weak memory
         * ordering lets the writer read stale PSRAM at the head and put
         * corrupted samples in the file. */
        __atomic_store_n(&s_cap.head, s_cap.head + keep, __ATOMIC_RELEASE);
    }

    heap_caps_free(buf);
    s_cap.capture_done = true;   /* the writer finishes the file */
    vTaskDelete(NULL);
}

static void writer_task(void *arg)
{
    bool ok = true;

    /* Stay HOLDBACK frames behind the live head while capture runs (never
     * read a ring region the capture core may still be flushing), and do not
     * write AT ALL until the ring is half full — every SD write burst is
     * audible in the recording, so they are made as rare as possible. */
    const uint32_t HOLDBACK = 4096;               /* 256 ms at 16 kHz */
    bool draining = false;

    while (true) {
        const bool done = s_cap.capture_done;     /* read BEFORE head (see below) */
        const uint32_t head = __atomic_load_n(&s_cap.head, __ATOMIC_ACQUIRE);
        uint32_t avail = head - s_cap.tail;

        if (!done) {
            avail = (avail > HOLDBACK) ? avail - HOLDBACK : 0;
            if (!draining && avail < SPILL_TRIGGER) {
                vTaskDelay(pdMS_TO_TICKS(500));
                continue;
            }
            draining = (avail >= CHUNK_BYTES / 2);  /* keep going until nearly empty */
            if (!draining) {
                continue;
            }
        } else if (avail == 0) {
            break;                                /* drained after capture end */
        }

        /* Write the largest contiguous span, so wrap-around needs no copy. */
        const uint32_t idx = s_cap.tail % RING_FRAMES;
        uint32_t span = RING_FRAMES - idx;
        if (span > avail) {
            span = avail;
        }

        const size_t bytes = (size_t)span * sizeof(int16_t);
        if (ok && fwrite(&s_cap.ring[idx], 1, bytes, s_cap.file) != bytes) {
            ESP_LOGE(TAG, "Write failed at %" PRIu32 " frames — card full? "
                          "Stopping the recording.", s_cap.tail);
            ok = false;
            s_cap.write_failed = true;
            s_cap.stop_requested = true;          /* wind down the capture task */
        }
        __atomic_store_n(&s_cap.tail, s_cap.tail + span, __ATOMIC_RELEASE);
    }
    /* capture_done is read before head: if done was false we may sleep once
     * more than needed, which is harmless; the reverse order could exit with
     * frames still unread. */

    /* Patch the header with the real length, now that it is known. */
    const uint32_t data_bytes = s_cap.tail * sizeof(int16_t);
    uint8_t hdr[44];
    wav_header(hdr, data_bytes, SAMPLE_RATE, 1, 16);
    if (ok) {
        ok = (fseek(s_cap.file, 0, SEEK_SET) == 0) &&
             (fwrite(hdr, 1, sizeof(hdr), s_cap.file) == sizeof(hdr));
    }
    fflush(s_cap.file);
    fsync(fileno(s_cap.file));
    fclose(s_cap.file);
    s_cap.file = NULL;

    heap_caps_free(s_cap.ring);
    s_cap.ring = NULL;

    if (data_bytes == 0) {
        unlink(s_cap.path);                /* nothing captured — no empty file */
        s_cap.tail = 0;
    }
    /* A write failure keeps the partial file: the header above matches what
     * actually reached the card, so it still plays. */
    if (s_cap.dropped) {
        ESP_LOGW(TAG, "%" PRIu32 " frames were dropped — the card could not "
                      "keep up for over a minute", s_cap.dropped);
    }

    s_state = AUDIO_STATE_IDLE;
    s_cap_mode = CAP_NONE;
    s_task = NULL;
    xSemaphoreGive(s_done);
    vTaskDelete(NULL);
}

esp_err_t audio_recorder_start(void)
{
    if (!s_ready) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_state != AUDIO_STATE_IDLE) {
        return ESP_ERR_INVALID_STATE;   /* already recording */
    }

    if (s_done == NULL) {
        s_done = xSemaphoreCreateBinary();
        if (s_done == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }

    memset(&s_cap, 0, sizeof(s_cap));
    s_cap.max_frames = (uint32_t)SAMPLE_RATE * CONFIG_DEMO_AUDIO_MAX_SECONDS;

    char dir[64];
    snprintf(dir, sizeof(dir), "%s%s", BSP_SD_MOUNT_POINT, REC_DIR);
    mkdir(dir, 0777);   /* harmless if it already exists */
    next_filename(s_cap.path, sizeof(s_cap.path));

    s_cap.file = fopen(s_cap.path, "wb");
    if (s_cap.file == NULL) {
        ESP_LOGE(TAG, "Could not create %s", s_cap.path);
        return ESP_FAIL;
    }

    /* Placeholder header so the writer streams pure PCM behind it; the real
     * sizes are patched in when the recording ends. */
    uint8_t hdr[44];
    wav_header(hdr, 0, SAMPLE_RATE, 1, 16);
    if (fwrite(hdr, 1, sizeof(hdr), s_cap.file) != sizeof(hdr)) {
        fclose(s_cap.file);
        unlink(s_cap.path);
        s_cap.file = NULL;
        return ESP_FAIL;
    }

    s_cap.ring = heap_caps_malloc((size_t)RING_FRAMES * sizeof(int16_t),
                                  MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (s_cap.ring == NULL) {
        ESP_LOGE(TAG, "No PSRAM for the capture ring");
        fclose(s_cap.file);
        unlink(s_cap.path);
        s_cap.file = NULL;
        return ESP_ERR_NO_MEM;
    }

    s_state = AUDIO_STATE_RECORDING;
    s_cap_mode = CAP_FILE;

    /* Writer below capture priority, so the I2S loop always wins the CPU. */
    TaskHandle_t writer = NULL;
    if (xTaskCreate(writer_task, "audio_wr", 4096, NULL, 5, &writer) != pdPASS ||
        xTaskCreate(capture_task, "audio_cap", 4096, NULL, 6, &s_task) != pdPASS) {
        if (writer) {
            /* The writer exits on its own: capture never started, so marking
             * capture_done with an empty ring satisfies its exit condition. */
            s_cap.capture_done = true;
            xSemaphoreTake(s_done, pdMS_TO_TICKS(2000));
        } else {
            heap_caps_free(s_cap.ring);
            s_cap.ring = NULL;
            fclose(s_cap.file);
            s_cap.file = NULL;
        }
        unlink(s_cap.path);
        s_state = AUDIO_STATE_IDLE;
        s_cap_mode = CAP_NONE;
        return ESP_ERR_NO_MEM;
    }

    if (s_cap.max_frames) {
        ESP_LOGI(TAG, "Recording started (configured limit %d s)",
                 CONFIG_DEMO_AUDIO_MAX_SECONDS);
    } else {
        ESP_LOGI(TAG, "Recording started — runs until stopped or the card fills");
    }
    return ESP_OK;
}

esp_err_t audio_recorder_stop(audio_recording_t *out)
{
    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));

    /* A push-to-talk capture is held in memory and has no file to finish; its
     * own stop entry point owns it. */
    if (s_cap_mode == CAP_RAM) {
        return ESP_ERR_INVALID_STATE;
    }

    /* The capture may already have stopped itself (card full, size limit), so
     * this is not an error — there is still a finished recording to report. */
    if (s_state == AUDIO_STATE_RECORDING) {
        s_cap.stop_requested = true;
        /* Generous: the ring can hold up to 16 MB of not-yet-written audio,
         * which at ~2 MB/s SD write speed takes several seconds to flush. */
        if (xSemaphoreTake(s_done, pdMS_TO_TICKS(90000)) != pdTRUE) {
            ESP_LOGE(TAG, "Capture pipeline did not finish in time");
            return ESP_ERR_TIMEOUT;
        }
    } else if (s_done != NULL) {
        /* Drain a completion left by an automatic stop. */
        xSemaphoreTake(s_done, 0);
    }

    if (s_cap.tail == 0) {
        return ESP_FAIL;
    }

    const uint32_t frames = s_cap.tail;   /* what actually reached the card */
    const float rms = (float)sqrt(s_cap.sum_squares / frames);

    snprintf(out->path, sizeof(out->path), "%s/%s",
             REC_DIR, strrchr(s_cap.path, '/') + 1);
    out->bytes       = frames * sizeof(int16_t) + 44;
    out->samples     = frames;
    out->sample_rate = SAMPLE_RATE;
    out->seconds     = (float)frames / (float)SAMPLE_RATE;
    out->peak        = (int16_t)s_cap.peak;
    out->peak_dbfs   = to_dbfs((float)s_cap.peak);
    out->rms_dbfs    = to_dbfs(rms);

    /* A dead or misconfigured microphone yields a flat stream of near-zero
     * samples, which is otherwise indistinguishable from a good recording of a
     * quiet room. Flag it so the UI can say so instead of implying success. */
    out->silent = (s_cap.peak < CONFIG_DEMO_AUDIO_SILENCE_THRESHOLD);

    ESP_LOGI(TAG, "Recorded %.2f s to %s (peak %d, %.1f dBFS, rms %.1f dBFS)%s",
             out->seconds, out->path, out->peak, out->peak_dbfs, out->rms_dbfs,
             out->silent ? "  <-- SILENT, check the microphone" : "");
    return ESP_OK;
}

/* -------------------------------------------------------------------------
 * Push-to-talk capture — straight into PSRAM, no card, no writer task
 * ------------------------------------------------------------------------- */
#define PTT_MAX_FRAMES  ((uint32_t)SAMPLE_RATE * CONFIG_DEMO_PTT_MAX_SECONDS)

static int16_t          *s_ptt_buf;
static volatile uint32_t s_ptt_frames;
static volatile bool     s_ptt_stop;
static SemaphoreHandle_t s_ptt_done;

static void ptt_capture_task(void *arg)
{
    int16_t *buf = heap_caps_malloc(CHUNK_BYTES, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    if (buf == NULL) {
        ESP_LOGE(TAG, "No DMA buffer for push-to-talk capture");
        goto done;
    }

    /* The RX channel free-runs from boot, so its DMA ring holds the last
     * fraction of a second of room noise from BEFORE the button went down.
     * Speech recognition does better without it. */
    for (int i = 0; i < 32; i++) {
        size_t drained = 0;
        if (i2s_channel_read(s_rx, buf, CHUNK_BYTES, &drained, 0) != ESP_OK ||
            drained == 0) {
            break;
        }
    }

    while (!s_ptt_stop && s_ptt_frames < PTT_MAX_FRAMES) {
        size_t read_bytes = 0;
        const esp_err_t ret = i2s_channel_read(s_rx, buf, CHUNK_BYTES, &read_bytes,
                                               pdMS_TO_TICKS(1000));
        if (ret != ESP_OK || read_bytes == 0) {
            ESP_LOGE(TAG, "I2S read failed during push-to-talk: %s",
                     esp_err_to_name(ret));
            break;
        }

        /* Stereo on the wire, left slot is the ADC's — same as the card path. */
        size_t frames = read_bytes / 4;
        const uint32_t room = PTT_MAX_FRAMES - s_ptt_frames;
        if (frames > room) {
            frames = room;
        }
        for (size_t i = 0; i < frames; i++) {
            s_ptt_buf[s_ptt_frames + i] = buf[i * 2];
        }
        s_ptt_frames += frames;
    }

    if (s_ptt_frames >= PTT_MAX_FRAMES) {
        ESP_LOGW(TAG, "Push-to-talk hit the %d s ceiling — sending what was heard",
                 CONFIG_DEMO_PTT_MAX_SECONDS);
    }
    heap_caps_free(buf);

done:
    s_state = AUDIO_STATE_IDLE;
    s_cap_mode = CAP_NONE;
    xSemaphoreGive(s_ptt_done);
    vTaskDelete(NULL);
}

esp_err_t audio_recorder_capture_start(void)
{
    if (!s_ready) {
        return ESP_ERR_INVALID_STATE;
    }
    /* One codec, one I2S port — the card recorder and the player both lock it. */
    if (s_state != AUDIO_STATE_IDLE || s_playing) {
        return ESP_ERR_INVALID_STATE;
    }

    if (s_ptt_done == NULL) {
        s_ptt_done = xSemaphoreCreateBinary();
        if (s_ptt_done == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }
    if (s_ptt_buf == NULL) {
        /* Held for the life of the process: a voice query is small (about
         * 1 MB at the default ceiling) and reallocating it per press would
         * fragment PSRAM for no gain. */
        s_ptt_buf = heap_caps_malloc((size_t)PTT_MAX_FRAMES * sizeof(int16_t),
                                     MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (s_ptt_buf == NULL) {
            ESP_LOGE(TAG, "No PSRAM for the push-to-talk buffer");
            return ESP_ERR_NO_MEM;
        }
    }

    s_ptt_frames = 0;
    s_ptt_stop   = false;
    s_state      = AUDIO_STATE_RECORDING;
    s_cap_mode   = CAP_RAM;

    if (xTaskCreate(ptt_capture_task, "ptt_cap", 4096, NULL, 6, NULL) != pdPASS) {
        s_state    = AUDIO_STATE_IDLE;
        s_cap_mode = CAP_NONE;
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

void audio_recorder_capture_stop(const int16_t **out_pcm, size_t *out_bytes)
{
    if (s_cap_mode == CAP_RAM) {
        s_ptt_stop = true;
        /* The loop wakes at worst one I2S read later; a second is generous. */
        if (xSemaphoreTake(s_ptt_done, pdMS_TO_TICKS(3000)) != pdTRUE) {
            ESP_LOGE(TAG, "Push-to-talk capture did not stop in time");
        }
    }

    if (out_pcm) {
        *out_pcm = s_ptt_buf;
    }
    if (out_bytes) {
        *out_bytes = (size_t)s_ptt_frames * sizeof(int16_t);
    }
}

audio_state_t audio_recorder_state(void)
{
    return s_state;
}

float audio_recorder_elapsed(void)
{
    return (float)s_cap.head / (float)SAMPLE_RATE;
}

/* =========================================================================
 * Local playback — file on the card -> ES8311 DAC -> speaker header
 * ========================================================================= */

typedef struct {
    FILE     *file;
    uint32_t  data_bytes;      /**< PCM payload length */
    uint32_t  played_bytes;
    uint32_t  sample_rate;
    uint16_t  channels;
    char      path[160];
    volatile bool stop_requested;
} play_ctx_t;

static play_ctx_t s_play;
static TaskHandle_t  s_play_task;

/*
 * Walk the RIFF chunk list rather than assuming a 44-byte header.
 *
 * Our own recordings are canonical 44-byte WAVs, but anything dropped on the
 * card by a PC may carry LIST/fact/bext chunks before the data, and a fixed
 * offset would then play metadata as audio — loudly.
 */
static esp_err_t wav_parse(FILE *f, uint32_t *rate, uint16_t *channels,
                           uint16_t *bits, uint32_t *data_len)
{
    uint8_t riff[12];
    if (fread(riff, 1, 12, f) != 12 ||
        memcmp(riff, "RIFF", 4) != 0 || memcmp(riff + 8, "WAVE", 4) != 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    bool have_fmt = false;
    for (int guard = 0; guard < 64; guard++) {
        uint8_t hdr[8];
        if (fread(hdr, 1, 8, f) != 8) {
            break;
        }
        uint32_t size;
        memcpy(&size, hdr + 4, 4);

        if (memcmp(hdr, "fmt ", 4) == 0) {
            uint8_t fmt[16];
            if (size < 16 || fread(fmt, 1, 16, f) != 16) {
                return ESP_ERR_NOT_SUPPORTED;
            }
            uint16_t audio_fmt, ch, bps;
            uint32_t sr;
            memcpy(&audio_fmt, fmt + 0, 2);
            memcpy(&ch,        fmt + 2, 2);
            memcpy(&sr,        fmt + 4, 4);
            memcpy(&bps,       fmt + 14, 2);
            if (audio_fmt != 1 || bps != 16 || ch < 1 || ch > 2) {
                ESP_LOGE(TAG, "Unsupported WAV: format %u, %u-bit, %u ch "
                              "(need uncompressed 16-bit mono or stereo)",
                         audio_fmt, bps, ch);
                return ESP_ERR_NOT_SUPPORTED;
            }
            *rate = sr; *channels = ch; *bits = bps;
            have_fmt = true;
            if (size > 16) {
                fseek(f, size - 16, SEEK_CUR);
            }
        } else if (memcmp(hdr, "data", 4) == 0) {
            if (!have_fmt) {
                return ESP_ERR_NOT_SUPPORTED;
            }
            *data_len = size;
            return ESP_OK;               /* file is positioned at the samples */
        } else {
            fseek(f, size + (size & 1), SEEK_CUR);   /* chunks are word-aligned */
        }
    }
    return ESP_ERR_NOT_FOUND;
}

/* Retune the shared I2S clock and the codec. Both channels must be idle. */
static esp_err_t set_rate(uint32_t rate)
{
    const uint32_t mult = mclk_multiple_for(rate);
    if (mult == 0) {
        ESP_LOGE(TAG, "The codec has no clock setting for %" PRIu32 " Hz", rate);
        return ESP_ERR_NOT_SUPPORTED;
    }

    i2s_std_clk_config_t clk = I2S_STD_CLK_DEFAULT_CONFIG(rate);
    clk.mclk_multiple = i2s_mclk_multiple(mult);

    ESP_RETURN_ON_ERROR(i2s_channel_reconfig_std_clock(s_tx, &clk), TAG, "tx clk");
    ESP_RETURN_ON_ERROR(i2s_channel_reconfig_std_clock(s_rx, &clk), TAG, "rx clk");
    return es8311_sample_frequency_config(s_codec, rate * mult, rate);
}

static void play_task(void *arg)
{
    const size_t CHUNK = 4096;                       /* stereo bytes per write */
    int16_t *file_buf = heap_caps_malloc(CHUNK, MALLOC_CAP_DEFAULT | MALLOC_CAP_8BIT);
    int16_t *out_buf  = heap_caps_malloc(CHUNK * 2, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);

    if (file_buf == NULL || out_buf == NULL) {
        ESP_LOGE(TAG, "No buffers for playback");
        goto done;
    }

    while (!s_play.stop_requested && s_play.played_bytes < s_play.data_bytes) {
        const uint32_t left = s_play.data_bytes - s_play.played_bytes;
        size_t want = left < CHUNK ? left : CHUNK;
        want &= ~(size_t)1;                          /* whole samples */
        if (want == 0) {
            break;
        }

        const size_t got = fread(file_buf, 1, want, s_play.file);
        if (got == 0) {
            break;
        }
        s_play.played_bytes += got;

        const size_t samples = got / sizeof(int16_t);
        size_t out_samples;
        if (s_play.channels == 1) {
            /* The I2S link always carries two slots, so a mono file has to be
             * duplicated into both — otherwise it plays at double speed with
             * every other slot silent. */
            for (size_t i = 0; i < samples; i++) {
                out_buf[i * 2]     = file_buf[i];
                out_buf[i * 2 + 1] = file_buf[i];
            }
            out_samples = samples * 2;
        } else {
            memcpy(out_buf, file_buf, got);
            out_samples = samples;
        }

        size_t written = 0;
        if (i2s_channel_write(s_tx, out_buf, out_samples * sizeof(int16_t),
                              &written, pdMS_TO_TICKS(2000)) != ESP_OK) {
            ESP_LOGE(TAG, "I2S write failed during playback");
            break;
        }
    }

done:
    free(file_buf);
    heap_caps_free(out_buf);

    /* Mute the amplifier before the clocks stop, so the speaker does not get a
     * click as the DAC output goes away. */
    gpio_set_level(BSP_AUDIO_PA_EN, 0);
    vTaskDelay(pdMS_TO_TICKS(20));

    i2s_channel_disable(s_tx);
    if (s_play.file) {
        fclose(s_play.file);
        s_play.file = NULL;
    }

    /* Put the shared clock back where recording expects it. */
    if (s_play.sample_rate != SAMPLE_RATE) {
        i2s_channel_disable(s_rx);
        set_rate(SAMPLE_RATE);
        i2s_channel_enable(s_rx);
    }

    ESP_LOGI(TAG, "Playback finished (%s)",
             s_play.stop_requested ? "stopped" : "end of file");

    s_playing = false;
    s_play_task = NULL;
    vTaskDelete(NULL);
}

esp_err_t audio_player_start(const char *rel_path)
{
    if (!s_ready) {
        return ESP_ERR_INVALID_STATE;
    }
    /* One codec, one I2S port: recording and playback cannot overlap. */
    if (s_state != AUDIO_STATE_IDLE || s_playing) {
        return ESP_ERR_INVALID_STATE;
    }
    if (rel_path == NULL || rel_path[0] != '/' || strstr(rel_path, "..") != NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(&s_play, 0, sizeof(s_play));
    snprintf(s_play.path, sizeof(s_play.path), "%s%s", BSP_SD_MOUNT_POINT, rel_path);

    s_play.file = fopen(s_play.path, "rb");
    if (s_play.file == NULL) {
        ESP_LOGE(TAG, "Could not open %s", s_play.path);
        return ESP_ERR_NOT_FOUND;
    }

    uint16_t bits = 0;
    esp_err_t ret = wav_parse(s_play.file, &s_play.sample_rate, &s_play.channels,
                              &bits, &s_play.data_bytes);
    if (ret != ESP_OK) {
        fclose(s_play.file);
        s_play.file = NULL;
        return ret;
    }

    /* Retune only when the file disagrees with the recording rate — the common
     * case is our own 16 kHz captures, which need no change. */
    if (s_play.sample_rate != SAMPLE_RATE) {
        i2s_channel_disable(s_rx);
        ret = set_rate(s_play.sample_rate);
        i2s_channel_enable(s_rx);
        if (ret != ESP_OK) {
            fclose(s_play.file);
            s_play.file = NULL;
            return ret;
        }
    }

    es8311_voice_volume_set(s_codec, CONFIG_DEMO_AUDIO_SPEAKER_VOLUME, NULL);
    es8311_voice_mute(s_codec, false);

    if (i2s_channel_enable(s_tx) != ESP_OK) {
        fclose(s_play.file);
        s_play.file = NULL;
        return ESP_FAIL;
    }
    gpio_set_level(BSP_AUDIO_PA_EN, 1);   /* speaker amplifier on */

    s_playing = true;
    if (xTaskCreate(play_task, "audio_play", 4096, NULL, 6, &s_play_task) != pdPASS) {
        gpio_set_level(BSP_AUDIO_PA_EN, 0);
        i2s_channel_disable(s_tx);
        fclose(s_play.file);
        s_play.file = NULL;
        s_playing = false;
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Playing %s (%" PRIu32 " Hz, %u ch, %.1f s)",
             rel_path, s_play.sample_rate, s_play.channels,
             (float)s_play.data_bytes / (float)(s_play.sample_rate * 2 * s_play.channels));
    return ESP_OK;
}

void audio_player_stop(void)
{
    if (s_playing) {
        s_play.stop_requested = true;
    }
}

bool audio_player_is_playing(void)
{
    return s_playing;
}

const char *audio_player_path(void)
{
    return s_playing ? s_play.path + strlen(BSP_SD_MOUNT_POINT) : "";
}

/* -------------------------------------------------------------------------
 * Streaming playback
 *
 * No task and no file: the caller pushes chunks as they arrive and blocks in
 * i2s_channel_write, which is what keeps a fast download from outrunning the
 * speaker. s_playing is still set, so the recorder and the file player refuse
 * to start underneath a stream in progress.
 * ------------------------------------------------------------------------- */
#define STREAM_FRAMES  1024                       /* mono frames per I2S write */

static int16_t *s_stream_buf;                     /* stereo expansion scratch */
static uint32_t s_stream_rate;
static uint8_t  s_stream_odd;                     /* half of a split sample */
static bool     s_stream_have_odd;

esp_err_t audio_player_stream_start(uint32_t sample_rate)
{
    if (!s_ready) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_state != AUDIO_STATE_IDLE || s_playing) {
        return ESP_ERR_INVALID_STATE;
    }
    if (mclk_multiple_for(sample_rate) == 0) {
        ESP_LOGE(TAG, "The codec cannot clock %" PRIu32 " Hz — supported rates "
                      "are 8000, 16000, 22050, 24000, 32000, 44100 and 48000",
                 sample_rate);
        return ESP_ERR_NOT_SUPPORTED;
    }

    s_stream_buf = heap_caps_malloc(STREAM_FRAMES * 2 * sizeof(int16_t),
                                    MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    if (s_stream_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    s_stream_have_odd = false;
    s_stream_rate = sample_rate;

    if (sample_rate != SAMPLE_RATE) {
        i2s_channel_disable(s_rx);
        const esp_err_t ret = set_rate(sample_rate);
        i2s_channel_enable(s_rx);
        if (ret != ESP_OK) {
            heap_caps_free(s_stream_buf);
            s_stream_buf = NULL;
            return ret;
        }
    }

    /* Quieter than file playback on purpose: synthesised speech arrives
     * normalised to full scale, and the codec's volume control applies
     * positive gain above 75, which clips it. */
    es8311_voice_volume_set(s_codec, CONFIG_DEMO_AUDIO_ASSISTANT_VOLUME, NULL);
    es8311_voice_mute(s_codec, false);

    if (i2s_channel_enable(s_tx) != ESP_OK) {
        heap_caps_free(s_stream_buf);
        s_stream_buf = NULL;
        return ESP_FAIL;
    }
    gpio_set_level(BSP_AUDIO_PA_EN, 1);

    s_playing = true;
    ESP_LOGI(TAG, "Streaming reply at %" PRIu32 " Hz", sample_rate);
    return ESP_OK;
}

esp_err_t audio_player_stream_write(const void *pcm, size_t bytes)
{
    if (!s_playing || s_stream_buf == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    const uint8_t *in = pcm;
    while (bytes > 0) {
        size_t frames = 0;

        /* A chunk boundary can split a 16-bit sample. Rejoin it here rather
         * than dropping the byte, which would swap the endianness of every
         * sample that follows and turn the reply into noise. */
        if (s_stream_have_odd) {
            const int16_t v = (int16_t)((uint16_t)s_stream_odd |
                                        ((uint16_t)in[0] << 8));
            s_stream_buf[0] = v;
            s_stream_buf[1] = v;
            s_stream_have_odd = false;
            in += 1;
            bytes -= 1;
            frames = 1;
        }

        /* The I2S link always carries two slots, so mono has to go into both
         * or it plays at double speed with every other slot silent. */
        while (frames < STREAM_FRAMES && bytes >= 2) {
            const int16_t v = (int16_t)((uint16_t)in[0] | ((uint16_t)in[1] << 8));
            s_stream_buf[frames * 2]     = v;
            s_stream_buf[frames * 2 + 1] = v;
            in += 2;
            bytes -= 2;
            frames++;
        }
        if (frames < STREAM_FRAMES && bytes == 1) {
            s_stream_odd = in[0];
            s_stream_have_odd = true;
            in += 1;
            bytes -= 1;
        }
        if (frames == 0) {
            break;
        }

        size_t written = 0;
        const esp_err_t ret = i2s_channel_write(s_tx, s_stream_buf,
                                                frames * 2 * sizeof(int16_t),
                                                &written, pdMS_TO_TICKS(5000));
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "I2S write failed while streaming: %s", esp_err_to_name(ret));
            return ret;
        }
    }
    return ESP_OK;
}

void audio_player_stream_end(void)
{
    if (!s_playing) {
        return;
    }

    /* Let the DMA queue empty before the amplifier goes quiet, otherwise the
     * tail of the reply is cut off. One buffer of headroom is 256 ms. */
    vTaskDelay(pdMS_TO_TICKS(300));

    gpio_set_level(BSP_AUDIO_PA_EN, 0);
    vTaskDelay(pdMS_TO_TICKS(20));      /* mute before the clock stops: no click */
    i2s_channel_disable(s_tx);

    if (s_stream_rate != SAMPLE_RATE) {
        i2s_channel_disable(s_rx);
        set_rate(SAMPLE_RATE);
        i2s_channel_enable(s_rx);
    }

    heap_caps_free(s_stream_buf);
    s_stream_buf = NULL;
    s_playing = false;
}

void audio_player_progress(float *out_position, float *out_duration)
{
    const uint32_t bytes_per_second =
        s_play.sample_rate ? s_play.sample_rate * 2 * s_play.channels : 1;
    if (out_position) {
        *out_position = (float)s_play.played_bytes / (float)bytes_per_second;
    }
    if (out_duration) {
        *out_duration = (float)s_play.data_bytes / (float)bytes_per_second;
    }
}
