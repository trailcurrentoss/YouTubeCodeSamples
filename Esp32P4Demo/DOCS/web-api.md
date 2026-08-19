# Web UI and HTTP API

Two servers:

- **Port 80** — the UI (embedded HTML/CSS/JS) and every REST endpoint.
- **Port 81** — the two video streams. A stream occupies its HTTP worker for
  its whole lifetime, so streams get their own server instance; on port 80
  one open stream would freeze the entire UI.

All JSON responses carry `ok: true`, or `ok: false` with a human-readable
`error`. Assets and API responses are sent `Cache-Control: no-store` — the UI
is versioned by the firmware itself, and stale assets against a new API are
worse than a re-download.

Endpoints are registered **per mode**: setup-mode endpoints do not exist once
the board is provisioned, and vice versa.

## Setup mode (unprovisioned)

| Method | Path | Purpose |
|---|---|---|
| GET | `/` | Provisioning UI |
| GET | `/api/status` | Mode and AP state |
| GET | `/api/scan` | Nearby networks — strongest first, duplicate SSIDs collapsed |
| POST | `/api/connect` | `{ssid, password}` → join, verify, save to NVS, reboot. Failure reports *why* (bad password vs. network gone vs. timeout) without saving |
| GET | `/generate_204`, `/hotspot-detect.html`, `/ncsi.txt`, … | Captive-portal probes; redirect to `/` so the page opens on its own |

## Normal mode (dashboard)

### Board and network

| Method | Path | Purpose |
|---|---|---|
| GET | `/api/status` | Mode, connection state, SSID, IP, MAC, RSSI, channel |
| GET | `/api/board` | Everything the Settings page shows: chip, revision, flash, PSRAM and internal free heap, IDF version, build date, uptime, microSD detail, mic state, network detail |
| POST | `/api/forget` | Erase credentials, reboot into setup mode |

### microSD

| Method | Path | Purpose |
|---|---|---|
| GET | `/api/sd/list?path=` | Directory listing (name, dir flag, size) |
| POST | `/api/sd/delete` | `{path}` → delete a file or **empty** folder |
| GET | `/api/sd/download?path=` | Stream a file in 4 KB chunks; `&dl=1` adds `Content-Disposition: attachment` |

Path safety in one place: values are percent-decoded centrally (ESP-IDF's
`httpd_query_key_value` returns them raw), anything containing `..` or `\` is
rejected, and the FAT mount point is the ceiling. Delete refuses non-empty
directories — recursive deletion behind a web button is too easy to trigger.

### Audio

| Method | Path | Purpose |
|---|---|---|
| POST | `/api/audio/start` | Begin recording (returns immediately) |
| POST | `/api/audio/stop` | Stop, finalize the WAV, return path, size, duration, peak/RMS dBFS, and a `silent` flag |
| GET | `/api/audio/state` | Recording + playback state, elapsed, position/duration |
| POST | `/api/audio/play` | `{path}` → play a WAV through the board's speaker |
| POST | `/api/audio/playstop` | Stop board playback |

### AI camera (detection)

| Method | Path | Purpose |
|---|---|---|
| POST | `/api/ai/start` | Bring up camera + detection. 400 if the Camera tab holds the sensor |
| POST | `/api/ai/stop` | Stop the pipeline |
| GET | `/api/ai/status` | `running, width, height, fps, inferMs, frames, model, sensor` |
| GET | `/api/ai/detections` | `seq` plus boxes: `label, score, x0, y0, x1, y1` in frame pixels |
| GET | `:81/stream` | MJPEG live view — `multipart/x-mixed-replace`, one JPEG per part, always the newest frame |

### Webcam (plain H.264)

| Method | Path | Purpose |
|---|---|---|
| POST | `/api/cam/start` | Bring up camera + hardware H.264. 400/500 if the AI tab holds the sensor or no camera is attached |
| POST | `/api/cam/stop` | Stop the pipeline |
| GET | `/api/cam/status` | `running, width, height, fps, frames, bitrate, sensor` |
| GET | `:81/h264` | The encoded stream: repeated `[u32 LE length][u32 LE pts ms][Annex-B access unit]`. One client at a time. Sends `Access-Control-Allow-Origin: *` — the dashboard fetches it cross-origin (port 81 vs 80) |

See [camera.md](camera.md) for the stream format and the browser-side
fMP4/MSE player.

## Frontend

[main/web/](../main/web) — no framework, no external resources, everything
embedded:

| File | Role |
|---|---|
| `setup.html` / `setup.js` | Provisioning UI (scan list, join form, progress) |
| `dashboard.html` / `dashboard.js` | The five-tab dashboard |
| `style.css` | Shared styling for both |

Dashboard tabs: **Home** (placeholder) · **Audio** (record/stop, level
result, inline playback) · **Camera** (H.264 live view) · **AI** (MJPEG +
detection overlay + class list) · **Files** (SD browser with web/board
playback and download) · **Settings** (network, SD, about, forget-network).
The active tab survives reload via the URL hash. Leaving a video tab
detaches the stream but leaves the pipeline running; stopping is explicit.

Notable client behaviors:

- While recording, the elapsed timer runs locally and the board is polled
  only every 10 s — every HTTP request is a radio burst that can couple into
  the microphone's analog path.
- If the page loads while a recording is already running (reload, second
  device), the UI resumes the session with the Stop button restored.
- The H.264 player is described in [camera.md](camera.md).
