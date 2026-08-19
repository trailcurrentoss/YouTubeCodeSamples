/*
 * MIPI-CSI camera + on-device object detection for the Waveshare ESP32-P4-WIFI6.
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

#include "driver/i2c_master.h"
#include "driver/jpeg_encode.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_video_init.h"
#include "esp_video_device.h"
#include "linux/videodev2.h"

#include "coco_detect.hpp"

#include "ai_camera.h"
#include "board_esp32p4_wifi6.h"

static const char *TAG = "ai_cam";

#define VIDEO_DEV        ESP_VIDEO_MIPI_CSI_DEVICE_NAME   /* "/dev/video0" */
#define FRAME_BUFFERS    2
#define JPEG_BUF_BYTES   (256 * 1024)
#define JPEG_QUALITY     75

/* -------------------------------------------------------------------------
 * State
 * ------------------------------------------------------------------------- */
static bool                  s_video_inited;
static volatile bool         s_running;
static volatile bool         s_stop_req;
static SemaphoreHandle_t     s_done;         /* counting: one give per task */
static SemaphoreHandle_t     s_lock;         /* guards the published outputs */

/* Capture/encode and detection are decoupled: the capture task publishes a
 * JPEG of EVERY frame at sensor rate, while the detection task chews on a
 * private copy of the newest frame at whatever rate inference allows. The
 * want-flag protocol keeps the copy race-free: the capture task writes
 * s_det_buf only while the detector is parked waiting on s_det_go. */
static uint8_t              *s_det_buf;      /* detector's frame copy (PSRAM) */
static volatile bool         s_det_want;     /* detector is ready for a frame */
static SemaphoreHandle_t     s_det_go;       /* "a fresh frame is in s_det_buf" */
static volatile bool         s_capture_started;  /* capture task was spawned */

static int                   s_fd = -1;
static void                 *s_fbuf[FRAME_BUFFERS];
static uint16_t              s_width, s_height;
static char                  s_sensor[24];

static COCODetect           *s_detector;
static jpeg_encoder_handle_t s_jpeg;
static uint8_t              *s_jpeg_out;     /* encoder output (DMA-capable) */
static size_t                s_jpeg_out_alloc;

/* published outputs */
static uint8_t              *s_pub_jpeg;
static size_t                s_pub_jpeg_len;
static uint32_t              s_pub_seq;
static ai_detection_t        s_pub_det[AI_MAX_DETECTIONS];
static int                   s_pub_det_count;
static uint32_t              s_pub_det_seq;
static float                 s_fps;
static uint32_t              s_infer_ms;
static uint32_t              s_frames;

/* COCO class names, index 0..79 */
static const char *COCO_NAMES[80] = {
    "person","bicycle","car","motorcycle","airplane","bus","train","truck",
    "boat","traffic light","fire hydrant","stop sign","parking meter","bench",
    "bird","cat","dog","horse","sheep","cow","elephant","bear","zebra",
    "giraffe","backpack","umbrella","handbag","tie","suitcase","frisbee",
    "skis","snowboard","sports ball","kite","baseball bat","baseball glove",
    "skateboard","surfboard","tennis racket","bottle","wine glass","cup",
    "fork","knife","spoon","bowl","banana","apple","sandwich","orange",
    "broccoli","carrot","hot dog","pizza","donut","cake","chair","couch",
    "potted plant","bed","dining table","toilet","tv","laptop","mouse",
    "remote","keyboard","cell phone","microwave","oven","toaster","sink",
    "refrigerator","book","clock","vase","scissors","teddy bear","hair drier",
    "toothbrush",
};

/* -------------------------------------------------------------------------
 * Bring-up
 * ------------------------------------------------------------------------- */
extern "C" esp_err_t camera_video_stack_init(void)
{
    if (s_video_inited) {
        return ESP_OK;
    }

    /* The audio module already created the i2c_master bus on GPIO 7/8; the
     * camera's SCCB is one more device on it. */
    i2c_master_bus_handle_t bus = NULL;
    ESP_RETURN_ON_ERROR(i2c_master_get_bus_handle(0, &bus), TAG,
                        "I2C bus not initialised (audio_recorder_init runs first)");

    esp_video_init_csi_config_t csi = {};
    csi.sccb_config.init_sccb = false;
    csi.sccb_config.i2c_handle = bus;
    csi.sccb_config.freq = 100000;
    /* No reset/powerdown lines on the Waveshare CSI connector wiring. */
    csi.reset_pin = GPIO_NUM_NC;
    csi.pwdn_pin  = GPIO_NUM_NC;

    esp_video_init_config_t cfg = {};
    cfg.csi = &csi;

    esp_err_t ret = esp_video_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_video_init failed: %s — is a camera connected to "
                      "the CSI port?", esp_err_to_name(ret));
        return ret == ESP_OK ? ESP_FAIL : ret;
    }
    s_video_inited = true;
    return ESP_OK;
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

    /* Ask the ISP for RGB565 at the sensor's native size: the esp-dl detector
     * takes RGB565 directly, and the hardware JPEG encoder eats it too. */
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(s_fd, VIDIOC_G_FMT, &fmt) != 0) {
        goto fail;
    }
    /* esp_video's G_FMT copies back the whole stored v4l2_format, whose .type
     * is still 0 until the first successful S_FMT — restore it or S_FMT is
     * rejected before format negotiation even happens. */
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
    if (ioctl(s_fd, VIDIOC_S_FMT, &fmt) != 0) {
        ESP_LOGE(TAG, "Camera does not offer RGB565 output");
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

    ESP_LOGI(TAG, "Camera \"%s\" streaming %ux%u RGB565", s_sensor, s_width, s_height);
    return ESP_OK;

fail:
    ESP_LOGE(TAG, "V4L2 setup failed");
    close(s_fd);
    s_fd = -1;
    return ESP_FAIL;
}

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

/* -------------------------------------------------------------------------
 * Pipeline tasks
 *
 * Decoupled: capture_task publishes a JPEG of EVERY frame at sensor rate, so
 * the MJPEG stream is smooth video; detect_task runs the model on a private
 * copy of the newest frame at whatever rate inference allows (~600 ms per
 * pass), so boxes update at ~1.5 Hz over the live stream. Before the split,
 * the whole stream ran at inference rate.
 * ------------------------------------------------------------------------- */
static void capture_task(void *arg);

static void detect_task(void *arg)
{
    /* The model loads lazily inside the first run(). Do that here on a blank
     * frame BEFORE the capture task exists: the multi-second load must not
     * compete with JPEG DMA and frame copies, and doing it up front also
     * makes the first live inference fast. The heap numbers bracket the load
     * so a memory-pressure failure is visible in the console. */
    ESP_LOGI(TAG, "Loading model: internal free %u KB (largest %u KB), PSRAM free %u KB",
             (unsigned)(heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024),
             (unsigned)(heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL) / 1024),
             (unsigned)(heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024));

    dl::image::img_t warm = {};
    warm.data = s_det_buf;
    warm.width = s_width;
    warm.height = s_height;
    warm.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB565LE;
    memset(s_det_buf, 0, (size_t)s_width * s_height * 2);
    s_detector->run(warm);

    ESP_LOGI(TAG, "Model ready: internal free %u KB (largest %u KB)",
             (unsigned)(heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024),
             (unsigned)(heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL) / 1024));

    if (!s_stop_req) {
        if (xTaskCreate(capture_task, "ai_cam", 6144, NULL, 4, NULL) == pdPASS) {
            s_capture_started = true;
        } else {
            ESP_LOGE(TAG, "Capture task failed to start");
            s_running = false;
        }
    }

    while (!s_stop_req) {
        /* Time-boxed wait so a stop request is noticed even when the capture
         * task died and no more frames are coming. */
        if (xSemaphoreTake(s_det_go, pdMS_TO_TICKS(500)) != pdTRUE) {
            continue;
        }
        if (s_stop_req) {
            break;
        }

        dl::image::img_t img = {};
        img.data = s_det_buf;
        img.width = s_width;
        img.height = s_height;
        img.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB565LE;

        const int64_t t0 = esp_timer_get_time();
        auto &results = s_detector->run(img);
        s_infer_ms = (uint32_t)((esp_timer_get_time() - t0) / 1000);

        xSemaphoreTake(s_lock, portMAX_DELAY);
        int n = 0;
        for (const auto &r : results) {
            if (n >= AI_MAX_DETECTIONS) {
                break;
            }
            s_pub_det[n].category = r.category;
            s_pub_det[n].score = r.score;
            s_pub_det[n].x0 = r.box[0];
            s_pub_det[n].y0 = r.box[1];
            s_pub_det[n].x1 = r.box[2];
            s_pub_det[n].y1 = r.box[3];
            n++;
        }
        s_pub_det_count = n;
        s_pub_det_seq++;
        xSemaphoreGive(s_lock);

        s_det_want = true;   /* park until the capture task copies a frame */
    }

    xSemaphoreGive(s_done);
    vTaskDelete(NULL);
}

static void capture_task(void *arg)
{
    int64_t window_start = esp_timer_get_time();
    uint32_t window_frames = 0;

    while (!s_stop_req) {
        struct v4l2_buffer buf = {};
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(s_fd, VIDIOC_DQBUF, &buf) != 0) {
            ESP_LOGE(TAG, "Frame dequeue failed, stopping");
            break;
        }

        /* ---- JPEG encode (hardware) ---- */
        jpeg_encode_cfg_t enc_cfg = {};
        enc_cfg.src_type = JPEG_ENCODE_IN_FORMAT_RGB565;
        enc_cfg.sub_sample = JPEG_DOWN_SAMPLING_YUV420;
        enc_cfg.image_quality = JPEG_QUALITY;
        enc_cfg.width = s_width;
        enc_cfg.height = s_height;

        uint32_t jpeg_len = 0;
        const esp_err_t jret = jpeg_encoder_process(
            s_jpeg, &enc_cfg, (const uint8_t *)s_fbuf[buf.index],
            (uint32_t)s_width * s_height * 2, s_jpeg_out,
            (uint32_t)s_jpeg_out_alloc, &jpeg_len);

        if (jret == ESP_OK && jpeg_len > 0 && jpeg_len <= JPEG_BUF_BYTES) {
            xSemaphoreTake(s_lock, portMAX_DELAY);
            memcpy(s_pub_jpeg, s_jpeg_out, jpeg_len);
            s_pub_jpeg_len = jpeg_len;
            s_pub_seq++;
            xSemaphoreGive(s_lock);
        }

        /* ---- hand the newest frame to the detector when it is ready ----
         * s_det_want is only true while the detector is parked on s_det_go,
         * so writing the copy here cannot race the model reading it. */
        if (s_det_want) {
            memcpy(s_det_buf, s_fbuf[buf.index], (size_t)s_width * s_height * 2);
            s_det_want = false;
            xSemaphoreGive(s_det_go);
        }

        ioctl(s_fd, VIDIOC_QBUF, &buf);

        s_frames++;
        window_frames++;
        const int64_t now = esp_timer_get_time();
        if (now - window_start > 2000000) {
            s_fps = window_frames * 1e6f / (float)(now - window_start);
            window_start = now;
            window_frames = 0;
        }
    }

    s_running = false;
    xSemaphoreGive(s_done);
    vTaskDelete(NULL);
}

/* -------------------------------------------------------------------------
 * Public interface
 * ------------------------------------------------------------------------- */
extern "C" esp_err_t ai_camera_start(void)
{
    if (s_running) {
        return ESP_OK;
    }

    if (s_lock == NULL) {
        s_lock = xSemaphoreCreateMutex();
        s_done = xSemaphoreCreateCounting(2, 0);   /* one give per task */
        s_det_go = xSemaphoreCreateBinary();
        if (s_lock == NULL || s_done == NULL || s_det_go == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }

    ESP_RETURN_ON_ERROR(camera_video_stack_init(), TAG, "video stack");
    ESP_RETURN_ON_ERROR(capture_open(), TAG, "capture");

    if (s_det_buf == NULL) {
        /* Frame size is fixed by the sensor mode, so this allocates once. */
        s_det_buf = (uint8_t *)heap_caps_malloc((size_t)s_width * s_height * 2,
                                                MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (s_det_buf == NULL) {
            capture_close();
            return ESP_ERR_NO_MEM;
        }
    }

    /* Model loads once and stays resident — reloading 3 MB per session would
     * make start sluggish for no benefit. */
    if (s_detector == NULL) {
        s_detector = new (std::nothrow) COCODetect();
        if (s_detector == NULL) {
            capture_close();
            return ESP_ERR_NO_MEM;
        }
    }

    if (s_jpeg == NULL) {
        const jpeg_encode_engine_cfg_t eng = {
            .intr_priority = 0,
            .timeout_ms = 1000,
        };
        if (jpeg_new_encoder_engine(&eng, &s_jpeg) != ESP_OK) {
            capture_close();
            return ESP_FAIL;
        }
        jpeg_encode_memory_alloc_cfg_t mem_cfg = { .buffer_direction = JPEG_ENC_ALLOC_OUTPUT_BUFFER };
        s_jpeg_out = (uint8_t *)jpeg_alloc_encoder_mem(JPEG_BUF_BYTES, &mem_cfg, &s_jpeg_out_alloc);
        s_pub_jpeg = (uint8_t *)heap_caps_malloc(JPEG_BUF_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (s_jpeg_out == NULL || s_pub_jpeg == NULL) {
            capture_close();
            return ESP_ERR_NO_MEM;
        }
    }

    s_stop_req = false;
    s_pub_jpeg_len = 0;
    s_pub_det_count = 0;
    s_frames = 0;
    s_fps = 0;
    s_det_want = true;                 /* detector starts hungry */
    xSemaphoreTake(s_det_go, 0);       /* clear a stale give from a past stop */
    s_capture_started = false;
    s_running = true;

    /* Only the detector starts here — it loads the model, then spawns the
     * capture task itself. Priority below the capture task so a long
     * inference pass never starves the encode/stream path; on the dual-core
     * P4 they run in parallel. The 16 KB stack is for model load, which runs
     * on this stack the first time. */
    if (xTaskCreate(detect_task, "ai_det", 16384, NULL, 3, NULL) != pdPASS) {
        s_running = false;
        capture_close();
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "AI pipeline running");
    return ESP_OK;
}

extern "C" void ai_camera_stop(void)
{
    if (!s_running && !s_capture_started) {
        return;
    }
    s_stop_req = true;
    xSemaphoreGive(s_det_go);          /* wake the detector if it is parked */
    /* Model load can take a while; give the detector time to notice. */
    xSemaphoreTake(s_done, pdMS_TO_TICKS(10000));  /* detect task */
    if (s_capture_started) {
        xSemaphoreTake(s_done, pdMS_TO_TICKS(5000));   /* capture task */
        s_capture_started = false;
    }
    s_running = false;
    capture_close();
    ESP_LOGI(TAG, "AI pipeline stopped");
}

extern "C" void ai_camera_status(ai_status_t *out)
{
    if (out == NULL) {
        return;
    }
    memset(out, 0, sizeof(*out));
    out->running = s_running;
    out->width = s_width;
    out->height = s_height;
    out->fps = s_fps;
    out->infer_ms = s_infer_ms;
    out->frames = s_frames;
    strlcpy(out->sensor, s_sensor, sizeof(out->sensor));
    strlcpy(out->model, "YOLO11n COCO (espdl)", sizeof(out->model));
}

extern "C" esp_err_t ai_camera_get_jpeg(uint8_t *buf, size_t buf_len,
                                        size_t *out_len, uint32_t *out_seq)
{
    if (s_lock == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret = ESP_OK;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    if (s_pub_jpeg_len == 0) {
        ret = ESP_ERR_NOT_FOUND;
    } else if (s_pub_jpeg_len > buf_len) {
        ret = ESP_ERR_NO_MEM;
    } else {
        memcpy(buf, s_pub_jpeg, s_pub_jpeg_len);
        *out_len = s_pub_jpeg_len;
        if (out_seq) {
            *out_seq = s_pub_seq;
        }
    }
    xSemaphoreGive(s_lock);
    return ret;
}

extern "C" int ai_camera_get_detections(ai_detection_t *out, int max, uint32_t *out_seq)
{
    if (s_lock == NULL || out == NULL || max <= 0) {
        return 0;
    }
    xSemaphoreTake(s_lock, portMAX_DELAY);
    const int n = s_pub_det_count < max ? s_pub_det_count : max;
    memcpy(out, s_pub_det, n * sizeof(ai_detection_t));
    if (out_seq) {
        *out_seq = s_pub_det_seq;
    }
    xSemaphoreGive(s_lock);
    return n;
}

extern "C" const char *ai_camera_class_name(int category)
{
    if (category < 0 || category >= 80) {
        return "?";
    }
    return COCO_NAMES[category];
}
