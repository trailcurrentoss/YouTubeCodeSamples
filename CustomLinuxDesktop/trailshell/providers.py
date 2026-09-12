"""System state the shell surfaces: clock, network, audio, brightness, power.

Everything that shells out does so through :func:`run_async` (Gio.Subprocess)
rather than blocking ``subprocess.run``. On a single-process shell a 50 ms
nmcli call on the main loop is a visible stutter during a swipe, and the shade
is the one surface where smoothness is most noticeable.

Hardware notes for the PocketTerm35, discovered by probing the device:
  * No ``/sys/class/backlight`` entry - the Waveshare DPI panel has no
    software-controllable backlight. Brightness therefore goes through
    ``xrandr --brightness``, which is a gamma ramp, not real backlight
    dimming, but is genuinely useful at night in a vehicle.
  * No battery. The battery indicator hides itself rather than showing 0%.
  * Audio is PipeWire via wpctl; the default sink is HDMI.
"""
from __future__ import annotations

import logging
import shutil
import time
from typing import Callable

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import Gio, GLib

log = logging.getLogger("trailshell.providers")

XRANDR_OUTPUT = "HDMI-1"
BRIGHTNESS_MIN = 0.25      # below this the panel is effectively unreadable
BRIGHTNESS_STATE = "brightness"


def run_async(argv: list[str], on_done: Callable[[str], None] | None = None) -> None:
    """Run a command off the main loop, handing stdout to ``on_done``."""
    if not shutil.which(argv[0]):
        if on_done:
            on_done("")
        return
    try:
        proc = Gio.Subprocess.new(
            argv, Gio.SubprocessFlags.STDOUT_PIPE | Gio.SubprocessFlags.STDERR_SILENCE
        )
    except GLib.Error as exc:
        log.debug("spawn %s failed: %s", argv[0], exc.message)
        if on_done:
            on_done("")
        return

    def _cb(p, res):
        try:
            ok, out, _err = p.communicate_utf8_finish(res)
            if on_done:
                on_done(out or "" if ok else "")
        except GLib.Error as exc:
            log.debug("%s failed: %s", argv[0], exc.message)
            if on_done:
                on_done("")

    proc.communicate_utf8_async(None, None, _cb)


def run_fire(argv: list[str]) -> None:
    """Fire-and-forget a command (used for setters)."""
    run_async(argv, None)


class Signal:
    """Minimal callback list - lighter than deriving from GObject."""

    def __init__(self) -> None:
        self._subs: list[Callable] = []

    def connect(self, fn: Callable) -> None:
        self._subs.append(fn)

    def emit(self, *args) -> None:
        for fn in list(self._subs):
            try:
                fn(*args)
            except Exception:
                log.exception("signal handler failed")


# ---------------------------------------------------------------- clock ----
class Clock:
    def __init__(self) -> None:
        self.changed = Signal()
        self._tick()

    def _tick(self) -> bool:
        self.changed.emit(self)
        # Re-arm exactly on the next minute boundary so the clock never shows
        # a stale minute, and we wake once a minute rather than once a second.
        delay = 60 - int(time.time()) % 60
        GLib.timeout_add_seconds(delay, self._tick)
        return False

    @property
    def time_str(self) -> str:
        return time.strftime("%H:%M")

    @property
    def date_str(self) -> str:
        return time.strftime("%a %-d %b")

    @property
    def long_date(self) -> str:
        return time.strftime("%A, %-d %B")


# -------------------------------------------------------------- network ----
class Network:
    POLL_S = 8

    def __init__(self) -> None:
        self.changed = Signal()
        self.wifi_enabled = True
        self.connected = False
        self.ssid = ""
        self.signal = 0            # 0-100
        self.airplane = False
        self.poll()
        GLib.timeout_add_seconds(self.POLL_S, self._repoll)

    def _repoll(self) -> bool:
        self.poll()
        return True

    @property
    def bars(self) -> int:
        """Signal mapped to the 3 arcs of the wifi glyph."""
        if not (self.wifi_enabled and self.connected):
            return 0
        return 1 if self.signal < 40 else (2 if self.signal < 70 else 3)

    def poll(self) -> None:
        run_async(["nmcli", "-t", "-f", "ACTIVE,SSID,SIGNAL", "dev", "wifi"], self._on_wifi)
        run_async(["nmcli", "-t", "radio", "wifi"], self._on_radio)

    def _on_wifi(self, out: str) -> None:
        ssid, sig, conn = "", 0, False
        for line in out.splitlines():
            parts = line.split(":")
            if parts and parts[0] == "yes":
                conn = True
                ssid = parts[1] if len(parts) > 1 else ""
                try:
                    sig = int(parts[2]) if len(parts) > 2 else 0
                except ValueError:
                    sig = 0
                break
        if (conn, ssid, sig) != (self.connected, self.ssid, self.signal):
            self.connected, self.ssid, self.signal = conn, ssid, sig
            self.changed.emit(self)

    def _on_radio(self, out: str) -> None:
        # `nmcli -t radio wifi` prints a bare "enabled" / "disabled". The
        # non-terse form is a header+values table and mis-parses trivially.
        val = out.strip().lower()
        enabled = val != "disabled"
        if enabled != self.wifi_enabled:
            self.wifi_enabled = enabled
            self.changed.emit(self)

    def set_wifi(self, on: bool) -> None:
        run_fire(["nmcli", "radio", "wifi", "on" if on else "off"])
        self.wifi_enabled = on
        self.changed.emit(self)
        GLib.timeout_add_seconds(2, lambda: (self.poll(), False)[1])

    def toggle_wifi(self) -> None:
        self.set_wifi(not self.wifi_enabled)


# ---------------------------------------------------------------- audio ----
class Audio:
    POLL_S = 4
    SINK = "@DEFAULT_AUDIO_SINK@"

    def __init__(self) -> None:
        self.changed = Signal()
        self.volume = 0.0          # 0.0 - 1.0
        self.muted = False
        self.poll()
        GLib.timeout_add_seconds(self.POLL_S, self._repoll)

    def _repoll(self) -> bool:
        self.poll()
        return True

    @property
    def level(self) -> int:
        """Volume mapped to the 3 arcs of the speaker glyph."""
        if self.muted or self.volume <= 0.001:
            return 0
        return 1 if self.volume < 0.34 else (2 if self.volume < 0.67 else 3)

    def poll(self) -> None:
        run_async(["wpctl", "get-volume", self.SINK], self._on_volume)

    def _on_volume(self, out: str) -> None:
        # "Volume: 0.40" or "Volume: 0.40 [MUTED]"
        vol, muted = self.volume, self.muted
        for tok in out.split():
            try:
                vol = float(tok)
                break
            except ValueError:
                continue
        muted = "[MUTED]" in out
        if (abs(vol - self.volume) > 0.005) or muted != self.muted:
            self.volume, self.muted = vol, muted
            self.changed.emit(self)

    def set_volume(self, v: float) -> None:
        v = max(0.0, min(1.0, v))
        self.volume = v
        run_fire(["wpctl", "set-volume", self.SINK, f"{v:.2f}"])
        self.changed.emit(self)

    def toggle_mute(self) -> None:
        self.muted = not self.muted
        run_fire(["wpctl", "set-mute", self.SINK, "toggle"])
        self.changed.emit(self)


# ----------------------------------------------------------- brightness ----
class Brightness:
    """Software dimming via the X gamma ramp.

    The PocketTerm35's real backlight is driven by the Pico keyboard firmware
    (its own function keys change it) and exposes no ``/sys/class/backlight``
    node, so the shell cannot and should not try to own it. What this provides
    instead is a *dimming layer* on top: an X gamma ramp that composes with the
    hardware backlight and lets the screen go darker than the firmware's own
    minimum - the case that matters at night in a vehicle.

    Presented in the UI as "Dim", not "Brightness", so it is not mistaken for
    the hardware control on the keyboard.
    """

    def __init__(self) -> None:
        self.changed = Signal()
        self.value = 1.0
        self._read()

    def _read(self) -> None:
        def done(out: str) -> None:
            cur = None
            for line in out.splitlines():
                s = line.strip()
                if s.startswith("Brightness:"):
                    try:
                        cur = float(s.split(":", 1)[1])
                    except ValueError:
                        pass
                    break
            if cur is not None and abs(cur - self.value) > 0.005:
                self.value = cur
                self.changed.emit(self)
        run_async(["xrandr", "--verbose", "--current"], done)

    def set(self, v: float) -> None:
        v = max(BRIGHTNESS_MIN, min(1.0, v))
        self.value = v
        run_fire(["xrandr", "--output", XRANDR_OUTPUT, "--brightness", f"{v:.2f}"])
        self.changed.emit(self)


# ------------------------------------------------------------- bluetooth ---
class Bluetooth:
    POLL_S = 10

    def __init__(self) -> None:
        self.changed = Signal()
        self.enabled = False
        self.present = False
        self.poll()
        GLib.timeout_add_seconds(self.POLL_S, lambda: (self.poll(), True)[1])

    def poll(self) -> None:
        run_async(["rfkill", "--output", "TYPE,SOFT,HARD", "list", "bluetooth"], self._on)

    def _on(self, out: str) -> None:
        # rfkill prints "unblocked"/"blocked" for the soft and hard states.
        # Substring matching is a trap here - "unblocked" contains "blocked" -
        # so compare whole tokens, and require both states to be unblocked.
        toks = out.lower().split()
        present = "bluetooth" in toks
        enabled = present and toks.count("unblocked") >= 2
        if (present, enabled) != (self.present, self.enabled):
            self.present, self.enabled = present, enabled
            self.changed.emit(self)

    def toggle(self) -> None:
        run_fire(["rfkill", "unblock" if not self.enabled else "block", "bluetooth"])
        self.enabled = not self.enabled
        self.changed.emit(self)
        GLib.timeout_add_seconds(1, lambda: (self.poll(), False)[1])


# ---------------------------------------------------------------- power ----
class Power:
    POLL_S = 30

    def __init__(self) -> None:
        self.changed = Signal()
        self.present = False
        self.percent = 0.0
        self.charging = False
        self.poll()
        GLib.timeout_add_seconds(self.POLL_S, lambda: (self.poll(), True)[1])

    def poll(self) -> None:
        run_async(["upower", "-i", "/org/freedesktop/UPower/devices/DisplayDevice"], self._on)

    def _on(self, out: str) -> None:
        pct, charging = self.percent, self.charging
        is_supply, saw_pct, missing_icon = False, False, False
        for line in out.splitlines():
            s = line.strip()
            if s.startswith("percentage:"):
                try:
                    pct = float(s.split(":", 1)[1].strip().rstrip("%"))
                    saw_pct = True
                except ValueError:
                    pass
            elif s.startswith("state:"):
                charging = s.split(":", 1)[1].strip() in ("charging", "fully-charged")
            elif s.startswith("power supply:"):
                is_supply = s.split(":", 1)[1].strip() == "yes"
            elif s.startswith("icon-name:"):
                missing_icon = "battery-missing" in s
        # UPower's DisplayDevice is synthetic and always present, so absence of
        # a real battery has to be inferred: it reports power-supply=no and a
        # battery-missing icon. The PocketTerm35 has no battery today, but this
        # keeps the indicator correct if one is ever added.
        present = is_supply and saw_pct and not missing_icon
        if (present, pct, charging) != (self.present, self.percent, self.charging):
            self.present, self.percent, self.charging = present, pct, charging
            self.changed.emit(self)


# --------------------------------------------------------------- bundle ----
class System:
    """One object holding every provider, passed around the shell."""

    def __init__(self) -> None:
        self.clock = Clock()
        self.net = Network()
        self.audio = Audio()
        self.brightness = Brightness()
        self.bluetooth = Bluetooth()
        self.power = Power()
        self.dnd = False           # shell-local: suppresses notification banners
        self.dnd_changed = Signal()
        #: Set by the shade's Airplane tile. Tracked explicitly rather than
        #: inferred from "wifi is off", so turning Wi-Fi off on its own does
        #: not light up the airplane tile.
        self.airplane = False
        self.airplane_changed = Signal()

    def toggle_dnd(self) -> None:
        self.dnd = not self.dnd
        self.dnd_changed.emit(self.dnd)

    def set_airplane(self, on: bool) -> None:
        """Down every radio (or restore Wi-Fi). No modem on this hardware."""
        self.airplane = on
        self.net.set_wifi(not on)
        if on and self.bluetooth.enabled:
            self.bluetooth.toggle()
        self.airplane_changed.emit(on)
