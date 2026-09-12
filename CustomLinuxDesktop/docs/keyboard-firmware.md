# The PocketTerm35 keyboard firmware

Complete notes on what the keyboard is, how to read and restore its firmware,
what every key currently does, and which codes are free for customisation.

Everything here was read off the device on 2026-09-06. **The backup described
below was taken before anything else; no firmware has been written.**

---

## 1. What it actually is

```
$ picotool info -a
Program Information
 name:              CircuitPython
 version:           10.0.0-beta.0-dirty
 features:          double reset -> BOOTSEL
 binary start:      0x10000000
 embedded drive:    0x00100000-0x00200000 (1024K): CircuitPython

Build Information
 build date:        2025-07-23
 build attributes:  BOARD=raspberry_pi_pico

Device Information
 type:              RP2040      revision: B2
 flash size:        4096K       flash id: 0x4150344134303600
```

It is a **Raspberry Pi Pico (RP2040) running CircuitPython 10.0.0-beta.0**,
with the keyboard logic written in plain Python in `code.py`. It is *not* a
compiled C or QMK firmware.

On USB it presents as:

```
ID 1209:0001   "My Company" / "My Custom Pico"   serial 4150344134303600
```

`1209:0001` is [pid.codes](https://pid.codes)' generic Test PID and the strings
are placeholders. It enumerates as a composite HID device — the kernel binds it
as both `My Custom Pico Keyboard` and `My Custom Pico Mouse`.

### Why no CIRCUITPY drive appears

```python
# boot.py
import storage
storage.disable_usb_drive()
```

That single line is the whole reason. CircuitPython would normally expose its
filesystem as a USB drive you could edit `code.py` on directly; this build
hides it so the device is only ever a keyboard. **This is the most important
fact for future customisation** — re-enabling that drive makes the keymap
editable as a text file, with no BOOTSEL and no flashing.

---

## 2. Hardware the keyboard controls

The keyboard is not only an input device. It drives panel and audio hardware
directly, which is why Linux has no control over them.

| Pin | Role | Notes |
|---|---|---|
| GP20 | **Screen backlight** PWM | 5 kHz. Driven through an NPN transistor pull-down, so **higher duty = dimmer** — the firmware's `bl_pwm_up()` *decreases* duty. Starts at 5000. |
| GP18 | **Analogue audio level** PWM | 5 kHz, starts at 32700 (mid). Steps of 6553 (≈10%). |
| GP19 | Mute toggle | GPIO, flipped by `toggle_gp19()` |
| GP21 | Screen on/off | GPIO, flipped by `toggle_gp21()` |
| GP22 | Caps-lock indicator | GPIO, also sends a real `CAPS_LOCK` keypress |

Two consequences worth stating plainly:

* **There is no `/sys/class/backlight` and there never will be.** The backlight
  is a PWM output of the keyboard's microcontroller. TrailShell therefore
  offers a *software dim* layer (an X gamma ramp) and calls it "Dim", not
  "Brightness", so it is not confused with the hardware control.
* **`Fn`+`E`/`Fn`+`R` do not change the Linux volume.** They change an
  analogue level on GP18, before the amplifier. That is a *different* volume
  from PipeWire's, which is what the shade's slider and `wpctl` control. The
  two are independent and multiply together.

---

## 3. The key matrix

7 rows x 10 columns, scanned in `code.py`. Two layers: base, and `Fn` held.

### Row 0 — the gamepad cluster

| col0 | col1 | col2 | col3 | col4 | col5 | col6 | col7 | col8 | col9 |
|---|---|---|---|---|---|---|---|---|---|
| `Up` | `Left` | `Down` | `Right` | `L` | `R` | `X` | `Y` | `B` | `A` |

**Identical on the Fn layer.** The D-pad sends real arrow keys and the six
buttons send the plain letters l, r, x, y, b, a — indistinguishable from
typing. Nothing here can be bound as a shortcut without changing the firmware.

### Row 6 — the bottom row, and the two dedicated buttons

| col0 | col1 | col2 | col3 | col4 | col5 | col6 | col7 | col8 |
|---|---|---|---|---|---|---|---|---|
| `Fn` | `Ctrl` | `L-Alt` | **`PrintScreen`** | `Space` | **`Pause`** | `R-Alt` | `Win` | `Fn` |

`PrintScreen` and `Pause` flank the spacebar and are **the only two keys on the
whole keyboard that emit something a person would never type.** They are the
SELECT and START buttons.

By the usual convention (SELECT left of the spacebar, START right):

| Button | Sends | X keycode | TrailShell action |
|---|---|---|---|
| SELECT | `Print` | 107 | Recents |
| START | `Pause` | 127 | Home |

If that is reversed on your unit, swap the two lines in
`trailshell/keymap.py:HARDWARE_NAV`.

> `Print`'s key-**press** is invisible on the stock Raspberry Pi desktop —
> a screenshot binding grabs it globally, so only the release arrives. In
> TrailShell's own session nothing competes for it. This is why the nav bar
> only draws a key legend when the grab actually succeeds.

### Base layer, rows 1-5

| Row | Keys |
|---|---|
| 1 | `1 2 3 4 5 6 7 8 9 0` |
| 2 | `Q W E R T Y U I O P` |
| 3 | `A S D F G H J K L Backspace` |
| 4 | `Z X C V B N M / Enter` |
| 5 | `Tab CapsLock - = ; ' , . Shift` |

### The Fn layer

| Fn + | Result | Handled by |
|---|---|---|
| `1`-`0` | **F1-F10** | HID |
| `O` / `P` | **F11 / F12** | HID |
| `Q` | Escape | HID |
| `W` | Mute (GP19 toggle) | **firmware** |
| `E` / `R` | **Analogue volume down / up** (GP18 PWM) | **firmware** |
| `T` / `Y` / `U` | Previous track / Play-pause / Next track | HID consumer control |
| `I` | Lock screen | firmware macro |
| `-` / `=` | **Backlight down / up** (GP20 PWM) | **firmware** |
| `C` | Screen on/off (GP21 toggle) | **firmware** |
| `A` `S` `D` `F` `G` `H` `J` | `` ` `` `~` `\` `|` `{` `}` `[` | HID |
| `Z` `X` `C` `V` `B` `N` `M` | Insert, Home, *(screen toggle)*, End, PgUp, PgDn, ScrollLock | HID |
| `Tab` / `CapsLock` | Tab / CapsLock | HID |

Rows marked **firmware** never reach Linux at all — no keysym is sent, so the
shell cannot see or rebind them, and must not try.

---

## 4. What is free for customisation

This is the direct answer to "assign function keys without overlapping
Waveshare's".

**Already taken — do not reuse:**

- `F1`-`F12` — the entire Fn number row plus Fn+O/P.
- Every letter and arrow — including the gamepad cluster, which sends
  `a b x y l r` and the arrow keys.
- `Fn`+`W E R I - = C` — consumed inside the firmware for mute, volume,
  lock, backlight and screen power.
- Media keys `SCAN_PREVIOUS_TRACK`, `PLAY_PAUSE`, `SCAN_NEXT_TRACK`.

**Free and safe:**

- **`F13`-`F24`.** Completely unused by this firmware, understood by X
  (`XK_F13`..`XK_F24`), and bound by essentially no Linux application by
  default. This is the right target for custom firmware.
- Remaining consumer-control codes not in the list above.

**Recommended remap for a future custom `code.py`:** move the six gamepad
buttons off letters onto `F13`-`F18`, which frees `a b x y l r` for typing and
gives the shell six real shortcut keys. Note this would break using the device
as a game controller with software expecting those letters — a deliberate
trade, not an obvious win.

---

## 5. Backup — how it was done

BOOTSEL entry on this board is **hold BOOTSEL while replugging USB**, or
**double-tap reset** (the firmware advertises `double reset -> BOOTSEL`).

While in BOOTSEL the board re-enumerates as `2e8a:0003` (RP2040) and mounts a
128 MB volume labelled `RPI-RP2`. That volume is effectively write-only — it
contains only `INDEX.HTM` and `INFO_UF2.TXT`, so **the existing firmware cannot
be recovered by copying files off it.** `picotool` is required.

> Entering BOOTSEL disconnects the keyboard. On this machine that is safe: the
> touchscreen still works and the device is reachable over SSH. Do not do it
> with no other input available.

```sh
sudo apt-get install -y picotool
./tools/kbd-backup.sh            # read-only; writes nothing to the device
```

What was captured, in `firmware/backup/`:

| File | Size | Contents |
|---|---|---|
| `*-keyboard-full.uf2` | 8.0 MB | the entire 4096K flash |
| `*-circuitpy-fs.bin` | 1.0 MB | just the CircuitPython filesystem, `0x10100000-0x10200000` |
| `*-info.txt` | | `picotool info -a` |
| `*-INFO_UF2.TXT` | | bootloader/board identification |
| `*-keyboard.sha256` | | checksum of the full image |

The 2026-09-06 backup: `a351b5dcd3867087587264226e8b4ad8df7cef7524450f1d9fa0a4e5949c433b`

The filesystem image is a plain FAT volume and can be read without the device:

```sh
mkdir -p /tmp/cp && sudo mount -o loop,ro firmware/backup/*-circuitpy-fs.bin /tmp/cp
```

Its contents are extracted into `firmware/circuitpy/` for reference:
`boot.py`, `code.py` (16 KB, the whole keyboard), and `lib/hid/*.mpy`.

### Leaving BOOTSEL

```sh
sudo picotool reboot     # "The device was rebooted into application mode."
```

Unplugging and replugging, or a power cycle, does the same.

### Caveats

* A full-flash image includes the flash chip's init sequence, so treat it as a
  backup for *this* board rather than a portable image.
* This is an RP2040, so readback is not blocked. On RP2350 secure boot can
  prevent it; if `picotool save -a` ever fails there, do not flash anything.

---

## 6. Restoring

```sh
# with the keyboard in BOOTSEL
sudo picotool load -x firmware/backup/<stamp>-keyboard-full.uf2
```

`-x` executes immediately, so the keyboard comes straight back.

Before flashing anything custom, all of these must hold:

1. `firmware/backup/*-keyboard-full.uf2` exists and its SHA-256 is recorded.
2. **A copy exists off this machine.** A backup that only lives on the device
   you are about to modify is not a backup.
3. `sudo picotool reboot` has been shown to return the keyboard to normal.

All three are satisfied for the 2026-09-06 backup except (2), which is manual.

---

## 7. What is actually flashed now (2026-09-06)

The device is **no longer running the stock firmware**. Only the 1 MB
CircuitPython filesystem region (`0x10100000`) was rewritten; the interpreter
itself was left untouched.

### code.py — six cells changed, nothing else

`KEY_MAP` row 0, columns 4-9 only:

| Button | Was | Now |
|---|---|---|
| L | `Keycode.L` | `F13` (0x68) |
| R | `Keycode.R` | `F14` (0x69) |
| X | `Keycode.X` | `F15` (0x6A) |
| Y | `Keycode.Y` | `F16` (0x6B) |
| B | `Keycode.B` | `F17` (0x6C) |
| A | `Keycode.A` | `F18` (0x6D) |

Everything else is byte-identical to the original, including:

* **The D-pad**, still real arrow keys — it is the only source of arrows on
  this keyboard, so remapping it would leave none at all.
* **`FN_MAP` row 0**, still the original letters, so `Fn`+button reproduces the
  old behaviour exactly. No capability was removed.
* Every letter key on the main rows. The firmware dispatches on `(row, col)`
  position, not on keycode, so changing `KEY_MAP[0][4..9]` cannot affect
  `L` at row3 col8, `R` at row2 col3, `X` at row4 col1, `Y` at row2 col5,
  `B` at row4 col4 or `A` at row3 col0. Verified by diffing both maps
  cell-by-cell: exactly 6 of 140 cells changed.

### boot.py — the CIRCUITPY drive is now conditional

The drive stays hidden in normal use, but **hold SELECT while plugging in the
keyboard and `CIRCUITPY` appears**, after which `code.py` is editable as a
plain text file — no BOOTSEL, no picotool, no flashing. The check is wrapped in
`try/except` and falls back to hiding the drive, so a failure leaves exactly
the stock behaviour.

### Two things that had to be verified, not assumed

**Does the HID report descriptor allow usages above F12?** Read back from the
live device (`/sys/bus/hid/devices/*/report_descriptor`):

```
95 06     Report Count (6)          6-key rollover
75 08     Report Size (8)
15 00     Logical Minimum (0)
26 FF 00  Logical Maximum (255)     <- full range
19 00     Usage Minimum (0)
2A FF 00  Usage Maximum (255)       <- full range
```

Yes — the whole keyboard usage page. F13-F24 transmit.

**Does X call them F13-F18?** *No*, and this would have silently broken
everything. Linux maps HID `0x68`-`0x6D` to `KEY_F13`..`KEY_F18`, which reach X
as keycodes 191-196 — but the stock xkb evdev map labels those `XF86Tools` and
`XF86Launch5`..`XF86Launch9`. Binding "F13" would never have fired. The session
script renames them:

```sh
xmodmap -e "keycode 191 = F13"   # ... through 196 = F18
```

### Measured result

| Key | Sends |
|---|---|
| L R X Y B A | `F13` `F14` `F15` `F16` `F17` `F18` |
| D-pad | `Left` `Up` `Down` `Right` |
| START / SELECT | `Pause` / `Print` |
| Letters | unchanged |

### Reverting

```sh
# keyboard in BOOTSEL
sudo picotool load -x firmware/backup/20260906-132456-keyboard-full.uf2
```

Or restore just the filesystem region, leaving the interpreter alone:

```sh
sudo picotool load -o 0x10100000 firmware/backup/20260906-132456-circuitpy-fs.bin
sudo picotool reboot
```

## 8. Two routes to customisation

**A. Hold SELECT while plugging in (now available).** `CIRCUITPY` mounts, and
`code.py` is a text file. This is the everyday route and needs no tools.

**B. Rebuild the filesystem image offline** — edit `firmware/custom/code.py`,
write it into a copy of the filesystem image, and flash that one 1 MB region:

```sh
cp firmware/backup/<stamp>-circuitpy-fs.bin firmware/custom/circuitpy-fs-custom.bin
sudo mount -o loop,rw firmware/custom/circuitpy-fs-custom.bin /mnt
sudo cp firmware/custom/code.py firmware/custom/boot.py /mnt/ && sync && sudo umount /mnt
# keyboard in BOOTSEL:
sudo picotool load -o 0x10100000 firmware/custom/circuitpy-fs-custom.bin && sudo picotool reboot
```

This is how the current firmware was flashed. It never touches the interpreter.

**C. Build a full UF2** — only if the CircuitPython runtime itself changes.

Either way, `code.py` is the keyboard: `KEY_MAP` and `FN_MAP` are plain
10-column lists, one per matrix row, and adding a key means putting a
`Keycode.F13` where a letter is now.

---

## Sources

- [raspberrypi/picotool](https://github.com/raspberrypi/picotool)
- [picotool man page](https://linuxcommandlibrary.com/man/picotool)
- [Upload UF2 using MSD vs picotool — Raspberry Pi Forums](https://forums.raspberrypi.com/viewtopic.php?t=362576)
- [Unable to reset pico — picotool issue #88](https://github.com/raspberrypi/picotool/issues/88)
- [Reset Pico without pressing BOOTSEL — Raspberry Pi Forums](https://forums.raspberrypi.com/viewtopic.php?t=336083)
- [How to add a reset button to your Raspberry Pi Pico](https://www.raspberrypi.com/news/how-to-add-a-reset-button-to-your-raspberry-pi-pico/)
- [CircuitPython `storage.disable_usb_drive()`](https://docs.circuitpython.org/en/latest/shared-bindings/storage/index.html)
