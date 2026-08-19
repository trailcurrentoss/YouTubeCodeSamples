# Camera pipelines

One sensor, two independent pipelines, selected from the dashboard:

- **Camera tab** — plain live view. Hardware H.264 at 25 fps, no AI.
- **AI tab** — live object detection. MJPEG stream at sensor rate with
  YOLO11n boxes overlaid, detection updating at ~1.5 Hz.

They are mutually exclusive: the sensor is a single-user resource, and each
side's start endpoint refuses with a clear message while the other runs.

## Shared foundation: sensor → ISP → V4L2

The OV5647 camera connects over 2-lane MIPI-CSI and outputs RAW8 Bayer at
**800×640, 50 fps** (chosen by `CONFIG_CAMERA_OV5647_MIPI_RAW8_800X640_50FPS`).
The P4's ISP converts Bayer into whatever each pipeline asks for; `esp_video`
exposes the whole thing as a V4L2 device (`/dev/video0`).

Each pipeline opens the device, sets its pixel format, maps `FRAME_BUFFERS`
DMA buffers, and runs a dequeue→process→requeue loop. The camera is probed
lazily on first start, so the board boots and serves everything else with no
camera attached.

**A V4L2 quirk worth knowing** (both modules carry the workaround):
`esp_video`'s `VIDIOC_G_FMT` copies its entire stored format struct back over
the caller's — including a `.type` field that is still `0` before the first
successful `S_FMT`. Reusing the struct for `S_FMT` without restoring
`fmt.type` makes `S_FMT` fail before format negotiation even starts, which
presents as "camera does not support format X" for a format it supports fine.

## Camera tab: hardware H.264 ([webcam.c](../main/webcam.c))

```
OV5647 (RAW8) → ISP → YUV420 800×640 @50fps
    → decimate to 25 fps
    → hardware H.264 encoder (4 Mbit/s, GOP 25)
    → ring buffer (768 KB PSRAM)
    → HTTP chunked stream (:81/h264)
    → browser: fetch → fMP4 remux → MSE <video>
```

Zero CPU pixel work: the ISP's `V4L2_PIX_FMT_YUV420` output is byte-for-byte
the `O_UYY_E_VYY` macroblock layout the P4's H.264 encoder consumes, so V4L2
capture buffers feed the encoder DMA directly.

### Encoder settings

Defined at the top of [webcam.c](../main/webcam.c):

| Knob | Value | Why |
|---|---|---|
| `H264_FPS` | 25 | Every second sensor frame is skipped. Visually smooth, and at fixed bitrate halving fps doubles bits per frame |
| `H264_BITRATE` | 4 Mbit/s | ≈20 KB per frame at 25 fps — keeps the quantizer in its clean range. (50 fps at 2.5 Mbit/s was visibly blocky) |
| `H264_GOP` | 25 | One IDR per second — bounds recovery after any dropped frame |
| QP range | 22–40 | Caps worst-case quality degradation |

The encoder instance is created **once** and kept resident;
`esp_h264_enc_open/close` run per session. `esp_h264_enc_hw_new()` allocates
~58 KB of internal SRAM for its reference frame — new/del per session
eventually dies with "No memory for reference frame" once the internal heap
tightens. Re-opening resets the frame counter, so every session starts on an
IDR.

### Wire format

`GET :81/h264` returns an endless `application/octet-stream` of packets:

```
┌────────────┬────────────┬───────────────────────────┐
│ u32 LE len │ u32 LE pts │ H.264 Annex-B access unit │
│            │    (ms)    │ (len bytes)               │
└────────────┴────────────┴───────────────────────────┘
```

One packet per encoded frame; IDR packets carry SPS+PPS inline. The response
sets `Access-Control-Allow-Origin: *` — the dashboard `fetch()`es this from
port 81, a *different origin* than the page on port 80, and without CORS
consent the browser discards the response. (The AI tab's MJPEG needs no CORS:
`<img>` loads are exempt.)

One streaming client at a time; a second `/h264` request gets HTTP 400. On
connect the ring is flushed so a new viewer starts on fresh frames. If the
ring fills (stalled client), frames are dropped and the stream heals at the
next IDR.

### Browser player ([dashboard.js](../main/web/dashboard.js))

The player remuxes each access unit into a one-sample **fragmented-MP4**
segment (a ~100-line muxer, no libraries) and appends it to a `<video>`
element through Media Source Extensions:

- **MSE, not WebCodecs, deliberately** — WebCodecs only exists in secure
  contexts, and this dashboard is plain `http://` on the LAN, so
  `VideoDecoder` is simply absent. MSE works on insecure origins and still
  hands decode to the browser's hardware decoder.
- The codec string (`avc1.PPCCLL`) is built from the profile/level bytes of
  the SPS in the first keyframe.
- The MP4 timeline is **synthetic and continuous** (one 40 ms tick per
  appended frame) rather than copied from board timestamps — a frame dropped
  anywhere must not leave a timestamp hole that stalls MSE playback.
- Backlog drops are **GOP-atomic**: once one frame is dropped, everything
  until the next keyframe goes too, so the decoder never sees a broken
  reference chain (that presents as smearing/ghosting).
- A live-edge chase keeps the playhead within ~0.7 s of the newest frame, and
  buffered history is trimmed so long sessions don't grow browser memory.

The page needs a browser with H.264 in MSE — stock Chrome/Edge/Safari/Firefox
all qualify; a codec-free Chromium build does not, and the tab says so.

### Diagnostics

The pipeline carries heap-integrity tripwires
(`heap_caps_check_integrity_all`) after the first encoded frame, every 256
frames, and at each teardown stage, logging `HEAP CORRUPT at <stage>` if the
check fails. They were added while chasing a PSRAM heap corruption that
surfaced as a TLSF assert during `webcam_stop`; the encoder output buffer
also carries 64 KB of deliberate slack from the same investigation. If the
console stays quiet, the pipeline is clean; a tripwire message plus the stage
name localizes any recurrence.

## AI tab: detection + MJPEG ([ai_camera.cpp](../main/ai_camera.cpp))

```
OV5647 (RAW8) → ISP → RGB565 800×640 @50fps
    ├── capture task: hardware JPEG (quality 75) → newest-frame buffer → :81/stream (MJPEG)
    └── detect task:  copy of newest frame → YOLO11n int8 → boxes → /api/ai/detections
```

### Decoupled capture and detection

Capture/encode and inference are **separate tasks**. The capture task
publishes a JPEG of *every* frame at sensor rate, so the stream is smooth
video; the detection task runs the model on a private copy of the newest
frame at whatever rate inference allows (~600 ms per pass → ~1.5 Hz box
updates). Before the split, the whole stream ran at inference rate — 1.5 fps
video.

The hand-off is a want-flag protocol, race-free without locks around the
frame data: the capture task copies into the detector's buffer *only* while
the detector is parked waiting, then signals it.

The detect task also owns model bring-up: on first start it loads and warms
the model on a blank frame **before** the capture task exists (the
multi-second load must not compete with JPEG DMA), then spawns the capture
task. Heap numbers are logged around the load.

### The model

| | |
|---|---|
| Network | YOLO11n (nano), ~2.6 M parameters, 80 COCO classes |
| Input | 320×320 (frames are letterbox-resized internally) |
| Quantization | int8, per-channel (`.espdl`, esp-dl format) |
| Size | 2.99 MB, embedded in the app image (`FLASH_RODATA`) |
| Runtime | Copied to PSRAM at load (a consequence of XIP-from-PSRAM), plus inference arena |
| Inference | ~600 ms/frame on the P4 at 360 MHz |

The 320 variant is flashed instead of the 640 one: roughly double the frame
rate for somewhat looser boxes — the right trade for a live overlay. Only one
variant is embedded (each is ~3 MB of image). Model selection is Kconfig
(`CONFIG_COCO_DETECT_YOLO11N_320_S8_V1`); the pipeline loads whatever
`.espdl` the coco_detect component provides, so another converted model drops
in without code changes.

### MJPEG stream

`GET :81/stream` is `multipart/x-mixed-replace`: the handler copies the
newest published JPEG whenever its sequence number changes and sends it as
one part. Frame pacing is pull-based — a slow client simply skips to the
latest frame, so the stream degrades gracefully to whatever the link
sustains. The browser displays it with a plain `<img>`, with detection boxes
drawn on an absolutely-positioned `<canvas>` overlay scaled to the frame.

Detections are polled separately (`/api/ai/detections`, 400 ms interval while
the tab is visible) and carry pixel-space corners plus class and score.

## Ownership and lifecycle

- Either pipeline starts on demand from its tab and **keeps running when you
  navigate away** — stopping is an explicit button press. Leaving a tab only
  detaches the browser-side stream/polling.
- `/api/ai/start` fails while the webcam runs and vice versa, each with a
  message naming the other tab.
- Stopping tears down the V4L2 session (streamoff, close, buffers freed) but
  leaves the expensive singletons (model, JPEG engine, H.264 encoder, rings)
  resident for the next start.
