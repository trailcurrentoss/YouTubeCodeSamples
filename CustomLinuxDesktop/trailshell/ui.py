"""Shared drawing and window helpers for the shell's Cairo-drawn surfaces.

Text is sized in *absolute device pixels* via Pango's ``set_absolute_size``
rather than points. The theme reasons in real millimetres on a known 228 PPI
panel, so letting Xft's DPI setting scale things underneath us would defeat the
whole exercise.
"""
from __future__ import annotations

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import Gdk, GdkPixbuf, GLib, Gtk, Pango, PangoCairo

from . import icons, theme as T

C = T.C


# ------------------------------------------------------------------ text ---
_FONT_CACHE: dict[tuple, Pango.FontDescription] = {}


def font(size_px: int, weight: int = T.WEIGHT_NORMAL) -> Pango.FontDescription:
    key = (size_px, weight)
    fd = _FONT_CACHE.get(key)
    if fd is None:
        fd = Pango.FontDescription()
        fd.set_family(T.FONT)
        fd.set_weight(Pango.Weight(weight))
        fd.set_absolute_size(size_px * Pango.SCALE)
        _FONT_CACHE[key] = fd
    return fd


def layout(cr, text: str, size_px: int, weight: int = T.WEIGHT_NORMAL,
           width: int | None = None, ellipsize: bool = True,
           align: str = "left") -> Pango.Layout:
    lo = PangoCairo.create_layout(cr)
    lo.set_font_description(font(size_px, weight))
    lo.set_text(text, -1)
    if width is not None:
        lo.set_width(width * Pango.SCALE)
        if ellipsize:
            lo.set_ellipsize(Pango.EllipsizeMode.END)
    lo.set_alignment({"left": Pango.Alignment.LEFT,
                      "center": Pango.Alignment.CENTER,
                      "right": Pango.Alignment.RIGHT}[align])
    return lo


def text_size(cr, text: str, size_px: int, weight: int = T.WEIGHT_NORMAL) -> tuple[int, int]:
    lo = layout(cr, text, size_px, weight)
    return lo.get_pixel_size()


def set_rgba(cr, color: str, alpha: float = 1.0) -> None:
    c = Gdk.RGBA()
    c.parse(color)
    cr.set_source_rgba(c.red, c.green, c.blue, c.alpha * alpha)


def draw_text(cr, text: str, x: float, y: float, size_px: int,
              color: str = None, weight: int = T.WEIGHT_NORMAL,
              width: int | None = None, align: str = "left",
              valign: str = "top", alpha: float = 1.0) -> tuple[int, int]:
    """Draw text at (x, y). ``valign='middle'`` centres it on y. Returns size."""
    set_rgba(cr, color or C["text"], alpha)
    lo = layout(cr, text, size_px, weight, width=width, align=align)
    w, h = lo.get_pixel_size()
    if valign == "middle":
        y = y - h / 2
    elif valign == "bottom":
        y = y - h
    cr.move_to(x, y)
    PangoCairo.show_layout(cr, lo)
    return w, h


# --------------------------------------------------------------- shapes ---
rounded_rect = icons.rounded_rect


def fill_rect(cr, x, y, w, h, color: str, radius: float = 0, alpha: float = 1.0) -> None:
    set_rgba(cr, color, alpha)
    if radius:
        rounded_rect(cr, x, y, w, h, radius)
    else:
        cr.rectangle(x, y, w, h)
    cr.fill()


def stroke_rect(cr, x, y, w, h, color: str, radius: float = 0,
                width: float = 1.0, alpha: float = 1.0) -> None:
    set_rgba(cr, color, alpha)
    cr.set_line_width(width)
    if radius:
        rounded_rect(cr, x, y, w, h, radius)
    else:
        cr.rectangle(x, y, w, h)
    cr.stroke()


def draw_pixbuf(cr, pb: GdkPixbuf.Pixbuf, x: float, y: float,
                size: int | None = None, alpha: float = 1.0) -> None:
    if pb is None:
        return
    cr.save()
    if size and (pb.get_width() != size or pb.get_height() != size):
        scale = size / max(pb.get_width(), pb.get_height())
        cr.translate(x, y)
        cr.scale(scale, scale)
        Gdk.cairo_set_source_pixbuf(cr, pb, 0, 0)
    else:
        Gdk.cairo_set_source_pixbuf(cr, pb, x, y)
    cr.paint_with_alpha(alpha)
    cr.restore()


def draw_glyph(cr, widget, name: str, color: str, size: int | None = None,
               alpha: float = 1.0, **kw) -> None:
    """Draw an icon centred in ``widget``'s allocation.

    Drawing at (0, 0) with ``icons.draw`` pins the glyph to the top-left of
    whatever GTK allocated, which is only ever correct by accident - a
    DrawingArea inside a Button gets stretched to the button's content box, so
    the icon ends up hard against the corner. Everything in the shell goes
    through here instead.
    """
    aw = widget.get_allocated_width()
    ah = widget.get_allocated_height()
    s = size if size is not None else min(aw, ah)
    icons.draw(cr, name, (aw - s) / 2.0, (ah - s) / 2.0, s, color, alpha, **kw)


def draw_pixbuf_centered(cr, pb, widget, size: int | None = None,
                         alpha: float = 1.0) -> None:
    """Same idea for a themed app icon."""
    if pb is None:
        return
    aw = widget.get_allocated_width()
    ah = widget.get_allocated_height()
    s = size if size is not None else min(aw, ah)
    draw_pixbuf(cr, pb, (aw - s) / 2.0, (ah - s) / 2.0, s, alpha)


# -------------------------------------------------------------- easing ----
def ease_out_cubic(t: float) -> float:
    t = max(0.0, min(1.0, t))
    return 1 - pow(1 - t, 3)


def lerp(a: float, b: float, t: float) -> float:
    return a + (b - a) * t


# ------------------------------------------------------- drag scrolling ---
#: How far a finger must travel before we treat it as a scroll rather than a
#: tap. ~1.3 mm on this panel: small enough to feel immediate, large enough
#: that pressing a button does not scroll the list out from under you.
DRAG_THRESHOLD = sp_px = T.sp(12)

#: Per-frame velocity decay during the fling. 0.94 at 16 ms gives roughly a
#: half-second glide, which matches the feel of a phone list.
FLING_FRICTION = 0.94
FLING_MIN_V = 0.02          # px/ms - below this, stop
FLING_FRAME_MS = 16


def attach_drag_scroll(scrolled: Gtk.ScrolledWindow) -> None:
    """Make a ScrolledWindow scroll by dragging its content, with inertia.

    GTK's own ``kinetic-scrolling`` only reacts to real touch sequences, and
    only when nothing else claims them. That leaves two gaps that matter here:
    a mouse or trackpad drag does nothing at all, and any child that handles
    button events can swallow the sequence - so in practice the only thing that
    scrolled reliably was the 6 px scrollbar, which is an unusable target on a
    3.5" touchscreen.

    This claims the gesture itself once movement is clearly vertical, so it
    behaves the same for finger and pointer, and children still receive plain
    taps because we do not claim below the threshold.
    """
    scrolled.set_kinetic_scrolling(False)   # we do the fling ourselves

    state = {"start": 0.0, "claimed": False, "anim": None,
             "last_y": 0.0, "last_t": 0, "v": 0.0}

    def stop_fling():
        if state["anim"] is not None:
            GLib.source_remove(state["anim"])
            state["anim"] = None

    def on_begin(gesture, _x, _y):
        stop_fling()
        adj = scrolled.get_vadjustment()
        state.update(start=adj.get_value() if adj else 0.0, claimed=False,
                     last_y=0.0, last_t=GLib.get_monotonic_time() // 1000, v=0.0)

    def on_update(gesture, dx, dy):
        adj = scrolled.get_vadjustment()
        if adj is None:
            return
        if not state["claimed"]:
            # Only claim once the movement is decisively vertical, so a
            # horizontal swipe-to-dismiss on a card still wins.
            if abs(dy) < DRAG_THRESHOLD or abs(dy) <= abs(dx):
                return
            state["claimed"] = True
            gesture.set_state(Gtk.EventSequenceState.CLAIMED)

        now = GLib.get_monotonic_time() // 1000
        span = now - state["last_t"]
        if span > 0:
            # Content follows the finger, so scrolling up means dy negative.
            state["v"] = -(dy - state["last_y"]) / span
            state["last_y"], state["last_t"] = dy, now
        adj.set_value(state["start"] - dy)

    def on_end(_gesture, _dx, _dy):
        if not state["claimed"]:
            return
        adj = scrolled.get_vadjustment()
        if adj is None or abs(state["v"]) < FLING_MIN_V:
            return

        def frame():
            a = scrolled.get_vadjustment()
            if a is None:
                state["anim"] = None
                return False
            state["v"] *= FLING_FRICTION
            if abs(state["v"]) < FLING_MIN_V:
                state["anim"] = None
                return False
            target = a.get_value() + state["v"] * FLING_FRAME_MS
            limit = max(a.get_lower(), a.get_upper() - a.get_page_size())
            if target <= a.get_lower() or target >= limit:
                a.set_value(max(a.get_lower(), min(limit, target)))
                state["anim"] = None
                return False        # stop dead at the ends, no bounce
            a.set_value(target)
            return True

        state["anim"] = GLib.timeout_add(FLING_FRAME_MS, frame)

    gesture = Gtk.GestureDrag.new(scrolled)
    gesture.set_touch_only(False)               # pointer drags too
    gesture.set_propagation_phase(Gtk.PropagationPhase.CAPTURE)
    gesture.connect("drag-begin", on_begin)
    gesture.connect("drag-update", on_update)
    gesture.connect("drag-end", on_end)
    # Keep a reference alive for the widget's lifetime.
    scrolled._ts_drag_scroll = gesture


def gamepad_action(keyval) -> str | None:
    """Map a key event's keyval to a gamepad action, or None.

    Used by the shell's own surfaces. The buttons are not grabbed globally, so
    this only ever fires while a TrailShell window has focus - applications get
    F13-F18 untouched.
    """
    from . import keymap
    name = Gdk.keyval_name(keyval)
    return keymap.GAMEPAD_ACTION.get(name)


# -------------------------------------------------------------- windows ---
def make_shell_window(width: int, height: int, x: int = 0, y: int = 0,
                      dock: bool = False, name: str = "trailshell",
                      keep_above: bool = True) -> Gtk.Window:
    """A borderless, non-resizable shell surface.

    ``dock=True`` also asks the WM to treat it as a panel, which combined with
    the strut set in :mod:`x11` keeps apps out from under it.

    ``keep_above`` must be False for surfaces an app is allowed to cover - the
    launcher, the switcher and the shortcuts sheet. Leaving it True there means
    a launched app can never come to the front, because the launcher outranks
    it forever.
    """
    win = Gtk.Window(type=Gtk.WindowType.TOPLEVEL)
    win.set_name(name)
    win.set_app_paintable(True)
    win.set_decorated(False)
    win.set_resizable(False)
    win.set_skip_taskbar_hint(True)
    win.set_skip_pager_hint(True)
    if keep_above:
        win.set_keep_above(True)
    win.set_type_hint(
        Gdk.WindowTypeHint.DOCK if dock else Gdk.WindowTypeHint.NORMAL
    )
    win.set_default_size(width, height)
    win.set_size_request(width, height)
    win.move(x, y)
    win.stick()
    # Enable per-pixel alpha where the compositor allows it, so scrims and the
    # partially-pulled shade can blend over the app underneath.
    screen = win.get_screen()
    visual = screen.get_rgba_visual()
    if visual is not None:
        win.set_visual(visual)
    return win


TOUCH_EVENTS = (
    Gdk.EventMask.BUTTON_PRESS_MASK
    | Gdk.EventMask.BUTTON_RELEASE_MASK
    | Gdk.EventMask.POINTER_MOTION_MASK
    | Gdk.EventMask.TOUCH_MASK
    | Gdk.EventMask.SMOOTH_SCROLL_MASK
    | Gdk.EventMask.KEY_PRESS_MASK
    | Gdk.EventMask.LEAVE_NOTIFY_MASK
)
