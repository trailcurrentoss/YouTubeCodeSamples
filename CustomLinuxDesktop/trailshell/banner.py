"""Heads-up notification banners.

Drops in just under the status bar, sized for a glance and gone in a few
seconds - the phone pattern, not the desktop corner-toast pattern, because on a
640x480 screen a toast in the corner is either unreadably small or covers the
app you are looking at.

Tapping the banner opens the shade (where the notification and its history
live); swiping it up dismisses it. Critical notifications do not auto-expire.
"""
from __future__ import annotations

from . import _gi  # noqa: F401
from gi.repository import Gdk, GLib, Gtk

from . import icons, theme as T, ui, x11

C = T.C

MARGIN = T.SP_2
#: Even a critical notification's banner comes down eventually. The spec lets
#: an app ask for a banner that never expires, but on a 640x480 screen that is
#: a permanent 84 px obstruction over whatever you are actually doing. The
#: notification itself persists in the shade with its urgent stripe, so nothing
#: is lost - only the heads-up goes away.
CRITICAL_HOLD_MS = 12000
WIDTH = T.SCREEN_W - MARGIN * 2
SWIPE_DISMISS = T.sp(26)


class Banner:
    def __init__(self, on_tap=None, on_expired=None) -> None:
        self.on_tap = on_tap
        self.on_expired = on_expired
        self._queue: list = []
        self._current = None
        self._timer = None

        self.win = ui.make_shell_window(
            WIDTH, T.sp(84), MARGIN, T.STATUS_H + MARGIN,
            dock=False, name="trailshell-banner")

        outer = Gtk.EventBox()
        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_2)
        box.get_style_context().add_class("ts-banner")

        self.mark = Gtk.DrawingArea()
        self.mark.set_size_request(T.sp(28), T.sp(28))
        self.mark.set_valign(Gtk.Align.START)
        self.mark.set_margin_top(T.sp(2))
        self.mark.connect("draw", self._draw_mark)

        text = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        text.set_valign(Gtk.Align.CENTER)
        self.lbl_app = Gtk.Label(xalign=0)
        self.lbl_app.get_style_context().add_class("ts-notif-app")
        self.lbl_app.set_ellipsize(3)
        self.lbl_summary = Gtk.Label(xalign=0)
        self.lbl_summary.get_style_context().add_class("ts-notif-title")
        self.lbl_summary.set_ellipsize(3)
        self.lbl_body = Gtk.Label(xalign=0)
        self.lbl_body.get_style_context().add_class("ts-notif-body")
        self.lbl_body.set_ellipsize(3)
        text.pack_start(self.lbl_app, False, False, 0)
        text.pack_start(self.lbl_summary, False, False, 0)
        text.pack_start(self.lbl_body, False, False, 0)

        box.pack_start(self.mark, False, False, 0)
        box.pack_start(text, True, True, 0)
        outer.add(box)
        self.win.add(outer)

        outer.connect("button-release-event", self._on_click)
        pan = Gtk.GesturePan.new(outer, Gtk.Orientation.VERTICAL)
        pan.connect("drag-end", self._on_pan_end)
        self._pan = pan

    def _draw_mark(self, w, cr) -> bool:
        n = self._current
        colour = C["danger"] if (n and n.is_critical) else C["accent"]
        ui.draw_glyph(cr, w, "bell", colour)
        return False

    # -- queue ------------------------------------------------------------
    def post(self, notif) -> None:
        self._queue.append(notif)
        if self._current is None:
            self._advance()

    def _advance(self) -> None:
        self._cancel_timer()
        if not self._queue:
            self._current = None
            self.win.hide()
            return
        n = self._queue.pop(0)
        self._current = n
        self.lbl_app.set_text(n.app_name)
        self.lbl_summary.set_text(n.summary)
        self.lbl_body.set_text(n.body)
        self.lbl_body.set_visible(bool(n.body))
        self.mark.queue_draw()
        self.win.show_all()
        self.lbl_body.set_visible(bool(n.body))
        self.win.present()
        self.win.realize()
        gw = self.win.get_window()
        if gw is not None:
            x11.get().mark_chrome(gw.get_xid())
        if n.is_critical:
            timeout = CRITICAL_HOLD_MS
        else:
            timeout = n.timeout_ms if n.timeout_ms and n.timeout_ms > 0 else 4500
        self._timer = GLib.timeout_add(timeout, self._expire)

    def _expire(self) -> bool:
        self._timer = None
        if self._current and self.on_expired:
            self.on_expired(self._current)
        self._advance()
        return False

    def _cancel_timer(self) -> None:
        if self._timer is not None:
            GLib.source_remove(self._timer)
            self._timer = None

    def dismiss(self) -> None:
        self._cancel_timer()
        self._advance()

    def hide_all(self) -> None:
        self._cancel_timer()
        self._queue.clear()
        self._current = None
        self.win.hide()

    # -- input ------------------------------------------------------------
    def _on_click(self, _w, _ev) -> bool:
        n = self._current
        self.dismiss()
        if self.on_tap:
            self.on_tap(n)
        return True

    def _on_pan_end(self, _g, _dx, dy) -> None:
        if dy <= -SWIPE_DISMISS:      # flick upward
            self.dismiss()
