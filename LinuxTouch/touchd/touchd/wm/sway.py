"""sway implementation of the WindowManager adapter.

Talks to sway through `swaymsg` rather than by speaking the i3 IPC protocol
over its socket directly. That costs a process spawn (~20 ms) per query, which
is irrelevant at the rate a 640x480 shell asks questions, and it buys a module
that can be read and debugged by anyone who can run `swaymsg -t get_tree` in a
terminal. If profiling ever says otherwise, the protocol is 80 lines of struct
packing and this file is the only thing that would change.

Window events arrive from one long-running `swaymsg -m -t subscribe` process,
which streams a JSON object per line for as long as it is alive.
"""

from __future__ import annotations

import asyncio
import glob
import json
import logging
import os
import re
from typing import Awaitable, Callable

from ..proc import CommandError, run
from .base import Window, WindowManager

log = logging.getLogger(__name__)


class SwayWM(WindowManager):
    def __init__(self) -> None:
        self._callbacks: list[Callable[[], Awaitable[None]]] = []
        self._monitor: asyncio.subprocess.Process | None = None
        self._task: asyncio.Task | None = None
        # Most-recently-focused first. sway's tree is in layout order and has
        # no concept of focus history beyond the single currently-focused node,
        # so the switcher's ordering has to be maintained here. Without it the
        # cards would sit in launch order, and the app you were just in would
        # not be the first one you see.
        self._mru: list[str] = []

    # ── lifecycle ────────────────────────────────────────────────────

    async def start(self) -> None:
        ensure_swaysock()
        self._task = asyncio.create_task(self._watch())

    async def stop(self) -> None:
        if self._task:
            self._task.cancel()
        if self._monitor and self._monitor.returncode is None:
            self._monitor.kill()
            await self._monitor.wait()

    def on_change(self, callback: Callable[[], Awaitable[None]]) -> None:
        self._callbacks.append(callback)

    # ── queries ──────────────────────────────────────────────────────

    async def windows(self) -> list[Window]:
        try:
            raw = await run(["swaymsg", "-r", "-t", "get_tree"], timeout=5)
        except CommandError as exc:
            # Not fatal. The daemon starts before the compositor when the unit
            # is enabled at boot, and the shell asks for windows immediately.
            # An empty list is the honest answer to "what is running" when we
            # cannot see the compositor at all.
            log.warning("get_tree failed: %s", exc)
            return []

        found: list[Window] = []
        _collect(json.loads(raw), found)

        # Drop the shell itself. It is the surface the switcher is drawn on;
        # showing it as a card would let the user "close" their own home screen.
        found = [w for w in found if not self._is_shell(w)]

        self._prune_mru({w.wid for w in found})
        order = {wid: i for i, wid in enumerate(self._mru)}
        found.sort(key=lambda w: order.get(w.wid, len(order)))
        return found

    async def focus(self, wid: str) -> None:
        # `fullscreen enable` is issued with every focus, not just at map time.
        #
        # sway allows only ONE fullscreen window per workspace, so the moment a
        # launched app goes fullscreen the shell's own fullscreen is revoked.
        # Focusing the shell again then shows it TILED beside that app — two
        # half-width windows on a 640px screen, which is exactly the desktop
        # behaviour this product exists to avoid. Observed on hardware
        # 2026-09-05 with galculator open.
        #
        # Re-asserting fullscreen on every switch is what actually implements
        # "one app at a time"; the for_window rule only covers the first map.
        await self._command(f"[con_id={_int(wid)}] focus, fullscreen enable")
        self._touch_mru(wid)

    async def close(self, wid: str) -> None:
        # `kill` in sway's command language is a close request to the client,
        # not SIGKILL — an editor with unsaved changes still gets to put up its
        # "save before closing?" dialog.
        await self._command(f"[con_id={_int(wid)}] kill")

    async def focus_shell(self) -> None:
        # By mark, not app_id — see WindowManager.shell_mark for why.
        # fullscreen enable for the same reason as focus() above.
        await self._command(
            f'[con_mark="{self.shell_mark}"] focus, fullscreen enable')

    async def shell_focused(self) -> bool:
        """True if the home screen already has focus.

        Start means "go home" from inside an app, but "show the switcher" when
        you are already home. Only the compositor knows which of those applies,
        because the shell cannot tell whether it is the focused window — it
        keeps rendering whatever screen it was last on while an app covers it.
        """
        try:
            raw = await run(["swaymsg", "-r", "-t", "get_tree"], timeout=5)
        except CommandError:
            return False
        found: list[Window] = []
        _collect(json.loads(raw), found)
        return any(w.focused and self._is_shell(w) for w in found)

    def _is_shell(self, window: Window) -> bool:
        if self.shell_mark in window.marks:
            return True
        # The mark rule may not have matched (a config that failed to load, a
        # different compositor). Falling back on the app_id pattern is better
        # than putting a closeable card for the home screen in the switcher.
        return re.search(self.shell_app_id_pattern, window.app_id) is not None

    # ── internals ────────────────────────────────────────────────────

    async def _command(self, cmd: str) -> None:
        try:
            await run(["swaymsg", "-r", "--", cmd], timeout=5)
        except CommandError as exc:
            log.warning("sway command %r failed: %s", cmd, exc)

    def _touch_mru(self, wid: str) -> None:
        if wid in self._mru:
            self._mru.remove(wid)
        self._mru.insert(0, wid)

    def _prune_mru(self, live: set[str]) -> None:
        self._mru = [w for w in self._mru if w in live]
        for wid in live:
            if wid not in self._mru:
                self._mru.append(wid)

    async def _watch(self) -> None:
        """Stream window events, retrying forever.

        The retry loop is not defensive padding. touchd is a user service that
        may start before sway has created its socket, and `swaymsg -m` exits
        immediately in that case. Without the loop the switcher would be
        permanently stale after any boot that lost that race.
        """
        while True:
            try:
                self._monitor = await asyncio.create_subprocess_exec(
                    "swaymsg", "-r", "-m", "-t", "subscribe", '["window"]',
                    stdout=asyncio.subprocess.PIPE,
                    stderr=asyncio.subprocess.DEVNULL,
                )
                assert self._monitor.stdout is not None
                async for line in self._monitor.stdout:
                    await self._on_event(line)
            except asyncio.CancelledError:
                raise
            except Exception as exc:  # noqa: BLE001 — must never kill the daemon
                log.warning("sway event stream failed: %s", exc)
            await asyncio.sleep(2)

    async def _on_event(self, line: bytes) -> None:
        try:
            event = json.loads(line)
        except json.JSONDecodeError:
            return

        change = event.get("change")
        container = event.get("container") or {}
        wid = str(container.get("id", ""))

        if change == "focus" and wid:
            self._touch_mru(wid)

        if change in ("new", "close", "focus", "title", "fullscreen_mode"):
            for cb in self._callbacks:
                try:
                    await cb()
                except Exception:  # noqa: BLE001
                    log.exception("window-change callback failed")


def ensure_swaysock() -> str | None:
    """Find sway's IPC socket if the environment did not provide it.

    Defensive, and worth the twenty lines. touchd is normally started by sway
    after `systemctl --user import-environment`, so SWAYSOCK is inherited — but
    a systemd user unit that starts any other way (an operator running
    `systemctl --user restart touchd` from an SSH session, a unit that got
    enabled, a race at login) has no SWAYSOCK at all. The failure that produces
    is silent and confusing: every swaymsg call fails, so the switcher is
    permanently empty and Home does nothing, while the daemon reports healthy.

    Discovering the socket ourselves makes the daemon independent of how it was
    launched. Also exports WAYLAND_DISPLAY, because applications launched from
    the grid inherit touchd's environment and need a compositor to connect to.
    """
    runtime = os.environ.get("XDG_RUNTIME_DIR") or f"/run/user/{os.getuid()}"

    if not os.environ.get("SWAYSOCK"):
        # Newest wins: if a previous session left a stale socket behind, the
        # live compositor's is the most recently created.
        sockets = sorted(
            glob.glob(f"{runtime}/sway-ipc.*.sock"),
            key=os.path.getmtime,
            reverse=True,
        )
        if sockets:
            os.environ["SWAYSOCK"] = sockets[0]
            log.info("discovered SWAYSOCK at %s", sockets[0])
        else:
            log.warning("no sway IPC socket under %s; window control unavailable",
                        runtime)

    if not os.environ.get("WAYLAND_DISPLAY"):
        displays = sorted(
            name for name in os.listdir(runtime)
            if name.startswith("wayland-") and not name.endswith(".lock")
        ) if os.path.isdir(runtime) else []
        if displays:
            # wayland-1 over wayland-0 when both exist: wayland-0 is typically
            # a parent compositor in a nested setup, and the app windows we
            # care about belong to ours.
            os.environ["WAYLAND_DISPLAY"] = displays[-1]
            log.info("discovered WAYLAND_DISPLAY=%s", displays[-1])

    return os.environ.get("SWAYSOCK")


def _collect(node: dict, out: list[Window]) -> None:
    """Walk sway's tree and pick out the real application windows.

    A node is an application window if it has a PID. Workspaces, outputs, and
    split containers do not, which is a cleaner test than matching on `type`
    because it treats floating windows and tiled ones identically.
    """
    if node.get("pid") is not None and node.get("name") is not None:
        props = node.get("window_properties") or {}
        out.append(
            Window(
                wid=str(node["id"]),
                # app_id is the Wayland identity; window_properties.class is
                # the X11 one, present only for XWayland clients. Mail clients
                # and older editors are still commonly the latter, so both have
                # to be handled or half the app grid produces nameless cards.
                app_id=node.get("app_id") or props.get("class") or "unknown",
                title=node.get("name") or "",
                pid=node.get("pid"),
                focused=bool(node.get("focused")),
                marks=list(node.get("marks") or []),
            )
        )
    for child in (node.get("nodes") or []) + (node.get("floating_nodes") or []):
        _collect(child, out)


def _int(wid: str) -> int:
    """Guard the one place a caller-supplied value reaches sway's parser.

    Window IDs come back to us from the shell over the WebSocket, so they are
    untrusted input being spliced into a command string. Forcing an int means
    a crafted wid cannot become extra sway commands.
    """
    try:
        return int(wid)
    except (TypeError, ValueError):
        raise ValueError(f"invalid window id: {wid!r}") from None
