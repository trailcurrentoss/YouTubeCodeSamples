"""The swipe-down shade: quick settings, sliders and notifications.

This is the shell's most-used surface, so it gets the most care.

Interaction model, borrowed from Android and iOS Control Centre:
  * Dragging down from the status bar reveals the sheet 1:1 with your finger;
    releasing settles open or closed based on distance and fling velocity.
  * The sheet stops 30 px short of the bottom so the nav bar stays visible and
    Back always closes it - there is no "tap outside to dismiss" region on a
    screen this small.
  * Quick settings show one row of four by default and expand to two, rather
    than permanently spending 76 px that the notification list needs more.

The reveal is done with an X shape region rather than by resizing or moving the
window: no compositor runs in this session, so alpha blending is not available,
and reshaping is far cheaper per frame than a resize round-trip to the WM.
"""
from __future__ import annotations

import logging
import os
import time

import cairo

from . import _gi  # noqa: F401
from gi.repository import Gdk, GLib, Gtk

from . import icons, providers, theme as T, ui, x11

log = logging.getLogger("trailshell.shade")
C = T.C

#: The sheet fills the app canvas: it starts under the status bar (a DOCK
#: window, which always stacks above us, so drawing behind it just wasted
#: 36 px) and stops short of the nav bar so Back always closes it.
SHADE_Y = T.STATUS_H
SHADE_H = T.SCREEN_H - T.STATUS_H - T.NAV_H

#: No clock in the header - the status bar directly above already shows the
#: time and date, and duplicating it cost 62 px that the notification list
#: needs far more.
HEADER_H = T.sp(22)
FOOTER_H = T.sp(56)
TILE_H = T.sp(68)
TILE_GAP = T.SP_2
PAD = T.SP_3

#: Release thresholds. Either a decisive flick or crossing the halfway point
#: commits the gesture; anything less springs back.
FLING_PX_PER_MS = 0.45
COMMIT_FRACTION = 0.4

FRAME_MS = 16


# --------------------------------------------------------------- widgets ---
class QSTile(Gtk.Button):
    """A quick-settings toggle: glyph over a label, unmistakably on or off."""

    def __init__(self, glyph: str, label: str, on_click, is_active=None,
                 momentary: bool = False) -> None:
        super().__init__()
        self.glyph = glyph
        self.is_active = is_active or (lambda: False)
        self.momentary = momentary
        self.get_style_context().add_class("ts-qs-tile")
        self.set_size_request(-1, TILE_H)
        self.set_relief(Gtk.ReliefStyle.NONE)

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_1)
        self.icon = Gtk.DrawingArea()
        self.icon.set_size_request(T.sp(28), T.sp(28))
        self.icon.connect("draw", self._draw_icon)
        self.icon.set_halign(Gtk.Align.CENTER)
        self.label = Gtk.Label(label=label)
        self.label.get_style_context().add_class("ts-qs-label")
        self.label.set_ellipsize(3)  # PANGO_ELLIPSIZE_END
        box.pack_start(self.icon, False, False, 0)
        box.pack_start(self.label, False, False, 0)
        box.set_valign(Gtk.Align.CENTER)
        self.add(box)
        self.connect("clicked", lambda *_: (on_click(), self.sync()))
        self.sync()

    def _draw_icon(self, w, cr) -> bool:
        # An active tile is filled with the accent, so its glyph flips to the
        # on-accent ink to stay legible (verified by tools/contrast.py).
        colour = C["on_accent"] if self.is_active() else C["text_dim"]
        ui.draw_glyph(cr, w, self.glyph, colour)
        return False

    def sync(self) -> None:
        ctx = self.get_style_context()
        if self.is_active() and not self.momentary:
            ctx.add_class("ts-on")
        else:
            ctx.remove_class("ts-on")
        self.icon.queue_draw()


class SliderRow(Gtk.Box):
    """Icon plus a fat, thumb-friendly slider."""

    def __init__(self, glyph: str, get_value, set_value,
                 lower: float = 0.0, upper: float = 1.0, on_icon=None) -> None:
        super().__init__(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_3)
        self.glyph = glyph
        self.get_value = get_value
        self.set_value = set_value
        self._syncing = False

        self.icon_btn = Gtk.Button()
        self.icon_btn.set_relief(Gtk.ReliefStyle.NONE)
        self.icon_btn.get_style_context().add_class("ts-btn")
        self.icon_btn.get_style_context().add_class("ts-flat")
        self.icon_btn.get_style_context().add_class("ts-compact")
        self.icon_btn.set_size_request(T.sp(52), T.sp(52))
        area = Gtk.DrawingArea()
        area.set_size_request(T.sp(26), T.sp(26))
        area.connect("draw", self._draw_icon)
        self.icon_btn.add(area)
        self._area = area
        if on_icon:
            self.icon_btn.connect("clicked", lambda *_: on_icon())
        else:
            self.icon_btn.set_sensitive(False)

        self.scale = Gtk.Scale.new_with_range(
            Gtk.Orientation.HORIZONTAL, lower, upper, 0.01)
        self.scale.set_draw_value(False)
        self.scale.set_hexpand(True)
        self.scale.connect("value-changed", self._on_changed)

        self.pack_start(self.icon_btn, False, False, 0)
        self.pack_start(self.scale, True, True, 0)
        self.sync()

    def _draw_icon(self, w, cr) -> bool:
        ui.draw_glyph(cr, w, self.glyph, C["text_dim"])
        return False

    def _on_changed(self, scale) -> None:
        if not self._syncing:
            self.set_value(scale.get_value())
            self._area.queue_draw()

    def sync(self) -> None:
        self._syncing = True
        self.scale.set_value(self.get_value())
        self._syncing = False
        self._area.queue_draw()


class NotifCard(Gtk.EventBox):
    """One notification. Tap the X, or swipe it sideways, to dismiss."""

    SWIPE_COMMIT = T.sp(110)

    def __init__(self, notif, on_dismiss) -> None:
        super().__init__()
        self.notif = notif
        self.on_dismiss = on_dismiss

        frame = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_2)
        ctx = frame.get_style_context()
        ctx.add_class("ts-notif")
        if notif.is_critical:
            ctx.add_class("ts-urgent")
        elif notif.urgency == 0:
            ctx.add_class("ts-low")

        text = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.sp(2))
        head = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_2)
        app = Gtk.Label(label=notif.app_name, xalign=0)
        app.get_style_context().add_class("ts-notif-app")
        app.set_ellipsize(3)
        age = Gtk.Label(label=notif.age_str, xalign=1)
        age.get_style_context().add_class("ts-notif-body")
        head.pack_start(app, True, True, 0)
        head.pack_start(age, False, False, 0)

        summary = Gtk.Label(label=notif.summary, xalign=0)
        summary.get_style_context().add_class("ts-notif-title")
        summary.set_ellipsize(3)
        text.pack_start(head, False, False, 0)
        text.pack_start(summary, False, False, 0)
        if notif.body:
            body = Gtk.Label(label=notif.body, xalign=0)
            body.get_style_context().add_class("ts-notif-body")
            body.set_ellipsize(3)
            body.set_lines(1)
            body.set_line_wrap(False)
            text.pack_start(body, False, False, 0)

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
        close.connect("clicked", lambda *_: self.on_dismiss(notif.id))

        frame.pack_start(text, True, True, 0)
        frame.pack_start(close, False, False, 0)
        self.add(frame)

        # Swipe-to-dismiss, the gesture people already expect from a phone.
        # GesturePan, not GestureDrag: it only claims a touch sequence once
        # the movement is predominantly horizontal, so a vertical flick still
        # reaches the ScrolledWindow and scrolls the list instead of being
        # swallowed as a half-hearted dismiss.
        self.drag = Gtk.GesturePan.new(self, Gtk.Orientation.HORIZONTAL)
        self.drag.set_propagation_phase(Gtk.PropagationPhase.BUBBLE)
        self.drag.connect("drag-update", self._on_drag)
        self.drag.connect("drag-end", self._on_drag_end)

    def _on_drag(self, _g, dx, _dy) -> None:
        if abs(dx) > T.sp(6):
            self.set_margin_start(max(0, int(dx)))
            self.set_opacity(max(0.25, 1.0 - abs(dx) / (self.SWIPE_COMMIT * 1.6)))

    def _on_drag_end(self, _g, dx, _dy) -> None:
        if abs(dx) >= self.SWIPE_COMMIT:
            self.on_dismiss(self.notif.id)
            return
        self.set_margin_start(0)
        self.set_opacity(1.0)


# ----------------------------------------------------------------- shade ---
class Shade:
    def __init__(self, system, store, server=None, on_open_settings=None,
                 on_show_shortcuts=None, on_open_launcher=None,
                 on_power=None) -> None:
        self.sys = system
        self.store = store
        self.server = server
        self.on_open_settings = on_open_settings
        self.on_show_shortcuts = on_show_shortcuts
        self.on_open_launcher = on_open_launcher
        self.on_power = on_power

        self.pull = 0.0           # px of sheet currently revealed
        self.is_open = False
        self._anim_id = None
        self._expanded = False
        self._pull_base = 0.0

        self.win = ui.make_shell_window(T.SCREEN_W, SHADE_H, 0, SHADE_Y,
                                        dock=False, name="trailshell-shade")
        self.win.connect("key-press-event", self._on_key)
        self.win.add_events(ui.TOUCH_EVENTS)
        self._build()
        T.on_theme_changed(lambda *_: self._sync_all())
        store.changed.connect(lambda *_: self._rebuild_notifications())
        for src in (system.net, system.audio, system.brightness,
                    system.bluetooth, system.clock):
            src.changed.connect(lambda *_: self._sync_all())

    # -- construction -----------------------------------------------------
    def _build(self) -> None:
        root = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        root.get_style_context().add_class("ts-shade")

        root.pack_start(self._build_header(), False, False, 0)

        scroller = Gtk.ScrolledWindow()
        scroller.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        # Kinetic scrolling is why the shade is GTK widgets and not Cairo:
        # flick-and-glide is the single biggest touch-feel win available here.
        scroller.set_kinetic_scrolling(True)
        scroller.set_overlay_scrolling(True)
        scroller.set_vexpand(True)
        # Without these the scroller demands its content's full natural height
        # and shoves the pinned footer off the bottom of the sheet.
        scroller.set_propagate_natural_height(False)
        scroller.set_min_content_height(T.sp(80))

        body = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_2)
        body.set_margin_start(PAD); body.set_margin_end(PAD)
        body.set_margin_top(T.SP_2); body.set_margin_bottom(T.SP_3)

        body.pack_start(self._build_quick_settings(), False, False, 0)
        body.pack_start(self._build_notifications(), False, False, 0)
        scroller.add(body)
        ui.attach_drag_scroll(scroller)
        root.pack_start(scroller, True, True, 0)

        root.pack_start(self._build_footer(), False, False, 0)
        self.win.add(root)

    def _build_header(self) -> Gtk.Widget:
        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        box.set_size_request(-1, HEADER_H)
        box.set_valign(Gtk.Align.CENTER)
        handle = Gtk.Box()
        handle.get_style_context().add_class("ts-shade-handle")
        handle.set_halign(Gtk.Align.CENTER)
        handle.set_valign(Gtk.Align.CENTER)
        box.pack_start(handle, True, False, 0)
        return box

    def _build_quick_settings(self) -> Gtk.Widget:
        """Quick settings, in two tiers.

        Collapsed shows only what is reached for constantly - the four primary
        toggles and the volume slider - and hands the remaining ~150 px to the
        notification list, which is the other half of what the swipe-down is
        for. Expanding reveals the secondary toggles and the dim slider.

        Volume stays in the collapsed tier and Dim does not, because the
        PocketTerm35's keyboard already has Fn+e / Fn+r for volume and Fn+- /
        Fn++ for the real backlight; of the two, volume is the one people still
        reach for on screen.
        """
        wrap = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=TILE_GAP)
        self.tiles: list[QSTile] = []

        def tile(glyph, label, click, active=None, momentary=False):
            t = QSTile(glyph, label, click, active, momentary)
            self.tiles.append(t)
            return t

        row1 = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=TILE_GAP,
                       homogeneous=True)
        row1.pack_start(tile("wifi", "Wi-Fi", self.sys.net.toggle_wifi,
                             lambda: self.sys.net.wifi_enabled), True, True, 0)
        row1.pack_start(tile("bluetooth", "Bluetooth", self.sys.bluetooth.toggle,
                             lambda: self.sys.bluetooth.enabled), True, True, 0)
        # "Silent", not "Do Not Disturb": the longer label ellipsises to
        # "Do Not Dist..." in a 146 px tile, which reads as nothing at all.
        row1.pack_start(tile("bell_off", "Silent", self._toggle_dnd,
                             lambda: self.sys.dnd), True, True, 0)
        row1.pack_start(tile("moon", "Dark", self._toggle_theme,
                             lambda: T.theme_name() == "dark"), True, True, 0)
        wrap.pack_start(row1, False, False, 0)

        self.row2 = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=TILE_GAP,
                            homogeneous=True)
        self.row2.pack_start(tile("airplane", "Airplane", self._toggle_airplane,
                                  lambda: self.sys.airplane), True, True, 0)
        self.row2.pack_start(tile("clear", "Screenshot", self.take_screenshot,
                                  momentary=True), True, True, 0)
        self.row2.pack_start(tile("keyboard", "Shortcuts", self._shortcuts,
                                  momentary=True), True, True, 0)
        self.row2.pack_start(tile("search", "Apps", self._open_launcher,
                                  momentary=True), True, True, 0)
        wrap.pack_start(self.row2, False, False, 0)

        sliders = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        sliders.set_margin_end(T.SP_3)   # room for the slider knob's overhang
        self.vol_row = SliderRow(
            "volume",
            lambda: self.sys.audio.volume,
            self.sys.audio.set_volume,
            on_icon=self.sys.audio.toggle_mute,
        )
        # Labelled by a sun glyph and treated as "Dim": the real backlight is
        # driven by the keyboard firmware, and this is a gamma layer on top.
        self.dim_row = SliderRow(
            "brightness",
            lambda: self.sys.brightness.value,
            self.sys.brightness.set,
            lower=providers.BRIGHTNESS_MIN, upper=1.0,
        )
        sliders.pack_start(self.vol_row, False, False, 0)
        sliders.pack_start(self.dim_row, False, False, 0)
        wrap.pack_start(sliders, False, False, 0)

        self.expander = Gtk.Button()
        self.expander.set_relief(Gtk.ReliefStyle.NONE)
        self.expander.get_style_context().add_class("ts-btn")
        self.expander.get_style_context().add_class("ts-flat")
        self.expander.get_style_context().add_class("ts-compact")
        self.expander.set_size_request(-1, T.sp(30))
        self._chev = Gtk.DrawingArea()
        self._chev.set_size_request(T.sp(24), T.sp(18))
        self._chev.connect("draw", self._draw_chevron)
        self.expander.add(self._chev)
        self.expander.connect("clicked", lambda *_: self._toggle_expand())
        wrap.pack_start(self.expander, False, False, 0)
        return wrap

    def _draw_chevron(self, w, cr) -> bool:
        ui.draw_glyph(cr, w, "chevron_up" if self._expanded else "chevron_down",
                      C["text_mute"])
        return False

    def _build_notifications(self) -> Gtk.Widget:
        self.notif_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_2)

        head = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        self.notif_title = Gtk.Label(xalign=0)
        self.notif_title.get_style_context().add_class("ts-sm")
        self.notif_title.get_style_context().add_class("ts-dim")
        self.clear_btn = Gtk.Button(label="Clear all")
        self.clear_btn.get_style_context().add_class("ts-btn")
        self.clear_btn.get_style_context().add_class("ts-flat")
        self.clear_btn.get_style_context().add_class("ts-compact")
        self.clear_btn.set_size_request(-1, T.sp(44))
        self.clear_btn.connect("clicked", lambda *_: self._clear_all())
        head.pack_start(self.notif_title, True, True, 0)
        head.pack_end(self.clear_btn, False, False, 0)

        self.notif_head = head
        self.notif_list = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=T.SP_2)
        self.notif_empty = Gtk.Label(label="No notifications")
        self.notif_empty.get_style_context().add_class("ts-sm")
        self.notif_empty.get_style_context().add_class("ts-mute")
        self.notif_empty.set_margin_top(T.SP_4)
        self.notif_empty.set_margin_bottom(T.SP_4)

        self.notif_box.pack_start(head, False, False, 0)
        self.notif_box.pack_start(self.notif_list, False, False, 0)
        self.notif_box.pack_start(self.notif_empty, False, False, 0)
        return self.notif_box

    def _build_footer(self) -> Gtk.Widget:
        box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=T.SP_2)
        box.set_size_request(-1, FOOTER_H)
        box.set_margin_start(PAD); box.set_margin_end(PAD)
        box.set_margin_top(T.SP_1); box.set_margin_bottom(T.SP_2)

        self.lbl_net = Gtk.Label(xalign=0)
        self.lbl_net.get_style_context().add_class("ts-sm")
        self.lbl_net.get_style_context().add_class("ts-dim")
        self.lbl_net.set_ellipsize(3)

        def icon_button(glyph, colour, on_click):
            b = Gtk.Button()
            b.get_style_context().add_class("ts-btn")
            b.set_size_request(T.TOUCH_MIN + T.SP_2, T.sp(52))
            a = Gtk.DrawingArea()
            a.set_size_request(T.sp(24), T.sp(24))
            a.connect("draw", lambda w, cr: ui.draw_glyph(cr, w, glyph, colour()))
            b.add(a)
            b.connect("clicked", lambda *_: on_click())
            return b

        gear = icon_button("settings", lambda: C["text_dim"], self._settings)
        power = icon_button("power", lambda: C["danger"], self._power)

        box.pack_start(self.lbl_net, True, True, 0)
        box.pack_end(power, False, False, 0)
        box.pack_end(gear, False, False, 0)
        return box

    # -- state sync -------------------------------------------------------
    def _sync_all(self) -> None:
        n = self.sys.net
        if not n.wifi_enabled:
            self.lbl_net.set_text("Wi-Fi off")
        elif n.connected:
            self.lbl_net.set_text(f"{n.ssid}  ·  {n.signal}%")
        else:
            self.lbl_net.set_text("Not connected")
        for t in self.tiles:
            t.sync()
        self.vol_row.sync()
        self.dim_row.sync()
        self._chev.queue_draw()

    def _rebuild_notifications(self) -> None:
        for child in self.notif_list.get_children():
            self.notif_list.remove(child)
        items = self.store.items
        for n in items:
            self.notif_list.pack_start(NotifCard(n, self._dismiss), False, False, 0)
        self.notif_title.set_text(
            f"Notifications  ({len(items)})" if items else "Notifications")
        self.notif_list.set_visible(bool(items))
        self.notif_empty.set_visible(not items)
        self.clear_btn.set_visible(bool(items))
        self.notif_list.show_all()

    # -- actions ----------------------------------------------------------
    def _dismiss(self, nid: int) -> None:
        if self.server:
            self.server.dismiss(nid)
        else:
            self.store.remove(nid)

    def _clear_all(self) -> None:
        if self.server:
            self.server.dismiss_all()
        else:
            self.store.clear()

    def _toggle_dnd(self) -> None:
        self.sys.toggle_dnd()
        from . import settings
        settings.set("dnd", self.sys.dnd)
        self._sync_all()

    def _toggle_theme(self) -> None:
        from . import settings
        name = T.toggle_theme()
        settings.set("theme", name)
        self._sync_all()

    def _toggle_airplane(self) -> None:
        self.sys.set_airplane(not self.sys.airplane)
        self._sync_all()

    def _toggle_expand(self) -> None:
        self._expanded = not self._expanded
        self._apply_expanded()

    def _apply_expanded(self) -> None:
        self.row2.set_visible(self._expanded)
        self.dim_row.set_visible(self._expanded)
        self._chev.queue_draw()

    def _open_launcher(self) -> None:
        self.close()
        if self.on_open_launcher:
            self.on_open_launcher()

    def take_screenshot(self) -> None:
        """Hide the shade, then grab the screen. Public: the hotkey uses it."""
        self.close()
        # Let the shade actually leave the screen before grabbing.
        GLib.timeout_add(T.DUR_BASE + 60, self._do_screenshot)

    def _do_screenshot(self) -> bool:
        try:
            root = Gdk.get_default_root_window()
            pb = Gdk.pixbuf_get_from_window(root, 0, 0, T.SCREEN_W, T.SCREEN_H)
            d = os.path.join(GLib.get_home_dir(), "Pictures")
            os.makedirs(d, exist_ok=True)
            path = os.path.join(d, time.strftime("screenshot-%Y%m%d-%H%M%S.png"))
            pb.savev(path, "png", [], [])
            log.info("screenshot saved: %s", path)
        except Exception:
            log.exception("screenshot failed")
        return False

    def _shortcuts(self) -> None:
        self.close()
        if self.on_show_shortcuts:
            self.on_show_shortcuts()

    def _settings(self) -> None:
        self.close()
        if self.on_open_settings:
            self.on_open_settings()

    def _power(self) -> None:
        self.close()
        if self.on_power:
            self.on_power()

    # -- reveal geometry --------------------------------------------------
    def _apply_pull(self) -> None:
        """Reveal the top ``pull`` pixels of the sheet via an X shape region."""
        gw = self.win.get_window()
        if gw is None:
            return
        p = int(max(0, min(SHADE_H, self.pull)))
        if p >= SHADE_H:
            gw.shape_combine_region(None, 0, 0)      # fully opaque
            return
        region = cairo.Region(cairo.RectangleInt(0, 0, T.SCREEN_W, p))
        gw.shape_combine_region(region, 0, 0)

    # -- gesture API driven by the status bar -----------------------------
    def begin_pull(self) -> None:
        self._stop_anim()
        self._pull_base = self.pull if self.is_open else 0.0
        if not self.win.get_visible():
            self.pull = 0.0
            self._sync_all()
            self._rebuild_notifications()
            self.win.show_all()
            self._apply_expanded()
            self._rebuild_notifications()
        self.win.present()
        self._apply_pull()

    def update_pull(self, dy: float) -> None:
        self.pull = max(0.0, min(SHADE_H, self._pull_base + dy))
        self._apply_pull()

    def end_pull(self, dy: float, velocity: float) -> None:
        committed = velocity >= FLING_PX_PER_MS or self.pull >= SHADE_H * COMMIT_FRACTION
        self._animate_to(SHADE_H if committed else 0.0)

    # -- open / close -----------------------------------------------------
    def open(self, animate: bool = True) -> None:
        self.begin_pull()
        if animate:
            self._animate_to(SHADE_H)
        else:
            self.pull = SHADE_H
            self.is_open = True
            self._apply_pull()

    def close(self, animate: bool = True) -> None:
        if not self.win.get_visible():
            return
        if animate:
            self._animate_to(0.0)
        else:
            self.pull = 0.0
            self._finish_close()

    def toggle(self) -> None:
        (self.close if self.is_open else self.open)()

    def _finish_close(self) -> None:
        self.win.hide()

    def _settle(self, target: float) -> None:
        """Land the sheet exactly on ``target`` and record the resulting state."""
        self.pull = target
        self._apply_pull()
        self.is_open = target > 0
        if target <= 0:
            self._finish_close()

    def _stop_anim(self) -> None:
        if self._anim_id is not None:
            GLib.source_remove(self._anim_id)
            self._anim_id = None

    def _animate_to(self, target: float) -> None:
        self._stop_anim()
        start = self.pull
        if abs(target - start) < 1:
            self._settle(target)
            return
        t0 = GLib.get_monotonic_time()
        duration = T.DUR_BASE * 1000.0     # microseconds

        def frame() -> bool:
            t = (GLib.get_monotonic_time() - t0) / duration
            if t >= 1.0:
                self._settle(target)
                self._anim_id = None
                return False
            self.pull = ui.lerp(start, target, ui.ease_out_cubic(t))
            self._apply_pull()
            return True

        self._anim_id = GLib.timeout_add(FRAME_MS, frame)

    # -- keyboard ---------------------------------------------------------
    def _on_key(self, _w, ev) -> bool:
        from . import ui as _ui
        action = _ui.gamepad_action(ev.keyval)
        if action in ("back", "shade"):
            self.close()
            return True
        name = Gdk.keyval_name(ev.keyval)
        if name in ("Escape", "Down"):
            self.close()
            return True
        return False

    def show_chrome_hint(self) -> None:
        """Tag the window as chrome once realized."""
        self.win.realize()
        gw = self.win.get_window()
        if gw is not None:
            x11.get().mark_chrome(gw.get_xid())
