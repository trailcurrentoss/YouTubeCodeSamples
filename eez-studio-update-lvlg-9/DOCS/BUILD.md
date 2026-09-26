# Building the Power Dashboard

Two targets, one GUI. The `.eez-project` is the source of truth for the screen;
`main/UI/` is where EEZ Studio exports it, and the same folder is what the
simulator compiles.

- [Prerequisites](#prerequisites)
- [1. Export the GUI](#1-export-the-gui)
- [2. Build for the board](#2-build-for-the-board)
- [3. Build in the full simulator](#3-build-in-the-full-simulator)
- [Regenerating the project file](#regenerating-the-project-file)
- [Known issues](#known-issues)

## Prerequisites

| Tool | Version used | Notes |
|---|---|---|
| ESP-IDF | 5.5.2 | 5.3 or newer per `main/idf_component.yml` |
| EEZ Studio | 0.29.0 | the LVGL 9 widgets need 0.26+; Scale needs a 9.x project |
| Docker | 29.8.1 | only for the full simulator |
| Python 3 | 3.12 | only to regenerate the `.eez-project` |

Component versions are resolved by the IDF component manager and pinned in
`main/idf_component.yml`:

| Component | Pin | Resolved |
|---|---|---|
| `waveshare/esp32_p4_wifi6_touch_lcd_7b` | `^1.0.2` | 1.0.4 |
| `lvgl/lvgl` | `~9.2.2` | 9.2.2 |
| `espressif/esp_lvgl_port` | `~2.7.2` | 2.7.2 |
| `espressif/esp_lcd_ek79007` | via BSP | 1.0.4 |
| `espressif/esp_lcd_touch_gt911` | via BSP | 1.2.x |

Three of those pins are load-bearing and are explained in
[Known issues](#known-issues): the LVGL version, the `esp_lvgl_port` version,
and the fact that the LVGL version has to match
`settings.general.lvglVersion` in the `.eez-project`.

## 1. Export the GUI

Open `GUI/TrailCurrentPowerDash.eez-project` in EEZ Studio and press **Ctrl+B**
(Build). It writes into `main/UI/`:

```
screens.c  screens.h     the objects struct and create_screen_* functions
ui.c       ui.h          ui_init() / ui_tick()
styles.c   styles.h      the 31 named styles, both themes
actions.h                extern void action_toggle_theme(lv_event_t *)
vars.h                   extern get_var_* / set_var_* for the 13 variables
fonts.h                  extern the four custom fonts
images.c   images.h      (no bitmaps in this project)
structs.h                flow-only, empty here
ui_font_dashicons16.c    merged Roboto + Font Awesome, 16 px
ui_font_dashicons22.c    merged Roboto + Font Awesome, 22 px
ui_font_mono26.c         RobotoMono 26 px
ui_font_mono44.c         RobotoMono 44 px
```

**Never hand-edit those.** The next export overwrites them.

Three files in the same folder are *not* generated and are safe to edit:

| File | What it is |
|---|---|
| `main/UI/dash_data.c` / `.h` | the data model, plus the simulated feed |
| `main/UI/vars.c` | implements the `get_var_*` / `set_var_*` accessors |
| `main/UI/actions.c` | implements `action_toggle_theme` |

They live in `main/UI/` rather than `main/` because of how the simulator
works — see [step 3](#3-build-in-the-full-simulator). `main/CMakeLists.txt`
globs the generated files by name and lists these three explicitly, so an
export that adds a new font or bitmap needs no CMake change.

The export is committed, so a fresh clone builds without opening EEZ Studio at
all.

## 2. Build for the board

```bash
. $IDF_PATH/export.sh
idf.py set-target esp32p4      # required once, before the first build
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

`set-target esp32p4` is not optional. The Waveshare BSP declares
`targets: [esp32p4]` in its own manifest, so on any other target the dependency
solver fails with *"no versions of waveshare/esp32_p4_wifi6_touch_lcd_7b match
^1.0.2"* — which reads like a registry problem and is not one.

The board shows moving gauges straight away: `main.c` defaults to
`DASH_DEMO_SOURCE 1`, a synthetic feed on the same periods the simulator uses.
Set it to `0` and wire a real transport into `dash_source_start()`.

### Changing anything in `sdkconfig.defaults`

`sdkconfig` is generated and gitignored. `sdkconfig.defaults` only *seeds* it —
ESP-IDF does not re-apply the defaults to an `sdkconfig` that already exists, so
editing the defaults has no effect on an existing tree. Regenerate:

```bash
rm sdkconfig && idf.py build
grep CONFIG_THE_FLAG_YOU_CHANGED sdkconfig    # confirm it actually landed
```

A flag that reads `=y` in the defaults but `# ... is not set` in `sdkconfig`
means the regeneration did not happen.

## 3. Build in the full simulator

EEZ Studio 0.26 added a simulator that compiles the project's **own C code** to
WebAssembly in a Docker container and runs it in a panel in the editor, so
actions and variables execute for real instead of being stubbed.

In EEZ Studio: make sure Docker is running, then use the simulator's **Build**
button. First run pulls the Emscripten image and clones LVGL, so it takes a
while; after that it is incremental. **Clean Build** wipes `/project/build`,
**Clean All** wipes the whole Docker volume.

### Why the C lives in `main/UI/`

This is the part worth understanding before moving files around.

The simulator does not compile the project. It copies **only the export
destination folder**:

```
docker cp <destinationFolder>/. <container>:/project/src/
```

and the simulator's CMakeLists.txt then does:

```cmake
file(GLOB_RECURSE my_src ./src/*.cpp ./src/*.c)
```

with `-sLLD_REPORT_UNDEFINED` at link time. So every symbol the generated UI
references has to be inside that one folder. `main/main.c` is **not** copied —
the simulator brings its own `main()` that calls `ui_init()` and `ui_tick()`.

If `action_toggle_theme()` and the `get_var_*()` functions lived in `main/`, the
simulator would fail to link on every one of them. Putting them in `main/UI/`
is what makes the same C run in both places. It also means `dash_data.c` has to
avoid ESP-IDF headers entirely, which is why the model is plain C and the
board-specific code stays in `main/main.c`.

### The one `#ifdef`

The simulator build defines `EEZ_LVGL_SIMULATOR`. `dash_data.c` uses it in
exactly one place:

```c
#ifdef EEZ_LVGL_SIMULATOR
    sim_feed(now_ms);       /* sweep every gauge through its range */
#endif
```

On the board the same function instead uses whatever `dash_data_set_*()` last
received. Nothing else in the project is conditional.

### What the simulator will not tell you

It runs the logic, not the hardware. It knows nothing about the EK79007 panel,
the GT911 touch controller, PSRAM bandwidth or how long a full-screen repaint
takes on a P4. Flash to check those.

### Driving it from the command line

The editor's Build button is the normal route. The same steps by hand, which is
also what to reach for when a build fails and you want the raw output:

```bash
export PROJECT_VOLUME=eez-studio-lvgl-build
DB=<eez-studio-resources>/docker-build          # ships with EEZ Studio

docker compose -f $DB/docker-compose.yml --project-directory $DB build

CID=$(docker compose -f $DB/docker-compose.yml --project-directory $DB \
        run -d emscripten-build sleep infinity)

docker exec $CID sh -c 'cd /project && git clone --recursive \
  https://github.com/eez-open/lvgl-simulator-for-studio-docker-build .'

docker exec $CID sh -c 'rm -rf /project/src && mkdir -p /project/src'
docker cp main/UI/. $CID:/project/src/
docker stop $CID

docker compose -f $DB/docker-compose.yml --project-directory $DB \
  run --rm emscripten-build sh -c \
  './build.sh --lvgl=9.2.2 --display-width=1024 --display-height=600'
```

The result is `index.html` / `.js` / `.wasm` in `/project/build` inside the
volume. EEZ Studio extracts those to `.docker-build-output/` next to the
project file (gitignored) and serves them in the preview panel.

Verified output for this project at 1024x600:

```
index.html      848 bytes
index.js     177,860 bytes
index.wasm 2,039,358 bytes
```

One thing to watch if you clone the simulator repo by hand: **do not use
`--depth 1`.** `build.sh` does `git checkout v9.2.2` inside the `lvgl`
submodule, and a shallow clone has no tags, so it fails with

```
error: pathspec 'v9.2.2' did not match any file(s) known to git
```

and then builds against whatever the submodule's default branch happens to be —
which is a different LVGL than the one the project was exported for. EEZ Studio's
own setup does a full `git clone --recursive`, which is why it does not hit this.

## Regenerating the project file

`GUI/TrailCurrentPowerDash.eez-project` was produced by
`GUI/tmp/gen_eez_project.py`, which keeps the layout arithmetic and the style
table in one reviewable place:

```bash
python3 GUI/tmp/gen_eez_project.py
```

It backs up the existing file, prints a layout worksheet, and refuses to write
if any of its own audits fail — screen-widget key set, dangling style/colour/
font/action/variable references, hex colours in styles, theme-contrast, icon
glyph coverage, non-ASCII label text.

`GUI/tmp/` is gitignored: it is working state, not part of the sample. Once the
project file exists, ordinary edits belong in EEZ Studio.

Independent checks, from the `eezstudio` skill:

```bash
python3 ~/.claude/skills/eezstudio/verify_project.py GUI/TrailCurrentPowerDash.eez-project
python3 ~/.claude/skills/eezstudio/verify_no_canvas_divergence.py .
```

## Known issues

### `esp_lvgl_port` resolves too new for LVGL 9.2.2

The Waveshare BSP asks for `espressif/esp_lvgl_port: ^2`, which resolves to the
newest 2.x — 2.9.0 at the time of writing. 2.9.0 is written against LVGL 9.3+
and references `LV_COLOR_FORMAT_RGB565_SWAPPED` and the DPI panel callback
`on_frame_buf_complete`, neither of which exists in 9.2.2. Unpinned, the build
fails **inside the component**:

```
esp_lvgl_port_disp.c:160: error: 'esp_lcd_dpi_panel_event_callbacks_t'
    has no member named 'on_frame_buf_complete'
esp_lvgl_port_disp.c:301: error: 'LV_COLOR_FORMAT_RGB565_SWAPPED' undeclared
```

which looks like a broken BSP or a broken IDF and is neither. `main/idf_component.yml`
pins `~2.7.2`. Bump that and the LVGL pin together, never one alone.

### The LVGL version string is a contract, not a preference

EEZ Studio switches the API it emits on `settings.general.lvglVersion`. At
`9.2.2` the Spangroup code generator emits `lv_spangroup_new_span()`,
`lv_spangroup_refr_mode()` and `lv_spangroup_set_lines()`; from `9.3.0` it emits
`lv_spangroup_add_span()`, `lv_spangroup_refresh()` and
`lv_spangroup_set_max_lines()`. Widget flag bits and style-property codes also
move between 9.0, 9.3 and 9.5.

So `settings.general.lvglVersion` and the `lvgl/lvgl` pin have to agree, or
`screens.c` will not compile against the library it was exported for. `9.2.2`
is also the version the simulator container checks out for a 9.2.2 project, so
device and simulator run the same LVGL.

### LVGL 8 → 9 Kconfig renames

Two settings changed name, and an 8.x name in `sdkconfig.defaults` is silently
ignored apart from an `unknown kconfig symbol` warning:

| LVGL 8.x | LVGL 9.x |
|---|---|
| `CONFIG_LV_MEM_CUSTOM=y` | `CONFIG_LV_USE_CLIB_MALLOC=y` |
| `CONFIG_LV_DISP_DEF_REFR_PERIOD` | `CONFIG_LV_DEF_REFR_PERIOD` |
| `CONFIG_LV_INDEV_DEF_READ_PERIOD` | folded into `CONFIG_LV_DEF_REFR_PERIOD` |

The third one matters on this board. LVGL 9 has a single period for "display
refresh, input device read and animation step", so the touch polling rate is no
longer tunable on its own. The Waveshare 7B leaves the GT911 INT line
unconnected, which puts LVGL in polling mode for touch, and a slow cadence drops
quick taps — hence `CONFIG_LV_DEF_REFR_PERIOD=10` rather than 16.

### Backlight PWM does not work on this board: GPIO 32 is a reserved MSPI pin

Every boot logs:

```
W (1067) ledc: GPIO 32 is not usable, maybe conflict with others
I (1269) ESP32_P4_EV: Setting LCD backlight: 100%
```

The panel lights up and the dashboard is perfectly visible, so this is easy to
skim past — but PWM dimming is silently inert.

The BSP defines `BSP_LCD_BACKLIGHT (GPIO_NUM_32)` and
`bsp_display_brightness_init()` sets up an LEDC channel on it. LEDC calls
`esp_gpio_reserve()` first and finds GPIO 32 already reserved, so it refuses to
route its output there and warns. The reservation comes from
`esp_mspi_pin_reserve()` in `spi_flash/flash_ops.c`, which reserves the whole
flash + PSRAM pin set at startup — and with `CONFIG_SPIRAM_MODE_HEX` (16 data
lines, required for the 32 MB PSRAM this board carries) GPIO 32 falls inside it.

Consequences:

- `bsp_display_backlight_on()` / `_off()` and `bsp_display_brightness_set()`
  do not change anything. The backlight sits at whatever the hardware default
  is, which on this board is on.
- Nothing in this project is affected — the dashboard has no brightness control,
  and the screen being permanently lit is what you want on a bench.
- If you add a brightness slider, it will appear to do nothing. There is no fix
  from software: the pin belongs to the memory interface in this PSRAM mode.

Worth re-checking on any other project that drives this board's backlight and
also sets `CONFIG_SPIRAM_MODE_HEX` — the same pin and the same PSRAM mode means
the same inert PWM.

### Panel orientation is 180 degrees

The Waveshare 7B scans out upside-down relative to the way the board is normally
mounted, so `main.c` calls

```c
bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);
```

Confirmed on the bench: at `LV_DISPLAY_ROTATION_0` the dashboard renders
inverted. `sw_rotate = true` in `bsp_display_cfg_t` is the prerequisite — without
it the call does nothing. If you are bench-testing a bare board with the ribbon
at the bottom, use `LV_DISPLAY_ROTATION_0` instead.

### EEZ Studio's headless `--build-project` emits no custom fonts

`EEZ Studio --build-project <file>` runs an export and exits, which is useful in
a script. On 0.29.0 it does not emit `ui_font_*.c` for any custom font, and it
reports success while doing it:

```
No error and no warning detected
File ".../main/UI/fonts.h" built          <- declares ui_font_dashicons22
File ".../main/UI/styles.c" built          <- references &ui_font_dashicons22
                                           <- no ui_font_dashicons22.c
```

The failure only shows up later, as `undefined reference to ui_font_dashicons22`
at link time.

Cause: font definitions are built by `Font.buildLvglFontDefinition()`, whose
first act is a cache lookup,

```js
this.fontsCacheStore.getCachedFontDefinition(params)
```

and the headless path constructs a `ProjectStore` without a `fontsCacheStore`.
The lookup throws a `TypeError`, the surrounding `catch` discards it, the font's
definition stays undefined, and `copyFontFiles()` skips any font whose
definition is empty — so nothing is written and nothing is logged.

Interactive **Ctrl+B** in the editor is unaffected: that store has a
`fontsCacheStore`. The exported font sources are committed here, so this only
bites if you script the export. If you do, export from the GUI once afterwards,
or check that `main/UI/` has one `ui_font_*.c` per font in the project.
