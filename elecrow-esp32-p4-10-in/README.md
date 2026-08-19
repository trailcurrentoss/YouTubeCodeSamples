# TrailCurrent Headwaters Map Viewer — Elecrow ESP32-P4 10.1"

Turnkey firmware for the Elecrow **CrowPanel Advance 10.1" ESP32-P4 HMI AI**
display (1024×600, EK79007 LCD + GT911 touch, ESP32-P4 SoC with 16 MB PSRAM +
ESP32-C6 for WiFi) that walks the user through:

1. **WiFi setup** — scan-and-pick, WPA2 password entry with on-screen keyboard.
2. **Token auth** — paste a Bearer token, verified against a
   [TrailCurrent Headwaters](../../Product/TrailCurrentHeadwaters) gateway on
   the LAN via `GET /api/auth/check`.
3. **Map view** — 4×2 raster tile grid fetched from Headwaters and painted
   on-screen; pan/zoom via the on-screen controls.

Credentials are persisted in NVS, so a rebooted device auto-connects and
jumps straight to the map view.

## Repo layout

```
CMakeLists.txt              # top-level ESP-IDF project
partitions.csv              # NVS + factory app + spiffs
sdkconfig.defaults          # PSRAM, LVGL fonts, TLS insecure for LAN
main/
  CMakeLists.txt
  idf_component.yml
  main.c                    # BSP init + LVGL + boot flow
  app_state.h/.c            # screen state + NVS wrappers
  wifi_task.h/.c            # scan + connect
  auth_task.h/.c            # token verification against Headwaters
  tile_task.h/.c            # HTTP tile fetch
  actions.c                 # LVGL action handlers (referenced by ui/actions.h)
  vars.c                    # placeholder for future EEZ vars
  ui/                       # EEZ Studio export target (auto-generated — don't edit)
GUI/
  ASSETS/                   # fonts referenced by the .eez-project
  elecrow-esp32-p4-10-in/
    elecrow-esp32-p4-10-in.eez-project   # source of truth for the GUI
tmp/
  add_screens.py            # generator script that produced the pages
```

## Build & flash

Prerequisites:

- ESP-IDF v5.3 or newer, targeting `esp32p4`.
- [EEZ Studio](https://github.com/eez-open/studio) — needed to export the
  UI to C. First-build users **must** run this before `idf.py build`.

Steps:

```bash
# 1. Export the UI from EEZ Studio.
#    Open GUI/elecrow-esp32-p4-10-in/elecrow-esp32-p4-10-in.eez-project
#    then File → Build (Ctrl+B). Output lands in main/ui/.

# 2. Build + flash + monitor.
idf.py set-target esp32p4
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Before EEZ Studio's export exists, `main/*.c` compile through `__has_include`
guards but the display shows a blank screen. That's expected — the guards
exist so the C code doesn't rot while the UI is in flux.

## First-run flow

1. Power on. The device shows **WiFi Setup**. Wait for the scan spinner to
   populate up to 8 nearby networks.
2. Tap the network you want. If it's open, the connection kicks off
   immediately; otherwise a keyboard overlay asks for the WPA2 password.
3. On successful DHCP lease the device saves the SSID + PSK to NVS and
   loads **Token Setup**.
4. Paste the Bearer token issued by Headwaters (`POST /api/auth/login`
   returns a session token — copy it from the PWA's login flow or issue an
   API key from **Settings → API Keys**).
5. Tap **Authenticate**. `GET /api/auth/check` runs; a 200 saves the URL +
   token and loads **Map**.
6. On subsequent boots the device skips straight to Map. Tap **Sign out**
   in the map toolbar to wipe credentials and start over.

## Map tile source

The device fetches raster tiles from
`<host>/api/tiles/{z}/{x}/{y}.png` with `Authorization: Bearer <token>`.

Headwaters' shipped nginx currently only serves the **PMTiles vector
archive** at `/maps/tiles.pmtiles`. The ESP32-P4 can't reasonably render
vector tiles, so a companion "raster tile proxy" needs to expose the endpoint
above on the Headwaters device. See
[PLANS/HeadwatersRasterProxy.md](PLANS/HeadwatersRasterProxy.md) for the
proposed sidecar.

Until that proxy exists, `tile_task.c` falls back to
`https://tile.openstreetmap.org/{z}/{x}/{y}.png` (requires the WiFi network
to have outbound Internet). PNG decode itself is stubbed pending the
`lv_lib_png` component being registered in `idf_component.yml` — the HTTP
path is verified end-to-end, but the tile grid stays at its placeholder
color until decode is turned on. That's a follow-up PR, not a blocker for
the WiFi + auth flow this project scaffolds.

## GUI editing

The `.eez-project` is the single source of truth for every screen. Rules
(from `~/.claude/skills/eezstudio`):

- Never hand-edit `main/ui/*` — EEZ Studio overwrites those on export.
- Never call `lv_obj_set_pos`, `lv_obj_set_size`, `lv_obj_set_align`,
  `lv_obj_move_foreground`, or any style-* geometry override on an
  `objects.<widget>` from C. Widget geometry lives in the JSON. The
  gate at `~/.claude/skills/eezstudio/verify_no_canvas_divergence.py`
  enforces this and must exit 0 before any build.
- Colors reference named tokens from the project palette; no hex literals
  in styles. If a new color is needed, add it via EEZ Studio's Colors
  panel first, then reference the name in the widget/style.
- Runtime-overridden widgets (keyboard, list, button matrix, dropdown,
  roller, tabview) need `align: TOP_LEFT` + matching `min_*/max_*` in
  their `localStyles.MAIN.DEFAULT` — otherwise LVGL's internal layout
  runs at render time and the widget ends up at LVGL defaults instead of
  its authored coords. The generator (`tmp/add_screens.py`) does this
  automatically for the keyboards on the WiFi + Token pages.

## Known limits

- **PNG decode not enabled.** `lv_lib_png` isn't in `idf_component.yml`
  yet, so tile fetches log status but the tile grid stays at its
  placeholder color. Enable it with `CONFIG_LV_USE_PNG=y` and add the
  decoder init to `main.c`.
- **Headwaters raster endpoint doesn't exist yet.** Falls back to public
  OSM until the proxy sidecar lands.
- **URL is hard-coded** to `https://headwaters.local` in `actions.c`
  (`action_TokenSubmit`). If your gateway is at a bare IP or a
  different mDNS name, patch the string or extend the token screen with
  a second textarea.
- **TLS is insecure by design** for LAN use. `CONFIG_ESP_TLS_INSECURE=y` +
  `CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY=y` are set because Headwaters
  ships with a self-signed cert. Do not adapt this pattern for
  Internet-facing hosts.
