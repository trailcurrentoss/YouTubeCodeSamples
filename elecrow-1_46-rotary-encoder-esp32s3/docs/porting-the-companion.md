# Porting the companion to another OS

The shipped companion runs on Linux/X11. This page is the specification
for writing one for Windows, macOS, or a Linux Wayland desktop. The
firmware needs no change for any of them except the macOS modifier note
below.

The companion is small on purpose. It has exactly four jobs, and only the
second is really OS-specific:

1. **Find the pad's serial port.**
2. **Watch which application has focus.**
3. **Send one protocol line when that changes** (and on every reconnect).
4. **Start at login and keep running.**

Use [`companion/rotary_hid_companion.py`](../companion/rotary_hid_companion.py)
as the reference implementation: jobs 1 and 3 are portable Python already
(pyserial runs on all three OSes), so a port is mostly a new `watch_focus()`
and a new installer.

## 1. Finding the port

Match on USB identity, never a fixed port name:

| | |
|---|---|
| VID:PID | `303a:4005` |
| Product string | `Rotary HID` |
| Serial interface string | `Rotary HID companion` |

`303a:4005` is a generic TinyUSB default, so also check the product or
interface string where the OS exposes it. With pyserial,
`serial.tools.list_ports.comports()` gives `vid`, `pid`, `product` and (on
Linux) `interface` on every OS.

| OS | What the port looks like |
|---|---|
| Linux | `/dev/ttyACM<n>`, and `/dev/serial/by-id/usb-TrailCurrent_Rotary_HID_<serial>-if00` |
| macOS | `/dev/cu.usbmodem<serial>` — open the **`cu.`** device, not `tty.` |
| Windows | `COM<n>`; Windows 10+ binds its built-in `usbser.sys` automatically, no driver install |

Open it at any baud rate **except 1200** — 1200 is the "reboot into the
flashing loader" signal. Opening the port must assert **DTR** (pyserial does
by default); DTR is how the pad knows a companion is present.

## 2. Watching focus

Produce one of `freecad`, `blender`, `kdenlive`, `vscodium`, `gimp`,
`inkscape`, `libreoffice`, `firefox`, `chromium`, `other`, event-driven if
the OS allows it. Map from the application's identity, not the window
title (titles contain file names).

| OS | Approach | Identity to match |
|---|---|---|
| Linux X11 | `_NET_ACTIVE_WINDOW` on the root window (`xprop -spy`, or python-xlib `PropertyNotify`) | `WM_CLASS` |
| Windows | `SetWinEventHook(EVENT_SYSTEM_FOREGROUND, …)` for events, or poll `GetForegroundWindow()`; then `GetWindowThreadProcessId` → `OpenProcess` → `QueryFullProcessImageNameW` | executable name: `FreeCAD.exe`, `blender.exe`, `kdenlive.exe`, `VSCodium.exe` / `Code.exe`, `gimp-3.2.exe` (any `gimp*`), `inkscape.exe`, `soffice.bin` / `soffice.exe`, `firefox.exe`, `chrome.exe` / `chromium.exe` |
| macOS | `NSWorkspace.sharedWorkspace().notificationCenter()` observing `NSWorkspaceDidActivateApplicationNotification` (pyobjc), or poll `frontmostApplication()` | bundle identifier: `org.freecad.FreeCAD`, `org.blenderfoundation.blender`, `org.kde.kdenlive`, `com.vscodium` / `com.microsoft.VSCode`, `org.gimp.gimp-3.2` (varies by version), `org.inkscape.Inkscape`, `org.libreoffice.script`, `org.mozilla.firefox`, `org.chromium.Chromium` / `com.google.Chrome` (verify on the installed build) |
| Linux Wayland (GNOME) | a GNOME Shell extension that exports `global.display.focus_window.get_wm_class()` over D-Bus, and the companion subscribing to it | `WM_CLASS` / app id |
| Linux Wayland (KDE) | a KWin script connected to `workspace.windowActivated` that calls out over D-Bus | `resourceClass` |

None of these need special permissions to learn *which app* is frontmost.
(macOS asks for Accessibility or Input Monitoring only for reading window
contents or synthesising input — neither is needed; the keystrokes come
from the pad's own USB keyboard.)

Poll no faster than a few times a second if you must poll, and send only
on change.

## 3. Protocol

```
host -> pad:  app freecad | app blender | app kdenlive | app vscodium |
              app gimp | app inkscape | app libreoffice | app firefox |
              app chromium | app other
              time HH:MM:SS                                    (+ '\n')
pad -> host:  lines starting with '@' are protocol; everything else is log
```

- Send the current `app` line and a `time` line (local time, 24 h)
  **immediately after every connect** — the pad forgets both across
  reboots, flashes and replugs — then a `time` line about once a minute.
- Reconnect by itself when the port disappears (the pad reboots when
  flashed).
- Ignore non-`@` lines unless the user asked to see the log.
- If focus tracking breaks, exit or recover — never keep the port open
  while no longer tracking, or the pad stays on a stale app.

Full reference: [USB and protocol](usb-and-protocol.md#protocol).

## 4. Starting at login

| OS | Mechanism | Notes |
|---|---|---|
| Linux | systemd **user** unit, `WantedBy=graphical-session.target` | see `companion/packaging/linux/` |
| Windows | a shortcut in the user's Startup folder, or `HKCU\Software\Microsoft\Windows\CurrentVersion\Run` | build a windowless exe (`pyinstaller --noconsole`) so no console window appears |
| macOS | a LaunchAgent plist in `~/Library/LaunchAgents` with `RunAtLoad` and `KeepAlive` | an unsigned app needs one right-click → Open the first time |

## Packaging

| OS | Freeze with | Installer |
|---|---|---|
| Linux | not needed — ships as a Python script | `.deb` ([build-deb.sh](../companion/packaging/linux/build-deb.sh)) |
| Windows | PyInstaller (`--onefile --noconsole`) | Inno Setup or MSI; installs the exe and the Startup entry |
| macOS | PyInstaller or py2app (`.app` bundle) | `.pkg` or `.dmg`; installs the app and the LaunchAgent |

Frozen executables must be built on the OS they target. Installers that
are not code-signed trigger Windows SmartScreen and macOS Gatekeeper
warnings; that is a signing-certificate question, not a code one.

Put new platforms beside the Linux one: `companion/packaging/windows/`,
`companion/packaging/macos/`, and a `watch_focus()` per platform selected
by `sys.platform`.

## macOS: Command instead of Control

The pad currently sends **Ctrl** for Undo, Redo and Kdenlive's Render. On
macOS those apps use **Cmd**. A macOS port needs a small firmware change as
well:

1. Add a protocol line, e.g. `os mac`, sent by the companion on connect.
2. In `main/app_model.c`, substitute `ROTARY_MOD_GUI` for `ROTARY_MOD_CTRL`
   when that flag is set.

Everything else — function keys, arrows, letters — is the same on all
three OSes. The FreeCAD add-on registers both the X11 names and plain
`F14`–`F16`, so it works anywhere FreeCAD runs;
on macOS F14–F16 may need the "Use F1, F2, etc. keys as standard function
keys" setting.

## Checklist for a new port

- [ ] Finds the pad by `303a:4005` + product/interface string
- [ ] Never opens at 1200 baud
- [ ] Sends the current `app` on every connect, and on every focus change
- [ ] Sends `time HH:MM:SS` on every connect and about once a minute
- [ ] Reconnects after unplug / reboot / flash
- [ ] Exits or recovers if focus tracking fails
- [ ] Starts at login with no terminal window
- [ ] Installer and uninstaller
- [ ] Documented next to [companion.md](companion.md)
