# USB and the companion protocol

## The USB device

While the firmware runs, the pad is a composite USB device built with
TinyUSB (`components/rotary_usb/`):

| Function | Interfaces | Purpose |
|---|---|---|
| HID keyboard (boot protocol) | 1 | the shortcuts sent to the focused app |
| CDC-ACM serial | 2 | the companion protocol **and** the firmware log |

| Field | Value |
|---|---|
| VID:PID | `303a:4005` (Espressif VID; TinyUSB's default PID for CDC+HID) |
| Manufacturer / product | `TrailCurrent` / `Rotary HID` |
| Serial number | the chip's MAC, as 12 hex digits |
| Serial interface name | `Rotary HID companion` |
| Keyboard interface name | `Rotary HID keyboard` |

Because `303a:4005` is a generic TinyUSB default, hosts should match on the
product or interface string too, not just the IDs.

**Why one serial port and not two.** A separate port for the log would be
tidier, but the ESP32-S3's USB-OTG has five IN FIFOs including EP0's. A CDC
port needs two IN endpoints and the keyboard one, so two CDC ports plus HID
do not fit. Log and protocol share the port; protocol lines are marked so
they can never be confused.

## Protocol

Plain ASCII over the serial port, one message per line, `\n` terminated.
Baud rate is irrelevant (it is USB) — except 1200, see below. Opening the
port asserts DTR, which the pad treats as "companion present".

**Host → pad**

| Line | Effect |
|---|---|
| `app freecad` | show FreeCAD keys; ring rotates the view |
| `app blender` | show Blender keys; ring rotates the selection |
| `app kdenlive` | show Kdenlive keys; ring scrubs |
| `app vscodium` | show VSCodium keys; ring scrolls the editor |
| `app gimp` | show GIMP keys; ring zooms |
| `app inkscape` | show Inkscape keys; ring zooms |
| `app libreoffice` | show LibreOffice keys; ring moves lines / pages |
| `app firefox` | show Firefox keys; ring scrolls the page |
| `app chromium` | show Chromium keys; ring scrolls the page |
| `time HH:MM:SS` | set the idle-page clock (local time, 24 h) |
| `app other` | idle screen; ring and keys do nothing |
| `ping` | pad answers `@pong` |
| `bootloader` | reboot into the ROM loader for flashing |

Unknown names after `app` are treated as `other`. Lines longer than 63
characters are truncated.

**Pad → host.** Every protocol line starts with `@`. Everything else is the
firmware's log (ESP-IDF format, e.g. `I (1234) app: focus: blender`) and
can be ignored or shown.

| Line | When |
|---|---|
| `@hello rotary_hid <version>` | the port was opened (DTR asserted) |
| `@app <name>` | acknowledges an `app` line |
| `@pong` | answers `ping` |

A companion should send its current `app` line and a `time` line **every
time** it opens the port — the pad remembers neither across a reboot, a
flash or a replug — and a `time` line about once a minute after that, to
keep the clock from drifting.

**Liveness.** The pad drops back to the idle screen when DTR goes low (the
companion closed the port or died). It does not time out an open but silent
port, so a companion that hangs with the port open leaves the pad on the
last app.

You can drive the pad from any serial terminal — type `app kdenlive`.

## Flashing over the same cable

TinyUSB takes the USB PHY away from the chip's USB-Serial/JTAG peripheral,
which is the one esptool talks to. To flash, the firmware gives it back:

1. It receives `bootloader`, or sees the port closed after being opened at
   **1200 baud** (the Arduino convention, so generic tools can trigger it).
2. It disconnects from USB and clears `RTCCNTL.usb_conf.sw_hw_usb_phy_sel`,
   returning the PHY to the hardware default (USB-Serial/JTAG). That
   register is in the RTC domain and survives a software reset, so without
   this step the ROM would come up on the wrong peripheral.
3. It sets `RTC_CNTL_FORCE_DOWNLOAD_BOOT` and restarts. The ROM loader
   appears as `Espressif USB JTAG/serial debug unit`, and `idf.py flash`
   works as usual.

`scripts/flash.sh` does all of this; see
[building and flashing](building-and-flashing.md#flashing).

## Keystroke timing

Each chord is held for 8 ms and released for 8 ms. The keyboard endpoint is
polled every 5 ms, so the host sees both edges of every key. One keystroke
takes about 16 ms: Blender's `R Z 5 Enter` about 80 ms, a 5-frame
Kdenlive scrub click (five arrow presses) about 80 ms, and a 3-line
VSCodium scroll click about 50 ms. The queue holds 64
chords and accepts a click's keystrokes all-or-nothing, so a fast spin
queues a few clicks and then drops the rest (logged as "queue full") rather
than typing for seconds after the ring has stopped.
