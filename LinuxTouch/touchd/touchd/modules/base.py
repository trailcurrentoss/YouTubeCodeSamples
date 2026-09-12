"""Module base class.

A module owns exactly one subsystem and is the only code allowed to shell out
for it. The shell never runs a command and never parses command output — if a
new capability needs a subprocess, it belongs behind an RPC method here.

Methods are exposed as `"<module>.<name>"`, so `rpc()` returning `{"list": ...}`
on the `apps` module publishes `apps.list`.
"""

from __future__ import annotations

from typing import Any, Awaitable, Callable

RPC = Callable[..., Awaitable[Any]]


class Module:
    name = "base"

    def __init__(self, hub) -> None:
        self.hub = hub

    async def start(self) -> None:
        """Called once, after every module is constructed."""

    async def stop(self) -> None:
        """Called on shutdown. Must not raise."""

    def rpc(self) -> dict[str, RPC]:
        """Method name → coroutine. Keyword arguments come from `params`."""
        return {}

    async def emit(self, event: str, data: Any) -> None:
        """Push an unsolicited event to every connected client."""
        await self.hub.broadcast(f"{self.name}.{event}", data)
