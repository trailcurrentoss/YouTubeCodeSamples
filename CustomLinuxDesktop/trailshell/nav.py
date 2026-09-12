"""The bottom navigation bar: Back, Home, Recents.

An explicit three-zone bar rather than pure edge gestures. Gestures are elegant
on a phone you use for hours a day, but this is a utility device that people
pick up occasionally, and a visible affordance is learnable in one glance where
an invisible one is not. It costs 30 px of a 480 px screen, which is the right
trade.

Each zone is a third of the screen wide and sits flush against the bottom
edge, so despite being only 3.3 mm tall it is an easy target - a screen edge
stops the finger for you, which is the same reason a Mac menu bar works.

Every action is also on the keyboard (Super, Super+Tab, Alt+Left), because the
PocketTerm35 has a real one and reaching for the screen is often the slower
option.
"""
from __future__ import annotations

import logging

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import Gdk, Gtk

from . import icons, keymap, theme as T, ui, x11

log = logging.getLogger("trailshell.nav")
C = T.C

ICON = T.sp(20)
GAP = T.SP_2
BACK, HOME, RECENTS = "back", "home", "recents"

#: Which shell action each zone triggers, so the hardware-key legends in
#: keymap.HARDWARE_NAV land on the right zone.
ZONE_ACTION = {BACK: "back", HOME: "home", RECENTS: "recents"}


class NavBar:
    def __init__(self, on_back=None, on_home=None, on_recents=None) -> None:
        #: action -> legend, e.g. {"home": "START"}. Populated by the shell
        #: with only those hardware keys whose own accelerator grabbed - not
        #: merely those reachable via a Super shortcut - so the bar can never
        #: advertise a key that does nothing.
        self.legends: dict[str, str] = {}
        self.on_back = on_back
        self.on_home = on_home
        self.on_recents = on_recents
        self._pressed: str | None = None
        self._press_y = 0.0
        self._zone_w = T.SCREEN_W / 3.0

        self.win = ui.make_shell_window(
            T.SCREEN_W, T.NAV_H, 0, T.SCREEN_H - T.NAV_H,
            dock=True, name="trailshell-navbar")
        self.area = Gtk.DrawingArea()
        self.area.add_events(ui.TOUCH_EVENTS)
        self.area.connect("draw", self._draw)
        self.area.connect("button-press-event", self._on_press)
        self.area.connect("button-release-event", self._on_release)
        self.area.connect("touch-event", self._on_touch)
        self.win.add(self.area)
        T.on_theme_changed(lambda *_: self.redraw())

    def show(self) -> None:
        self.win.show_all()
        self.win.realize()
        gw = self.win.get_window()
        if gw is not None:
            X = x11.get()
            X.mark_chrome(gw.get_xid())
            X.set_strut(gw.get_xid(), bottom=T.NAV_H,
                        screen_w=T.SCREEN_W, screen_h=T.SCREEN_H)

    def redraw(self) -> None:
        self.area.queue_draw()

    def set_legends(self, legends: dict[str, str]) -> None:
        self.legends = dict(legends)
        self.redraw()

    # -- painting ---------------------------------------------------------
    def _draw(self, _w, cr) -> bool:
        h = T.NAV_H
        ui.fill_rect(cr, 0, 0, T.SCREEN_W, h, C["bg"])
        ui.fill_rect(cr, 0, 0, T.SCREEN_W, 1, C["border"])

        for i, key in enumerate((BACK, HOME, RECENTS)):
            zx = i * self._zone_w
            if self._pressed == key:
                ui.fill_rect(cr, zx, 1, self._zone_w, h - 1, C["surface_alt"])

            # The home pill is the primary action, so it gets the accent; the
            # other two are secondary and stay in plain ink.
            colour = C["accent"] if key == HOME else C["text_dim"]
            legend = self.legends.get(ZONE_ACTION[key], "")

            if legend:
                # Icon and legend are centred as one unit, so the pairing reads
                # as "this key does this" rather than as two separate marks.
                lw, _ = ui.text_size(cr, legend, T.TEXT_LEGEND, T.WEIGHT_SEMIBOLD)
                total = ICON + GAP + lw
                ix = zx + (self._zone_w - total) / 2
                icons.draw(cr, key, ix, (h - ICON) / 2, ICON, colour)
                ui.draw_text(cr, legend, ix + ICON + GAP, h / 2,
                             T.TEXT_LEGEND, C["text_mute"],
                             T.WEIGHT_SEMIBOLD, valign="middle")
            else:
                icons.draw(cr, key, zx + (self._zone_w - ICON) / 2,
                           (h - ICON) / 2, ICON, colour)
        return False

    # -- input ------------------------------------------------------------
    def _zone(self, x: float) -> str:
        idx = min(2, max(0, int(x // self._zone_w)))
        return (BACK, HOME, RECENTS)[idx]

    def _begin(self, x: float, y_root: float) -> None:
        self._pressed = self._zone(x)
        self._press_y = y_root
        self.redraw()

    def _finish(self, x: float, y_root: float) -> None:
        zone, self._pressed = self._pressed, None
        self.redraw()
        if zone is None:
            return
        # A decisive upward flick anywhere on the bar means Recents, matching
        # the swipe-up-for-multitasking idiom people already know.
        if self._press_y - y_root > T.sp(28):
            self._fire(RECENTS)
            return
        if zone != self._zone(x):
            return          # finger slid off the zone - treat as a cancel
        self._fire(zone)

    def _fire(self, zone: str) -> None:
        cb = {BACK: self.on_back, HOME: self.on_home, RECENTS: self.on_recents}[zone]
        if cb:
            cb()

    def _on_press(self, _w, ev) -> bool:
        if ev.button == 1:
            self._begin(ev.x, ev.y_root)
        return True

    def _on_release(self, _w, ev) -> bool:
        if ev.button == 1:
            self._finish(ev.x, ev.y_root)
        return True

    def _on_touch(self, _w, ev) -> bool:
        if ev.type == Gdk.EventType.TOUCH_BEGIN:
            self._begin(ev.touch.x, ev.touch.y_root)
        elif ev.type == Gdk.EventType.TOUCH_END:
            self._finish(ev.touch.x, ev.touch.y_root)
        elif ev.type == Gdk.EventType.TOUCH_CANCEL:
            self._pressed = None
            self.redraw()
        return True
