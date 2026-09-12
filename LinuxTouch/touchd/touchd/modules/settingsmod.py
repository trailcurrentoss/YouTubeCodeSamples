"""Settings RPC.

The store itself is touchd/settings.py; this only exposes it. Kept separate so
that modules can read settings synchronously during start() without depending
on the RPC layer being up.
"""

from __future__ import annotations

from typing import Any

from .base import Module


class SettingsModule(Module):
    name = "settings"

    def rpc(self):
        return {"all": self.all, "get": self.get, "set": self.set}

    async def all(self) -> dict:
        return self.hub.settings.all()

    async def get(self, key: str) -> Any:
        return self.hub.settings.get(key)

    async def set(self, key: str, value: Any) -> dict:
        await self.hub.settings.set(key, value)
        await self.emit("changed", {"key": key, "value": value})
        return {"ok": True}
