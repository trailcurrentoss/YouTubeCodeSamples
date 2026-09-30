# Companion (Linux)

The companion is a small program on the computer that watches which window
has focus and tells the pad, so the pad can show that app's keys. It sends
one line whenever focus changes — `app blender`, `app gimp`, `app other`,
… — over the
pad's USB serial port. The keystrokes themselves go over the pad's USB
keyboard, not through the companion.

It is one Python file: [`companion/rotary_hid_companion.py`](../companion/rotary_hid_companion.py).

**Supported:** Linux desktops running **X11** (Xorg). Tested on Ubuntu 24.04
with GNOME on Xorg.
**Not supported:** Wayland sessions — see [below](#wayland). Windows and
macOS — see [porting the companion](porting-the-companion.md).

## Install

Build the package (or download it from a release), then install it:

```bash
companion/packaging/linux/build-deb.sh
sudo apt install ./dist/rotary-hid-companion_0.5.0_all.deb
```

Double-clicking the `.deb` in the file manager also works on Ubuntu.

If `apt` ends with *"N: Download is performed unsandboxed as root as file
… couldn't be accessed by user '_apt'"*, that is a notice, not an error:
apt's download helper cannot read a file in your home folder or on an
external drive, so it read it as root instead. The package installed
normally.

That is the whole setup. The package:

| Installs | Where | Why |
|---|---|---|
| the companion | `/usr/bin/rotary-hid-companion` | the program |
| a systemd **user** service | `/usr/lib/systemd/user/rotary-hid-companion.service` | starts it with every desktop session, restarts it if it dies |
| a udev rule | `/usr/lib/udev/rules.d/70-rotary-hid.rules` | lets the logged-in user open the pad's port — no `dialout` group, no re-login |
| dependencies | `python3-serial`, `x11-utils` | pyserial, and `xprop` for window focus |

It enables the service for all users and starts it straight away for anyone
already logged in. Plug the pad in and its screen should change from
"Companion not running" to **"USB HID connected"**.

## Remove

```bash
sudo apt remove rotary-hid-companion
```

## Everyday use

There is nothing to do: focus a supported app and the pad
follows. Useful commands:

```bash
systemctl --user status rotary-hid-companion      # is it running?
journalctl --user -u rotary-hid-companion -f      # focus changes, connects
systemctl --user restart rotary-hid-companion
systemctl --user stop rotary-hid-companion        # e.g. to use idf.py monitor
```

Run it by hand instead of as a service (stop the service first — only one
program should hold the port):

```bash
rotary-hid-companion --verbose --log
```

| Option | Meaning |
|---|---|
| `--verbose` | log every focus change (the service uses this) |
| `--log` | also print the pad's own firmware log |
| `--port PATH` | use this serial port instead of finding the pad by USB id |
| `--version` | print the version |

## How it works

- **Finding the pad:** it lists serial ports and picks the one with USB id
  `303a:4005` whose interface is `Rotary HID companion`. If the pad is not
  plugged in it waits, and it reconnects on its own after a replug, reboot
  or firmware flash — sending the current app again each time.
- **Watching focus:** it runs `xprop -spy -root _NET_ACTIVE_WINDOW`, which
  prints a line whenever the active window changes (event-driven — no
  polling), then reads that window's `WM_CLASS`.
- **Matching apps:** `WM_CLASS` is matched case-insensitively against
  `freecad`, `blender`, `kdenlive`, `codium` (VSCodium), `gimp`,
  `inkscape`, `libreoffice`, `soffice`, `firefox` (including the snap's
  `firefox_firefox`), `chromium` and `google-chrome`; the exact class
  `code` (Microsoft VS Code) also maps to VSCodium. Anything else is
  `other`. To add an
  app, add it to `APPS` in the script *and* teach the firmware the name.
- **Keeping the clock:** it sends `time HH:MM:SS` (local time) whenever it
  connects and once a minute, which is what sets the hands on the pad's
  idle-screen clock.
- **Failure handling:** if focus tracking stops (X restarted, `xprop`
  killed) it exits with an error so the service restarts it, rather than
  leaving the pad showing a stale app.

The line protocol is documented in [USB and protocol](usb-and-protocol.md#protocol).

## Wayland

Under Wayland an ordinary program cannot see which window has focus — that
is a deliberate part of Wayland's design. The companion detects a Wayland
session and exits with a message rather than reporting every app as
"other".

On Ubuntu, pick **Ubuntu on Xorg** from the gear icon on the login screen.
Supporting Wayland means a desktop-specific helper — a GNOME Shell
extension or a KWin script that exposes the focused app over D-Bus — which
[porting the companion](porting-the-companion.md#2-watching-focus) outlines.

## Troubleshooting

| Symptom | Check |
|---|---|
| Pad says "Companion not running" | `systemctl --user status rotary-hid-companion`. Not running → `journalctl --user -u rotary-hid-companion` for why. |
| Journal: "DISPLAY is not set" | The service started outside the desktop session. `systemctl --user import-environment DISPLAY XAUTHORITY`, then restart it. |
| Journal: "needs an X11 session" | You are on Wayland — see above. |
| Journal: "cannot open /dev/ttyACM0: Permission denied" | The udev rule did not apply: unplug and replug the pad. `ls -l /dev/rotary-hid` should exist. |
| Journal: "waiting for the pad" forever | `lsusb` should list `303a:4005 TrailCurrent Rotary HID`. If you see `Espressif USB JTAG/serial debug unit` instead, the pad is in the ROM loader — press **Reset**. |
| Pad shows the wrong app | `rotary-hid-companion --verbose` and switch windows: the log shows the app each window maps to. `xprop WM_CLASS` + click a window shows its class. |

## Building the package

```bash
companion/packaging/linux/build-deb.sh
```

Needs `dpkg-deb` and `fakeroot` only. The version comes from `__version__`
in `rotary_hid_companion.py`. The output goes to `dist/` (gitignored).
Package sources — the service unit, udev rule and maintainer scripts — are
in [`companion/packaging/linux/`](../companion/packaging/linux/).
