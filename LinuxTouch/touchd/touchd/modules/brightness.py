"""Screen brightness — a compositor gamma adjustment, not a backlight.

THIS PANEL HAS NO BACKLIGHT CONTROL. `/sys/class/backlight/` is empty on the
PocketTerm35 and no kernel driver provides one, verified on hardware. So the
only lever available is the compositor's gamma ramp, applied through
`gammastep`, which changes what is *drawn* rather than how brightly the panel
lights it.

Two consequences the UI must be honest about:

  * It reduces emitted light and glare — the real need when using this thing at
    night — but it does NOT reduce power draw. The Settings screen labels the
    control accordingly and must keep doing so.
  * There is no hardware floor to stop at. At 0.0 the screen is black,
    including the slider needed to undo it, on a device whose only other
    recovery is a power cycle. Hence MINIMUM below.

Tracer solved the same problem with a CSS overlay inside its own kiosk. That
cannot work here: a CSS dim in the shell would not touch a fullscreen mail
client. Gamma applies to the whole output, including third-party apps.
"""

from __future__ import annotations

import asyncio
import logging

from ..proc import have
from .base import Module

log = logging.getLogger(__name__)

MINIMUM = 0.35
# Neutral daylight. gammastep is a colour-temperature tool; -O fixes the
# temperature so only the -b brightness factor is in play, and 6500K is the
# no-op temperature that leaves colours unshifted.
TEMPERATURE = "6500"


class BrightnessModule(Module):
    name = "brightness"

    def __init__(self, hub) -> None:
        super().__init__(hub)
        self._proc: asyncio.subprocess.Process | None = None

    def rpc(self):
        return {"get": self.get, "set": self.set}

    async def start(self) -> None:
        level = float(self.hub.settings.get("brightness", 1.0))
        if level < 1.0:
            await self._apply(level)

    async def stop(self) -> None:
        await self._kill()

    async def get(self) -> dict:
        return {
            "available": have("gammastep"),
            "level": float(self.hub.settings.get("brightness", 1.0)),
            "minimum": MINIMUM,
            # The shell shows this next to the slider. Wording lives here so
            # there is one place to change it, and so it cannot drift into
            # implying hardware control.
            "note": "Dims what is drawn. This panel has no backlight, so it "
                    "reduces glare but not battery use.",
        }

    async def set(self, level: float) -> dict:
        level = max(MINIMUM, min(1.0, float(level)))
        await self.hub.settings.set("brightness", level)
        await self._apply(level)
        state = await self.get()
        await self.emit("changed", state)
        return state

    async def _apply(self, level: float) -> None:
        await self._kill()
        if level >= 1.0:
            return  # full brightness is the absence of a gamma process
        if not have("gammastep"):
            log.warning("gammastep not installed; brightness control unavailable")
            return
        # -P resets the ramp before applying, so repeated changes compose from
        # the identity ramp instead of stacking multiplicatively and marching
        # the screen to black over a few taps.
        self._proc = await asyncio.create_subprocess_exec(
            "gammastep", "-P", "-O", TEMPERATURE, "-b", f"{level:.2f}",
            stdout=asyncio.subprocess.DEVNULL,
            stderr=asyncio.subprocess.DEVNULL,
        )

    async def _kill(self) -> None:
        if self._proc and self._proc.returncode is None:
            self._proc.terminate()
            try:
                await asyncio.wait_for(self._proc.wait(), timeout=3)
            except asyncio.TimeoutError:
                self._proc.kill()
        self._proc = None
