"""Cairo-drawn vector glyphs for the shell.

Why not the system icon theme? At the sizes the status and nav bars use
(20-30 px) a rasterised theme icon on a 228 PPI panel is a muddy smear, and
Adwaita/PiXflat cover maybe half of what a phone-style shell needs. These are
drawn as paths in a unit box and scaled at draw time, so they stay razor sharp
at any size and all pick up the TrailCurrent palette from the caller.

Every glyph is authored inside the unit square (0,0)-(1,1) and centred there.
"""
from __future__ import annotations

import math
from typing import Callable

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import Gdk

#: Stroke weight in unit space. 0.085 lands on ~2 px at 24 px, which is the
#: thinnest line that still reads cleanly on this panel.
LW = 0.085

_REGISTRY: dict[str, Callable] = {}


def glyph(name: str):
    def deco(fn):
        _REGISTRY[name] = fn
        return fn
    return deco


def names() -> list[str]:
    return sorted(_REGISTRY)


def rgba(color: str) -> tuple[float, float, float, float]:
    c = Gdk.RGBA()
    c.parse(color)
    return c.red, c.green, c.blue, c.alpha


def draw(cr, name: str, x: float, y: float, size: float,
         color: str = "#ffffff", alpha: float = 1.0, **kw) -> None:
    """Draw glyph ``name`` with its top-left at (x, y) in a size x size box."""
    fn = _REGISTRY.get(name)
    if fn is None:
        return
    r, g, b, a = rgba(color)
    cr.save()
    # Drop any current point left behind by the caller: arc() would otherwise
    # draw a connecting line from it into the glyph.
    cr.new_path()
    cr.translate(x, y)
    cr.scale(size, size)
    cr.set_source_rgba(r, g, b, a * alpha)
    cr.set_line_width(LW)
    cr.set_line_cap(1)   # round
    cr.set_line_join(1)  # round
    try:
        fn(cr, **kw)
    finally:
        cr.restore()


# ---------------------------------------------------------------- status ---
@glyph("wifi")
def _wifi(cr, level: int = 3, **_):
    """Three arcs plus a dot; dimmed arcs show signal strength."""
    cx, cy = 0.5, 0.80
    ink = cr.get_source()
    for i, r in enumerate((0.20, 0.38, 0.56)):
        cr.push_group()
        cr.set_source(ink)
        cr.set_line_width(LW)
        cr.set_line_cap(1)
        cr.arc(cx, cy, r, math.radians(-145), math.radians(-35))
        cr.stroke()
        cr.pop_group_to_source()
        # Arcs above the current strength stay visible but faded, so the icon
        # reads as "wifi, weak" rather than changing shape as signal drops.
        cr.paint_with_alpha(1.0 if i < level else 0.25)
    cr.set_source(ink)
    cr.arc(cx, cy, LW * 1.05, 0, 2 * math.pi)
    cr.fill()


@glyph("wifi_off")
def _wifi_off(cr, **_):
    _wifi(cr, level=0)
    cr.move_to(0.16, 0.16)
    cr.line_to(0.84, 0.84)
    cr.stroke()


@glyph("cell")
def _cell(cr, level: int = 4, **_):
    ink = cr.get_source()
    for i in range(4):
        h = 0.22 + i * 0.20
        cr.push_group()
        cr.set_source(ink)
        cr.rectangle(0.10 + i * 0.22, 0.90 - h, 0.13, h)
        cr.fill()
        cr.pop_group_to_source()
        cr.paint_with_alpha(1.0 if i < level else 0.25)
    cr.set_source(ink)


@glyph("volume")
def _volume(cr, level: int = 2, **_):
    cr.move_to(0.10, 0.36)
    cr.line_to(0.26, 0.36)
    cr.line_to(0.46, 0.16)
    cr.line_to(0.46, 0.84)
    cr.line_to(0.26, 0.64)
    cr.line_to(0.10, 0.64)
    cr.close_path()
    cr.fill()
    for i, r in enumerate((0.14, 0.28, 0.42)):
        if i >= level:
            break
        cr.arc(0.50, 0.50, r + 0.06, math.radians(-52), math.radians(52))
        cr.stroke()


@glyph("volume_mute")
def _volume_mute(cr, **_):
    _volume(cr, level=0)
    cr.move_to(0.60, 0.34)
    cr.line_to(0.90, 0.66)
    cr.move_to(0.90, 0.34)
    cr.line_to(0.60, 0.66)
    cr.stroke()


@glyph("battery")
def _battery(cr, level: float = 1.0, charging: bool = False, **_):
    x, y, w, h = 0.06, 0.28, 0.76, 0.44
    r = 0.06
    _rounded(cr, x, y, w, h, r)
    cr.stroke()
    # terminal nub
    cr.rectangle(0.86, 0.42, 0.08, 0.16)
    cr.fill()
    pad = LW * 0.9
    fill_w = max(0.0, (w - 2 * pad) * max(0.0, min(1.0, level)))
    if fill_w > 0:
        _rounded(cr, x + pad, y + pad, fill_w, h - 2 * pad, r * 0.5)
        cr.fill()
    if charging:
        # Punch the bolt out of the fill so it reads at 20 px instead of
        # disappearing into same-coloured ink.
        cr.save()
        cr.set_operator(0)  # CLEAR
        cr.set_line_width(LW * 1.6)
        cr.move_to(0.46, 0.24)
        cr.line_to(0.30, 0.54)
        cr.line_to(0.42, 0.54)
        cr.line_to(0.36, 0.80)
        cr.line_to(0.56, 0.46)
        cr.line_to(0.44, 0.46)
        cr.close_path()
        cr.fill_preserve()
        cr.stroke()
        cr.restore()


@glyph("bluetooth")
def _bluetooth(cr, **_):
    cr.move_to(0.30, 0.30)
    cr.line_to(0.70, 0.70)
    cr.line_to(0.50, 0.88)
    cr.line_to(0.50, 0.12)
    cr.line_to(0.70, 0.30)
    cr.line_to(0.30, 0.70)
    cr.stroke()


@glyph("brightness")
def _brightness(cr, **_):
    cr.arc(0.5, 0.5, 0.20, 0, 2 * math.pi)
    cr.stroke()
    for i in range(8):
        a = i * math.pi / 4
        cr.move_to(0.5 + math.cos(a) * 0.32, 0.5 + math.sin(a) * 0.32)
        cr.line_to(0.5 + math.cos(a) * 0.44, 0.5 + math.sin(a) * 0.44)
    cr.stroke()


@glyph("moon")
def _moon(cr, **_):
    cr.arc(0.52, 0.50, 0.38, math.radians(60), math.radians(300))
    cr.arc_negative(0.34, 0.50, 0.40, math.radians(300), math.radians(60))
    cr.close_path()
    cr.fill()


@glyph("airplane")
def _airplane(cr, **_):
    cr.move_to(0.50, 0.06)
    cr.line_to(0.58, 0.20)
    cr.line_to(0.58, 0.42)
    cr.line_to(0.94, 0.62)
    cr.line_to(0.94, 0.72)
    cr.line_to(0.58, 0.62)
    cr.line_to(0.58, 0.80)
    cr.line_to(0.70, 0.90)
    cr.line_to(0.70, 0.96)
    cr.line_to(0.50, 0.90)
    cr.line_to(0.30, 0.96)
    cr.line_to(0.30, 0.90)
    cr.line_to(0.42, 0.80)
    cr.line_to(0.42, 0.62)
    cr.line_to(0.06, 0.72)
    cr.line_to(0.06, 0.62)
    cr.line_to(0.42, 0.42)
    cr.line_to(0.42, 0.20)
    cr.close_path()
    cr.fill()


# ------------------------------------------------------------- nav bar -----
@glyph("back")
def _back(cr, **_):
    cr.move_to(0.62, 0.16)
    cr.line_to(0.34, 0.50)
    cr.line_to(0.62, 0.84)
    cr.stroke()


@glyph("home")
def _home(cr, **_):
    """A pill, matching the modern iOS/Android home indicator."""
    _rounded(cr, 0.16, 0.40, 0.68, 0.20, 0.10)
    cr.fill()


@glyph("recents")
def _recents(cr, **_):
    _rounded(cr, 0.22, 0.22, 0.56, 0.56, 0.12)
    cr.stroke()


# -------------------------------------------------------------- actions ----
@glyph("close")
def _close(cr, **_):
    cr.move_to(0.24, 0.24); cr.line_to(0.76, 0.76)
    cr.move_to(0.76, 0.24); cr.line_to(0.24, 0.76)
    cr.stroke()


@glyph("search")
def _search(cr, **_):
    cr.arc(0.44, 0.44, 0.28, 0, 2 * math.pi)
    cr.stroke()
    cr.move_to(0.64, 0.64)
    cr.line_to(0.86, 0.86)
    cr.stroke()


@glyph("settings")
def _settings(cr, **_):
    cr.arc(0.5, 0.5, 0.16, 0, 2 * math.pi)
    cr.stroke()
    for i in range(6):
        a = i * math.pi / 3
        cr.save()
        cr.translate(0.5, 0.5)
        cr.rotate(a)
        _rounded(cr, -0.075, -0.44, 0.15, 0.18, 0.05)
        cr.fill()
        cr.restore()
    cr.arc(0.5, 0.5, 0.30, 0, 2 * math.pi)
    cr.stroke()


@glyph("grid")
def _grid(cr, **_):
    for r in range(3):
        for c in range(3):
            _rounded(cr, 0.12 + c * 0.30, 0.12 + r * 0.30, 0.16, 0.16, 0.04)
            cr.fill()


@glyph("power")
def _power(cr, **_):
    cr.arc(0.5, 0.55, 0.32, math.radians(-60), math.radians(240))
    cr.stroke()
    cr.move_to(0.5, 0.10)
    cr.line_to(0.5, 0.46)
    cr.stroke()


@glyph("rotate")
def _rotate(cr, **_):
    cr.arc(0.5, 0.5, 0.32, math.radians(30), math.radians(300))
    cr.stroke()
    cr.move_to(0.72, 0.06); cr.line_to(0.80, 0.28); cr.line_to(0.58, 0.30)
    cr.close_path(); cr.fill()


@glyph("logout")
def _logout(cr, **_):
    """A door with an arrow leaving it - reads at 26 px where a figure does not."""
    cr.move_to(0.52, 0.14)
    cr.line_to(0.16, 0.14)
    cr.line_to(0.16, 0.86)
    cr.line_to(0.52, 0.86)
    cr.stroke()
    cr.move_to(0.42, 0.50)
    cr.line_to(0.86, 0.50)
    cr.stroke()
    cr.move_to(0.70, 0.32)
    cr.line_to(0.88, 0.50)
    cr.line_to(0.70, 0.68)
    cr.stroke()


@glyph("bell")
def _bell(cr, **_):
    cr.move_to(0.22, 0.68)
    cr.curve_to(0.30, 0.60, 0.26, 0.46, 0.28, 0.38)
    cr.curve_to(0.32, 0.18, 0.68, 0.18, 0.72, 0.38)
    cr.curve_to(0.74, 0.46, 0.70, 0.60, 0.78, 0.68)
    cr.close_path()
    cr.fill()
    cr.arc(0.5, 0.80, 0.09, 0, math.pi)
    cr.fill()


@glyph("bell_off")
def _bell_off(cr, **_):
    _bell(cr)
    cr.save()
    cr.set_operator(0)  # CLEAR - punch the slash through the bell
    cr.set_line_width(LW * 2.4)
    cr.move_to(0.14, 0.10); cr.line_to(0.90, 0.86); cr.stroke()
    cr.restore()
    cr.move_to(0.18, 0.12); cr.line_to(0.86, 0.84); cr.stroke()


@glyph("keyboard")
def _keyboard(cr, **_):
    _rounded(cr, 0.06, 0.26, 0.88, 0.48, 0.08)
    cr.stroke()
    for r in range(2):
        for c in range(4):
            cr.rectangle(0.16 + c * 0.18, 0.38 + r * 0.14, 0.09, 0.07)
    cr.fill()
    cr.rectangle(0.32, 0.60, 0.36, 0.07)
    cr.fill()


@glyph("clear")
def _clear(cr, **_):
    _rounded(cr, 0.18, 0.26, 0.64, 0.60, 0.08)
    cr.stroke()
    cr.move_to(0.10, 0.22); cr.line_to(0.90, 0.22); cr.stroke()
    cr.move_to(0.40, 0.16); cr.line_to(0.60, 0.16); cr.stroke()


@glyph("chevron_down")
def _chev_down(cr, **_):
    cr.move_to(0.20, 0.38); cr.line_to(0.50, 0.66); cr.line_to(0.80, 0.38)
    cr.stroke()


@glyph("chevron_up")
def _chev_up(cr, **_):
    cr.move_to(0.20, 0.62); cr.line_to(0.50, 0.34); cr.line_to(0.80, 0.62)
    cr.stroke()


@glyph("lock")
def _lock(cr, **_):
    _rounded(cr, 0.18, 0.44, 0.64, 0.44, 0.08)
    cr.fill()
    cr.arc(0.5, 0.44, 0.20, math.pi, 2 * math.pi)
    cr.stroke()


# ------------------------------------------------------------- helpers -----
def _rounded(cr, x: float, y: float, w: float, h: float, r: float) -> None:
    r = min(r, w / 2, h / 2)
    cr.new_sub_path()
    cr.arc(x + w - r, y + r, r, -math.pi / 2, 0)
    cr.arc(x + w - r, y + h - r, r, 0, math.pi / 2)
    cr.arc(x + r, y + h - r, r, math.pi / 2, math.pi)
    cr.arc(x + r, y + r, r, math.pi, 3 * math.pi / 2)
    cr.close_path()


def rounded_rect(cr, x, y, w, h, r) -> None:
    """Public rounded-rect path helper for other shell widgets."""
    r = min(r, w / 2, h / 2)
    cr.new_sub_path()
    cr.arc(x + w - r, y + r, r, -math.pi / 2, 0)
    cr.arc(x + w - r, y + h - r, r, 0, math.pi / 2)
    cr.arc(x + r, y + h - r, r, math.pi / 2, math.pi)
    cr.arc(x + r, y + r, r, math.pi, 3 * math.pi / 2)
    cr.close_path()
