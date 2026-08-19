/*
 * Plain webcam streaming — camera to hardware H.264, no inference.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"

#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_video_device.h"
#include "linux/videodev2.h"

#include "esp_h264_enc_single_hw.h"
#include "esp_h264_alloc.h"

#include "ai_camera.h"
#include "webcam.h"

static const char *TAG = "webcam";

#define VIDEO_DEV        ESP_VIDEO_MIPI_CSI_DEVICE_NAME   /* "/dev/video0" */
#define FRAME_BUFFERS    3

/* The sensor runs at 50 fps but every second frame is skipped: 25 fps is
 * visually smooth, and at a fixed bitrate halving the frame rate doubles the
 * bits available per frame. 800x640 at 4 Mbit/s / 25 fps ≈ 20 KB per frame,
 * which keeps the quantizer in its clean range — at 50 fps / 2.5 Mbit/s the
 * encoder was starved down to visible blockiness. */
#define H264_FPS         25
#define H264_BITRATE     4000000
#define H264_GOP         25            /* one IDR per second — bounds recovery
                                        * time after a dropped frame */
#define PKT_HDR_BYTES    8             /* uint32 LE length + uint32 LE pts_ms */

/* Encoded access units queue here between the encoder task and the HTTP
 * streamer. Sized for several IDR frames so a brief Wi-Fi stall drops frames
 * (recoverable at the next IDR) instead of blocking the pipeline. */
#define STREAM_RING_BYTES (768 * 1024)

static volatile bool         s_running;
static volatile bool         s_stop_req;
static TaskHandle_t          s_task;
static SemaphoreHandle_t     s_done;

static int                   s_fd = -1;
static void                 *s_fbuf[FRAME_BUFFERS];
static uint16_t              s_width, s_height;
static char                  s_sensor[24];

static esp_h264_enc_handle_t s_enc;
static uint8_t              *s_enc_out;
static uint32_t              s_enc_out_len;

static RingbufHandle_t       s_ring;
static volatile bool         s_consumer;    /* one streaming client at a time */

static float                 s_fps;
static uint32_t              s_frames;

/* -------------------------------------------------------------------------
 * Capture
 * ------------------------------------------------------------------------- */
static void capture_close(void)
{
    if (s_fd >= 0) {
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(s_fd, VIDIOC_STREAMOFF, &type);
        close(s_fd);
        s_fd = -1;
    }
    memset(s_fbuf, 0, sizeof(s_fbuf));
}

static esp_err_t capture_open(void)
{
    s_fd = open(VIDEO_DEV, O_RDWR);
    if (s_fd < 0) {
        ESP_LOGE(TAG, "No camera: open(%s) failed — check the CSI cable", VIDEO_DEV);
        return ESP_ERR_NOT_FOUND;
    }

    struct v4l2_capability cap = {};
    struct v4l2_format fmt = {};
    struct v4l2_requestbuffers req = {};
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(s_fd, VIDIOC_QUERYCAP, &cap) == 0) {
        strlcpy(s_sensor, (const char *)cap.card, sizeof(s_sensor));
    }

    /* YUV420 from the ISP is the O_UYY_E_VYY layout the H.264 hardware wants,
     * so the encoder reads the capture buffer directly. */
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(s_fd, VIDIOC_G_FMT, &fmt) != 0) {
        goto fail;
    }
    /* esp_video's G_FMT copies back the whole stored v4l2_format, whose .type
     * is still 0 until the first successful S_FMT — restore it or S_FMT is
     * rejected before format negotiation even happens. */
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
    if (ioctl(s_fd, VIDIOC_S_FMT, &fmt) != 0) {
        ESP_LOGE(TAG, "Camera does not offer YUV420 output");
        goto fail;
    }
    s_width  = fmt.fmt.pix.width;
    s_height = fmt.fmt.pix.height;

    req.count  = FRAME_BUFFERS;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(s_fd, VIDIOC_REQBUFS, &req) != 0) {
        goto fail;
    }

    for (int i = 0; i < FRAME_BUFFERS; i++) {
        struct v4l2_buffer buf = {};
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;
        if (ioctl(s_fd, VIDIOC_QUERYBUF, &buf) != 0) {
            goto fail;
        }
        s_fbuf[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                         s_fd, buf.m.offset);
        if (s_fbuf[i] == MAP_FAILED) {
            goto fail;
        }
        if (ioctl(s_fd, VIDIOC_QBUF, &buf) != 0) {
            goto fail;
        }
    }

    if (ioctl(s_fd, VIDIOC_STREAMON, &type) != 0) {
        goto fail;
    }

    ESP_LOGI(TAG, "Camera \"%s\" streaming %ux%u YUV420", s_sensor, s_width, s_height);
    return ESP_OK;

fail:
    ESP_LOGE(TAG, "V4L2 setup failed");
    close(s_fd);
    s_fd = -1;
    return ESP_FAIL;
}

/* -------------------------------------------------------------------------
 * Encoder
 * ------------------------------------------------------------------------- */
/* Heap-corruption tripwire. A TLSF assert during webcam_stop showed PSRAM
 * heap metadata being scribbled while the H.264 pipeline runs; these probes
 * bracket the pipeline stages so the console shows WHEN the damage first
 * appears (first frame? periodic? teardown?) instead of only detecting it
 * when a later free() trips over it. */
static void heap_probe(const char *where)
{
    if (!heap_caps_check_integrity_all(true)) {
        ESP_LOGE(TAG, "HEAP CORRUPT at %s (frame %u)", where, (unsigned)s_frames);
    }
}

/* The encoder instance is created ONCE and kept resident, like the AI tab's
 * detector and JPEG engine. esp_h264_enc_hw_new() allocates ~58 KB of
 * INTERNAL SRAM for the reference frame; new/del per session eventually
 * fails once the internal heap tightens or fragments ("No memory for
 * reference frame"). Per session only open/close run — they just claim the
 * interrupt and reset the frame counter, so every session starts on an IDR. */
static esp_err_t encoder_open(void)
{
    if (s_enc == NULL) {
        esp_h264_enc_cfg_hw_t cfg = {};
        cfg.pic_type   = ESP_H264_RAW_FMT_O_UYY_E_VYY;
        cfg.gop        = H264_GOP;
        cfg.fps        = H264_FPS;
        cfg.res.width  = s_width;
        cfg.res.height = s_height;
        cfg.rc.bitrate = H264_BITRATE;
        cfg.rc.qp_min  = 22;
        cfg.rc.qp_max  = 40;

        /* Worst case an access unit cannot exceed the raw frame; PSRAM is
         * fine — esp_video feeds this same encoder from PSRAM buffers. The
         * extra 64 KB is deliberate slack between this hardware-DMA-written
         * buffer and its heap neighbours while a corruption source in this
         * pipeline is being pinned down. */
        s_enc_out = esp_h264_aligned_calloc(16, 1,
                                            (uint32_t)s_width * s_height * 3 / 2 + 65536,
                                            &s_enc_out_len, ESP_H264_MEM_SPIRAM);
        if (s_enc_out == NULL) {
            return ESP_ERR_NO_MEM;
        }
        s_enc_out_len -= 65536;   /* keep the slack out of the DMA's budget */
        if (esp_h264_enc_hw_new(&cfg, &s_enc) != ESP_H264_ERR_OK) {
            return ESP_FAIL;
        }
    }

    if (esp_h264_enc_open(s_enc) != ESP_H264_ERR_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void encoder_close(void)
{
    if (s_enc) {
        esp_h264_enc_close(s_enc);     /* handle stays for the next session */
    }
}

/* -------------------------------------------------------------------------
 * Pipeline task
 * ------------------------------------------------------------------------- */
static void webcam_task(void *arg)
{
    const int64_t t_start = esp_timer_get_time();
    int64_t window_start = t_start;
    uint32_t window_frames = 0;
    uint32_t seq = 0;

    while (!s_stop_req) {
        struct v4l2_buffer buf = {};
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(s_fd, VIDIOC_DQBUF, &buf) != 0) {
            ESP_LOGE(TAG, "Frame dequeue failed, stopping");
            break;
        }

        /* Decimate 50 fps sensor output down to the encoder's frame rate. */
        if ((seq++ & 1) != 0) {
            ioctl(s_fd, VIDIOC_QBUF, &buf);
            continue;
        }

        esp_h264_enc_in_frame_t in = {};
        in.raw_data.buffer = s_fbuf[buf.index];
        in.raw_data.len    = (uint32_t)s_width * s_height * 3 / 2;
        in.pts             = (uint32_t)((esp_timer_get_time() - t_start) / 1000);

        esp_h264_enc_out_frame_t out = {};
        out.raw_data.buffer = s_enc_out;
        out.raw_data.len    = s_enc_out_len;

        const esp_h264_err_t eret = esp_h264_enc_process(s_enc, &in, &out);

        ioctl(s_fd, VIDIOC_QBUF, &buf);   /* encoder is done with the frame */

        if (eret == ESP_H264_ERR_OK && out.length > 0) {
            if (s_frames == 0) {
                heap_probe("first encoded frame");
            } else if ((s_frames & 255) == 0) {
                heap_probe("periodic");
            }
            /* Only queue while someone is watching. A full ring (client
             * stalled) drops the frame; the stream heals at the next IDR. */
            if (s_consumer) {
                uint8_t *slot = NULL;
                if (xRingbufferSendAcquire(s_ring, (void **)&slot,
                                           PKT_HDR_BYTES + out.length, 0) == pdTRUE) {
                    const uint32_t len = out.length;
                    const uint32_t pts = in.pts;
                    memcpy(slot, &len, 4);
                    memcpy(slot + 4, &pts, 4);
                    memcpy(slot + PKT_HDR_BYTES, s_enc_out, out.length);
                    xRingbufferSendComplete(s_ring, slot);
                }
            }

            s_frames++;
            window_frames++;
            const int64_t now = esp_timer_get_time();
            if (now - window_start > 2000000) {
                s_fps = window_frames * 1e6f / (float)(now - window_start);
                window_start = now;
                window_frames = 0;
            }
        }
    }

    s_running = false;
    s_task = NULL;
    xSemaphoreGive(s_done);
    vTaskDelete(NULL);
}

/* -------------------------------------------------------------------------
 * Public interface
 * ------------------------------------------------------------------------- */
esp_err_t webcam_start(void)
{
    if (s_running) {
        return ESP_OK;
    }

    ai_status_t ai;
    ai_camera_status(&ai);
    if (ai.running) {
        return ESP_ERR_INVALID_STATE;   /* the AI pipeline owns the sensor */
    }

    if (s_done == NULL) {
        s_done = xSemaphoreCreateBinary();
        if (s_done == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }
    if (s_ring == NULL) {
        s_ring = xRingbufferCreateWithCaps(STREAM_RING_BYTES, RINGBUF_TYPE_NOSPLIT,
                                           MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (s_ring == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }

    ESP_RETURN_ON_ERROR(camera_video_stack_init(), TAG, "video stack");
    ESP_RETURN_ON_ERROR(capture_open(), TAG, "capture");

    if (encoder_open() != ESP_OK) {
        ESP_LOGE(TAG, "H.264 encoder failed to open");
        capture_close();
        return ESP_FAIL;
    }

    s_stop_req = false;
    s_frames = 0;
    s_fps = 0;
    s_running = true;

    if (xTaskCreate(webcam_task, "webcam", 6144, NULL, 4, &s_task) != pdPASS) {
        s_running = false;
        encoder_close();
        capture_close();
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Webcam pipeline running (H.264 %u bit/s, GOP %u)",
             (unsigned)H264_BITRATE, (unsigned)H264_GOP);
    return ESP_OK;
}

void webcam_stop(void)
{
    if (!s_running) {
        return;
    }
    s_stop_req = true;
    xSemaphoreTake(s_done, pdMS_TO_TICKS(5000));
    heap_probe("stop, before encoder close");
    encoder_close();
    heap_probe("stop, after encoder close");
    capture_close();
    heap_probe("stop, after capture close");
    ESP_LOGI(TAG, "Webcam pipeline stopped");
}

bool webcam_is_running(void)
{
    return s_running;
}

void webcam_get_status(webcam_status_t *out)
{
    if (out == NULL) {
        return;
    }
    memset(out, 0, sizeof(*out));
    out->running = s_running;
    out->width = s_width;
    out->height = s_height;
    out->fps = s_fps;
    out->frames = s_frames;
    out->bitrate = H264_BITRATE;
    strlcpy(out->sensor, s_sensor, sizeof(out->sensor));
}

esp_err_t webcam_stream_open(void)
{
    if (!s_running || s_ring == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_consumer) {
        return ESP_ERR_NOT_SUPPORTED;   /* someone is already streaming */
    }

    /* Drain anything a previous client left behind so the new one starts on
     * fresh frames, not a stale backlog. */
    for (;;) {
        size_t len = 0;
        void *item = xRingbufferReceive(s_ring, &len, 0);
        if (item == NULL) {
            break;
        }
        vRingbufferReturnItem(s_ring, item);
    }

    s_consumer = true;
    return ESP_OK;
}

void webcam_stream_close(void)
{
    s_consumer = false;
}

esp_err_t webcam_stream_read(uint8_t **out_item, size_t *out_len,
                             uint32_t timeout_ms)
{
    size_t len = 0;
    void *item = xRingbufferReceive(s_ring, &len, pdMS_TO_TICKS(timeout_ms));
    if (item == NULL) {
        return ESP_ERR_TIMEOUT;
    }
    *out_item = item;
    *out_len = len;
    return ESP_OK;
}

void webcam_stream_return(uint8_t *item)
{
    vRingbufferReturnItem(s_ring, item);
}
