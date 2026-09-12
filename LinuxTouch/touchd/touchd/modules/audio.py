"""Volume, via PipeWire's wpctl.

The speaker is fed from HDMI, not the SoC's PWM audio block, so `dtparam=audio`
in config.txt does not control it either way — the sink to adjust is the HDMI
one, which is also the default sink. See docs/hardware.md.
"""

from __future__ import annotations

import logging

from ..proc import CommandError, have, run
from .base import Module

log = logging.getLogger(__name__)

SINK = "@DEFAULT_AUDIO_SINK@"


class AudioModule(Module):
    name = "audio"

    def rpc(self):
        return {"get": self.get, "set": self.set, "setMuted": self.set_muted}

    async def get(self) -> dict:
        if not have("wpctl"):
            return {"available": False, "volume": 0, "muted": True}
        try:
            out = await run(["wpctl", "get-volume", SINK], timeout=5)
        except CommandError as exc:
            log.warning("wpctl get-volume: %s", exc)
            return {"available": False, "volume": 0, "muted": True}
        # "Volume: 0.45" or "Volume: 0.45 [MUTED]"
        parts = out.split()
        volume = 0.0
        for index, token in enumerate(parts):
            if token == "Volume:" and index + 1 < len(parts):
                try:
                    volume = float(parts[index + 1])
                except ValueError:
                    volume = 0.0
        return {
            "available": True,
            "volume": round(volume * 100),
            "muted": "[MUTED]" in out,
        }

    async def set(self, volume: int) -> dict:
        # Clamped rather than validated-and-rejected: a slider that can be
        # dragged past its own end should saturate, not throw an error onto a
        # screen with no room for one. The 100 ceiling avoids wpctl's software
        # boost, which distorts badly on this speaker.
        level = max(0, min(100, int(volume)))
        await run(["wpctl", "set-volume", SINK, f"{level}%"], timeout=5)
        state = await self.get()
        await self.emit("changed", state)
        return state

    async def set_muted(self, muted: bool) -> dict:
        await run(["wpctl", "set-mute", SINK, "1" if muted else "0"], timeout=5)
        state = await self.get()
        await self.emit("changed", state)
        return state
