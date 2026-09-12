"""Window control — the switcher's backend.

A thin RPC surface over the WM adapter, so the shell never learns which
compositor is running. See touchd/wm/base.py for why that boundary exists.
"""

from __future__ import annotations

import logging

from .base import Module

log = logging.getLogger(__name__)


class WindowsModule(Module):
    name = "wm"

    def __init__(self, hub) -> None:
        super().__init__(hub)
        self.wm = hub.wm

    def rpc(self):
        return {
            "windows": self.windows,
            "focus": self.focus,
            "close": self.close,
            "home": self.home,
        }

    async def start(self) -> None:
        self.wm.on_change(self._changed)
        await self.wm.start()

    async def stop(self) -> None:
        await self.wm.stop()

    async def _changed(self) -> None:
        await self.emit("windows", await self.windows())

    async def windows(self) -> list[dict]:
        return [w.as_dict() for w in await self.wm.windows()]

    async def focus(self, wid: str) -> dict:
        await self.wm.focus(str(wid))
        return {"ok": True}

    async def close(self, wid: str) -> dict:
        await self.wm.close(str(wid))
        return {"ok": True}

    async def home(self) -> dict:
        await self.wm.focus_shell()
        return {"ok": True}
