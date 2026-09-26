# Off-Grid Power Dashboard — EEZ Studio + LVGL 9 on the ESP32-P4

A battery / solar / tanks dashboard for the **Waveshare ESP32-P4-WIFI6-Touch-LCD-7B**
(7" 1024x600 IPS, GT911 touch), built in **EEZ Studio 0.29** against **LVGL 9.2.2**
and running on **ESP-IDF 5.5**.

The same C code also builds and runs in EEZ Studio's full simulator, so the
logic can be exercised on a laptop before the board is plugged in.

![screen layout](#) <!-- add a bench photo / simulator capture here -->

MIT licensed. Demonstration firmware — it reads a battery bank and switches
nothing.

## What this sample is for

It exercises the LVGL 9 and EEZ Studio features that did not exist when the
earlier EEZ Studio samples on this channel were made:

| Feature | Added in | Where to look |
|---|---|---|
| Full simulator via Docker | 0.26 | `main/UI/dash_data.c`, the one `EEZ_LVGL_SIMULATOR` block |
| Grid layout, `FR()` and `CONTENT` tracks | 0.26 | screen root and `body` local styles |
| Scale widget with sections | 0.26 (LVGL 9 only) | `batt_scale`, `solar_strip` |
| Arc start/end angle from an expression | 0.26 | `solar_arc`, `useAngle: true` |
| Spangroup | 0.27 | every value readout |
| Merged fonts with unicode ranges | 0.27 | `dashicons16` / `dashicons22` |
| Font definition caching | 0.27 | `settings.general.cacheFonts` |
| Static label text | 0.28 | every fixed label, `useStaticText: true` |

## Layout

Nothing is placed by pixel. The screen is three grid rows, the body is three
`FR(1)` columns, and each card is a flex column, so changing
`displayWidth` / `displayHeight` in the project settings reflows the whole
dashboard instead of stranding widgets off-screen.

```
screen 1024x600            GRID  cols: FR(1)   rows: CONTENT / FR(1) / CONTENT
├── header    64 px        FLEX row     bolt icon, POWER, link state, theme toggle
├── body      496 px       GRID  cols: FR(1) FR(1) FR(1)   rows: FR(1)
│   ├── card_battery       FLEX column  Scale ring + concentric Arc + 2 Spangroups
│   ├── card_solar         FLEX column  Arc (angle from expression) + Scale strip
│   └── card_tanks         FLEX column  2 Bars + 2 Spangroups
└── footer    40 px        FLEX row     source, uptime, versions
```

The battery gauge is the clearest LVGL 9 story. Version 9 dropped the Meter
widget and replaced it with **Scale**, which draws the graduated ring, the
labels and the coloured **sections** — below 11.8 V red, to 12.4 V amber, above
that the accent colour — all configured in the editor with no C. Scale has no
needle property, so a concentric **Arc** over the same range supplies the moving
indicator. Range is in decivolts (100 = 10.0 V) because Scale is integer-only.

The solar arc uses the other new trick: `useAngle: true` with **End angle** bound
to an expression, so C computes the sweep from watts rather than feeding a value
into a fixed range.

## Data flow

`flowSupport` is off, so every global variable in the project becomes a
`get_var_*` / `set_var_*` pair that C implements, and EEZ Studio's generated
`tick_screen()` calls the getters each frame.

```
                 board                          simulator
                 -----                          ---------
  main.c demo task / real transport      dash_data.c sim_feed()
                 |                                |
                 v                                v
            dash_data_set_battery() / _solar() / _tanks() / _link()
                                  |
                          dash_data_tick()   formats strings, derives
                                  |          decivolts and arc angle
                                  v
                            dash_view_t  (static)
                                  |
                          vars.c get_var_*()   <- called by tick_screen()
                                  |
                                  v
                    Scale / Arc / Bar / Spangroup / Label
```

`vars.c` also drives the model: whichever getter LVGL reaches first in a frame
advances `dash_data_tick()`, throttled to 10 Hz. That is deliberate — the
simulator's `main()` belongs to the eez-open simulator repo and only calls
`ui_init()` and `ui_tick()`, so nothing else would pump it there.

## Repository layout

```
CMakeLists.txt              ESP-IDF project
sdkconfig.defaults          P4 + PSRAM + LVGL 9 configuration (edit this, not sdkconfig)
partitions.csv
GUI/
  TrailCurrentPowerDash.eez-project    source of truth for the screen
  ASSETS/                              Roboto, RobotoMono, Font Awesome
  tmp/gen_eez_project.py               generator (gitignored working state)
main/
  main.c                    BSP bring-up, board data source
  idf_component.yml         component pins
  CMakeLists.txt
  UI/                       EEZ Studio export destination
    screens.* ui.* styles.* fonts.h actions.h vars.h images.* ui_font_*.c
                            ^ generated — never hand-edit
    dash_data.c/.h          data model + simulated feed   (hand-written)
    vars.c                  get_var_* / set_var_*          (hand-written)
    actions.c               action_toggle_theme            (hand-written)
DOCS/
  BUILD.md                  how to build both targets, and the traps
  SCHEMA_NOTES.md           the LVGL 9 widget JSON, from EEZ Studio's own source
```

The hand-written C sits in `main/UI/` because the simulator copies only the
export folder into its build container and links with
`-sLLD_REPORT_UNDEFINED`. Anything the UI references has to be in there.
[BUILD.md](DOCS/BUILD.md#why-the-c-lives-in-mainui) has the detail.

## Quick start

```bash
. $IDF_PATH/export.sh
idf.py set-target esp32p4        # required once — the BSP is esp32p4-only
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

The export is committed, so this works without opening EEZ Studio. The board
shows moving gauges immediately: `main.c` ships with `DASH_DEMO_SOURCE 1`, a
synthetic feed. Set it to `0` and wire a real transport into
`dash_source_start()` — on this rig the readings arrive over MQTT.

To change the screen: edit the project in EEZ Studio, press **Ctrl+B**, rebuild.

Full instructions, the simulator, and the version pins that matter:
**[DOCS/BUILD.md](DOCS/BUILD.md)**.

## Things that cost time

Each of these is written up in [BUILD.md](DOCS/BUILD.md#known-issues):

- **`esp_lvgl_port` resolves too new.** The BSP asks for `^2`, which picks 2.9.0,
  which needs LVGL 9.3+ symbols and fails to compile against 9.2.2 — with errors
  inside the component that look like a broken BSP. Pinned to `~2.7.2`.
- **The LVGL version string is a contract.** EEZ Studio emits different
  Spangroup API calls at 9.2.2 than at 9.3+, so
  `settings.general.lvglVersion` and the `lvgl/lvgl` pin have to match.
- **Two Kconfig options were renamed in LVGL 9**, and the old names are silently
  ignored. `LV_INDEV_DEF_READ_PERIOD` no longer exists at all, which matters
  here because this board leaves the GT911 interrupt line unconnected and LVGL
  polls for touch.
- **EEZ Studio's headless `--build-project` silently emits no custom fonts**, and
  reports success. Interactive Ctrl+B is fine.

## Licence

MIT — see [LICENSE](LICENSE). The bundled fonts keep their own licences:
Roboto and Roboto Mono are Apache 2.0, Font Awesome Free is OFL 1.1 / CC BY 4.0.
