# PocketTerm35 hardware — verified findings

**Status:** measured on a live unit on 2026-09-04, over SSH. Every claim below is
backed by a command whose output is quoted. Where something is *not* yet verified it
says so explicitly, so nothing here reads as more settled than it is.

The unit was running **stock Raspberry Pi OS Trixie desktop** (labwc + `wf-panel-pi` +
`pcmanfm`) at the time of capture — not a LinuxTouch image. That is the starting point
`scripts/provision.sh` expects.

Several of these findings independently reproduce what
[TrailCurrentTracer](../../../Product/TrailCurrentTracer/docs/hardware.md) measured on
the same panel in September 2026. Where they agree, the agreement is noted — two
separate SD cards on the same hardware is a stronger result than either alone.

---

## Platform

| | Verified value | Source |
|---|---|---|
| Board | Raspberry Pi 5 Model B Rev 1.0 | `/proc/device-tree/model` |
| RAM | 8 GB (`8062 MB` usable) | `free -m` |
| OS | Debian GNU/Linux 13 (trixie) | `/etc/os-release` |
| Kernel | `6.12.47+rpt-rpi-2712` aarch64 | `uname -a` |
| Session | lightdm → labwc (`graphical.target`) | `systemctl get-default`, `/usr/share/wayland-sessions/` |

### Free space — check before provisioning

The first capture of this unit showed a card that had not yet been expanded:

```
$ df -h /
/dev/mmcblk0p2  6.8G  5.7G  696M  90% /
```

It expanded on the next boot, and now reads 81 G with 72 G free. Both numbers are
real; the point is that **a stock Pi OS card may still be unexpanded**, and
`provision.sh` installs roughly 60 MB of packages (`sway`, `python3-aiohttp`,
`gammastep`). The script therefore refuses to run below 300 MB free and warns below
800 MB, pointing at `raspi-config` to expand the filesystem — a half-installed device
from an `apt` that ran out of space is a worse outcome than a refusal.

---

## Display — HDMI at native 640×480

The panel is **HDMI**, driven by the stock `vc4-kms-v3d` KMS path. There is no SPI
framebuffer, no `fbcp-ili9341`, and no tinydrm shim.

```
$ cat /sys/class/drm/card1-HDMI-A-1/status
connected
$ cat /sys/class/drm/card1-HDMI-A-1/modes
640x480
800x600
640x480
```

640×480 is the **native and preferred mode**. The shell therefore targets a fixed
640×480 canvas with no scaling, no letterboxing, and `--force-device-scale-factor=1`.

**Consequence for the compositor config:** pin the mode explicitly anyway
(`output HDMI-A-1 mode 640x480@60Hz` in [`wm/sway/config`](../wm/sway/config)) so that
a mode-list reordering after a firmware update cannot silently select 800×600 and make
every hit target wrong. This is cheap insurance, not a fix for an observed problem.

Do **not** add a display overlay and do not set legacy `hdmi_*` keys — the Pi 5 ignores
them under KMS.

### The overlay name is misleading

`dtoverlay=waveshare-35dpi-5b` says "dpi" and ships in a folder called
`3.5HDMI_E_DTBO`, but the overlay contains **only a touch controller node and no
display node at all**. Display and touch are configured completely independently; the
display needs no overlay assistance whatsoever. (Decompiled and confirmed in Tracer;
not re-decompiled here.)

---

## Touch — Goodix GT911 on i2c-1 @ 0x5d

```
$ dmesg | grep -i goodix
Goodix-TS 1-005d: supply AVDD28 not found, using dummy regulator
Goodix-TS 1-005d: supply VDDIO not found, using dummy regulator
Goodix-TS 1-005d: ID 911, version: 1060
input: 1-005d Goodix Capacitive TouchScreen as .../i2c-1/1-005d/input/input7
Goodix-TS 1-0014: supply AVDD28 not found, using dummy regulator
```

- Live at **`/dev/input/event7`**.
- `B: PROP=2` — that is `INPUT_PROP_DIRECT`, so the kernel classifies it as a real
  touchscreen rather than a trackpad. **wlroots (sway) picks it up with no
  configuration and no calibration matrix**, because it reports 640×480, 1:1 with the
  display.
- The `dummy regulator` lines are normal: the overlay declares no `AVDD28`/`VDDIO`
  supplies, so the driver substitutes dummies. Not an error.
- Requires `dtparam=i2c_arm=on`, already present in the stock config.

### The 0x14 probe failure is expected, and it is log noise to suppress

The vendor `-5b` overlay declares the controller at **both** `0x14` and `0x5d`, because
Waveshare ships panels strapped to either address. Only `0x5d` is populated on this
unit, so the `0x14` node fails on **every boot**. This reproduces Tracer's finding
exactly, and it is why
[`pocketterm35-kali`](https://github.com/darkLabz001/pocketterm35-kali) reports the
`-4b` overlay failing outright — it declares `0x14` *only*.

[`overlays/linuxtouch-gt911-overlay.dts`](../overlays/linuxtouch-gt911-overlay.dts) is
the vendor `-5b` overlay with the `0x14` node removed. Harmless to keep the vendor
overlay installed alongside as the fallback for a replacement panel strapped to `0x14`.

---

## Input — a USB HID keyboard, not a gamepad

The keyboard, D-pad, and face buttons are one RP2040 presenting a **keyboard + mouse +
consumer-control** HID device. There is no Gamepad or Joystick collection, so nothing
on this device can emit gamepad events.

```
$ grep -E '^N:|^H:' /proc/bus/input/devices
N: Name="pwr_button"                             H: Handlers=kbd event0
N: Name="My Company My Custom Pico Keyboard"     H: Handlers=sysrq kbd leds event1
N: Name="My Company My Custom Pico Mouse"        H: Handlers=mouse0 event3
N: Name="1-005d Goodix Capacitive TouchScreen"   H: Handlers=kbd mouse1 event7
```

Note the keyboard is **`event1` on this card**, where Tracer measured `event0` on its
own. Device node numbers are allocation order, not identity — **never hardcode an
event number.** Match on the device name or use `/dev/input/by-id/`.

### The button → keycode mapping

Captured on hardware by Tracer over two independent passes and reused here verbatim as
[`touchd/keymap.default.json`](../touchd/keymap.default.json):

| Logical | Keycode | `KEY_*` | X keysym | Also a character? |
|---|---|---|---|---|
| `dpad_up` / `down` / `left` / `right` | 103 / 108 / 105 / 106 | `KEY_UP` … | `Up` … | no |
| `a` `b` `x` `y` `l` `r` | 30 48 45 21 38 19 | `KEY_A` … | `a` … | **yes** |
| `start` | 119 | `KEY_PAUSE` | `Pause` | no |
| `select` | 99 | `KEY_SYSRQ` | `Print` | no |

There are **no physical shoulder buttons** — L and R are the letter keys. The D-pad is
the keyboard's real arrow cluster.

**Only `start` and `select` carry no character**, which is the entire basis of the
control scheme in [controls.md](controls.md).

### `kernel.sysrq` must be set to 0 — mandatory

```
$ cat /proc/sys/kernel/sysrq
438
```

`Select` is `KEY_SYSRQ`, the kernel binds a `sysrq` handler directly to this keyboard
(`H: Handlers=sysrq kbd leds event1`), and 438 enables **reboot/poweroff (128)** and
**remount-read-only (32)**. An Alt-plus-Select chord therefore hard-cuts power without
syncing — on the button a user presses constantly to go Back.

LinuxTouch installs `/etc/sysctl.d/60-linuxtouch-sysrq.conf` setting `kernel.sysrq=0`.
Reading the key as an ordinary keycode is unaffected; only the kernel's magic handling
is disabled.

### The RP2040 is a single point of failure for input

The buttons and the speaker amplifier both hang off the RP2040, whose BOOT and RESET
buttons are exposed on the back of the case. A double-tap of RESET drops it into the
USB bootloader, the USB ID flips `1209:0001` → `2e8a:0003`, and **every button stops
working**; recovery is `picotool reboot -a`, not a power cycle.

The GT911 is on I²C and is unaffected. **Touch is the recovery path from a stranded
Pico**, which is why every LinuxTouch screen must be fully operable by touch alone —
that is a hard requirement, not a courtesy.

---

## No battery gauge and no backlight control

Both confirmed on this unit, and both reproduce Tracer's finding:

```
$ ls /sys/class/power_supply/     # empty
$ ls /sys/class/backlight/        # empty
```

- **Battery:** the carrier gives the Pi no way to read charge state. Waveshare exposes
  it only as LEDs on the UPS board. The status bar therefore shows **no battery
  indicator at all** — a gauge permanently reading `--` reads as a flat battery, which
  is worse than no gauge.
- **Backlight:** there is no kernel backlight device, so the Brightness control is a
  **compositor gamma adjustment** (`gammastep -b`), applied across every window
  including third-party apps. It reduces emitted light and glare, which is the actual
  need at night — but it does **not** reduce power draw, and the UI must not imply it
  does. Floored well above zero so the screen can never be dimmed to unreadable.

---

## Boot configuration as shipped

```
$ grep -vE '^\s*#|^\s*$' /boot/firmware/config.txt
dtparam=audio=on
camera_auto_detect=1
display_auto_detect=1
auto_initramfs=1
dtoverlay=vc4-kms-v3d
max_framebuffers=2
disable_fw_kms_setup=1
arm_64bit=1
disable_overscan=1
arm_boost=1
[cm4]
otg_mode=1
[cm5]
dtoverlay=dwc2,dr_mode=host
[all]
dtparam=i2c_arm=on
dtoverlay=waveshare-35dpi-5b
dtoverlay=dwc2,dr_mode=host
dtparam=cooling_fan=on
dtparam=fan_temp0=58000,fan_temp0_hyst=5000,fan_temp0_speed=75
dtparam=fan_temp1=68000,fan_temp1_hyst=5000,fan_temp1_speed=125
dtparam=fan_temp2=74000,fan_temp2_hyst=5000,fan_temp2_speed=175
dtparam=fan_temp3=78000,fan_temp3_hyst=5000,fan_temp3_speed=250
```

Three things worth knowing about this block:

- `dtoverlay=dwc2,dr_mode=host` is **required** — it is what puts the USB-C port in
  host mode so the RP2040 keyboard enumerates. Carry it into any image.
- `dtparam=cooling_fan=on` is present, with an explicit fan curve. Tracer found that
  without this line the firmware fails to detect the Active Cooler on this chassis and
  the board idles at 66–70 °C, with the fan spinning briefly at power-on in a way that
  makes the failure look like success. **Do not remove it.** This card's curve is
  slightly more relaxed than Tracer's (first step at 58 °C vs 50 °C); either is fine.
- `dtparam=audio=on` — the speaker is fed from HDMI, not the SoC's PWM audio block, so
  this parameter does not control it either way.

```
$ cat /boot/firmware/cmdline.txt
console=tty1 root=PARTUUID=... rootfstype=ext4 fsck.repair=yes rootwait quiet splash
plymouth.ignore-serial-consoles cfg80211.ieee80211_regdom=US
```

`console=tty1` still puts kernel output on the panel during boot. Removing it is part
of the image work, not the provisioning script — see [building.md](building.md).

---

## Software present on the stock image

Checked because the provisioning script's package list depends on it:

| Package | State |
|---|---|
| `chromium` | **installed**, 152.0.7977.75 |
| `xwayland` | **installed** — X11 apps will run |
| `labwc` | installed (the stock session) |
| `python3-evdev` | installed |
| `wlr-randr` | installed |
| `nmcli`, `wpctl`, `amixer` | present; NetworkManager running |
| `sway` | **not installed** — apt candidate 1.10.1-2+rpt1 |
| `python3-aiohttp` | **not installed** — apt candidate 3.11.16 |
| `gammastep` | **not installed** — apt candidate 2.0.9 |
| `seatd`, `cage`, `wayfire` | not installed, not needed |

Python is **3.13.5**.

---

## Still to verify on hardware

An honest list. None of these are believed to be problems; none have been run yet.

- **sway on this panel.** Not yet installed or started. Touch, the 640×480 mode pin,
  and XWayland app behaviour are all unexercised.
- **`Pause` / `Print` as sway keybinds firing while a third-party app holds focus.**
  This is the single load-bearing assumption of the control scheme. It follows from how
  compositor keybinds work, but it has not been demonstrated on this unit.
- **`gammastep -b` as a brightness control** under sway, and whether it survives
  window changes.
- **Touch scrolling inside ordinary GTK and Qt apps** at this size — expected to work
  via libinput, but hit-target size in real mail clients and editors is the open UX
  question of the whole project.
- **Boot time** into the shell. No target set yet, no measurement taken.
