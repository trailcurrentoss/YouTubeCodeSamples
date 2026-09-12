"""The home screen: a touch grid of apps that is also a keyboard search field.

This is where the PocketTerm35 differs most from a phone. There is no on-screen
keyboard and there never will be, because the device has a real one - so the
launcher is built around the idea that *typing is the fast path* and the grid
is the browsing fallback, rather than the other way round.

Concretely: the search field is always focused, so the moment you start typing
a name you are filtering. Enter launches the top hit. No tap is needed to
"enter search mode", which on a phone is an unavoidable extra step.

Ordering is by frecency (see apps.AppCatalogue), so the handful of things
actually used on this device sit in the first row.
"""
from __future__ import annotations

import logging

from . import _gi  # noqa: F401
from gi.repository import Gdk, GLib, Gtk

from . import apps as apps_mod, icons, theme as T, ui, x11

log = logging.getLogger("trailshell.launcher")
C = T.C


class AppCell(Gtk.FlowBoxChild):
    def __init__(self, app, catalogue) -> None:
        super().__init__()
        self.app = app
        self.catalogue = catalogue
        self.get_style_context().add_class("ts-app-cell")
        self.set_size_request(T.CELL_W, T.CELL_H)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_1)
        box.set_valign(Gtk.Align.CENTER)

        area = Gtk.DrawingArea()
        area.set_size_request(T.ICON_PX, T.ICON_PX)
        area.set_halign(Gtk.Align.CENTER)
        area.connect("draw", self._draw_icon)

        label = Gtk.Label(label=app.name)
        label.get_style_context().add_class("ts-app-label")
        label.set_ellipsize(3)              # PANGO_ELLIPSIZE_END
        label.set_max_width_chars(13)
        label.set_justify(Gtk.Justification.CENTER)
        label.set_lines(2)
        label.set_line_wrap(True)
        label.set_line_wrap_mode(2)         # PANGO_WRAP_WORD_CHAR

        box.pack_start(area, False, False, 0)
        box.pack_start(label, False, False, 0)
        self.add(box)

    def _draw_icon(self, w, cr) -> bool:
        size = min(w.get_allocated_width(), w.get_allocated_height())
        pb = self.catalogue.icon(self.app, size)
        if pb is not None:
            ui.draw_pixbuf_centered(cr, pb, w, size)
        else:
            ui.fill_rect(cr, 0, 0, size, size, C["surface_alt"], T.RADIUS_LG)
            ui.draw_glyph(cr, w, "grid", C["text_mute"], size=size * 0.5)
        return False


class Launcher:
    def __init__(self, system=None, on_launch=None) -> None:
        self.catalogue = apps_mod.get()
        self.on_launch = on_launch
        self._results: list = []

        self.win = ui.make_shell_window(
            T.SCREEN_W, T.APP_H, 0, T.APP_Y, dock=False, name="trailshell-launcher", keep_above=False)
        self.win.get_style_context().add_class("ts-launcher")
        self.win.connect("key-press-event", self._on_key)
        self._build()
        T.on_theme_changed(lambda *_: self.win.queue_draw())

    # -- construction -----------------------------------------------------
    def _build(self) -> None:
        root = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        root.get_style_context().add_class("ts-launcher")

        # Search row -------------------------------------------------------
        row = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_2)
        row.set_margin_start(T.SP_4); row.set_margin_end(T.SP_4)
        row.set_margin_top(T.SP_3); row.set_margin_bottom(T.SP_2)

        # GTK3 clears an entry's placeholder text the moment it takes focus,
        # and this entry is focused on purpose so typing filters immediately.
        # So the hint is drawn as a pass-through overlay instead.
        self.entry = Gtk.Entry()
        self.entry.get_style_context().add_class("ts-search")
        self.entry.set_hexpand(True)
        self.entry.set_size_request(-1, T.SEARCH_H)
        self.entry.connect("changed", self._on_search_changed)
        self.entry.connect("activate", lambda *_: self._launch_first())

        overlay = Gtk.Overlay()
        overlay.add(self.entry)

        hint = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_2)
        hint.set_halign(Gtk.Align.START)
        hint.set_valign(Gtk.Align.CENTER)
        hint.set_margin_start(T.SP_4)
        glyph = Gtk.DrawingArea()
        glyph.set_size_request(T.sp(24), T.sp(24))
        glyph.connect("draw", lambda w, cr: ui.draw_glyph(
            cr, w, "search", C["text_mute"]))
        self.hint_label = Gtk.Label(label="Search apps")
        self.hint_label.get_style_context().add_class("ts-base")
        self.hint_label.get_style_context().add_class("ts-mute")
        hint.pack_start(glyph, False, False, 0)
        hint.pack_start(self.hint_label, False, False, 0)
        self.hint = hint

        overlay.add_overlay(hint)
        overlay.set_overlay_pass_through(hint, True)
        self.overlay = overlay
        row.pack_start(overlay, True, True, 0)
        root.pack_start(row, False, False, 0)

        # Grid -------------------------------------------------------------
        self.scroller = Gtk.ScrolledWindow()
        self.scroller.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        self.scroller.set_kinetic_scrolling(True)
        self.scroller.set_overlay_scrolling(True)
        self.scroller.set_vexpand(True)

        self.flow = Gtk.FlowBox()
        self.flow.set_valign(Gtk.Align.START)
        self.flow.set_min_children_per_line(T.GRID_COLS)
        self.flow.set_max_children_per_line(T.GRID_COLS)
        self.flow.set_homogeneous(True)
        self.flow.set_selection_mode(Gtk.SelectionMode.BROWSE)
        self.flow.set_activate_on_single_click(True)
        self.flow.connect("child-activated", self._on_activated)
        self.scroller.add(self.flow)
        ui.attach_drag_scroll(self.scroller)
        root.pack_start(self.scroller, True, True, 0)

        self.empty = Gtk.Label(label="No matching apps")
        self.empty.get_style_context().add_class("ts-base")
        self.empty.get_style_context().add_class("ts-mute")
        root.pack_start(self.empty, False, False, T.SP_5)

        self.win.add(root)
        self._refilter()

    # -- content ----------------------------------------------------------
    def _on_search_changed(self, *_a) -> None:
        # The glyph stays; only the wording goes away, so the field never
        # loses its "this is a search box" affordance mid-query.
        self.hint_label.set_visible(not self.entry.get_text())
        self._refilter()

    def _refilter(self) -> None:
        query = self.entry.get_text()
        results = self.catalogue.search(query, limit=200) if query else \
            self.catalogue.grid_order()
        self._results = results

        for child in self.flow.get_children():
            self.flow.remove(child)
        for app in results:
            self.flow.add(AppCell(app, self.catalogue))
        self.flow.show_all()

        self.empty.set_visible(not results)
        self.scroller.set_visible(bool(results))
        # Pre-select the top hit so Enter always has an obvious target and the
        # accent outline tells you what it is.
        if results:
            first = self.flow.get_child_at_index(0)
            if first:
                self.flow.select_child(first)
        adj = self.scroller.get_vadjustment()
        if adj:
            adj.set_value(0)

    # -- actions ----------------------------------------------------------
    def _on_activated(self, _flow, child) -> None:
        self._launch(child.app)

    def _launch_first(self) -> None:
        if self._results:
            self._launch(self._results[0])

    def _launch(self, app) -> None:
        log.info("launching %s", app.app_id)
        if self.catalogue.launch(app):
            self.entry.set_text("")
            if self.on_launch:
                self.on_launch(app)

    # -- lifecycle --------------------------------------------------------
    def show(self) -> None:
        self._refilter()
        self.win.show_all()
        self.empty.set_visible(not self._results)
        self.hint_label.set_visible(not self.entry.get_text())
        self.win.present()
        self.win.realize()
        gw = self.win.get_window()
        if gw is not None:
            x11.get().mark_chrome(gw.get_xid())
        # Focus the entry, not the grid: typing must filter immediately.
        GLib.idle_add(self._focus_entry)

    def _focus_entry(self) -> bool:
        self.entry.grab_focus()
        self.entry.set_position(-1)
        self.hint_label.set_visible(not self.entry.get_text())
        return False

    def hide(self) -> None:
        self.win.hide()

    def reset(self) -> None:
        """Return to a clean home screen (called when Home is pressed again)."""
        self.entry.set_text("")
        adj = self.scroller.get_vadjustment()
        if adj:
            adj.set_value(0)
        self._focus_entry()

    # -- gamepad buttons --------------------------------------------------
    def _on_button(self, action: str) -> bool:
        """A/B/X/Y/L/R while the launcher is up."""
        if action == "select":
            sel = self.flow.get_selected_children()
            self._launch(sel[0].app) if sel else self._launch_first()
        elif action in ("back", "close_item"):
            # Nothing to go "back" to from home, so B clears the query - which
            # is the thing you actually want to undo here.
            self.entry.set_text("")
        elif action in ("page_up", "page_down"):
            adj = self.scroller.get_vadjustment()
            if adj:
                step = adj.get_page_size() * (1 if action == "page_down" else -1)
                adj.set_value(adj.get_value() + step)
        else:
            return False
        return True

    # -- keyboard ---------------------------------------------------------
    def _on_key(self, _w, ev) -> bool:
        action = ui.gamepad_action(ev.keyval)
        if action:
            return self._on_button(action)
        name = Gdk.keyval_name(ev.keyval)
        if name == "Escape":
            if self.entry.get_text():
                self.entry.set_text("")
            return True
        if name in ("Return", "KP_Enter"):
            sel = self.flow.get_selected_children()
            if sel:
                self._launch(sel[0].app)
            else:
                self._launch_first()
            return True
        # Up/Down always move into the grid. Left/Right only do so when the
        # query is empty - otherwise they are the caret keys you need to fix a
        # typo, and stealing them makes the search field feel broken.
        if name in ("Up", "Down") or (
                name in ("Left", "Right") and not self.entry.get_text()):
            self.flow.grab_focus()
            return False
        if not self.entry.has_focus():
            self.entry.grab_focus()
            self.entry.set_position(-1)
        return False
