"""The 36 px status bar, and the grab handle for the swipe-down shade.

Everything is drawn with Cairo rather than assembled from GTK widgets: at this
size the layout is a dozen glyphs and two strings, and drawing it directly is
both sharper and far easier to keep on a strict pixel budget.

The bar doubles as the shade's gesture surface. Dragging down from it pulls the
shade with your finger (the shade tracks the drag 1:1 and settles based on
release velocity); a plain tap opens the shade outright, because a precise
36 px-tall swipe is a genuinely annoying target on a 3.5" panel.
"""
from __future__ import annotations

import logging

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import Gdk, GLib, Gtk

from . import icons, theme as T, ui, x11

log = logging.getLogger("trailshell.bar")
C = T.C

PAD_X = T.SP_3
ICON = T.sp(20)
ICON_GAP = T.sp(10)


class StatusBar:
    def __init__(self, system, on_tap=None,
                 on_pull_begin=None, on_pull_update=None, on_pull_end=None) -> None:
        self.sys = system
        self.on_tap = on_tap
        self.on_pull_begin = on_pull_begin
        self.on_pull_update = on_pull_update
        self.on_pull_end = on_pull_end
        self.notif_count = 0

        self.win = ui.make_shell_window(T.SCREEN_W, T.STATUS_H, 0, 0,
                                        dock=True, name="trailshell-statusbar")
        self.area = Gtk.DrawingArea()
        self.area.add_events(ui.TOUCH_EVENTS)
        self.area.connect("draw", self._draw)
        self.win.add(self.area)

        # Gesture state
        self._drag_y0 = 0.0
        self._drag_t0 = 0
        self._drag_last = 0.0
        self._dragging = False
        self._moved = False

        self.area.connect("button-press-event", self._on_press)
        self.area.connect("motion-notify-event", self._on_motion)
        self.area.connect("button-release-event", self._on_release)
        self.area.connect("touch-event", self._on_touch)

        for src in (system.clock, system.net, system.audio,
                    system.bluetooth, system.power):
            src.changed.connect(lambda *_: self.redraw())
        system.dnd_changed.connect(lambda *_: self.redraw())

    # -- lifecycle --------------------------------------------------------
    def show(self) -> None:
        self.win.show_all()
        self.win.realize()
        gdk_win = self.win.get_window()
        if gdk_win is not None:
            xid = gdk_win.get_xid()
            X = x11.get()
            X.mark_chrome(xid)
            X.set_strut(xid, top=T.STATUS_H, screen_w=T.SCREEN_W, screen_h=T.SCREEN_H)

    def redraw(self) -> None:
        self.area.queue_draw()

    def set_notification_count(self, n: int) -> None:
        if n != self.notif_count:
            self.notif_count = n
            self.redraw()

    # -- painting ---------------------------------------------------------
    def _draw(self, _w, cr) -> bool:
        h = T.STATUS_H
        mid = h / 2

        ui.fill_rect(cr, 0, 0, T.SCREEN_W, h, C["bg"])
        ui.fill_rect(cr, 0, h - 1, T.SCREEN_W, 1, C["surface"])

        # --- left: clock, date, notification indicator ---
        x = PAD_X
        w, _ = ui.draw_text(cr, self.sys.clock.time_str, x, mid, T.TEXT_XS,
                            C["text"], T.WEIGHT_SEMIBOLD, valign="middle")
        x += w + T.SP_2
        w, _ = ui.draw_text(cr, self.sys.clock.date_str, x, mid, T.TEXT_XS,
                            C["text_dim"], T.WEIGHT_NORMAL, valign="middle")
        x += w + T.SP_3

        if self.notif_count:
            icons.draw(cr, "bell", x, mid - ICON / 2, ICON, C["accent"])
            x += ICON + T.SP_1
            label = str(self.notif_count) if self.notif_count < 10 else "9+"
            ui.draw_text(cr, label, x, mid, T.TEXT_XS, C["accent"],
                         T.WEIGHT_SEMIBOLD, valign="middle")

        # --- right: status glyphs, laid out right-to-left ---
        x = T.SCREEN_W - PAD_X
        iy = mid - ICON / 2

        if self.sys.power.present:
            x -= ICON + T.sp(4)
            lvl = self.sys.power.percent / 100.0
            colour = C["danger"] if lvl < 0.15 and not self.sys.power.charging \
                else (C["accent"] if self.sys.power.charging else C["text"])
            icons.draw(cr, "battery", x, iy, ICON, colour,
                       level=lvl, charging=self.sys.power.charging)
            x -= T.SP_1
            pct = f"{self.sys.power.percent:.0f}%"
            tw, _ = ui.text_size(cr, pct, T.TEXT_XS, T.WEIGHT_MEDIUM)
            x -= tw
            ui.draw_text(cr, pct, x, mid, T.TEXT_XS, colour,
                         T.WEIGHT_MEDIUM, valign="middle")
            x -= ICON_GAP

        x -= ICON
        if not self.sys.net.wifi_enabled:
            icons.draw(cr, "wifi_off", x, iy, ICON, C["text_mute"])
        elif not self.sys.net.connected:
            icons.draw(cr, "wifi", x, iy, ICON, C["text_mute"], level=0)
        else:
            icons.draw(cr, "wifi", x, iy, ICON, C["text"], level=self.sys.net.bars)
        x -= ICON_GAP

        x -= ICON
        if self.sys.audio.muted or self.sys.audio.level == 0:
            icons.draw(cr, "volume_mute", x, iy, ICON, C["text_mute"])
        else:
            icons.draw(cr, "volume", x, iy, ICON, C["text"],
                       level=self.sys.audio.level)

        if self.sys.bluetooth.enabled:
            x -= ICON_GAP + ICON
            icons.draw(cr, "bluetooth", x, iy, ICON, C["text"])

        if self.sys.dnd:
            x -= ICON_GAP + ICON
            icons.draw(cr, "moon", x, iy, ICON, C["accent"])

        return False

    # -- gestures ---------------------------------------------------------
    #: A drag has to travel this far before we stop treating it as a tap.
    TAP_SLOP = T.sp(8)

    def _begin(self, y_root: float, t: int) -> None:
        self._drag_y0 = y_root
        self._drag_last = y_root
        self._drag_t0 = t
        self._dragging = True
        self._moved = False
        if self.on_pull_begin:
            self.on_pull_begin()

    def _update(self, y_root: float) -> None:
        if not self._dragging:
            return
        dy = y_root - self._drag_y0
        if abs(dy) > self.TAP_SLOP:
            self._moved = True
        self._drag_last = y_root
        if self.on_pull_update:
            self.on_pull_update(max(0.0, dy))

    def _finish(self, y_root: float, t: int) -> None:
        if not self._dragging:
            return
        self._dragging = False
        dy = max(0.0, y_root - self._drag_y0)
        dt = max(1, t - self._drag_t0)
        velocity = dy / dt          # px per ms
        if not self._moved:
            # Treat it as a tap: open the shade outright.
            if self.on_tap:
                self.on_tap()
            elif self.on_pull_end:
                self.on_pull_end(dy, 999.0)
            return
        if self.on_pull_end:
            self.on_pull_end(dy, velocity)

    def _on_press(self, _w, ev) -> bool:
        if ev.button == 1:
            self._begin(ev.y_root, ev.time)
        return True

    def _on_motion(self, _w, ev) -> bool:
        if self._dragging:
            self._update(ev.y_root)
        return True

    def _on_release(self, _w, ev) -> bool:
        if ev.button == 1:
            self._finish(ev.y_root, ev.time)
        return True

    def _on_touch(self, _w, ev) -> bool:
        # GTK delivers the whole touch sequence to the widget that saw BEGIN,
        # so the drag keeps tracking after the finger leaves the 36 px bar.
        t = ev.type
        if t == Gdk.EventType.TOUCH_BEGIN:
            self._begin(ev.touch.y_root, ev.touch.time)
        elif t == Gdk.EventType.TOUCH_UPDATE:
            self._update(ev.touch.y_root)
        elif t in (Gdk.EventType.TOUCH_END, Gdk.EventType.TOUCH_CANCEL):
            self._finish(ev.touch.y_root, ev.touch.time)
        return True
