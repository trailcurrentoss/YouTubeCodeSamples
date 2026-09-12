"""Window policy: apps are full-screen, chromeless and immovable.

Openbox's own config (session/openbox-rc.xml) asks for undecorated, maximised
windows, but that is not sufficient on its own, for three reasons:

  * Openbox has no "this window may not be moved" rule. An Alt+drag, or a
    client sending _NET_WM_MOVERESIZE from its own header bar, still moves a
    maximised window - and moving it unmaximises it.
  * Applications with client-side decorations (Chromium, and any GTK app with
    a header bar) map themselves at a remembered windowed size and ignore the
    window manager's placement preferences.
  * "Maximised" is computed by the WM from whatever struts are currently set.
    While TrailShell runs on top of another desktop for testing, those are the
    *other* panel's struts, so apps end up covering our nav bar.

So the shell enforces the policy itself over EWMH rather than delegating it.
That holds under any window manager, which is also what lets `trailshell` run
sensibly on top of an existing session.

The canvas is the region between the status bar and the nav bar: 640x414 at
y=36. Nothing that is not shell chrome is ever allowed outside it.

Size hints are respected rather than overridden. A terminal resizes in whole
character cells - lxterminal reports base 15x34 with increments of 8x17, so the
largest it can be inside the canvas is 639x408. Demanding 640x414 anyway gets
639x408 back, and treating that as a failure produces an endless resize war.
"""
from __future__ import annotations

import logging

from gi.repository import GLib

from . import theme as T, x11

log = logging.getLogger("trailshell.wm")

#: Ignore differences this small; sub-pixel disagreements about frame extents
#: must not start a resize war.
TOLERANCE = 2

#: Never re-place the same window more often than this.
REENFORCE_MS = 200

#: After this many placements of one window we accept whatever it settled on.
#: Belt-and-braces behind the size-hint maths: a client that insists on a
#: geometry we cannot predict must not be fought forever. What actually matters
#: - inside the canvas, undecorated, not movable - is already true by then.
MAX_ATTEMPTS = 4

#: Dialogs stay floating, but are clamped inside the canvas so they can never
#: cover the status bar or the nav bar.
DIALOG_MARGIN = T.SP_3

#: How far one pan step moves an oversized window.
PAN_STEP = T.sp(80)


class WindowPolicy:
    def __init__(self, X: "x11.X11") -> None:
        self.X = X
        self._known: dict[int, float] = {}    # xid -> last placement time (ms)
        self._attempts: dict[int, int] = {}   # xid -> placements so far
        self._hints: dict[int, dict] = {}     # xid -> cached WM_NORMAL_HINTS
        self._pending: set[int] = set()
        #: xid -> pixels the window is scrolled up by. Some applications have a
        #: minimum size larger than the whole canvas - GIMP's main window will
        #: not go below 640x503 no matter how it is asked - so the bottom of
        #: the window, where the buttons live, is simply off screen. Since the
        #: policy also forbids dragging, panning is the replacement escape
        #: hatch, not a luxury.
        self._pan: dict[int, int] = {}
        #: Windows that have exhausted their placement budget without taking
        #: the size we asked for. GIMP's "Welcome to GIMP" dialog is 653x684
        #: and will not shrink; once a window is in here we stop trying and
        #: switch to keeping it reachable instead.
        self._refused: set[int] = set()
        #: Called with True/False when the focused window's oversize state
        #: changes, so the shell can show or hide the pan control.
        self.on_oversize_changed = None
        self._oversize_shown = False

    def start(self) -> None:
        self.X.on_configure(self._on_configure)
        self.sync()
        # Safety net: a client can change geometry in ways that produce no
        # ConfigureNotify we see (for instance on remap). A slow sweep costs
        # nothing for a handful of windows and guarantees convergence.
        GLib.timeout_add_seconds(2, self._sweep)

    def _sweep(self) -> bool:
        self.sync()
        return True

    # -- policy -----------------------------------------------------------
    def sync(self) -> None:
        """Apply the policy to every current window; adopt any new ones."""
        live = set()
        for info in self.X.list_windows():
            live.add(info.xid)
            if info.xid not in self._known:
                self._adopt(info)
            self.enforce(info)
        for xid in set(self._known) - live:
            self._forget(xid)
        self.check_oversize()

    def _adopt(self, info) -> None:
        """First contact with a window: strip its chrome and start watching."""
        log.info("adopting %s (%s)", info.wm_class or "?", info.title[:40])
        self._known[info.xid] = 0.0
        self._attempts[info.xid] = 0
        self._hints[info.xid] = self.X.size_hints(info.xid)
        self.X.watch_geometry(info.xid)
        self.X.set_undecorated(info.xid)
        # Clear maximised state: while it is set the WM recomputes geometry
        # from struts and overrides the placement we are about to request.
        self.X.unmaximize(info.xid)

    def _forget(self, xid: int) -> None:
        self._known.pop(xid, None)
        self._attempts.pop(xid, None)
        self._hints.pop(xid, None)
        self._pan.pop(xid, None)
        self._refused.discard(xid)

    def enforce(self, info) -> None:
        target = self._target_for(info)
        if target is None:
            return
        x, y, w, h = info.geometry
        tx, ty, tw, th = target
        panning = self._panning(info)

        if (abs(x - tx) <= TOLERANCE and abs(y - ty) <= TOLERANCE
                and abs(w - tw) <= TOLERANCE and abs(h - th) <= TOLERANCE):
            if not panning:
                self._attempts[info.xid] = 0      # settled; reset the budget
            return

        if not panning and self._attempts.get(info.xid, 0) >= MAX_ATTEMPTS:
            # This client will not take the size we ask for. Record that, so
            # _target_for switches to keeping it pannable rather than fighting.
            if w > T.SCREEN_W or h > T.APP_H:
                self._refused.add(info.xid)
                self.check_oversize()
            return

        now = GLib.get_monotonic_time() / 1000.0
        if now - self._known.get(info.xid, 0.0) < REENFORCE_MS:
            self._defer(info.xid)
            return
        self._known[info.xid] = now
        if not panning:
            self._attempts[info.xid] = self._attempts.get(info.xid, 0) + 1
        log.debug("placing %s (try %d)%s: %dx%d+%d+%d -> %dx%d+%d+%d",
                  info.wm_class, self._attempts.get(info.xid, 0),
                  " [pan]" if panning else "", w, h, x, y, tw, th, tx, ty)
        if panning:
            # Size is the client's own - only the position is ours - and the
            # move has to bypass both the WM and the client (see move_frame).
            if not self.X.move_frame(info.xid, tx, ty):
                self.X.moveresize(info.xid, tx, ty, tw, th)
        else:
            self.X.moveresize(info.xid, tx, ty, tw, th)

    def _panning(self, info) -> bool:
        """True once a window has refused to fit and is being panned instead."""
        return (info.xid in self._refused
                and (info.geometry[3] > T.APP_H or info.geometry[2] > T.SCREEN_W))

    def _target_for(self, info) -> tuple[int, int, int, int] | None:
        """Where this window belongs, or None to leave it alone."""
        aw, ah = info.geometry[2], info.geometry[3]

        if self._panning(info):
            # The client refused to fit and we have stopped asking. Keep its
            # own size, centre what overflows horizontally, and honour the pan
            # offset vertically so the bottom is still reachable. This applies
            # to dialogs too - an oversized modal whose buttons are off screen
            # is the worst case of all, since it also blocks its parent.
            pan = max(0, min(self._pan.get(info.xid, 0), max(0, ah - T.APP_H)))
            self._pan[info.xid] = pan
            return ((T.SCREEN_W - aw) // 2, T.APP_Y - pan, aw, ah)

        if info.is_dialog:
            # Floating, but clamped inside the canvas and centred. A modal
            # confirmation stretched to full screen looks broken; a modal that
            # covers the nav bar traps the user. This does neither.
            w = min(aw, T.SCREEN_W - DIALOG_MARGIN * 2)
            h = min(ah, T.APP_H - DIALOG_MARGIN * 2)
            return ((T.SCREEN_W - w) // 2, T.APP_Y + (T.APP_H - h) // 2, w, h)

        w, h = self._fit_to_hints(info.xid, T.SCREEN_W, T.APP_H)
        # Centre horizontally if the size hints forced it narrower than the
        # screen, so the leftover pixel column is split rather than dumped on
        # one edge.
        return ((T.SCREEN_W - w) // 2, T.APP_Y, w, h)

    def _fit_to_hints(self, xid: int, w: int, h: int) -> tuple[int, int]:
        """Largest size not exceeding (w, h) that the client can adopt."""
        hints = self._hints.get(xid)
        if not hints:
            return w, h

        def snap(want: int, base: int, inc: int, lo: int, hi: int) -> int:
            want = max(lo, min(hi, want))
            if inc > 1:
                steps = (want - base) // inc
                if steps < 0:
                    return max(lo, base)
                want = base + steps * inc
            return max(lo, want)

        return (snap(w, hints["base_w"], hints["inc_w"], hints["min_w"], hints["max_w"]),
                snap(h, hints["base_h"], hints["inc_h"], hints["min_h"], hints["max_h"]))

    # -- panning ----------------------------------------------------------
    def pan_range(self, xid: int) -> int:
        """How many pixels of this window are off the bottom of the canvas.

        Measured from the window's ACTUAL height, not from its size hints:
        GIMP's main window advertises a 527 px minimum but settles at 503, and
        panning by the advertised figure would scroll past the end.
        """
        if xid not in self._refused:
            return 0
        info = self.X.describe(xid)
        if info is None:
            return 0
        return max(0, info.geometry[3] - T.APP_H)

    def is_oversized(self, xid: int) -> bool:
        """True if part of this window is off the bottom of the canvas."""
        return self.pan_range(xid) > 0

    def pan(self, xid: int, delta: int) -> None:
        """Scroll an oversized window by ``delta`` pixels (positive = down)."""
        span = self.pan_range(xid)
        if span <= 0:
            return
        current = self._pan.get(xid, 0)
        new = max(0, min(span, current + delta))
        if new == current:
            return
        self._pan[xid] = new
        # A pan is a deliberate new target, so the placement budget resets -
        # otherwise MAX_ATTEMPTS from the initial placement would block it.
        self._attempts[xid] = 0
        self._known[xid] = 0.0
        info = self.X.describe(xid)
        if info is not None:
            self.enforce(info)

    def pan_active(self, delta_steps: int) -> bool:
        """Pan the focused window. True if there was anything to pan."""
        info = self.X.active_window()
        if info is None or not self.is_oversized(info.xid):
            return False
        self.pan(info.xid, delta_steps * PAN_STEP)
        return True

    def check_oversize(self) -> None:
        """Tell the shell whether the focused window needs the pan control."""
        info = self.X.active_window()
        now = bool(info and self.is_oversized(info.xid))
        if now != self._oversize_shown:
            self._oversize_shown = now
            if self.on_oversize_changed:
                self.on_oversize_changed(now)

    # -- events -----------------------------------------------------------
    def _on_configure(self, xid: int) -> None:
        if xid not in self._known:
            return
        info = self.X.describe(xid)
        if info is not None:
            self.enforce(info)

    def _defer(self, xid: int) -> None:
        """Re-check shortly, coalescing repeats for the same window."""
        if xid in self._pending:
            return
        self._pending.add(xid)

        def again() -> bool:
            self._pending.discard(xid)
            info = self.X.describe(xid)
            if info is not None:
                self.enforce(info)
            return False

        GLib.timeout_add(REENFORCE_MS + 30, again)
