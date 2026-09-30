# Building and flashing

## Requirements

- **ESP-IDF 5.5** (built and tested with 5.5.2). Any install method works;
  the component manager fetches everything else (LVGL 9.2.2, esp_lvgl_port,
  panel and touch drivers, TinyUSB) on the first build.
- **EEZ Studio 0.29** only if you change the GUI — the generated UI is
  committed, so a plain clone builds without it. See [GUI](gui.md).

## First build

```bash
. $IDF_PATH/export.sh            # or open an ESP-IDF terminal in your IDE
idf.py set-target esp32s3        # once per clone
idf.py build
```

`sdkconfig.defaults` sets `CONFIG_IDF_TARGET="esp32s3"`, but ESP-IDF gives
an `IDF_TARGET` in the **environment** priority over it. If your terminal
exports a different target (see [IDE notes](#ide-notes-vs-code--vscodium)),
the build configures for the wrong chip and dependency solving fails with
"no versions of espressif/esp_tinyusb match". `set-target` fixes it.

## Configuration

**Never edit `sdkconfig`.** It is generated and gitignored. All settings
live in the tracked `sdkconfig.defaults`, which only seeds a *new*
`sdkconfig` — after changing it, regenerate:

```bash
rm sdkconfig && idf.py build
```

Project options (`idf.py menuconfig`):

| Menu | Option | Default | Meaning |
|---|---|---|---|
| Rotary Macro Pad | `ROTARY_STEP_DEG` | 5 | Degrees per ring detent in FreeCAD and Blender |
| Rotary Macro Pad | `ROTARY_KDENLIVE_FRAMES_PER_DETENT` | 5 | Frames scrubbed per ring click in Kdenlive's frame mode |
| Rotary Macro Pad | `ROTARY_KDENLIVE_FPS` | 30 | Frame rate used for the pad's timecode display |
| Rotary Macro Pad | `ROTARY_VSCODIUM_LINES_PER_DETENT` | 3 | Lines scrolled per ring click in VSCodium |
| Rotary Macro Pad | `ROTARY_LIBREOFFICE_LINES_PER_DETENT` | 3 | Lines moved per ring click in LibreOffice |
| Rotary Macro Pad | `ROTARY_BROWSER_LINES_PER_DETENT` | 3 | Scroll steps per ring click in Firefox and Chromium |
| MaTouch 2.1in board | `MATOUCH_LCD_SWAP_RB` | y | Red/blue swap (verified) |
| MaTouch 2.1in board | `MATOUCH_TOUCH_MIRROR_X/Y`, `SWAP_XY` | n | Touch orientation |
| MaTouch 2.1in board | `MATOUCH_ENCODER_STEPS_PER_DETENT` | 4 | Quadrature counts per click |
| MaTouch 2.1in board | `MATOUCH_ENCODER_INVERT` | y | Ring direction (verified) |
| MaTouch 2.1in board | `MATOUCH_ENCODER_DEBUG` | n | Log raw ring counts |
| MaTouch 2.1in board | `MATOUCH_LONG_PRESS_MS` | 700 | Long-press threshold |

Settings in `sdkconfig.defaults` worth knowing about, each commented in the
file: octal PSRAM, 1 kHz FreeRTOS tick (smooth ring input), LVGL on the
ESP heap (the built-in 64 KB pool is too small), the TinyUSB classes, and
the console on UART0.

## Flashing

The pad's single USB-C port is used for **both** flashing and normal use,
but not at the same time:

- **Normal use:** the firmware runs TinyUSB, so the port is a USB keyboard
  plus a serial port. esptool cannot reset the chip through it.
- **Flashing:** the chip's ROM loader, on the built-in USB-Serial/JTAG
  peripheral — the port `idf.py flash` expects.

`scripts/flash.sh` moves between the two for you:

```bash
scripts/flash.sh              # flash, then open the monitor on the firmware's port
scripts/flash.sh --no-monitor
```

It looks for the ROM loader port; if it is not there, it writes
`bootloader` to the firmware's serial port, which makes the firmware hand
the USB PHY back to USB-Serial/JTAG and reboot into the ROM loader. Ports
are found by USB identity under `/dev/serial/by-id`, never by fixed name.
The mechanism is described in [USB and protocol](usb-and-protocol.md#flashing-over-the-same-cable).

The very first flash of a board still running factory firmware (or any
USB-Serial/JTAG firmware) finds the ROM port directly and just flashes.

You can also trigger the reboot by hand — any tool that opens the port at
**1200 baud** and closes it does it (the Arduino convention), or send the
line `bootloader` from a serial terminal.

### Recovery

If a build crashes before USB starts, there is no serial port to ask.
Hold **Flash** while pressing **Reset**, then run `scripts/flash.sh` again —
the ROM loader port will be there.

## Serial log

The firmware's log goes to two places:

- **The USB serial port**, shared with the companion. The companion
  ignores log lines; run it with `--log` to see them. While the companion
  holds the port, `idf.py monitor` cannot open it — stop the service first
  (`systemctl --user stop rotary-hid-companion`) if you want the monitor.
- **UART0** (GPIO43 TX / GPIO44 RX, 115200) — useful when USB itself is
  what you are debugging. Lines logged before USB starts only appear here.

## IDE notes (VS Code / VSCodium)

The Espressif extension writes two per-workspace settings into
`.vscode/settings.json`, which is gitignored because both are specific to
your machine:

- `idf.currentSetup` — the path of *your* ESP-IDF install.
- `idf.customExtraVars.IDF_TARGET` — **set this to `esp32s3`.** With no
  `sdkconfig` present the extension otherwise exports `IDF_TARGET=esp32`
  into its terminals, which overrides `sdkconfig.defaults` and breaks the
  build as described above.

If the extension reports *"Required file {filePath} could not be found for
terminal activation"*, `idf.currentSetup` points at an ESP-IDF install the
extension knows about but cannot activate (typically one registered by an
older installer). Select the current install with **ESP-IDF: Select
current ESP-IDF version**.
