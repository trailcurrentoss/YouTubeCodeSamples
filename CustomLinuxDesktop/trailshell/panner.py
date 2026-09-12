"""The pan control for windows that are taller than the screen.

Some applications refuse to shrink below a minimum that is larger than the
whole 640x410 canvas. GIMP is the clear case: its main window clamps at
640x503 however it is asked, so 93 px of it - including the bottom edge where
buttons live - sits off screen.

On a normal desktop you would drag the window up to reach the rest. TrailShell
deliberately forbids dragging, so it owes the user a replacement: this control.

It appears only while the focused window is actually oversized, sits against
the right edge where it covers the least, and is deliberately narrow. Both
halves are still ~7 mm tall, so they meet the touch-target floor even though
the strip is slim.
"""
from __future__ import annotations

from . import _gi  # noqa: F401
from gi.repository import Gdk, Gtk

from . import icons, theme as T, ui, x11

C = T.C

WIDTH = T.sp(40)
HALF = T.sp(64)          # 7.1 mm per button - the standard touch target
HEIGHT = HALF * 2
UP, DOWN = "up", "down"


class Panner:
    def __init__(self, on_pan=None) -> None:
        #: Called with -1 (up) or +1 (down) in pan steps.
        self.on_pan = on_pan
        self._pressed: str | None = None

        self.win = ui.make_shell_window(
            WIDTH, HEIGHT,
            T.SCREEN_W - WIDTH, T.APP_Y + (T.APP_H - HEIGHT) // 2,
            dock=False, name="trailshell-panner", keep_above=True)
        self.area = Gtk.DrawingArea()
        self.area.add_events(ui.TOUCH_EVENTS)
        self.area.connect("draw", self._draw)
        self.area.connect("button-press-event", self._on_press)
        self.area.connect("button-release-event", self._on_release)
        self.area.connect("touch-event", self._on_touch)
        self.win.add(self.area)
        T.on_theme_changed(lambda *_: self.area.queue_draw())

    # -- painting ---------------------------------------------------------
    def _draw(self, _w, cr) -> bool:
        r = T.RADIUS_MD
        ui.fill_rect(cr, 0, 0, WIDTH, HEIGHT, C["surface_alt"], r)
        ui.stroke_rect(cr, 0.5, 0.5, WIDTH - 1, HEIGHT - 1,
                       C["border_strong"], r, 1.0)
        for i, (key, glyph) in enumerate(((UP, "chevron_up"), (DOWN, "chevron_down"))):
            y = i * HALF
            if self._pressed == key:
                ui.fill_rect(cr, 2, y + 2, WIDTH - 4, HALF - 4,
                             C["surface_hi"], T.RADIUS_SM)
            size = T.sp(24)
            icons.draw(cr, glyph, (WIDTH - size) / 2, y + (HALF - size) / 2,
                       size, C["accent"])
        ui.fill_rect(cr, T.SP_2, HALF, WIDTH - T.SP_2 * 2, 1, C["border"])
        return False

    # -- input ------------------------------------------------------------
    def _zone(self, y: float) -> str:
        return UP if y < HALF else DOWN

    def _begin(self, y: float) -> None:
        self._pressed = self._zone(y)
        self.area.queue_draw()

    def _finish(self, y: float) -> None:
        zone, self._pressed = self._pressed, None
        self.area.queue_draw()
        if zone is None or zone != self._zone(y):
            return
        if self.on_pan:
            self.on_pan(-1 if zone == UP else +1)

    def _on_press(self, _w, ev) -> bool:
        if ev.button == 1:
            self._begin(ev.y)
        return True

    def _on_release(self, _w, ev) -> bool:
        if ev.button == 1:
            self._finish(ev.y)
        return True

    def _on_touch(self, _w, ev) -> bool:
        if ev.type == Gdk.EventType.TOUCH_BEGIN:
            self._begin(ev.touch.y)
        elif ev.type == Gdk.EventType.TOUCH_END:
            self._finish(ev.touch.y)
        elif ev.type == Gdk.EventType.TOUCH_CANCEL:
            self._pressed = None
            self.area.queue_draw()
        return True

    # -- lifecycle --------------------------------------------------------
    def set_visible(self, visible: bool) -> None:
        if visible == self.win.get_visible():
            return
        if visible:
            self.win.show_all()
            self.win.realize()
            gw = self.win.get_window()
            if gw is not None:
                x11.get().mark_chrome(gw.get_xid())
        else:
            self.win.hide()
