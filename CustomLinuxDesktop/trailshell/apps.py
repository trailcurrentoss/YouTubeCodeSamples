"""Application catalogue: discovery, icons, search ranking and launching.

Built on ``Gio.DesktopAppInfo`` rather than a hand-rolled .desktop parser so we
inherit correct Exec field-code expansion, ``Terminal=true`` handling, D-Bus
activation and the XDG ``NoDisplay``/``OnlyShowIn`` visibility rules for free.

Search matters more here than on a normal desktop: the PocketTerm has a real
QWERTY keyboard and no on-screen keyboard, so type-to-search is the *fastest*
way to open anything, and the touch grid is the browsing fallback.
"""
from __future__ import annotations

import json
import logging
import math
import os
import time
from dataclasses import dataclass, field
from pathlib import Path

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import Gio, GdkPixbuf, GLib, Gtk

log = logging.getLogger("trailshell.apps")

STATE_DIR = Path(GLib.get_user_config_dir()) / "trailshell"
USAGE_FILE = STATE_DIR / "usage.json"

#: Apps that are meaningless or actively confusing on a 3.5" touch panel.
#: Hidden from the grid; still reachable by typing their exact name.
DEMOTED = {
    "gcr-prompter", "gcr-viewer", "mimeinfo.cache", "openbox",
    "org.freedesktop.xwayland", "qt5ct", "qt6ct",
    "realvnc-vnclicensewiz", "python3.13",
}


@dataclass
class App:
    app_id: str                     # desktop file id, e.g. "firefox.desktop"
    name: str
    info: Gio.DesktopAppInfo
    icon_name: str = ""
    comment: str = ""
    categories: list[str] = field(default_factory=list)
    keywords: list[str] = field(default_factory=list)
    exec_base: str = ""
    wm_class: str = ""
    launches: int = 0
    last_used: float = 0.0

    @property
    def search_blob(self) -> str:
        return " ".join(
            [self.name, self.comment, self.exec_base, " ".join(self.keywords)]
        ).lower()


class AppCatalogue:
    def __init__(self) -> None:
        self.apps: list[App] = []
        self._by_id: dict[str, App] = {}
        self._usage: dict[str, dict] = {}
        self._icon_cache: dict[tuple[str, int], GdkPixbuf.Pixbuf | None] = {}
        self._load_usage()
        self.refresh()
        # Pick up apps installed while the shell is running.
        Gio.AppInfoMonitor.get().connect("changed", lambda *_: self.refresh())

    # -- discovery --------------------------------------------------------
    def refresh(self) -> None:
        apps: list[App] = []
        for info in Gio.AppInfo.get_all():
            if not isinstance(info, Gio.DesktopAppInfo):
                continue
            if not info.should_show():
                continue
            app_id = info.get_id() or ""
            stem = app_id.removesuffix(".desktop").lower()
            if stem in DEMOTED:
                continue
            name = info.get_display_name() or info.get_name() or stem
            icon = info.get_icon()
            icon_name = ""
            if isinstance(icon, Gio.ThemedIcon):
                names = icon.get_names()
                icon_name = names[0] if names else ""
            elif isinstance(icon, Gio.FileIcon):
                icon_name = icon.get_file().get_path() or ""

            exec_line = info.get_commandline() or ""
            exec_base = os.path.basename(exec_line.split(" ")[0]) if exec_line else ""

            u = self._usage.get(app_id, {})
            apps.append(App(
                app_id=app_id,
                name=name,
                info=info,
                icon_name=icon_name,
                comment=info.get_description() or "",
                categories=[c for c in (info.get_categories() or "").split(";") if c],
                keywords=list(info.get_keywords() or []),
                exec_base=exec_base,
                wm_class=(info.get_startup_wm_class() or "").lower(),
                launches=u.get("launches", 0),
                last_used=u.get("last_used", 0.0),
            ))

        apps.sort(key=lambda a: a.name.casefold())
        self.apps = apps
        self._by_id = {a.app_id: a for a in apps}
        log.info("catalogue: %d apps", len(apps))

    # -- ordering ---------------------------------------------------------
    def _frecency(self, app: App) -> float:
        """Usage score with a 14-day half-life.

        Recency dominates raw count, so the two or three things you actually
        use on this device float to the front of the grid without permanently
        pinning something you used a lot once.
        """
        if not app.launches:
            return 0.0
        age_days = max(0.0, (time.time() - app.last_used) / 86400.0)
        return app.launches * math.pow(0.5, age_days / 14.0)

    def grid_order(self) -> list[App]:
        """Frecent apps first, then everything else alphabetically."""
        scored = [(self._frecency(a), a) for a in self.apps]
        hot = sorted([p for p in scored if p[0] > 0], key=lambda p: -p[0])
        cold = [p for p in scored if p[0] == 0]
        return [a for _, a in hot] + [a for _, a in cold]

    # -- search -----------------------------------------------------------
    def search(self, query: str, limit: int = 24) -> list[App]:
        q = query.strip().lower()
        if not q:
            return self.grid_order()[:limit]

        results: list[tuple[float, App]] = []
        for app in self.apps:
            name = app.name.lower()
            score = 0.0
            if name == q:
                score = 1000.0
            elif name.startswith(q):
                score = 800.0 - len(name)
            elif any(w.startswith(q) for w in name.split()):
                score = 600.0 - len(name)
            elif q in name:
                score = 400.0 - len(name)
            elif app.exec_base.lower().startswith(q):
                score = 350.0
            elif any(k.lower().startswith(q) for k in app.keywords):
                score = 250.0
            elif q in app.search_blob:
                score = 100.0
            if score:
                # Nudge by frecency so repeat launches beat alphabetical ties.
                results.append((score + min(self._frecency(app) * 5, 50), app))

        results.sort(key=lambda p: -p[0])
        return [a for _, a in results[:limit]]

    # -- icons ------------------------------------------------------------
    def icon(self, app: App, size: int) -> GdkPixbuf.Pixbuf | None:
        key = (app.icon_name or app.app_id, size)
        if key in self._icon_cache:
            return self._icon_cache[key]
        pb = self._load_icon(app, size)
        self._icon_cache[key] = pb
        return pb

    def _load_icon(self, app: App, size: int) -> GdkPixbuf.Pixbuf | None:
        name = app.icon_name
        try:
            if name and os.path.isabs(name) and os.path.exists(name):
                return GdkPixbuf.Pixbuf.new_from_file_at_size(name, size, size)
            theme = Gtk.IconTheme.get_default()
            for candidate in filter(None, [name, app.exec_base,
                                           app.app_id.removesuffix(".desktop")]):
                info = theme.lookup_icon(
                    candidate, size,
                    Gtk.IconLookupFlags.FORCE_SIZE | Gtk.IconLookupFlags.USE_BUILTIN,
                )
                if info:
                    return info.load_icon()
            info = theme.lookup_icon("application-x-executable", size,
                                     Gtk.IconLookupFlags.FORCE_SIZE)
            if info:
                return info.load_icon()
        except Exception as exc:
            log.debug("icon load failed for %s: %s", app.app_id, exc)
        return None

    # -- launching --------------------------------------------------------
    def launch(self, app: App) -> bool:
        try:
            app.info.launch([], Gio.AppLaunchContext())
            self._record(app)
            return True
        except GLib.Error as exc:
            log.error("launch %s failed: %s", app.app_id, exc.message)
            return False

    def _record(self, app: App) -> None:
        app.launches += 1
        app.last_used = time.time()
        self._usage[app.app_id] = {"launches": app.launches, "last_used": app.last_used}
        self._save_usage()

    # -- persistence ------------------------------------------------------
    def _load_usage(self) -> None:
        try:
            self._usage = json.loads(USAGE_FILE.read_text())
        except (FileNotFoundError, json.JSONDecodeError):
            self._usage = {}
        except Exception as exc:
            log.warning("usage load failed: %s", exc)
            self._usage = {}

    def _save_usage(self) -> None:
        try:
            STATE_DIR.mkdir(parents=True, exist_ok=True)
            tmp = USAGE_FILE.with_suffix(".tmp")
            tmp.write_text(json.dumps(self._usage))
            tmp.replace(USAGE_FILE)
        except Exception as exc:
            log.warning("usage save failed: %s", exc)


_shared: AppCatalogue | None = None


def get() -> AppCatalogue:
    global _shared
    if _shared is None:
        _shared = AppCatalogue()
    return _shared
