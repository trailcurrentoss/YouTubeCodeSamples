"""Recents: the list of open windows, as swipeable cards.

Deliberately a vertical list rather than the horizontal carousel phones use.
A carousel shows one app at a time and trades screen space for a preview; at
640x480 there is no space for a useful preview anyway, and a list shows five
apps at once with a title you can actually read. It also maps cleanly onto
Super+Tab from the physical keyboard, which is how this will mostly be driven.

Swipe a card sideways (or press Delete) to close that window - the same
gesture as dismissing a notification, so there is one thing to learn.
"""
from __future__ import annotations

import logging

from . import _gi  # noqa: F401
from gi.repository import Gdk, GLib, Gtk

from . import apps as apps_mod, icons, theme as T, ui, x11

log = logging.getLogger("trailshell.switcher")
C = T.C

CARD_H = T.sp(72)
SWIPE_COMMIT = T.sp(120)


class WindowCard(Gtk.EventBox):
    def __init__(self, info, catalogue, on_activate, on_close) -> None:
        super().__init__()
        self.info = info
        self.on_activate = on_activate
        self.on_close = on_close
        self.app = self._match_app(info, catalogue)
        self.catalogue = catalogue

        self.set_size_request(-1, CARD_H)
        frame = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_3)
        frame.get_style_context().add_class("ts-switch-card")
        frame.set_border_width(T.SP_2)

        area = Gtk.DrawingArea()
        area.set_size_request(T.sp(44), T.sp(44))
        area.set_valign(Gtk.Align.CENTER)
        area.connect("draw", self._draw_icon)

        text = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        text.set_valign(Gtk.Align.CENTER)
        title = Gtk.Label(label=info.title, xalign=0)
        title.get_style_context().add_class("ts-switch-title")
        title.set_ellipsize(3)
        sub = Gtk.Label(label=(self.app.name if self.app else info.wm_class), xalign=0)
        sub.get_style_context().add_class("ts-xs")
        sub.get_style_context().add_class("ts-mute")
        sub.set_ellipsize(3)
        text.pack_start(title, False, False, 0)
        text.pack_start(sub, False, False, 0)

        close = Gtk.Button()
        close.set_relief(Gtk.ReliefStyle.NONE)
        close.get_style_context().add_class("ts-btn")
        close.get_style_context().add_class("ts-flat")
        close.set_size_request(T.TOUCH_MIN, T.TOUCH_MIN)
        close.set_valign(Gtk.Align.CENTER)
        x_area = Gtk.DrawingArea()
        x_area.set_size_request(T.sp(22), T.sp(22))
        x_area.connect("draw", lambda w, cr: ui.draw_glyph(
            cr, w, "close", C["text_dim"]))
        close.add(x_area)
        close.connect("clicked", lambda *_: self.on_close(self.info.xid))

        frame.pack_start(area, False, False, 0)
        frame.pack_start(text, True, True, 0)
        frame.pack_start(close, False, False, 0)
        self.frame = frame
        self.add(frame)

        self.connect("button-release-event", self._on_click)
        # GesturePan so a vertical flick scrolls the list rather than being
        # swallowed as a horizontal dismiss. See the note in shade.NotifCard.
        self.drag = Gtk.GesturePan.new(self, Gtk.Orientation.HORIZONTAL)
        self.drag.set_propagation_phase(Gtk.PropagationPhase.BUBBLE)
        self.drag.connect("drag-update", self._on_drag)
        self.drag.connect("drag-end", self._on_drag_end)
        self._dragged = False

    @staticmethod
    def _match_app(info, catalogue):
        """Map an X window back to its .desktop entry for a proper icon."""
        wanted = (info.wm_class or "").lower()
        inst = (info.app_id or "").lower()
        for a in catalogue.apps:
            if a.wm_class and a.wm_class in (wanted, inst):
                return a
        for a in catalogue.apps:
            if a.exec_base and a.exec_base.lower() in (wanted, inst):
                return a
        for a in catalogue.apps:
            if a.app_id.removesuffix(".desktop").lower() in (wanted, inst):
                return a
        return None

    def _draw_icon(self, w, cr) -> bool:
        size = min(w.get_allocated_width(), w.get_allocated_height())
        pb = self.catalogue.icon(self.app, size) if self.app else None
        if pb is not None:
            ui.draw_pixbuf_centered(cr, pb, w, size)
        else:
            ui.fill_rect(cr, 0, 0, size, size, C["surface_hi"], T.RADIUS_MD)
            ui.draw_glyph(cr, w, "grid", C["text_mute"], size=size * 0.5)
        return False

    def _on_click(self, _w, _ev) -> bool:
        if not self._dragged:
            self.on_activate(self.info.xid)
        self._dragged = False
        return False

    def _on_drag(self, _g, dx, _dy) -> None:
        if abs(dx) > T.sp(6):
            self._dragged = True
            self.set_margin_start(max(0, int(dx)))
            self.set_opacity(max(0.25, 1.0 - abs(dx) / (SWIPE_COMMIT * 1.6)))

    def _on_drag_end(self, _g, dx, _dy) -> None:
        if abs(dx) >= SWIPE_COMMIT:
            self.on_close(self.info.xid)
            return
        self.set_margin_start(0)
        self.set_opacity(1.0)
        GLib.timeout_add(50, lambda: (setattr(self, "_dragged", False), False)[1])


class Switcher:
    def __init__(self, on_dismissed=None, on_activated=None) -> None:
        self.X = x11.get()
        self.catalogue = apps_mod.get()
        self.on_dismissed = on_dismissed
        self.on_activated = on_activated
        self.windows = []

        self.win = ui.make_shell_window(
            T.SCREEN_W, T.APP_H, 0, T.APP_Y, dock=False, name="trailshell-switcher", keep_above=False)
        self.win.connect("key-press-event", self._on_key)
        self._build()
        T.on_theme_changed(lambda *_: self._rebuild())

    def _build(self) -> None:
        root = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_2)
        root.get_style_context().add_class("ts-launcher")
        root.set_margin_start(T.SP_3); root.set_margin_end(T.SP_3)
        root.set_margin_top(T.SP_3); root.set_margin_bottom(T.SP_3)

        head = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        title = Gtk.Label(label="Recent apps", xalign=0)
        title.get_style_context().add_class("ts-lg")
        title.get_style_context().add_class("ts-semibold")
        self.close_all = Gtk.Button(label="Close all")
        self.close_all.get_style_context().add_class("ts-btn")
        self.close_all.get_style_context().add_class("ts-flat")
        self.close_all.connect("clicked", lambda *_: self._close_all())
        head.pack_start(title, True, True, 0)
        head.pack_end(self.close_all, False, False, 0)
        root.pack_start(head, False, False, 0)

        self.scroller = Gtk.ScrolledWindow()
        self.scroller.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        self.scroller.set_kinetic_scrolling(True)
        self.scroller.set_vexpand(True)
        self.list = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_2)
        self.scroller.add(self.list)
        ui.attach_drag_scroll(self.scroller)
        root.pack_start(self.scroller, True, True, 0)

        self.empty = Gtk.Label(label="No open apps")
        self.empty.get_style_context().add_class("ts-base")
        self.empty.get_style_context().add_class("ts-mute")
        root.pack_start(self.empty, False, False, T.SP_5)

        self.win.add(root)

    # -- content ----------------------------------------------------------
    def _rebuild(self) -> None:
        for child in self.list.get_children():
            self.list.remove(child)
        # _NET_CLIENT_LIST_STACKING is bottom-to-top; reverse so the app you
        # were most recently in is the first card.
        self.windows = list(reversed(self.X.list_windows()))
        for info in self.windows:
            self.list.pack_start(
                WindowCard(info, self.catalogue, self._activate, self._close),
                False, False, 0)
        self.list.show_all()
        self.empty.set_visible(not self.windows)
        self.scroller.set_visible(bool(self.windows))
        self.close_all.set_visible(bool(self.windows))

    # -- actions ----------------------------------------------------------
    def _activate(self, xid: int) -> None:
        self.hide()
        if self.on_activated:
            self.on_activated()
        self.X.activate(xid)

    def _close(self, xid: int) -> None:
        self.X.close(xid)
        GLib.timeout_add(250, lambda: (self._rebuild(), False)[1])

    def _close_all(self) -> None:
        for info in list(self.windows):
            self.X.close(info.xid)
        GLib.timeout_add(400, lambda: (self._rebuild(), False)[1])

    def cycle(self) -> None:
        """Super+Tab: jump straight to the previous app without showing UI."""
        wins = list(reversed(self.X.list_windows()))
        if len(wins) >= 2:
            self.X.activate(wins[1].xid)
        elif wins:
            self.X.activate(wins[0].xid)

    # -- lifecycle --------------------------------------------------------
    def show(self) -> None:
        self._rebuild()
        self.win.show_all()
        self.empty.set_visible(not self.windows)
        self.scroller.set_visible(bool(self.windows))
        self.close_all.set_visible(bool(self.windows))
        self.win.present()
        self.win.realize()
        gw = self.win.get_window()
        if gw is not None:
            x11.get().mark_chrome(gw.get_xid())

    def hide(self) -> None:
        self.win.hide()
        if self.on_dismissed:
            self.on_dismissed()

    @property
    def visible(self) -> bool:
        return self.win.get_visible()

    def _on_key(self, _w, ev) -> bool:
        action = ui.gamepad_action(ev.keyval)
        if action:
            if action == "select" and self.windows:
                self._activate(self.windows[0].xid)
            elif action == "back":
                self.hide()
            elif action == "close_item" and self.windows:
                self._close(self.windows[0].xid)
            elif action in ("page_up", "page_down"):
                adj = self.scroller.get_vadjustment()
                if adj:
                    step = adj.get_page_size() * (1 if action == "page_down" else -1)
                    adj.set_value(adj.get_value() + step)
            else:
                return False
            return True
        name = Gdk.keyval_name(ev.keyval)
        if name == "Escape":
            self.hide()
            return True
        if name in ("Delete", "BackSpace") and self.windows:
            self._close(self.windows[0].xid)
            return True
        return False
