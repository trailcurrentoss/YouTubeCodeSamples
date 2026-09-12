"""Shutdown and reboot.

Both refuse to act without `confirm: true`. That is enforced here, in the
daemon, rather than by the shell showing a dialog — a UI-only guard is one
mis-wired button away from powering the device off mid-write, and this is a
handheld that lives in a bag.

The physical power button never hard-kills the unit either: the image sets
`HandlePowerKey=ignore` with `HandlePowerKeyLongPress=poweroff`, so a short
press surfaces as an event the shell turns into a confirmation, and a long
press remains the escape hatch at the logind level. See docs/controls.md.
"""

from __future__ import annotations

from ..proc import run
from .base import Module


class PowerModule(Module):
    name = "power"

    def rpc(self):
        return {"shutdown": self.shutdown, "reboot": self.reboot}

    async def shutdown(self, confirm: bool = False) -> dict:
        if not confirm:
            return {"ok": False, "error": "confirmation required"}
        await run(["systemctl", "poweroff"], timeout=10)
        return {"ok": True}

    async def reboot(self, confirm: bool = False) -> dict:
        if not confirm:
            return {"ok": False, "error": "confirmation required"}
        await run(["systemctl", "reboot"], timeout=10)
        return {"ok": True}
