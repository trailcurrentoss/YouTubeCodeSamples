"""The keyboard cheat sheet.

On a device with a physical keyboard and no on-screen one, a discoverable list
of shortcuts is not a power-user nicety - it is the manual. It is reachable
from a quick-settings tile as well as Super+/ , because someone who has
forgotten the shortcuts cannot be expected to remember the shortcut for the
shortcut list.
"""
from __future__ import annotations

from . import _gi  # noqa: F401
from gi.repository import Gdk, Gtk

from . import keymap, theme as T, ui, x11

C = T.C


class Shortcuts:
    def __init__(self) -> None:
        self.win = ui.make_shell_window(
            T.SCREEN_W, T.APP_H, 0, T.APP_Y, dock=False, name="trailshell-shortcuts", keep_above=False)
        self.win.connect("key-press-event", self._on_key)
        self._resolved: dict[str, str] = {}
        self._build()
        T.on_theme_changed(lambda *_: self._refresh())

    def set_resolved(self, resolved: dict[str, str]) -> None:
        """Record which accelerator actually bound, per action.

        A binding can lose its grab to another client, and showing a shortcut
        that does nothing is worse than showing none - so the sheet reports
        reality, not the wish list in keymap.py.
        """
        self._resolved = dict(resolved)
        self._refresh()

    def _build(self) -> None:
        root = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_2)
        root.get_style_context().add_class("ts-launcher")
        root.set_margin_start(T.SP_4); root.set_margin_end(T.SP_4)
        root.set_margin_top(T.SP_3); root.set_margin_bottom(T.SP_3)

        head = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        title = Gtk.Label(label="Keyboard shortcuts", xalign=0)
        title.get_style_context().add_class("ts-lg")
        title.get_style_context().add_class("ts-semibold")
        hint = Gtk.Label(label="Esc to close", xalign=1)
        hint.get_style_context().add_class("ts-xs")
        hint.get_style_context().add_class("ts-mute")
        head.pack_start(title, True, True, 0)
        head.pack_end(hint, False, False, 0)
        root.pack_start(head, False, False, 0)

        scroller = Gtk.ScrolledWindow()
        scroller.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        scroller.set_kinetic_scrolling(True)
        # Non-overlay: an overlay scrollbar sits on top of the accelerator
        # column and clips exactly the text you came here to read.
        scroller.set_overlay_scrolling(False)
        scroller.set_propagate_natural_height(False)
        scroller.set_vexpand(True)
        self.body = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_1)
        scroller.add(self.body)
        ui.attach_drag_scroll(scroller)
        root.pack_start(scroller, True, True, 0)
        self.win.add(root)
        self._refresh()

    def _refresh(self) -> None:
        for child in self.body.get_children():
            self.body.remove(child)

        # The gamepad buttons first: they are the least discoverable thing on
        # this device, since nothing on the key itself says what it does.
        self._section("Buttons")
        for keysym, (button, what) in keymap.GAMEPAD.items():
            self._row(what, button)
        self._row("Home", "START")
        self._row("Recent apps", "SELECT")

        for section, items in keymap.by_section().items():
            if not items:
                continue
            self._section(section)
            for b in items:
                shown = b.display or self._resolved.get(b.action, "")
                self._row(b.label, shown or "unavailable",
                          dim=not shown)
        self.body.show_all()

    def _section(self, title: str) -> None:
        head = Gtk.Label(label=title, xalign=0)
        for cls in ("ts-xs", "ts-accent", "ts-semibold"):
            head.get_style_context().add_class(cls)
        head.set_margin_top(T.SP_3)
        self.body.pack_start(head, False, False, 0)

    def _row(self, what: str, key_label: str, dim: bool = False) -> None:
        row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_3)
        row.set_size_request(-1, T.sp(44))
        label = Gtk.Label(label=what, xalign=0)
        label.get_style_context().add_class("ts-sm")
        label.set_ellipsize(3)

        key = Gtk.Label(label=key_label.replace("+", " + "), xalign=1)
        key.get_style_context().add_class("ts-sm")
        key.get_style_context().add_class("ts-mute" if dim else "ts-dim")
        key.get_style_context().add_class("ts-medium")
        key.set_margin_end(T.SP_2)

        row.pack_start(label, True, True, 0)
        row.pack_end(key, False, False, 0)
        self.body.pack_start(row, False, False, 0)

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

    def toggle(self) -> None:
        (self.hide if self.visible else self.show)()

    def _on_key(self, _w, ev) -> bool:
        if Gdk.keyval_name(ev.keyval) == "Escape":
            self.hide()
            return True
        return False
