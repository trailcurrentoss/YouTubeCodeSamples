# System architecture

Firmware for the Waveshare ESP32-P4-WIFI6 (SKU 31647): a self-contained demo
that provisions itself onto Wi-Fi, then serves a browser dashboard exposing
every major subsystem of the board — camera (with and without on-device AI),
microphone and speaker, microSD storage, and board telemetry.

Everything is served from the firmware itself. The web assets are compiled
into the binary (`EMBED_FILES`), so the UI always matches the firmware and
survives the SD card being absent or reformatted.

## Boot flow

`app_main()` in [main/main.c](../main/main.c) runs strictly in this order:

1. **Hardware report** — prints chip revision, detected flash and PSRAM, free
   heap; warns if either memory does not match the expected 32 MB.
2. **microSD mount** at `/sdcard` (slot 0, 4-bit). Absence is logged, not
   fatal. A blank card is given a filesystem at mount. Optional self-tests
   (`CONFIG_DEMO_SD_RUN_TESTS`) and opt-in wipe (`CONFIG_DEMO_SD_FORMAT_ON_BOOT`).
3. **Audio codec init** — ES8311 over I2C, I2S channels created. Failure
   disables recording but boots on.
4. **Voice assistant** — the `prg_voice` worker starts, then
   `/sdcard/environment.conf` is read for the Peregrine URL and token. This
   order matters: the config reader calls setters the worker owns. A missing
   card or file is not an error — push-to-talk simply reports itself
   unconfigured when the button is pressed.
5. **Push-to-talk button** — GPIO20 configured, polling task started.
6. **Wi-Fi init** — via the onboard ESP32-C6 (see below). Failure halts with a
   clear console message; there is nothing to serve without a network.
7. **Mode selection**:
   - **Setup mode** (no saved credentials): SoftAP `TrailCurrent-Setup-XXXX`,
     captive-portal DNS, provisioning UI. Credentials are verified by actually
     joining the network *before* they are saved; then the board reboots.
   - **Normal mode**: joins the saved network, serves the dashboard, prints
     the dashboard URL on the console once an IP is obtained.

After that the system is entirely event-driven; `app_main` idles.

## The radio is a second chip

The ESP32-P4 has no Wi-Fi. The board's ESP32-C6 provides it over SDIO
(slot 1), driven by two managed components:

- `espressif/esp_wifi_remote` — presents the standard `esp_wifi` API and
  forwards calls to the C6.
- `espressif/esp_hosted` — the SDIO transport underneath.

The C6 must be running ESP-Hosted slave firmware (boards ship with it). All
Wi-Fi code in [main/wifi_manager.c](../main/wifi_manager.c) looks like
ordinary ESP32 Wi-Fi code because the remote API is transparent.

Note the SDMMC split: **slot 0 = microSD, slot 1 = C6 radio.** Both are in
use; the card must stay pinned to slot 0 (see [storage.md](storage.md)).

## Module map

| Module | Responsibility |
|---|---|
| [main.c](../main/main.c) | Boot orchestration, hardware report, mode selection |
| [wifi_manager.c](../main/wifi_manager.c) | NVS credentials, SoftAP/APSTA provisioning, station mode, scan, verified join |
| [web_server.c](../main/web_server.c) | Both HTTP servers, static assets, the whole REST API, both stream endpoints |
| [dns_portal.c](../main/dns_portal.c) | Captive-portal DNS responder (setup mode only) |
| [sd_card.c](../main/sd_card.c) | LDO power-up, mount/format/unmount, capacity |
| [sd_test.c](../main/sd_test.c) | Offline SD self-test suite |
| [audio_recorder.c](../main/audio_recorder.c) | ES8311 + I2S capture to WAV, level metering, speaker playback, push-to-talk capture and streamed playback |
| [record_button.c](../main/record_button.c) | Polls the header push button, debounced, and drives push-to-talk |
| [peregrine_voice.c](../main/peregrine_voice.c) | Uploads a held-button capture to Peregrine and streams the spoken reply back |
| [sd_config.c](../main/sd_config.c) | Reads `/sdcard/environment.conf` at boot (Peregrine URL and token) |
| [ai_camera.cpp](../main/ai_camera.cpp) | Camera + on-device object detection + MJPEG (the **AI** tab) |
| [webcam.c](../main/webcam.c) | Camera + hardware H.264 streaming, no AI (the **Camera** tab) |
| [board_esp32p4_wifi6.h](../main/board_esp32p4_wifi6.h) | Every pin assignment in one place |
| [components/es8311](../components/es8311) | Vendored codec driver, ported to `i2c_master` |
| [main/web/](../main/web) | Embedded frontend: setup UI, dashboard UI, shared CSS |

Full camera detail is in [camera.md](camera.md); the HTTP surface is in
[web-api.md](web-api.md).

## Task model (normal mode, everything running)

| Task | Prio | Created by | Role |
|---|---|---|---|
| `httpd` (port 80) | 5 | web_server | UI + REST API. One worker task — handlers must return quickly |
| `httpd` (port 81) | 5 | web_server | Stream server. MJPEG (`/stream`) and H.264 (`/h264`) handlers occupy their worker for the stream's lifetime, which is why they get their own server instance |
| `ai_cam` | 4 | ai_camera | AI capture loop: V4L2 dequeue → hardware JPEG → publish, at sensor rate |
| `ai_det` | 3 | ai_camera | Detection loop: model inference on a private copy of the newest frame (~600 ms/pass). Loads the model on first start, then spawns `ai_cam` |
| `webcam` | 4 | webcam | Webcam loop: V4L2 dequeue → hardware H.264 → ring buffer, 25 fps |
| audio capture / writer | — | audio_recorder | Mic capture into a 16 MB PSRAM ring; a lower-priority writer drains it to the card |
| `ptt_cap` | 6 | audio_recorder | Push-to-talk capture straight into PSRAM while the button is held. No card involvement |
| `record_btn` | 4 | record_button | Polls the button every 10 ms, 30 ms debounce, and calls press/release |
| `prg_voice` | 5 | peregrine_voice | Owns one voice exchange end to end: upload, then reply body → speaker as it arrives |
| ESP-Hosted tasks | 23 | esp_hosted | SDIO transport to the C6 |

The AI pair and the webcam task never run at the same time — the camera
sensor is a single-user resource and both sides enforce it (HTTP 400 with an
explanatory message if the other pipeline holds it).

## Two HTTP servers, one socket budget

`esp_http_server` runs one worker task per instance. A video stream occupies
its handler until the client disconnects, so streams live on a **second
instance on port 81** — otherwise one open stream would freeze the entire UI.

Sockets are a global LWIP pool (`CONFIG_LWIP_MAX_SOCKETS=16`). Budget:

| Consumer | Sockets |
|---|---|
| 2 listeners + 2 UDP control | 4 |
| UI server sessions (port 80) | up to 7 |
| Stream server sessions (port 81) | up to 2 |

The stream server is deliberately capped at 2 (`max_open_sockets`) — streams
are one-per-viewer, and the default of 7 per instance exhausted the pool: new
connections then die inside `accept()` with errno 23 (`ENFILE`), which
presents as a phone that "can't connect" while an existing session works.

## Memory

| Resource | Size | Notes |
|---|---|---|
| Flash | 32 MB | App partition 8 MB (image ≈ 6.3 MB, ~half of it the model), FAT partition 16 MB, ~8 MB unallocated |
| PSRAM | 32 MB, 200 MHz HEX, XIP | Code/rodata execute from PSRAM. Video frame buffers, H.264 stream ring (768 KB), audio ring (16 MB), model copy (~3 MB) and inference arena all live here |
| Internal SRAM | ~356 KB free at boot | Task stacks, DMA descriptors, the H.264 encoder's reference frame (~58 KB), Wi-Fi/LWIP |

Large, hardware-facing allocations are made once and kept resident across
start/stop cycles (detector model, JPEG engine, H.264 encoder, ring buffers).
Re-allocating them per session eventually fails on a tightening internal
heap; resident singletons make start/stop idempotent instead.

## Partition table

| Name | Type | Offset | Size |
|---|---|---|---|
| `nvs` | data/nvs | 0x9000 | 24 KB |
| `phy_init` | data/phy | 0xf000 | 4 KB |
| `factory` | app | 0x10000 | 8 MB |
| `storage` | data/fat | 0x810000 | 16 MB |
