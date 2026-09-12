"""Thin EWMH layer over python-xlib.

The shell keeps a single X connection here for two jobs:

1. Reserving screen edges (``_NET_WM_STRUT_PARTIAL``) so the status bar and nav
   bar are never covered by an app, and every app is automatically sized to the
   418 px canvas between them. GTK does not set struts for us.
2. Enumerating / activating / closing toplevel windows, which is what the task
   switcher and the nav bar's Back and Recents actions drive.

Shell chrome tags itself with ``_TRAILSHELL_CHROME`` so it never shows up in
its own switcher.
"""
from __future__ import annotations

import logging
from dataclasses import dataclass, field
from typing import Callable, Iterable

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import GLib

log = logging.getLogger("trailshell.x11")

try:
    from Xlib import X, XK, Xatom, display as xdisplay, error as xerror
    from Xlib.protocol import event as xevent

    HAVE_XLIB = True
except ImportError:  # pragma: no cover - install-deps.sh provides python3-xlib
    HAVE_XLIB = False
    X = XK = Xatom = xdisplay = xerror = xevent = None  # type: ignore

CHROME_PROP = "_TRAILSHELL_CHROME"

#: Window types that belong in the task switcher. Everything else (docks,
#: menus, tooltips, splash screens) is chrome or transient.
_SWITCHABLE_TYPES = {"_NET_WM_WINDOW_TYPE_NORMAL", "_NET_WM_WINDOW_TYPE_DIALOG"}


@dataclass
class WindowInfo:
    xid: int
    title: str = ""
    wm_class: str = ""
    app_id: str = ""
    pid: int = 0
    is_active: bool = False
    is_dialog: bool = False
    geometry: tuple = field(default_factory=lambda: (0, 0, 0, 0))


class X11:
    """Lazily-connected EWMH client. All methods are no-ops without python-xlib."""

    def __init__(self) -> None:
        self.d = None
        self.root = None
        self._atoms: dict[str, int] = {}
        self._watch_id = None
        self._on_change: list[Callable[[], None]] = []
        self._keys: dict[tuple[int, int], Callable[[], None]] = {}
        self._grabbed: list[tuple[int, int]] = []
        self._on_configure: list[Callable[[int], None]] = []
        if HAVE_XLIB:
            try:
                self.d = xdisplay.Display()
                self.root = self.d.screen().root
            except Exception as exc:  # pragma: no cover
                log.error("cannot open X display: %s", exc)
                self.d = None

    # -- plumbing ---------------------------------------------------------
    @property
    def ok(self) -> bool:
        return self.d is not None

    def atom(self, name: str) -> int:
        if name not in self._atoms:
            self._atoms[name] = self.d.intern_atom(name)
        return self._atoms[name]

    def _prop(self, win, name: str, kind=None):
        """Read a window property, returning its raw value list or None."""
        if not self.ok:
            return None
        try:
            r = win.get_full_property(self.atom(name), kind if kind is not None else X.AnyPropertyType)
        except Exception:
            return None
        return r.value if r else None

    def _text_prop(self, win, name: str) -> str:
        v = self._prop(win, name)
        if not v:
            return ""
        if isinstance(v, bytes):
            return v.decode("utf-8", "replace")
        try:
            return bytes(v).decode("utf-8", "replace")
        except Exception:
            return str(v)

    # -- struts -----------------------------------------------------------
    def set_strut(self, xid: int, top: int = 0, bottom: int = 0,
                  screen_w: int = 0, screen_h: int = 0) -> None:
        """Reserve a horizontal band at the top or bottom of the screen.

        Sets both the modern partial strut and the legacy one, because some
        window managers only honour the older property.
        """
        if not self.ok:
            return
        win = self.d.create_resource_object("window", xid)
        # left, right, top, bottom
        base = [0, 0, top, bottom]
        # then the four start/end pairs describing how far the strut spans
        partial = base + [
            0, 0,                      # left_start_y,  left_end_y
            0, 0,                      # right_start_y, right_end_y
            0, screen_w - 1 if top else 0,     # top_start_x, top_end_x
            0, screen_w - 1 if bottom else 0,  # bottom_start_x, bottom_end_x
        ]
        try:
            win.change_property(self.atom("_NET_WM_STRUT_PARTIAL"), Xatom.CARDINAL, 32, partial)
            win.change_property(self.atom("_NET_WM_STRUT"), Xatom.CARDINAL, 32, base)
            self.d.flush()
        except Exception as exc:
            log.warning("set_strut failed: %s", exc)

    def mark_chrome(self, xid: int) -> None:
        """Tag a window as shell chrome so we exclude it from the switcher."""
        if not self.ok:
            return
        try:
            win = self.d.create_resource_object("window", xid)
            win.change_property(self.atom(CHROME_PROP), Xatom.CARDINAL, 32, [1])
            self.d.flush()
        except Exception as exc:
            log.warning("mark_chrome failed: %s", exc)

    # -- window enumeration -----------------------------------------------
    def _active_xid(self) -> int:
        v = self._prop(self.root, "_NET_ACTIVE_WINDOW")
        return int(v[0]) if v else 0

    def list_windows(self) -> list[WindowInfo]:
        """Toplevel app windows, most-recently-stacked last."""
        if not self.ok:
            return []
        ids = self._prop(self.root, "_NET_CLIENT_LIST_STACKING") or \
              self._prop(self.root, "_NET_CLIENT_LIST") or []
        active = self._active_xid()
        out: list[WindowInfo] = []
        for xid in ids:
            info = self.describe(int(xid))
            if info is None:
                continue
            info.is_active = info.xid == active
            out.append(info)
        return out

    def describe(self, xid: int) -> WindowInfo | None:
        """Build a WindowInfo, or None if the window is not user-switchable."""
        if not self.ok:
            return None
        try:
            win = self.d.create_resource_object("window", xid)
            if self._prop(win, CHROME_PROP):
                return None

            types = self._prop(win, "_NET_WM_WINDOW_TYPE") or []
            names = {self.d.get_atom_name(t) for t in types}
            # An untyped window is conventionally NORMAL.
            if names and not (names & _SWITCHABLE_TYPES):
                return None
            is_dialog = "_NET_WM_WINDOW_TYPE_DIALOG" in names

            states = self._prop(win, "_NET_WM_STATE") or []
            state_names = {self.d.get_atom_name(s) for s in states}
            if "_NET_WM_STATE_SKIP_TASKBAR" in state_names:
                return None

            title = self._text_prop(win, "_NET_WM_NAME")
            if not title:
                raw = self._prop(win, "WM_NAME")
                title = bytes(raw).decode("utf-8", "replace") if raw else ""

            cls = self._prop(win, "WM_CLASS") or b""
            parts = bytes(cls).decode("utf-8", "replace").split("\x00")
            instance = parts[0] if parts else ""
            klass = parts[1] if len(parts) > 1 else instance

            pid_v = self._prop(win, "_NET_WM_PID")
            geo = win.get_geometry()
            # get_geometry() is relative to the parent, and a managed window is
            # reparented into a WM frame - so it reports 0,0 wherever the frame
            # really is. Translate to root coordinates to get the truth.
            try:
                t = win.translate_coords(self.root, 0, 0)
                abs_x, abs_y = -t.x, -t.y
            except Exception:
                abs_x, abs_y = geo.x, geo.y
            return WindowInfo(
                xid=xid,
                title=title or klass or "Untitled",
                wm_class=klass,
                app_id=(instance or klass).lower(),
                pid=int(pid_v[0]) if pid_v else 0,
                is_dialog=is_dialog,
                geometry=(abs_x, abs_y, geo.width, geo.height),
            )
        except Exception:
            # Windows race with us constantly; a vanished window is normal.
            return None

    # -- window actions ---------------------------------------------------
    def _client_message(self, win, msg: str, data: Iterable[int]) -> None:
        d = list(data)[:5] + [0] * (5 - len(list(data)[:5]))
        ev = xevent.ClientMessage(
            window=win, client_type=self.atom(msg), data=(32, d)
        )
        self.root.send_event(
            ev, event_mask=X.SubstructureRedirectMask | X.SubstructureNotifyMask
        )
        self.d.flush()

    def activate(self, xid: int) -> None:
        if not self.ok:
            return
        try:
            win = self.d.create_resource_object("window", xid)
            # source=2 (pager) makes compliant WMs raise + focus without the
            # focus-stealing-prevention delay applied to source=1.
            self._client_message(win, "_NET_ACTIVE_WINDOW", [2, X.CurrentTime, 0])
        except Exception as exc:
            log.warning("activate(%s) failed: %s", xid, exc)

    def close(self, xid: int) -> None:
        if not self.ok:
            return
        try:
            win = self.d.create_resource_object("window", xid)
            self._client_message(win, "_NET_CLOSE_WINDOW", [X.CurrentTime, 2])
        except Exception as exc:
            log.warning("close(%s) failed: %s", xid, exc)

    def active_window(self) -> WindowInfo | None:
        xid = self._active_xid()
        return self.describe(xid) if xid else None

    # -- window policy ----------------------------------------------------
    def set_undecorated(self, xid: int) -> None:
        """Strip the title bar and border via Motif hints.

        Openbox (and most X WMs) honour _MOTIF_WM_HINTS even for windows it is
        already managing, so this works without a WM restart - and, usefully,
        it works under whatever WM happens to be running rather than depending
        on TrailShell's own openbox config.
        """
        if not self.ok:
            return
        try:
            win = self.d.create_resource_object("window", xid)
            atom = self.atom("_MOTIF_WM_HINTS")
            # flags = MWM_HINTS_DECORATIONS (1<<1); decorations = 0 (none)
            win.change_property(atom, atom, 32, [2, 0, 0, 0, 0])
            self.d.flush()
        except Exception as exc:
            log.debug("set_undecorated(%s): %s", xid, exc)

    #: _NET_MOVERESIZE_WINDOW flag word: x, y, width and height all supplied
    #: (bits 8-11), source indication "pager" (2 << 12).
    _MOVERESIZE_FLAGS = (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (2 << 12)

    def moveresize(self, xid: int, x: int, y: int, w: int, h: int) -> None:
        """Place a managed window, via EWMH so the WM stays in the loop."""
        if not self.ok:
            return
        try:
            win = self.d.create_resource_object("window", xid)
            self._client_message(win, "_NET_MOVERESIZE_WINDOW",
                                 [self._MOVERESIZE_FLAGS, x, y, w, h])
        except Exception as exc:
            log.debug("moveresize(%s): %s", xid, exc)

    def unmaximize(self, xid: int) -> None:
        """Clear maximised state so our explicit geometry is not overridden."""
        if not self.ok:
            return
        try:
            win = self.d.create_resource_object("window", xid)
            self._client_message(win, "_NET_WM_STATE", [
                0,  # _NET_WM_STATE_REMOVE
                self.atom("_NET_WM_STATE_MAXIMIZED_VERT"),
                self.atom("_NET_WM_STATE_MAXIMIZED_HORZ"),
                2,
            ])
        except Exception as exc:
            log.debug("unmaximize(%s): %s", xid, exc)

    def frame_of(self, xid: int):
        """The window manager's frame for a client, or the client itself.

        A managed window is reparented into a frame, so the thing that has to
        move is the frame, not the client. Walk up until the parent is root.
        """
        if not self.ok:
            return None
        try:
            w = self.d.create_resource_object("window", xid)
            for _ in range(16):     # guard against a pathological tree
                t = w.query_tree()
                if t.parent is None or t.parent.id == self.root.id:
                    return w
                w = t.parent
        except Exception as exc:
            log.debug("frame_of(%s): %s", xid, exc)
        return None

    def move_frame(self, xid: int, x: int, y: int) -> bool:
        """Move a window by repositioning its WM frame directly.

        _NET_MOVERESIZE_WINDOW is the polite route and is what normal
        placement uses, but some clients re-centre themselves the moment the
        window manager moves them - GIMP's "Welcome to GIMP" dialog does
        exactly that, so a requested move is silently undone. Configuring the
        frame goes under both the WM's placement policy and the client's own
        opinion, which is what panning an oversized window needs.
        """
        fr = self.frame_of(xid)
        if fr is None:
            return False
        try:
            fr.configure(x=int(x), y=int(y))
            self.d.sync()
            return True
        except Exception as exc:
            log.debug("move_frame(%s): %s", xid, exc)
            return False

    def size_hints(self, xid: int) -> dict:
        """WM_NORMAL_HINTS constraints on what size a client can actually take.

        Terminals resize in whole character cells: lxterminal reports
        base 15x34 with increments of 8x17, so the largest size it can adopt
        inside a 640x414 canvas is 639x408. Asking for 640x414 anyway gets
        639x408 back, and if the policy reads that as failure it re-places the
        window forever. Snapping the target to the increments avoids the fight
        entirely rather than papering over it with a tolerance.
        """
        out = {"base_w": 0, "base_h": 0, "inc_w": 1, "inc_h": 1,
               "min_w": 1, "min_h": 1, "max_w": 1 << 30, "max_h": 1 << 30}
        if not self.ok:
            return out
        try:
            win = self.d.create_resource_object("window", xid)
            h = win.get_wm_normal_hints()
            if not h:
                return out
            out["inc_w"] = max(1, h.width_inc or 1)
            out["inc_h"] = max(1, h.height_inc or 1)
            out["base_w"] = h.base_width or 0
            out["base_h"] = h.base_height or 0
            out["min_w"] = max(1, h.min_width or 1)
            out["min_h"] = max(1, h.min_height or 1)
            if h.max_width:
                out["max_w"] = h.max_width
            if h.max_height:
                out["max_h"] = h.max_height
        except Exception as exc:
            log.debug("size_hints(%s): %s", xid, exc)
        return out

    def watch_geometry(self, xid: int) -> None:
        """Ask for ConfigureNotify on a client window so drags are seen."""
        if not self.ok:
            return
        try:
            win = self.d.create_resource_object("window", xid)
            # StructureNotify is not an exclusive selection, so this does not
            # conflict with the window manager's own interest in the window.
            win.change_attributes(event_mask=X.StructureNotifyMask)
            self.d.flush()
        except Exception as exc:
            log.debug("watch_geometry(%s): %s", xid, exc)

    def on_configure(self, callback: Callable[[int], None]) -> None:
        self._on_configure.append(callback)

    # -- change notifications ---------------------------------------------
    def watch(self, callback: Callable[[], None]) -> None:
        """Call ``callback`` whenever the window list or focus changes.

        Hooks the X connection's fd into the GLib main loop rather than
        polling, so an idle shell costs no CPU.
        """
        if not self.ok:
            return
        self._on_change.append(callback)
        if self._watch_id is not None:
            return
        self.root.change_attributes(event_mask=X.PropertyChangeMask)
        self.d.flush()
        interesting = {
            self.atom("_NET_CLIENT_LIST"),
            self.atom("_NET_CLIENT_LIST_STACKING"),
            self.atom("_NET_ACTIVE_WINDOW"),
        }

        def _pump(*_a):
            dirty = False
            try:
                for _ in range(self.d.pending_events()):
                    ev = self.d.next_event()
                    etype = getattr(ev, "type", None)
                    if etype == X.KeyPress:
                        self._dispatch_key(ev)
                    elif etype == X.ConfigureNotify:
                        for cb in self._on_configure:
                            try:
                                cb(ev.window.id)
                            except Exception:
                                log.exception("configure callback failed")
                    elif etype == X.PropertyNotify and ev.atom in interesting:
                        dirty = True
            except Exception as exc:
                log.warning("x event pump: %s", exc)
            if dirty:
                for cb in self._on_change:
                    try:
                        cb()
                    except Exception:
                        log.exception("window-change callback failed")
            return True

        self._watch_id = GLib.io_add_watch(self.d.fileno(), GLib.IO_IN, _pump)
        # python-xlib buffers events internally, so any flush()/sync() the
        # shell does elsewhere can drain the socket and leave events queued
        # with the fd no longer readable - the watch then never fires and the
        # launcher silently fails to get out of a newly-opened app's way.
        # A slow drain alongside the fd watch closes that gap for ~nothing.
        self._drain_id = GLib.timeout_add(350, _pump)


    # -- global hotkeys ---------------------------------------------------
    #: Modifiers that are on/off state rather than intent. A grab must be
    #: registered for every combination of them or the binding mysteriously
    #: stops working the moment Num Lock is on.
    _LOCK_MASKS = (0, 1 << 1, 1 << 4, (1 << 1) | (1 << 4))  # Lock, Mod2

    _MOD_NAMES = {
        "shift": 1 << 0,
        "ctrl": 1 << 2, "control": 1 << 2,
        "alt": 1 << 3, "mod1": 1 << 3,
        "super": 1 << 6, "mod4": 1 << 6, "win": 1 << 6,
    }

    def bind(self, accel: str, callback: Callable[[], None]) -> bool:
        """Grab a global hotkey, e.g. ``"Super+a"`` or ``"XF86AudioMute"``.

        Returns False if the key does not exist in the current keymap, which
        is normal for media keys the PocketTerm35's firmware does not emit.
        """
        if not self.ok:
            return False
        parts = accel.split("+")
        keyname, modnames = parts[-1], [p.lower() for p in parts[:-1]]

        mask = 0
        for m in modnames:
            if m not in self._MOD_NAMES:
                log.warning("unknown modifier %r in %r", m, accel)
                return False
            mask |= self._MOD_NAMES[m]

        # python-xlib's keysym table omits the XF86 block, so fall back to a
        # small table of the ones the shell actually cares about.
        keysym = XK.string_to_keysym(keyname) or _xf86_keysym(keyname)
        if not keysym:
            log.info("no keysym for %r - not binding %s", keyname, accel)
            return False
        keycode = self.d.keysym_to_keycode(keysym)
        if not keycode:
            log.info("%r is not on this keymap - not binding %s", keyname, accel)
            return False

        # X reports grab failures asynchronously, so without a catcher plus an
        # explicit sync a key already held by another client looks like it
        # bound fine and then silently never fires.
        catcher = xerror.CatchError(xerror.BadAccess)
        taken = []
        for extra in self._LOCK_MASKS:
            self.root.grab_key(keycode, mask | extra, False,
                               X.GrabModeAsync, X.GrabModeAsync, onerror=catcher)
            taken.append((keycode, mask | extra))
        self.d.sync()
        if catcher.get_error():
            # Someone else owns it. Release the partial grab so we do not leave
            # the key half-captured.
            for kc, m in taken:
                try:
                    self.root.ungrab_key(kc, m)
                except Exception:
                    pass
            self.d.sync()
            log.warning("%s is already grabbed by another client - skipping", accel)
            return False

        self._keys[(keycode, mask)] = callback
        self._grabbed.extend(taken)
        return True

    def ungrab_all(self) -> None:
        if not self.ok:
            return
        for keycode, mask in self._grabbed:
            try:
                self.root.ungrab_key(keycode, mask)
            except Exception:
                pass
        self._grabbed.clear()
        self._keys.clear()
        self.d.flush()

    def _dispatch_key(self, ev) -> bool:
        # Mask out Caps/Num Lock; they are state, not intent.
        state = ev.state & ~((1 << 1) | (1 << 4))
        cb = self._keys.get((ev.detail, state))
        if cb is None:
            return False
        try:
            cb()
        except Exception:
            log.exception("hotkey handler failed")
        return True


#: XF86 keysyms live in a fixed block that python-xlib's table does not carry.
_XF86_KEYSYMS = {
    "XF86AudioLowerVolume": 0x1008FF11,
    "XF86AudioMute": 0x1008FF12,
    "XF86AudioRaiseVolume": 0x1008FF13,
    "XF86MonBrightnessUp": 0x1008FF02,
    "XF86MonBrightnessDown": 0x1008FF03,
    "XF86PowerOff": 0x1008FF2A,
    "XF86Search": 0x1008FF1B,
}


def _xf86_keysym(name: str) -> int:
    return _XF86_KEYSYMS.get(name, 0)


#: Process-wide connection. The shell is single-process by design.
_shared: X11 | None = None


def get() -> X11:
    global _shared
    if _shared is None:
        _shared = X11()
    return _shared
