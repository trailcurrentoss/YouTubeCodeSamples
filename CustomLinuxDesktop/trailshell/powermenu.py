"""Power sheet: shut down, restart, log out.

The Raspberry Pi desktop's own `pishutdown` dialog is sized for a normal
desktop and looks wrong at 640x480, and the LXDE helper the shade originally
called (`lxde-pi-shutdown-helper`) is not installed on this image at all - the
button silently did nothing. So the shell owns this.

Actions go through logind (`systemctl poweroff|reboot`), which polkit permits
for a local active session without a password.

Shut down is styled as destructive and is *not* the first item, because the
top of a list is where a thumb lands by default and an accidental power-off on
a device with no battery means an unclean stop.
"""
from __future__ import annotations

import logging

from . import _gi  # noqa: F401
from gi.repository import Gdk, Gtk

from . import icons, providers, theme as T, ui, x11

log = logging.getLogger("trailshell.powermenu")
C = T.C


class PowerMenu:
    def __init__(self, on_logout=None) -> None:
        self.on_logout = on_logout
        self.win = ui.make_shell_window(
            T.SCREEN_W, T.APP_H, 0, T.APP_Y,
            dock=False, name="trailshell-power", keep_above=True)
        self.win.connect("key-press-event", self._on_key)
        self._build()
        T.on_theme_changed(lambda *_: self.win.queue_draw())

    def _build(self) -> None:
        root = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_3)
        root.get_style_context().add_class("ts-launcher")
        root.set_margin_start(T.SP_4); root.set_margin_end(T.SP_4)
        root.set_margin_top(T.SP_4); root.set_margin_bottom(T.SP_4)
        root.set_valign(Gtk.Align.CENTER)

        title = Gtk.Label(label="Power", xalign=0)
        title.get_style_context().add_class("ts-xl")
        title.get_style_context().add_class("ts-semibold")
        root.pack_start(title, False, False, 0)

        def row(glyph, label, colour, on_click, destructive=False):
            btn = Gtk.Button()
            btn.get_style_context().add_class("ts-btn")
            if destructive:
                btn.get_style_context().add_class("ts-destructive")
            btn.set_size_request(-1, T.ROW_H)
            box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_3)
            area = Gtk.DrawingArea()
            area.set_size_request(T.sp(26), T.sp(26))
            area.set_valign(Gtk.Align.CENTER)
            area.connect("draw", lambda w, cr: ui.draw_glyph(cr, w, glyph, colour()))
            lbl = Gtk.Label(label=label, xalign=0)
            lbl.get_style_context().add_class("ts-base")
            lbl.get_style_context().add_class("ts-medium")
            box.pack_start(area, False, False, 0)
            box.pack_start(lbl, True, True, 0)
            box.set_halign(Gtk.Align.START)
            btn.add(box)
            btn.connect("clicked", lambda *_: on_click())
            return btn

        # Restart first, shut down second: the top row is where a thumb lands.
        root.pack_start(row("rotate", "Restart", lambda: C["text"], self._restart),
                        False, False, 0)
        root.pack_start(row("power", "Shut down", lambda: "#000000", self._poweroff,
                            destructive=True), False, False, 0)
        root.pack_start(row("logout", "Log out", lambda: C["text"], self._logout),
                        False, False, 0)

        cancel = Gtk.Button(label="Cancel")
        cancel.get_style_context().add_class("ts-btn")
        cancel.set_size_request(-1, T.ROW_H)
        cancel.connect("clicked", lambda *_: self.hide())
        root.pack_start(cancel, False, False, T.SP_2)

        self.win.add(root)

    # -- actions ----------------------------------------------------------
    def _poweroff(self) -> None:
        self.hide()
        log.info("powering off")
        providers.run_fire(["systemctl", "poweroff"])

    def _restart(self) -> None:
        self.hide()
        log.info("restarting")
        providers.run_fire(["systemctl", "reboot"])

    def _logout(self) -> None:
        self.hide()
        log.info("logging out")
        if self.on_logout:
            self.on_logout()

    # -- lifecycle --------------------------------------------------------
    def show(self) -> None:
        self.win.show_all()
        self.win.present()
        self.win.realize()
        gw = self.win.get_window()
        if gw is not None:
            x11.get().mark_chrome(gw.get_xid())

    def hide(self) -> None:
        self.win.hide()

    @property
    def visible(self) -> bool:
        return self.win.get_visible()

    def _on_key(self, _w, ev) -> bool:
        if Gdk.keyval_name(ev.keyval) == "Escape":
            self.hide()
            return True
        return False
