#!/usr/bin/env python3
"""Rotary HID companion -- tells the macro pad which app has focus.

Watches the X11 active window and, whenever it changes, sends one line to
the pad's USB serial port:

    app freecad | app blender | app kdenlive | app vscodium | app gimp |
    app inkscape | app libreoffice | app firefox | app chromium | app other

The pad swaps its key ring and ring behaviour to match. That is the whole
job; the keystrokes themselves go over the pad's USB keyboard, not through
this script.

Requirements: X11 (not Wayland -- see below), `xprop` (x11-utils) and
pyserial (`python3-serial`). The Debian package installs both, plus a udev
rule that lets the logged-in user open the pad's port and a systemd user
service that runs this at every login -- see docs/companion.md.

Wayland: an ordinary program cannot see which window has focus under
Wayland, so this script refuses to run there rather than silently reporting
every app as "other".

Usage:
    rotary_hid_companion.py                 find the pad automatically
    rotary_hid_companion.py --port PATH     use this serial port
    rotary_hid_companion.py --log           also print the pad's own log
    rotary_hid_companion.py --verbose       print every focus change
"""

import argparse
import os
import re
import subprocess
import sys
import threading
import time

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    sys.exit("pyserial is required: sudo apt install python3-serial")

__version__ = "0.5.0"

# USB identity the firmware reports (components/rotary_usb). The interface
# string is checked as well because the VID/PID pair is TinyUSB's generic
# CDC+HID default, which other TinyUSB gadgets may share.
USB_VID = 0x303A
USB_PID = 0x4005
USB_INTERFACE = "Rotary HID companion"

# WM_CLASS substrings -> app name. Matched case-insensitively against both
# the instance and class parts of WM_CLASS.
APPS = [
    ("freecad", "freecad"),
    ("blender", "blender"),
    ("kdenlive", "kdenlive"),
    ("codium", "vscodium"),
    ("gimp", "gimp"),
    ("inkscape", "inkscape"),
    ("libreoffice", "libreoffice"),   # libreoffice-writer / -calc / -impress
    ("soffice", "libreoffice"),
    ("firefox", "firefox"),           # firefox, and the snap's firefox_firefox
    ("chromium", "chromium"),
    ("google-chrome", "chromium"),    # same shortcuts as Chromium
]

# Whole WM_CLASS words -> app name, for classes too short to match as a
# substring. Microsoft VS Code is "code"/"Code" and shares VSCodium's
# keybindings, so it gets the same keys.
APPS_EXACT = {
    "code": "vscodium",
}


def log(msg):
    print(time.strftime("%H:%M:%S"), msg, flush=True)


# ---------------------------------------------------------------------------
# Focus tracking (X11)
# ---------------------------------------------------------------------------

def window_class(win_id):
    """WM_CLASS of a window, lower-cased, or '' if it has none."""
    try:
        out = subprocess.run(["xprop", "-id", win_id, "WM_CLASS"],
                             capture_output=True, text=True, timeout=2).stdout
    except (subprocess.SubprocessError, OSError):
        return ""
    # WM_CLASS(STRING) = "instance", "Class"
    return " ".join(re.findall(r'"([^"]*)"', out)).lower()


def app_for_window(win_id):
    if not win_id or win_id == "0x0":
        return "other"
    wm_class = window_class(win_id)
    for needle, app in APPS:
        if needle in wm_class:
            return app
    for word in wm_class.split():
        if word in APPS_EXACT:
            return APPS_EXACT[word]
    return "other"


def watch_focus(on_change):
    """Call on_change(app) whenever the focused app changes. Blocks.

    `xprop -spy` prints a line every time _NET_ACTIVE_WINDOW changes, so
    this is event-driven: no polling, no CPU while nothing happens.
    """
    proc = subprocess.Popen(["xprop", "-spy", "-root", "_NET_ACTIVE_WINDOW"],
                            stdout=subprocess.PIPE, text=True)
    for line in proc.stdout:
        m = re.search(r"window id # (0x[0-9a-fA-F]+)", line)
        on_change(app_for_window(m.group(1) if m else ""))
    raise RuntimeError("xprop exited -- is the X session still running?")


# ---------------------------------------------------------------------------
# Serial link to the pad
# ---------------------------------------------------------------------------

def find_port():
    for p in list_ports.comports():
        if p.vid == USB_VID and p.pid == USB_PID and \
                (p.interface in (None, USB_INTERFACE)):
            return p.device
    return None


class Pad:
    """Holds the serial port, reconnecting whenever the pad goes away (a
    replug, a reboot, a firmware flash)."""

    def __init__(self, port_arg, show_log):
        self.port_arg = port_arg
        self.show_log = show_log
        self.ser = None
        self.app = "other"
        self.lock = threading.Lock()
        self.last_time_sent = 0.0

    def set_app(self, app):
        with self.lock:
            self.app = app
            self._send(f"app {app}")

    def _send(self, line):
        if self.ser is None:
            return
        try:
            self.ser.write((line + "\n").encode("ascii"))
        except (serial.SerialException, OSError):
            self._drop()

    def _drop(self):
        if self.ser is not None:
            try:
                self.ser.close()
            except Exception:
                pass
            self.ser = None
            log("pad disconnected")

    def run(self):
        """Connect, and read the pad's output until it disappears. Forever."""
        announced_missing = False
        while True:
            port = self.port_arg or find_port()
            if not port:
                if not announced_missing:
                    log("waiting for the pad (USB " f"{USB_VID:04x}:{USB_PID:04x})...")
                    announced_missing = True
                time.sleep(1)
                continue
            try:
                ser = serial.Serial(port, 115200, timeout=0.5)
            except (serial.SerialException, OSError) as e:
                log(f"cannot open {port}: {e}")
                time.sleep(2)
                continue

            announced_missing = False
            with self.lock:
                self.ser = ser
                log(f"pad connected on {port}")
                self._send(f"app {self.app}")   # bring it up to date
                self._send_time()

            self._read_loop(ser)
            with self.lock:
                self._drop()
            time.sleep(1)

    def _send_time(self):
        """The pad has no clock of its own; it counts from these updates."""
        self._send(time.strftime("time %H:%M:%S"))
        self.last_time_sent = time.monotonic()

    def _read_loop(self, ser):
        buf = b""
        while True:
            if time.monotonic() - self.last_time_sent >= 60:
                with self.lock:
                    self._send_time()
            try:
                chunk = ser.read(256)
            except (serial.SerialException, OSError):
                return
            if not chunk:
                continue
            buf += chunk
            while b"\n" in buf:
                raw, buf = buf.split(b"\n", 1)
                line = raw.decode("utf-8", "replace").rstrip("\r")
                if line.startswith("@"):
                    if line.startswith("@hello"):
                        log(f"pad says: {line[1:]}")
                elif self.show_log and line:
                    print("  pad|", line, flush=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--port", help="serial port (default: find the pad by USB id)")
    ap.add_argument("--log", action="store_true", help="print the pad's log output")
    ap.add_argument("--verbose", action="store_true", help="print every focus change")
    ap.add_argument("--version", action="version", version=f"%(prog)s {__version__}")
    args = ap.parse_args()

    if os.environ.get("XDG_SESSION_TYPE") == "wayland" or \
            (os.environ.get("WAYLAND_DISPLAY") and not os.environ.get("DISPLAY")):
        sys.exit("This companion needs an X11 session: Wayland does not let "
                 "ordinary programs see which window has focus.")
    if not os.environ.get("DISPLAY"):
        sys.exit("DISPLAY is not set -- run this inside your desktop session.")

    pad = Pad(args.port, args.log)

    last = [None]
    def on_focus(app):
        if app != last[0]:
            last[0] = app
            if args.verbose:
                log(f"focus -> {app}")
            pad.set_app(app)

    def focus_thread():
        # If focus tracking dies, the pad would be left on a stale app with
        # nothing to correct it. Exit instead, so whatever started us (the
        # systemd unit, Restart=on-failure) starts a fresh one.
        try:
            watch_focus(on_focus)
        except Exception as e:
            log(f"focus tracking stopped: {e}")
        os._exit(1)

    threading.Thread(target=focus_thread, daemon=True).start()
    try:
        pad.run()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
