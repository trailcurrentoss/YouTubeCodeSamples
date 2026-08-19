/*
 * Plain webcam streaming for the Waveshare ESP32-P4-WIFI6 — no inference.
 *
 * Pipeline per frame: OV5647 (RAW8 over 2-lane CSI) -> ISP -> YUV420 frame ->
 * hardware H.264 encoder -> length-prefixed Annex-B stream over HTTP.
 *
 * The ISP's V4L2_PIX_FMT_YUV420 output is the exact O_UYY_E_VYY macroblock
 * layout the P4's H.264 encoder consumes, so frames go from the camera to the
 * encoder with zero CPU format conversion — the whole pipeline runs at sensor
 * rate, unlike the AI tab whose rate is bounded by inference.
 *
 * The camera sensor is a single-user resource: this module and the AI pipeline
 * (ai_camera.h) cannot run at the same time.
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

typedef struct {
    bool     running;
    uint16_t width;              /**< capture frame width */
    uint16_t height;             /**< capture frame height */
    float    fps;                /**< end-to-end pipeline rate */
    uint32_t frames;             /**< frames encoded since start */
    uint32_t bitrate;            /**< encoder target bitrate, bits/s */
    char     sensor[24];         /**< detected sensor name, for the UI */
} webcam_status_t;

/**
 * @brief Start the camera + hardware H.264 pipeline.
 *
 * Returns ESP_ERR_INVALID_STATE if the AI pipeline currently owns the camera,
 * ESP_ERR_NOT_FOUND if no camera responds on the CSI connector.
 */
esp_err_t webcam_start(void);

/** @brief Stop the pipeline and release the camera. */
void webcam_stop(void);

/** @brief True while the pipeline is running (used for mutual exclusion). */
bool webcam_is_running(void);

/** @brief Pipeline state and performance counters. */
void webcam_get_status(webcam_status_t *out);

/**
 * @brief Claim the (single) stream consumer slot and flush stale frames.
 *
 * Returns ESP_ERR_INVALID_STATE if another client is already streaming.
 */
esp_err_t webcam_stream_open(void);

/** @brief Release the consumer slot. */
void webcam_stream_close(void);

/**
 * @brief Wait for the next encoded packet.
 *
 * Each packet is wire-ready: an 8-byte header (uint32 LE payload length,
 * uint32 LE PTS in ms) followed by one H.264 Annex-B access unit. Pass the
 * pointer back via webcam_stream_return() once sent. Returns ESP_ERR_TIMEOUT
 * if no frame arrived within @p timeout_ms.
 */
esp_err_t webcam_stream_read(uint8_t **out_item, size_t *out_len,
                             uint32_t timeout_ms);

/** @brief Return a packet obtained from webcam_stream_read(). */
void webcam_stream_return(uint8_t *item);

#ifdef __cplusplus
}
#endif
