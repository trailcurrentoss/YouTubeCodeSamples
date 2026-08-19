/*
 * peregrine_voice.c — see peregrine_voice.h for the flow.
 *
 * Threading
 * ---------
 * press() and release() are called from the button task. press() starts the
 * capture, which is cheap. release() stops it and drops a job on a length-1
 * queue for the worker task, so the button task is never held up by HTTP.
 *
 * The worker owns the request end to end: it uploads the WAV, then reads the
 * reply body chunk by chunk and pushes each chunk to the speaker. Playback
 * paces the read, so a long reply never has to be buffered.
 *
 * The reply's sample rate is taken from its own WAV header rather than
 * assumed. Peregrine resamples to whatever ITS target rate is, which need not
 * match the capture rate — playing a 22.05 kHz reply at 16 kHz pitches the
 * voice down about four semitones.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "peregrine_voice.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"

#include "audio_recorder.h"

static const char *TAG = "peregrine";

#define PATH_VOICE        "/api/voice"
#define URL_MAX           128
#define TOKEN_MAX         128
#define WAV_HDR_LEN       44
#define RESP_CHUNK_BYTES  4096
#define UPLOAD_SLICE      8192   /* keeps one write off the Wi-Fi stack's back */

static char s_url[URL_MAX]     = CONFIG_DEMO_PEREGRINE_URL;
static char s_token[TOKEN_MAX] = CONFIG_DEMO_PEREGRINE_TOKEN;

static volatile peregrine_voice_state_t s_state = PRG_VOICE_IDLE;

static TaskHandle_t  s_worker;
static QueueHandle_t s_jobs;

/* The samples stay owned by audio_recorder; the length is snapshotted at
 * release so a later press cannot change it under an upload in flight. */
typedef struct {
    const int16_t *pcm;
    size_t         bytes;
} voice_job_t;

/* Canonical 44-byte RIFF/WAVE header for 16-bit PCM. */
static void wav_header(uint8_t *hdr, uint32_t data_bytes, uint32_t sample_rate,
                       uint16_t channels, uint16_t bits)
{
    const uint32_t byte_rate   = sample_rate * channels * bits / 8;
    const uint16_t block_align = channels * bits / 8;
    const uint32_t riff_size   = data_bytes + 36;
    const uint32_t fmt_size    = 16;
    const uint16_t pcm_fmt     = 1;

    memcpy(hdr, "RIFF", 4);
    memcpy(hdr + 4, &riff_size, 4);
    memcpy(hdr + 8, "WAVEfmt ", 8);
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

/* Play the reply body as it arrives. The first 44 bytes are its WAV header,
 * which may straddle chunks, so it is accumulated before playback opens. */
static void stream_reply(esp_http_client_handle_t client)
{
    static uint8_t chunk[RESP_CHUNK_BYTES];
    uint8_t  hdr[WAV_HDR_LEN];
    size_t   hdr_got = 0;
    size_t   body = 0;
    bool     playing = false;

    while (true) {
        const int n = esp_http_client_read(client, (char *)chunk, sizeof(chunk));
        if (n < 0) {
            /* Peregrine sends no Content-Length and simply closes when the
             * reply is complete, so a read error at the end is expected. */
            ESP_LOGD(TAG, "read returned %d — treating as end of reply", n);
            break;
        }
        if (n == 0) {
            break;
        }
        body += (size_t)n;

        size_t offset = 0;
        if (hdr_got < WAV_HDR_LEN) {
            const size_t need = WAV_HDR_LEN - hdr_got;
            const size_t take = ((size_t)n < need) ? (size_t)n : need;
            memcpy(hdr + hdr_got, chunk, take);
            hdr_got += take;
            offset = take;

            if (hdr_got == WAV_HDR_LEN) {
                uint32_t rate;
                memcpy(&rate, hdr + 24, 4);     /* fmt chunk, sample rate */
                if (audio_player_stream_start(rate) != ESP_OK) {
                    ESP_LOGE(TAG, "Could not play the reply at %" PRIu32 " Hz — "
                                  "set PEREGRINE_VOICE_TARGET_SR on the server to "
                                  "a rate the codec supports (16000 is a good "
                                  "match for the microphone)", rate);
                    return;
                }
                playing = true;
                s_state = PRG_VOICE_PLAYING;
            }
            if ((size_t)n <= take) {
                continue;
            }
        }
        if (playing) {
            audio_player_stream_write(chunk + offset, (size_t)n - offset);
        }
    }

    if (playing) {
        audio_player_stream_end();
    }
    ESP_LOGI(TAG, "Reply finished (%u bytes)", (unsigned)body);
}

/* URL-encoded headers Peregrine adds so the client can log what was heard. */
static void log_header(esp_http_client_handle_t client, const char *name,
                       const char *label)
{
    char *value = NULL;
    if (esp_http_client_get_header(client, name, &value) == ESP_OK && value) {
        ESP_LOGI(TAG, "%s: %s", label, value);
    }
}

static void run_job(const voice_job_t *job)
{
    const int64_t t_start = esp_timer_get_time();

    if (job->pcm == NULL || job->bytes == 0) {
        ESP_LOGW(TAG, "Nothing was captured — not sending");
        s_state = PRG_VOICE_IDLE;
        return;
    }

    char url[URL_MAX + sizeof(PATH_VOICE)];
    snprintf(url, sizeof(url), "%s%s", s_url, PATH_VOICE);

    const esp_http_client_config_t cfg = {
        .url            = url,
        .method         = HTTP_METHOD_POST,
        .timeout_ms     = CONFIG_DEMO_PEREGRINE_TIMEOUT_MS,
        .buffer_size    = RESP_CHUNK_BYTES,
        .buffer_size_tx = 1024,
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (client == NULL) {
        ESP_LOGE(TAG, "HTTP client init failed");
        s_state = PRG_VOICE_ERROR;
        return;
    }

    char auth[TOKEN_MAX + 8];
    snprintf(auth, sizeof(auth), "Bearer %s", s_token);
    esp_http_client_set_header(client, "Content-Type", "audio/wav");
    esp_http_client_set_header(client, "Authorization", auth);

    s_state = PRG_VOICE_UPLOADING;
    ESP_LOGI(TAG, "Sending %.2f s to %s",
             (float)job->bytes / (float)(CONFIG_DEMO_AUDIO_SAMPLE_RATE * 2), url);

    uint8_t hdr[WAV_HDR_LEN];
    wav_header(hdr, (uint32_t)job->bytes, CONFIG_DEMO_AUDIO_SAMPLE_RATE, 1, 16);

    esp_err_t err = esp_http_client_open(client, (int)(WAV_HDR_LEN + job->bytes));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not reach %s: %s", url, esp_err_to_name(err));
        esp_http_client_cleanup(client);
        s_state = PRG_VOICE_ERROR;
        return;
    }

    if (esp_http_client_write(client, (const char *)hdr, WAV_HDR_LEN) != WAV_HDR_LEN) {
        ESP_LOGE(TAG, "Short write sending the WAV header");
        goto out;
    }
    for (size_t sent = 0; sent < job->bytes; ) {
        size_t take = job->bytes - sent;
        if (take > UPLOAD_SLICE) {
            take = UPLOAD_SLICE;
        }
        const int w = esp_http_client_write(client,
                                            (const char *)job->pcm + sent, take);
        if (w <= 0) {
            ESP_LOGE(TAG, "Upload failed at %u of %u bytes",
                     (unsigned)sent, (unsigned)job->bytes);
            goto out;
        }
        sent += (size_t)w;
    }
    const int64_t t_uploaded = esp_timer_get_time();

    esp_http_client_fetch_headers(client);
    const int status = esp_http_client_get_status_code(client);
    if (status != 200) {
        /* 401 means the token on the card and the token on the server differ —
         * by far the most common setup mistake, so name it. */
        ESP_LOGE(TAG, "Peregrine answered HTTP %d%s", status,
                 status == 401 ? " — the bearer token was rejected" : "");
        goto out;
    }

    log_header(client, "X-Peregrine-Transcript", "Heard");
    log_header(client, "X-Peregrine-Response", "Replied");

    stream_reply(client);

    ESP_LOGI(TAG, "Exchange took %lld ms (upload %lld ms)",
             (esp_timer_get_time() - t_start) / 1000,
             (t_uploaded - t_start) / 1000);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    s_state = PRG_VOICE_IDLE;
    return;

out:
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    s_state = PRG_VOICE_ERROR;
}

static void worker_task(void *arg)
{
    voice_job_t job;
    while (true) {
        if (xQueueReceive(s_jobs, &job, portMAX_DELAY) == pdTRUE) {
            run_job(&job);
        }
    }
}

void peregrine_voice_init(void)
{
    if (s_worker) {
        return;
    }
    s_jobs = xQueueCreate(1, sizeof(voice_job_t));
    if (s_jobs == NULL) {
        ESP_LOGE(TAG, "Job queue create failed");
        return;
    }
    /* 8 KB: the HTTP client plus the streaming path sits around 3-4 KB. */
    if (xTaskCreate(worker_task, "prg_voice", 8192, NULL, 5, &s_worker) != pdPASS) {
        ESP_LOGE(TAG, "Worker task create failed");
        s_worker = NULL;
    }
}

bool peregrine_voice_configured(void)
{
    return s_url[0] != '\0' && s_token[0] != '\0';
}

void peregrine_voice_set_url(const char *url)
{
    if (url == NULL || *url == '\0') {
        url = CONFIG_DEMO_PEREGRINE_URL;
    }
    strlcpy(s_url, url, sizeof(s_url));
    ESP_LOGI(TAG, "URL set to %s", s_url);
}

void peregrine_voice_set_token(const char *token)
{
    strlcpy(s_token, token ? token : "", sizeof(s_token));
    /* Never log the token itself — the console goes into bug reports. */
    ESP_LOGI(TAG, "Token %s", s_token[0] ? "set" : "cleared");
}

void peregrine_voice_press(void)
{
    if (s_state != PRG_VOICE_IDLE && s_state != PRG_VOICE_ERROR) {
        ESP_LOGW(TAG, "Still busy with the last question — press ignored");
        return;
    }
    if (audio_recorder_capture_start() != ESP_OK) {
        ESP_LOGE(TAG, "Could not start capturing");
        s_state = PRG_VOICE_ERROR;
        return;
    }
    s_state = PRG_VOICE_LISTENING;
    ESP_LOGI(TAG, "Listening…");
}

void peregrine_voice_release(void)
{
    if (s_state != PRG_VOICE_LISTENING) {
        return;         /* a release with no matching press — nothing to send */
    }

    const int16_t *pcm = NULL;
    size_t bytes = 0;
    audio_recorder_capture_stop(&pcm, &bytes);

    const voice_job_t job = { .pcm = pcm, .bytes = bytes };
    if (xQueueSend(s_jobs, &job, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Worker still busy — dropping this question");
        s_state = PRG_VOICE_IDLE;
    }
}

peregrine_voice_state_t peregrine_voice_get_state(void)
{
    return s_state;
}
