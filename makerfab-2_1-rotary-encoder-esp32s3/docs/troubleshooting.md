# Troubleshooting

## The pad's screen

| Screen shows | Meaning | Fix |
|---|---|---|
| "No UI exported yet" | firmware built without `main/ui/` | open `GUI/RotaryHid.eez-project` in EEZ Studio, Ctrl+B, rebuild — [GUI](gui.md#changing-the-gui) |
| "USB not connected" | the host has not configured the USB device | check the cable carries data; `lsusb` should list `303a:4005` |
| "Companion not running" | USB is fine; nothing has the serial port open | start the companion — [companion](companion.md#everyday-use) |
| "USB HID connected" + "No supported app" | all working; the focused window is not a supported app | focus one of those |
| Idle clock has no hands | the pad has not received the time since it powered up — the companion is not running or not connected | start the companion; the hands appear within a second of it connecting |
| Idle clock is wrong | the computer's clock or time zone is wrong — the pad copies it | fix the computer's time; the pad follows within a minute |
| Stays on one app after switching windows | the companion is not seeing focus changes | [companion troubleshooting](companion.md#troubleshooting) |
| Black screen, backlight on | panel not initialised | see [hardware](hardware.md#things-this-board-does-that-you-would-not-expect) |
| Red and blue swapped | colour channel order | toggle `CONFIG_MATOUCH_LCD_SWAP_RB` |

## Input

| Symptom | Fix |
|---|---|
| Ring turns the wrong way | toggle `CONFIG_MATOUCH_ENCODER_INVERT` |
| One click moves two steps, or two clicks move one | set `CONFIG_MATOUCH_ENCODER_STEPS_PER_DETENT`; enable `CONFIG_MATOUCH_ENCODER_DEBUG` and read the raw count for one click |
| Pressing a key hard does the push action instead | expected — a press firm enough to click the ring is a push. Tap lightly. [Why](firmware-architecture.md#touch-vs-push) |
| Touches land beside the target | flip `CONFIG_MATOUCH_TOUCH_MIRROR_X/Y` / `SWAP_XY` if it is mirrored; a constant ~20 px offset is a known property of this panel |
| Keys work but the ring does nothing in FreeCAD | the add-on is not installed or not loaded — run `freecad/RotaryHid_Install.FCMacro` ([setup](linux-setup.md#part-3--freecad-add-on)); click in the 3D view so a model tab has focus |
| Blender ignores the ring | the mouse pointer must be over the 3D viewport, in Object or Edit mode |
| Keys type the wrong characters | the host is not on a US layout; shortcuts are sent as US key positions |
| The ring moves a text cursor in Firefox / Chromium instead of scrolling | a text field has focus — click on the page itself |
| Kdenlive scrub too slow or too coarse | set `CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT` (menuconfig → Rotary Macro Pad) |
| Fast spinning in Blender "loses" clicks | by design: the keystroke queue drops excess rather than typing for seconds — [details](usb-and-protocol.md#keystroke-timing) |

## Building

| Error | Fix |
|---|---|
| `no versions of espressif/esp_tinyusb match` / "Building ESP-IDF components for target esp32" | wrong target: `idf.py set-target esp32s3`; in VS Code/VSCodium also set `idf.customExtraVars.IDF_TARGET` — [details](building-and-flashing.md#ide-notes-vs-code--vscodium) |
| "Required file {filePath} could not be found for terminal activation" | the IDE extension points at an ESP-IDF install it cannot activate — [details](building-and-flashing.md#ide-notes-vs-code--vscodium) |
| `undefined reference to ui_font_*` / `objects` / `action_*` | the export in `main/ui/` is missing or stale: reopen the project in EEZ Studio, Ctrl+B |
| A setting in `sdkconfig.defaults` has no effect | `sdkconfig` predates it: `rm sdkconfig && idf.py build` |

## Flashing

| Symptom | Fix |
|---|---|
| `scripts/flash.sh`: "No pad found" | plug it in; if the screen is dead, hold **Flash** while pressing **Reset** |
| "The ROM loader did not appear" | the firmware did not respond to `bootloader` (crashed, or USB not up): hold **Flash**, press **Reset**, rerun |
| `idf.py monitor` cannot open the port | the companion holds it: `systemctl --user stop rotary-hid-companion`, or use `rotary-hid-companion --log` |
| Want the very first boot log | it goes to UART0 (GPIO43/44) before USB starts |
