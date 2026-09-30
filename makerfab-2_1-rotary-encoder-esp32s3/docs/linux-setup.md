# Linux setup: from a bare machine to a working pad

This is the whole procedure, start to finish, for one Linux computer and
one pad. There are three parts; do them in order the first time:

1. **Firmware** — build it and flash it onto the pad (once, and again after
   any firmware change).
2. **Companion** — a small background program on the computer that tells
   the pad which app has focus and keeps its clock set (once per computer).
3. **FreeCAD add-on** — lets the ring rotate FreeCAD's view (once per
   computer, only if you use FreeCAD).

Tested on Ubuntu 24.04 with GNOME on **Xorg**. Other Debian-based
distributions work the same way; see [requirements](#requirements) for the
one hard limit.

## What you get

| App | Ring turn | Ring push | Needs anything extra? |
|---|---|---|---|
| FreeCAD | rotate the view about X / Y / Z | cycle the axis | the add-on in [part 3](#part-3--freecad-add-on) |
| Blender | rotate the selection about X / Y / Z | cycle the axis | no |
| Kdenlive | scrub 5 frames (or 1 second) | cut at the playhead | no |
| VSCodium / VS Code | scroll the code | lines ↔ pages | no |
| GIMP | zoom in / out | fit image in window | no |
| Inkscape | zoom in / out | zoom to page | no |
| LibreOffice (Writer, Calc, Impress) | move 3 lines / a page | lines ↔ pages | no |
| Firefox | scroll the page | lines ↔ pages | no |
| Chromium / Google Chrome | scroll the page | lines ↔ pages | no |
| anything else | — | — | the pad shows an analog clock |

Every app also gets eight on-screen keys for its most-used shortcuts — the
full list is in [app mappings](apps.md).

## Requirements

| | |
|---|---|
| Board | Makerfabs MaTouch ESP32-S3 Rotary IPS 2.1" (ST7701) — see [hardware](hardware.md) |
| Cable | USB-C **data** cable (a charge-only cable powers the pad but nothing else works) |
| OS | Linux, **X11 (Xorg) session**. Wayland is not supported by the companion — [why](companion.md#wayland) |
| Keyboard layout | US (the pad sends shortcuts as US key positions) |
| Disk | ~3 GB for ESP-IDF and its toolchain |

**Check your session type:**

```bash
echo $XDG_SESSION_TYPE
```

It must print `x11`. If it prints `wayland`, log out, click the gear icon on
the Ubuntu login screen, choose **Ubuntu on Xorg**, and log back in.

---

## Part 1 — Firmware

### 1.1 Serial port permission

Flashing talks to the chip's built-in serial port, which belongs to the
`dialout` group:

```bash
sudo usermod -aG dialout $USER
```

**Log out and back in** (group changes only apply to new sessions). Check
with `groups` — `dialout` should be listed.

### 1.2 Install ESP-IDF 5.5

ESP-IDF is Espressif's SDK; it brings the compiler and build tools. Skip
this if you already have ESP-IDF 5.5.

```bash
sudo apt install git wget flex bison gperf python3 python3-pip python3-venv \
     cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

mkdir -p ~/esp && cd ~/esp
git clone -b v5.5.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v5.5.2
cd esp-idf-v5.5.2
./install.sh esp32s3
```

`install.sh` downloads the toolchain into `~/.espressif` and takes a few
minutes. Espressif's own guide covers other install methods:
<https://docs.espressif.com/projects/esp-idf/en/v5.5.2/esp32s3/get-started/linux-macos-setup.html>.

**Every new terminal** that builds firmware needs the environment loaded:

```bash
. ~/esp/esp-idf-v5.5.2/export.sh
```

### 1.3 Get the source

```bash
cd ~
git clone https://github.com/trailcurrentoss/YouTubeCodeSamples.git
cd YouTubeCodeSamples/makerfab-2_1-rotary-encoder-esp32s3
```

All commands from here on run from this folder.

### 1.4 Build

```bash
idf.py set-target esp32s3      # first time only
idf.py build
```

The first build downloads its libraries (LVGL, display and touch drivers,
TinyUSB) and takes a few minutes. It ends with `Project build complete`.

If it fails with `no versions of espressif/esp_tinyusb match` and mentions
`target esp32`, something in your environment set the wrong chip — see
[building and flashing](building-and-flashing.md#first-build).

### 1.5 Flash

Plug the pad into the computer with the USB-C cable, then:

```bash
scripts/flash.sh
```

The script finds the pad, flashes it, and opens its serial log. Press
**Ctrl+]** to leave the log.

**Check it.** The pad shows a clock dial **with no hands**, "No supported
app", and a pill reading **"Companion not running"**. That is correct: the
pad has no clock of its own and gets both the time and the focused app from
the companion, which part 2 installs.

If the script says no pad was found, or the pad's screen stays dark: hold
the **Flash** button, press and release **Reset**, release **Flash**, and
run `scripts/flash.sh` again.

---

## Part 2 — Companion

### 2.1 Build and install the package

```bash
companion/packaging/linux/build-deb.sh
sudo apt install ./dist/rotary-hid-companion_0.5.0_all.deb
```

`apt` pulls in the two things it needs (`python3-serial` and `x11-utils`),
installs a udev rule so your user can talk to the pad, and starts the
companion immediately. It will also start by itself at every login from now
on.

If `apt` ends with *"N: Download is performed unsandboxed as root as file
… couldn't be accessed by user '_apt'"*, that is a notice, not an error:
apt's download helper cannot read a file in your home folder or on an
external drive, so it read it as root instead. The package installed
normally.

### 2.2 Check it

Within a couple of seconds:

- the pill changes to **"USB HID connected"**, and
- the clock gets its hands, showing your computer's time.

Then click into any supported app — the pad switches to that app's logo
and keys. Click on the desktop and it returns to the clock.

If the pad still says "Companion not running":

```bash
systemctl --user status rotary-hid-companion
journalctl --user -u rotary-hid-companion -n 30
```

and see [companion troubleshooting](companion.md#troubleshooting).

---

## Part 3 — FreeCAD add-on

FreeCAD has no keyboard command for "rotate the view a few degrees about an
axis", so the pad's ring needs this small add-on. Skip this part if you do
not use FreeCAD. The on-screen keys work in FreeCAD without it; only the
ring's rotation needs it.

### 3.1 Install

1. Start FreeCAD.
2. **File → Open…**, go to this repository's `freecad/` folder and open
   **`RotaryHid_Install.FCMacro`**. It opens in FreeCAD's macro editor.
3. Press the green **Run** (▶) button in the Macro toolbar, or
   **Macro → Execute macro** (Ctrl+F6).
4. A message confirms: *"Installed to …/Mod/RotaryHid — Ring shortcuts
   active: X/Y/Z, both directions."* If FreeCAD instead asks for a folder,
   pick the repository's `freecad/` folder.
5. Close the macro editor tab.

Do not use the folder button in **Macro → Macros…** to reach the file — it
changes FreeCAD's macro folder setting permanently.

That is all. The installer copied the add-on into FreeCAD's `Mod` folder,
so it loads every time FreeCAD starts, and switched it on in the FreeCAD
you have open.

### 3.2 Check it

1. Open or create any model and click inside the 3D view.
2. The pad shows the FreeCAD screen with **Z** highlighted.
3. Turn the ring one click: the model turns 5° about Z, and the pad's angle
   reads **+5°**.
4. Push the ring to select **X**, turn, and the model tips about X.

### 3.3 Options

The rotation step is 5° per click. To change it, in FreeCAD open
**Tools → Edit parameters**, go to
`BaseApp → Preferences → Mod → RotaryHid`, right-click → **New float
item**, name it `StepDeg` and give it a value. Set the firmware's
`CONFIG_ROTARY_STEP_DEG` (`idf.py menuconfig` → Rotary Macro Pad) to the
same number and reflash, so the angle shown on the pad matches.

---

## Everything is working when…

| You do | You see |
|---|---|
| Plug the pad in | "USB HID connected" within a few seconds, and the clock shows the current time |
| Click on the desktop or any unsupported app | the clock with "No supported app" |
| Click into a supported app | the pad shows that app's logo and eight keys |
| Tap a key on the pad | the app runs that shortcut — [full list](apps.md) |
| Turn the ring in FreeCAD | the view rotates about the highlighted axis; push to change axis |
| Turn the ring in Blender (pointer over the 3D view, object selected) | the object rotates 5° about the highlighted axis |
| Turn the ring in Kdenlive | the playhead moves 5 frames per click; tap the pad's centre for 1-second steps |
| Push the ring in Kdenlive | the clip is cut at the playhead and a red mark appears on the pad |
| Turn the ring in VSCodium | the code scrolls 3 lines per click; push for page steps |
| Turn the ring in GIMP or Inkscape | the canvas zooms in / out; push fits the image (GIMP) or page (Inkscape) |
| Turn the ring in LibreOffice | the cursor (Writer) or active cell (Calc) moves 3 lines per click; push for page steps |
| Turn the ring in Firefox or Chromium | the page scrolls; push for screen-sized steps |

Something not right? See [troubleshooting](troubleshooting.md).

## Settings you may want to change

All in `idf.py menuconfig` → **Rotary Macro Pad**; rebuild and reflash
afterwards. The full list is in
[building and flashing](building-and-flashing.md#configuration).

| Setting | Default | What it changes |
|---|---|---|
| `ROTARY_STEP_DEG` | 5 | FreeCAD / Blender degrees per click |
| `ROTARY_KDENLIVE_FRAMES_PER_DETENT` | 5 | Kdenlive frames per click |
| `ROTARY_VSCODIUM_LINES_PER_DETENT` | 3 | VSCodium lines per click |
| `ROTARY_LIBREOFFICE_LINES_PER_DETENT` | 3 | LibreOffice lines per click |
| `ROTARY_BROWSER_LINES_PER_DETENT` | 3 | Firefox / Chromium scroll steps per click |

## Updating

- **Firmware:** `git pull`, then `idf.py build` and
  `scripts/flash.sh --no-monitor` (the companion is holding the serial
  port, so the log monitor cannot open it; the companion reconnects by
  itself once the pad restarts). If the build complains that Rotary Macro
  Pad options are missing, run `idf.py reconfigure` first.
- **Companion:** rebuild the `.deb` and `sudo apt install` it again.
- **FreeCAD add-on:** run `RotaryHid_Install.FCMacro` again.

## Removing everything

```bash
sudo apt remove rotary-hid-companion                        # companion, service, udev rule
rm -rf ~/.local/share/FreeCAD/v1-1/Mod/RotaryHid            # FreeCAD add-on (restart FreeCAD)
```

(The FreeCAD path contains its version, e.g. `v1-1`; **Help → About
FreeCAD → Copy to clipboard** shows your user data folder.) The pad itself
keeps its firmware; flash any other ESP32-S3 project onto it to replace it.
