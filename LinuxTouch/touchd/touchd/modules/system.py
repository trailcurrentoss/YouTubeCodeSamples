"""Device information for the status bar and the About screen.

Note what is NOT here: battery. The PocketTerm35's carrier gives the Pi no way
to read charge state — `/sys/class/power_supply/` is empty and nothing answers
on i2c-1; Waveshare exposes it only as LEDs on the UPS board. A gauge that
permanently reads `--` looks like a flat battery, which is worse than no gauge
at all, so the status bar deliberately has none. If a future carrier exposes a
real supply, read it from /sys/class/power_supply rather than guessing.
"""

from __future__ import annotations

import asyncio
import logging
import shutil
import socket
import time
from pathlib import Path

from .base import Module

log = logging.getLogger(__name__)


class SystemModule(Module):
    name = "system"

    def __init__(self, hub) -> None:
        super().__init__(hub)
        self._task: asyncio.Task | None = None

    def rpc(self):
        return {"info": self.info}

    async def start(self) -> None:
        self._task = asyncio.create_task(self._poll())

    async def stop(self) -> None:
        if self._task:
            self._task.cancel()

    async def _poll(self) -> None:
        # Only the clock needs to be live in the status bar, and the shell keeps
        # its own. This exists so temperature and memory on the About screen
        # update while it is open, at a rate that costs nothing.
        while True:
            await asyncio.sleep(10)
            try:
                await self.emit("info", await self.info())
            except asyncio.CancelledError:
                raise
            except Exception:  # noqa: BLE001
                log.exception("system poll failed")

    async def info(self) -> dict:
        return await asyncio.to_thread(self._info)

    def _info(self) -> dict:
        return {
            "hostname": socket.gethostname(),
            "model": _read("/proc/device-tree/model", "unknown").rstrip("\x00"),
            "os": _os_name(),
            "kernel": _read("/proc/sys/kernel/osrelease", "unknown").strip(),
            "uptime": _uptime(),
            "temperature": _temperature(),
            "memory": _memory(),
            "disk": _disk(),
            # Explicit, so the About screen can say so rather than leaving a
            # blank where users expect a battery reading.
            "battery": None,
        }


def _read(path: str, default: str = "") -> str:
    try:
        return Path(path).read_text(errors="replace")
    except OSError:
        return default


def _os_name() -> str:
    for line in _read("/etc/os-release").splitlines():
        if line.startswith("PRETTY_NAME="):
            return line.split("=", 1)[1].strip().strip('"')
    return "unknown"


def _uptime() -> int:
    try:
        return int(float(_read("/proc/uptime").split()[0]))
    except (ValueError, IndexError):
        return int(time.monotonic())


def _temperature() -> float | None:
    raw = _read("/sys/class/thermal/thermal_zone0/temp").strip()
    try:
        return round(int(raw) / 1000.0, 1)
    except ValueError:
        return None


def _memory() -> dict:
    values = {}
    for line in _read("/proc/meminfo").splitlines():
        key, _, rest = line.partition(":")
        try:
            values[key] = int(rest.split()[0]) * 1024
        except (IndexError, ValueError):
            continue
    total = values.get("MemTotal", 0)
    available = values.get("MemAvailable", 0)
    return {"total": total, "available": available, "used": total - available}


def _disk() -> dict:
    try:
        usage = shutil.disk_usage("/")
    except OSError:
        return {"total": 0, "free": 0, "used": 0}
    return {"total": usage.total, "free": usage.free, "used": usage.used}
