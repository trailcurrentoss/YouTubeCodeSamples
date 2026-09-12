"""Application discovery and launching.

This is what makes the claim "full Linux" literal rather than aspirational:
the grid is built from the XDG `.desktop` files already on the system, so
`apt install thunderbird` puts Thunderbird on the home screen with no
LinuxTouch change at all. There is no curated whitelist anywhere in this file.
"""

from __future__ import annotations

import asyncio
import configparser
import logging
import os
import shlex
from pathlib import Path

from .base import Module

log = logging.getLogger(__name__)

# Where the XDG spec says application entries live, most specific first, so a
# user's own override of a system entry wins.
def _app_dirs() -> list[Path]:
    home = Path(os.environ.get("XDG_DATA_HOME") or Path.home() / ".local/share")
    dirs = [home / "applications"]
    raw = os.environ.get("XDG_DATA_DIRS") or "/usr/local/share:/usr/share"
    dirs += [Path(d) / "applications" for d in raw.split(":") if d]
    return dirs


# Icon themes are searched in this order, then anything else installed. PiXflat
# is Raspberry Pi OS's own theme and carries icons for the Pi-specific tools
# that no other theme has.
_THEME_ORDER = ("PiXflat", "Papirus", "Adwaita", "hicolor")

# Preferred first. 64 leads because the grid draws at 64 CSS px on a 1:1 panel;
# anything larger is downscaled and anything smaller looks soft.
_ICON_SIZES = ("64x64", "48x48", "96x96", "128x128", "72x72", "32x32", "256x256")

# Exec field codes, per the Desktop Entry Specification. %f/%u and friends are
# placeholders for files or URLs to open; launching with no argument means they
# must be REMOVED, not left in place — a literal "%U" on the command line is a
# filename to most programs, and several will error or open a blank document.
_FIELD_CODES = ("%f", "%F", "%u", "%U", "%d", "%D", "%n", "%N",
                "%i", "%c", "%k", "%v", "%m")


class AppsModule(Module):
    name = "apps"

    def __init__(self, hub) -> None:
        super().__init__(hub)
        self._cache: dict[str, dict] = {}

    def rpc(self):
        return {
            "list": self.list,
            "launch": self.launch,
            "rescan": self.rescan,
            "setHidden": self.set_hidden,
        }

    async def start(self) -> None:
        await self.rescan()

    # ── discovery ────────────────────────────────────────────────────

    async def rescan(self) -> dict:
        # Reading ~200 small files off an SD card is slow enough to be felt at
        # 640x480, so it happens in a thread rather than stalling the loop that
        # is also serving the shell's first paint.
        self._cache = await asyncio.to_thread(self._scan)
        await self.emit("changed", {"count": len(self._cache)})
        return {"count": len(self._cache)}

    def _scan(self) -> dict[str, dict]:
        found: dict[str, dict] = {}
        for directory in _app_dirs():
            if not directory.is_dir():
                continue
            for path in sorted(directory.glob("*.desktop")):
                if path.name in found:
                    continue  # a more specific directory already won
                entry = _parse(path)
                if entry:
                    found[path.name] = entry
        log.info("discovered %d applications", len(found))
        return found

    async def list(self) -> list[dict]:
        hidden = set(self.hub.settings.get("hidden_apps", []))
        apps = []
        for app_id, entry in self._cache.items():
            apps.append({**entry, "hidden": app_id in hidden})
        # Case-insensitive, because otherwise every capitalised name sorts
        # ahead of every lowercase one and the grid looks arbitrary.
        apps.sort(key=lambda a: a["name"].lower())
        return apps

    async def set_hidden(self, id: str, hidden: bool) -> dict:
        current = set(self.hub.settings.get("hidden_apps", []))
        if hidden:
            current.add(id)
        else:
            current.discard(id)
        await self.hub.settings.set("hidden_apps", sorted(current))
        await self.emit("changed", {"count": len(self._cache)})
        return {"ok": True}

    def icon_path(self, app_id: str) -> Path | None:
        entry = self._cache.get(app_id)
        return Path(entry["icon_path"]) if entry and entry.get("icon_path") else None

    # ── launching ────────────────────────────────────────────────────

    async def launch(self, id: str) -> dict:
        entry = self._cache.get(id)
        if entry is None:
            raise KeyError(f"no such application: {id}")

        argv = list(entry["argv"])
        if entry["terminal"]:
            # Console programs still deserve to be on the grid. x-terminal-
            # emulator is Debian's alternatives symlink, so this follows
            # whatever terminal the system actually has rather than pinning one.
            argv = ["x-terminal-emulator", "-e", *argv]

        # start_new_session detaches the child into its own process group. Without
        # it, the app is in touchd's group and a `systemctl restart touchd` would
        # take every running application down with it.
        proc = await asyncio.create_subprocess_exec(
            *argv,
            stdout=asyncio.subprocess.DEVNULL,
            stderr=asyncio.subprocess.DEVNULL,
            start_new_session=True,
            cwd=str(Path.home()),
        )
        log.info("launched %s as pid %d", id, proc.pid)
        # The PID is the useful half of the answer: the shell hands it back to
        # wm.windows to find the window this launch produced, which is exactly
        # the mapping the sway adapter exists to provide.
        return {"id": id, "pid": proc.pid}


def _parse(path: Path) -> dict | None:
    """Read one .desktop file, or return None if it should not be shown."""
    parser = configparser.ConfigParser(interpolation=None, strict=False)
    try:
        # Desktop files are UTF-8 by spec, but a stray latin-1 comment in a
        # third-party entry must not take out the whole scan.
        parser.read(path, encoding="utf-8")
    except (configparser.Error, UnicodeDecodeError) as exc:
        log.debug("skipping %s: %s", path, exc)
        return None

    if not parser.has_section("Desktop Entry"):
        return None
    section = parser["Desktop Entry"]

    if section.get("Type", "Application") != "Application":
        return None
    if section.getboolean("NoDisplay", fallback=False):
        return None
    if section.getboolean("Hidden", fallback=False):
        return None

    exec_line = section.get("Exec", "").strip()
    if not exec_line:
        return None

    # TryExec names the binary that must exist for the entry to be valid. Debian
    # leaves entries behind for removed packages often enough that skipping this
    # check puts dead icons on the grid.
    try_exec = section.get("TryExec")
    if try_exec and not _resolvable(try_exec):
        return None

    argv = _exec_argv(exec_line)
    if not argv or not _resolvable(argv[0]):
        return None

    icon = section.get("Icon", "").strip()
    return {
        "id": path.name,
        "name": section.get("Name", path.stem),
        "comment": section.get("Comment", ""),
        "categories": [c for c in section.get("Categories", "").split(";") if c],
        "terminal": section.getboolean("Terminal", fallback=False),
        "argv": argv,
        "icon": icon,
        "icon_path": _find_icon(icon) if icon else None,
    }


def _exec_argv(exec_line: str) -> list[str]:
    try:
        parts = shlex.split(exec_line)
    except ValueError:
        return []
    argv = [p for p in parts if p not in _FIELD_CODES]
    # Field codes also appear glued to other text ("--file=%f"). Dropping the
    # whole token there would lose the flag, so strip just the code.
    cleaned = []
    for part in argv:
        for code in _FIELD_CODES:
            part = part.replace(code, "")
        if part:
            cleaned.append(part)
    return cleaned


def _resolvable(binary: str) -> bool:
    if "/" in binary:
        return Path(binary).exists()
    from shutil import which

    return which(binary) is not None


def _find_icon(name: str) -> str | None:
    """Resolve an Icon= value to a file on disk.

    Deliberately a plain filesystem search rather than a call into GTK's icon
    theme machinery: touchd has no GTK dependency, must run headless for
    testing, and this covers every layout Debian themes actually use.
    """
    if name.startswith("/"):
        return name if Path(name).exists() else None

    roots = [
        Path.home() / ".local/share/icons",
        Path("/usr/local/share/icons"),
        Path("/usr/share/icons"),
    ]
    themes: list[Path] = []
    for root in roots:
        if not root.is_dir():
            continue
        installed = {p.name: p for p in root.iterdir() if p.is_dir()}
        for preferred in _THEME_ORDER:
            if preferred in installed:
                themes.append(installed.pop(preferred))
        themes.extend(installed.values())

    for theme in themes:
        for size in _ICON_SIZES:
            for pattern in (f"{size}/apps/{name}.*", f"apps/{size}/{name}.*"):
                for hit in theme.glob(pattern):
                    return str(hit)
        for hit in theme.glob(f"scalable/apps/{name}.svg"):
            return str(hit)

    # pixmaps is the pre-theme fallback, and a surprising number of long-lived
    # Debian packages still ship only there.
    for ext in ("png", "svg", "xpm"):
        candidate = Path(f"/usr/share/pixmaps/{name}.{ext}")
        if candidate.exists():
            return str(candidate)
    return None
