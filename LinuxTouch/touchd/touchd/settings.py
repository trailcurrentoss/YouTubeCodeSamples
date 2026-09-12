"""Persisted settings — one JSON file, no database.

Written atomically because this device is a handheld that gets powered off by
holding a button, and a settings file truncated mid-write is a device that
boots to a shell with no configuration and no obvious reason why.
"""

from __future__ import annotations

import asyncio
import json
import logging
import os
from pathlib import Path
from typing import Any

log = logging.getLogger(__name__)

DEFAULTS: dict[str, Any] = {
    # .desktop ids the user has removed from the grid. Hiding, not
    # uninstalling — the app stays on the system and in search.
    "hidden_apps": [],
    # Grid order for pinned apps; anything not listed sorts alphabetically after.
    "favorites": [],
    # 0.35–1.0. Floored well above zero because this is a gamma dim with no
    # hardware backlight behind it: at 0.0 the screen is black, including the
    # slider you would need to see to undo it. See docs/hardware.md.
    "brightness": 1.0,
    "24_hour_clock": True,
}


class Settings:
    def __init__(self, path: Path) -> None:
        self.path = path
        self._data: dict[str, Any] = dict(DEFAULTS)
        self._lock = asyncio.Lock()

    def load(self) -> None:
        try:
            stored = json.loads(self.path.read_text())
        except FileNotFoundError:
            return
        except (json.JSONDecodeError, OSError) as exc:
            # Keep going with defaults rather than refusing to start. A corrupt
            # settings file must not be able to brick the shell.
            log.warning("settings unreadable (%s); using defaults", exc)
            return
        if isinstance(stored, dict):
            self._data.update(stored)

    def get(self, key: str, default: Any = None) -> Any:
        return self._data.get(key, DEFAULTS.get(key, default))

    def all(self) -> dict[str, Any]:
        return dict(self._data)

    async def set(self, key: str, value: Any) -> None:
        async with self._lock:
            self._data[key] = value
            await asyncio.to_thread(self._write)

    def _write(self) -> None:
        self.path.parent.mkdir(parents=True, exist_ok=True)
        tmp = self.path.with_suffix(".tmp")
        tmp.write_text(json.dumps(self._data, indent=2, sort_keys=True))
        # fsync before rename: rename is atomic, but on ext4 without the fsync
        # the rename can land before the data does, which after a power cut
        # leaves a valid-looking file full of zeroes.
        with open(tmp, "rb") as handle:
            os.fsync(handle.fileno())
        tmp.replace(self.path)
