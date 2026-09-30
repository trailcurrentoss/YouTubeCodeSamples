# GUI (EEZ Studio)

The screens are authored in **EEZ Studio 0.29** as an LVGL 9.2.2 project:
[`GUI/RotaryHid.eez-project`](../GUI/RotaryHid.eez-project). It is the
single source of truth for how the pad looks.

## Screens

| Page | Shown when | Contents |
|---|---|---|
| `PageIdle` | no supported app focused, or no companion | analog clock (tick ring, numerals, hour/minute/second hands, centre cap) with "No supported app", "Focus a supported app to load its keys." and the USB status pill below the centre |
| `PageFreecad` | FreeCAD focused | 8 keys, FreeCAD logo, "Rotate View - Z", angle, X/Y/Z chips |
| `PageBlender` | Blender focused | 8 keys, Blender logo, "Rotate Object - Z", angle, X/Y/Z chips |
| `PageKdenlive` | Kdenlive focused | 8 keys, Kdenlive logo, timecode, scrub strip, step pill (the centre is a button) |
| `PageVscodium` | VSCodium or VS Code focused | 8 keys, VSCodium logo, "Scroll - Push for Page", scroll icon, step pill |
| `PageGimp` | GIMP focused | 8 keys, GIMP logo, "Zoom - Push to Fit", zoom icon |
| `PageInkscape` | Inkscape focused | 8 keys, Inkscape logo, "Zoom - Push to Fit", zoom icon |
| `PageLibreoffice` | LibreOffice (Writer, Calc, Impress) focused | 8 keys, LibreOffice logo, "Scroll - Push for Page", scroll icon, step pill |
| `PageFirefox` | Firefox focused | 8 keys, Firefox logo, "Scroll - Push for Page", scroll icon, step pill |
| `PageChromium` | Chromium or Google Chrome focused | 8 keys, Chromium logo, "Scroll - Push for Page", scroll icon, step pill |

Keys are 84 px circles on a 180 px radius, numbered 0–7 clockwise from 12
o'clock (`fc_key0`…`fc_key7`, `bl_key…`, `kd_key…`, `vs_key…`, `gp_key…`, `ik_key…`, `lo_key…`, `ff_key…`, `cr_key…`). Every key fires the
`KeyTap` action with its index as `userData`; the chord it sends is in
`main/app_model.c`. See [apps](apps.md) for the full tables.

### The idle-page clock

The clock is a Scale widget (`idle_clock`, 460 px, `ROUND_INNER`, 61 ticks,
a major tick every 5 with the numerals 12, 1 … 11) whose three Line
children are **scale needles**. EEZ Studio generates the code that points
each needle at its bound variable, so the hands move without any C
touching their geometry. The scale runs 0–3600 per revolution rather than
0–60, so the hour hand moves smoothly instead of jumping every 12 minutes.

The pad has no clock of its own. The companion sends the time when it
connects and once a minute (`main/clock.c` counts in between). Until the
first time arrives — the companion has not run since power-up — the hands
are hidden, so the dial never shows a wrong time.

## Variables and actions

| Name | Kind | Implemented in |
|---|---|---|
| `UsbStatusText` | string variable | `vars.c` → `ui_glue.c` |
| `AxisModeText`, `AngleText`, `TimecodeText`, `StepText`, `ScrollStepText` | string variables | `vars.c` → `app_model.c` |
| `ClockHour`, `ClockMinute`, `ClockSecond` | integer variables, 0–3600 per revolution | `vars.c` → `clock.c` |
| `KeyTap` | action (`userData` = key index) | `actions.c` |
| `ToggleStep` | action | `actions.c` |

Flow support is off: variables and actions are plain C functions EEZ
Studio declares and the firmware implements. `ui_tick()` (driven by a 50 ms
timer in `main.c`) reads the variables and updates bound labels.

## Colours and themes

Every colour is a **named token**; no style or widget uses a hex value. The
tokens and the two themes (**Default** = light, **Dark**) are the TrailCurrent
palette shared with the Capstan project for the same device. The firmware
starts in **Dark**, matching the design. To change a colour, change the
token's value per theme in EEZ Studio's Colors panel — never type a hex
into a style.

## Fonts and images

| Font | Source | Used for |
|---|---|---|
| `fa24`, `fa48` | Font Awesome 7.2 Free Solid (`GUI/ASSETS/fa-solid-900.otf`) | key icons; scroll and zoom icons — only the glyphs used are embedded |
| `mono10`, `mono26` | Roboto Mono Medium (`mono10` also merges Font Awesome ← → as a second source, for the browsers' Back / Forward labels) | key shortcuts, timecode |
| `num40` | Roboto Regular | the angle (digits, `+`, `-`, `°`) |
| `MONTSERRAT_12/14/22` | LVGL built-ins | everything else — enabled in `sdkconfig.defaults` |

The app logos (`GUI/ASSETS/logo_*.png`, 52×52) are rendered from the
projects' official SVG logos (`logo_*.svg`) and remain their owners'
trademarks. The VSCodium logo is from the VSCodium repository (MIT). The GIMP
and Inkscape logos are the icons their own packages install
(`logo_gimp_source.png`, `logo_inkscape.svg`), and the LibreOffice logo is
the suite icon its package installs (`logo_libreoffice.svg`). The Firefox and
Chromium logos are the icons their snaps install (`logo_firefox_source.png`,
`logo_chromium_source.png`).

## Changing the GUI

1. Open `GUI/RotaryHid.eez-project` in EEZ Studio and edit.
2. **Build (Ctrl+B).** It exports to `main/ui/`.
3. `idf.py build`.

If the `.eez-project` was changed by something else while EEZ Studio had it
open, **File → Close Project and reopen before Ctrl+B** — EEZ Studio does
not reload a file changed underneath it, and would export its stale
in-memory copy.

`main/ui/` is **entirely generated and disposable**: delete it at any time
and re-export. Nothing hand-written lives there. Without it the project
still builds, as a placeholder firmware whose screen asks for an export —
`main/CMakeLists.txt` detects the export and passes `APP_HAVE_UI` to the
compiler.

Rules the C side follows, and anyone extending it should too:

- C may set **state** (`LV_STATE_CHECKED` on the axis chips and the USB
  dot), **flags**, **content**, and **load screens**.
- C must never set the **position, size, alignment or font** of an
  EEZ-authored widget. Put it in the project, or the EEZ Studio canvas and
  the device will disagree.
- Adding a key? Keep the label order in the project and the chord order in
  `app_model.c` identical.
- Any new Montserrat size needs its `CONFIG_LV_FONT_MONTSERRAT_<n>=y` in
  `sdkconfig.defaults`.
- Any new Font Awesome icon needs its codepoint added to the font's
  glyph range, or it renders as an empty box.
