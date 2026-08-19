/*
 * MIPI-CSI camera + on-device object detection for the Waveshare ESP32-P4-WIFI6.
 *
 * Pipeline per frame: OV5647 (RAW8 over 2-lane CSI) -> ISP -> RGB565 frame ->
 * esp-dl COCO detector (quantized YOLO11n .espdl; a converted YOLO26n drops in
 * via the same loader) -> hardware JPEG encoder -> MJPEG over HTTP.
 *
 * C interface so the C web server can drive the C++ esp-dl pipeline.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AI_MAX_DETECTIONS 16

typedef struct {
    int   category;              /**< COCO class index 0..79 */
    float score;                 /**< 0..1 */
    int   x0, y0, x1, y1;        /**< box corners in frame coordinates */
} ai_detection_t;

typedef struct {
    bool     running;
    uint16_t width;              /**< capture frame width */
    uint16_t height;             /**< capture frame height */
    float    fps;                /**< end-to-end pipeline rate */
    uint32_t infer_ms;           /**< last inference duration */
    uint32_t frames;             /**< frames processed since start */
    char     model[32];          /**< model name, for the UI */
    char     sensor[24];         /**< detected sensor name, for the UI */
} ai_status_t;

/**
 * @brief One-time esp_video/CSI bring-up, shared with the webcam pipeline.
 *
 * Idempotent. Lives here because esp_video may only be initialised once per
 * boot, whichever pipeline touches the camera first.
 */
esp_err_t camera_video_stack_init(void);

/**
 * @brief Start the camera + detection pipeline.
 *
 * Initialises esp_video on first use (sharing the I2C bus the audio codec
 * already created), starts streaming and spawns the inference task. Returns
 * ESP_ERR_NOT_FOUND if no camera responds on the CSI connector.
 */
esp_err_t ai_camera_start(void);

/** @brief Stop the pipeline and release frame buffers (sensor stays probed). */
void ai_camera_stop(void);

/** @brief Pipeline state and performance counters. */
void ai_camera_status(ai_status_t *out);

/**
 * @brief Copy of the newest JPEG frame.
 *
 * Copies into @p buf (up to @p buf_len) and sets @p out_len. Returns
 * ESP_ERR_NOT_FOUND while no frame has been produced yet, ESP_ERR_NO_MEM if
 * the buffer is too small. @p out_seq reports the frame's sequence number so
 * a streamer can wait for a NEW frame instead of resending the same one.
 */
esp_err_t ai_camera_get_jpeg(uint8_t *buf, size_t buf_len, size_t *out_len,
                             uint32_t *out_seq);

/** @brief Newest detection set. Returns the number of boxes written. */
int ai_camera_get_detections(ai_detection_t *out, int max, uint32_t *out_seq);

/** @brief COCO class name for a category index, e.g. 0 -> "person". */
const char *ai_camera_class_name(int category);

#ifdef __cplusplus
}
#endif
