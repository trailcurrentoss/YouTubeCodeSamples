# Rotary Macro Pad

A per-app macro pad built on the **Elecrow CrowPanel 1.28" HMI ESP32 Rotary
Display** — a 240×240 round touchscreen inside a rotary ring. It plugs in over
USB-C as a keyboard, and a small companion program on the computer tells it
which app has focus, so the ring of keys on screen changes with the app.

This is the 1.28" port of the [MaTouch 2.1" pad](../makerfab-2_1-rotary-encoder-esp32s3):
the same apps, keys, ring actions, companion and FreeCAD add-on, with the
screens redrawn for the smaller glass. Both identify as the same USB device,
so one companion install serves either.

## Supported apps

| App | Turn the ring | Push the ring | Touch |
|---|---|---|---|
| **FreeCAD** | rotate the view about the active axis | cycle X → Y → Z | views, hide/show, undo/redo |
| **Blender** | rotate the selection (`R <axis> ±5 Enter`) | cycle X → Y → Z | grab/rotate/scale, edit mode, camera, render |
| **Kdenlive** | scrub 5 frames (or 1 second) per click | cut at the playhead | play, in/out, razor, spacer…; tap the centre for frame/second |
| **VSCodium** / VS Code | scroll the code | switch lines ↔ pages | palette, open, find, go to definition, rename, comment, terminal, save |
| **GIMP** | zoom in / out | fit image in window | move, rectangle select, crop, brush, eraser, text, undo/redo |
| **Inkscape** | zoom in / out | zoom to page | selector, node, rectangle, ellipse, pen, text, undo/redo |
| **LibreOffice** (Writer, Calc, Impress) | move 3 lines / a page | switch lines ↔ pages | bold, italic, underline, find & replace, save, print, undo/redo |
| **Firefox** | scroll the page | switch lines ↔ pages | back, forward, reload, new/close tab, find, sidebar, history |
| **Chromium** / Google Chrome | scroll the page | switch lines ↔ pages | back, forward, reload, new/close tab, bookmark, history, downloads |

Anything else focused shows an analog clock with "No supported app", and
the pad stays quiet. The companion keeps the clock set.

## Quick start

For people who already have ESP-IDF 5.5 installed and are in the `dialout`
group; otherwise start with [Linux setup](docs/linux-setup.md).

**1. Flash the firmware**:

```bash
git clone https://github.com/trailcurrentoss/YouTubeCodeSamples.git
cd YouTubeCodeSamples/elecrow-1_28-rotary-encoder-esp32s3
. $IDF_PATH/export.sh
idf.py set-target esp32s3
idf.py build
scripts/flash.sh
```

**2. Install the companion** (Linux, X11):

```bash
companion/packaging/linux/build-deb.sh
sudo apt install ./dist/rotary-hid-companion_0.5.0_all.deb
```

The pad's screen changes to **"USB HID connected"**. Focus any
supported app — FreeCAD, Blender, Kdenlive, VSCodium, GIMP, Inkscape,
LibreOffice, Firefox or Chromium.

**3. FreeCAD only:** open `freecad/RotaryHid_Install.FCMacro` in FreeCAD and
press **Run** — this lets the ring rotate the view.

**New to this?** Follow **[Linux setup](docs/linux-setup.md)** instead — it
walks through everything from a bare Linux machine: installing ESP-IDF,
serial-port permission, building, flashing, the companion, the FreeCAD
add-on, and a checklist to confirm every part works.

## Documentation

| | |
|---|---|
| **[Linux setup](docs/linux-setup.md)** | **start here** — everything from a bare machine to a working pad |
| [Hardware](docs/hardware.md) | the board, pin map, and its surprises |
| [Building and flashing](docs/building-and-flashing.md) | build, configuration, flashing over the same cable, recovery, IDE notes |
| [App mappings](docs/apps.md) | every key and ring action per app, and FreeCAD setup |
| [Companion](docs/companion.md) | installing, running and troubleshooting the Linux companion |
| [USB and protocol](docs/usb-and-protocol.md) | the USB device, the companion protocol, reboot-to-flash |
| [Firmware architecture](docs/firmware-architecture.md) | how the firmware is put together |
| [GUI](docs/gui.md) | the EEZ Studio project and how to change the screens |
| [Porting the companion](docs/porting-the-companion.md) | what a Windows, macOS or Wayland companion needs to do |
| [Troubleshooting](docs/troubleshooting.md) | symptoms and fixes |

## Repository layout

```
GUI/RotaryHid.eez-project   the screens, authored in EEZ Studio (source of truth)
GUI/ASSETS/                 fonts and logos the project embeds
main/                       firmware: behaviour, UI glue, actions, variables
main/ui/                    EEZ Studio's export — generated, safe to delete and re-export
components/crowpanel_board/ panel, touch, ring (CrowPanel 1.28" only)
components/rotary_usb/      USB keyboard + serial link to the companion
companion/                  the Linux companion and its .deb packaging
freecad/                    FreeCAD add-on for view rotation, and its installer
scripts/flash.sh            flash over the pad's USB-C cable
docs/                       full documentation
```

## Platform support

The firmware is platform-neutral — it is a standard USB keyboard. The
companion currently runs on **Linux with X11**. Windows, macOS and Wayland
need their own focus watcher and installer;
[porting the companion](docs/porting-the-companion.md) specifies exactly
what one must do.

## Credits

FreeCAD, Blender, Kdenlive, VSCodium, GIMP, Inkscape, LibreOffice, Firefox
and Chromium logos are trademarks of their respective projects, used to identify the app the pad is set up for. Icons
from Font Awesome Free (SIL OFL 1.1). Fonts: Roboto (Apache 2.0), Roboto Mono
and Montserrat (SIL OFL 1.1), DejaVu Sans Condensed (Bitstream Vera licence,
[GUI/ASSETS/DejaVu-LICENSE.txt](GUI/ASSETS/DejaVu-LICENSE.txt)).

## License

MIT — see [LICENSE](LICENSE).
