"""Persisted shell preferences (~/.config/trailshell/settings.json).

Deliberately tiny: the shell has few knobs, and the ones it has should survive
a restart without needing a settings daemon.
"""
from __future__ import annotations

import json
import logging
from pathlib import Path

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import GLib

log = logging.getLogger("trailshell.settings")

DIR = Path(GLib.get_user_config_dir()) / "trailshell"
FILE = DIR / "settings.json"

DEFAULTS = {
    "theme": "light",      # light | dark - light is the default
    "ui_scale": 1.0,       # applied at startup; changing it restarts the shell
    "dnd": False,
    "dim": 1.0,
    #: Xft DPI for ordinary applications (not the shell, which sizes in
    #: absolute pixels). Higher is more readable but inflates every app's
    #: minimum window size against a 640x410 canvas. Read by the session
    #: script at login; changing it needs a re-login.
    "app_dpi": 144,
}

_data: dict | None = None


def _load() -> dict:
    global _data
    if _data is None:
        d = dict(DEFAULTS)
        try:
            d.update(json.loads(FILE.read_text()))
        except FileNotFoundError:
            pass
        except Exception as exc:
            log.warning("settings load failed, using defaults: %s", exc)
        _data = d
    return _data


def get(key: str, default=None):
    return _load().get(key, DEFAULTS.get(key, default))


def set(key: str, value) -> None:
    d = _load()
    if d.get(key) == value:
        return
    d[key] = value
    try:
        DIR.mkdir(parents=True, exist_ok=True)
        tmp = FILE.with_suffix(".tmp")
        tmp.write_text(json.dumps(d, indent=2))
        tmp.replace(FILE)
    except Exception as exc:
        log.warning("settings save failed: %s", exc)
