"""The window-manager adapter interface.

Everything above this line is compositor-agnostic. `sway.py` is the only
implementation today; a `labwc.py` would be a drop-in replacement and nothing
else in touchd would change.

That boundary is the whole reason this file exists. sway was chosen because its
IPC reports a **PID per window**, which is what lets the shell answer "which
window did the app I just launched produce?" — the `wlr-foreign-toplevel`
protocol that labwc offers carries no PID, so the same question becomes
guesswork against app IDs and titles. If that ever stops mattering, swapping
compositors is one file. See docs/architecture.md.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Awaitable, Callable


@dataclass
class Window:
    """One toplevel window.

    `wid` is opaque to everything above the adapter — it happens to be sway's
    con_id, but no caller may assume that.
    """

    wid: str
    app_id: str
    title: str
    pid: int | None = None
    focused: bool = False
    marks: list[str] = field(default_factory=list)

    def as_dict(self) -> dict:
        return {
            "wid": self.wid,
            "app_id": self.app_id,
            "title": self.title,
            "pid": self.pid,
            "focused": self.focused,
        }


class WindowManager:
    """Abstract compositor control."""

    #: How the shell's own window is recognised. The adapter must never report
    #: it as an ordinary app, or the home screen would appear as a card in its
    #: own switcher — with a close button on it.
    #:
    #: A compositor mark rather than an app_id, because Chromium ignores
    #: --class on Wayland and names its window after the URL it was given
    #: ("chrome-127.0.0.1__-Default"). The mark is applied by a for_window rule
    #: in wm/sway/config, which is the one place that fragile string lives.
    shell_mark = "linuxtouch_shell"

    #: Fallback for compositors with no mark concept, and for the case where
    #: the mark rule has not matched. Matches Chromium's URL-derived app_id.
    shell_app_id_pattern = r"^chrome-127\.0\.0\.1"

    async def start(self) -> None:
        """Connect, and begin watching for window changes."""

    async def stop(self) -> None:
        """Release anything held by start()."""

    async def windows(self) -> list[Window]:
        """Every toplevel window except the shell's own, most recent first."""
        raise NotImplementedError

    async def focus(self, wid: str) -> None:
        raise NotImplementedError

    async def close(self, wid: str) -> None:
        """Ask the window to close. Politely — this is not a kill."""
        raise NotImplementedError

    async def focus_shell(self) -> None:
        """Bring the home screen forward. This is what Home does."""
        raise NotImplementedError

    async def shell_focused(self) -> bool:
        """True if the home screen already holds focus."""
        raise NotImplementedError

    def on_change(self, callback: Callable[[], Awaitable[None]]) -> None:
        """Register a coroutine to run when the window set changes."""
        raise NotImplementedError
