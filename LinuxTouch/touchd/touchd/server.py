"""The daemon: HTTP for the shell bundle, WebSocket for everything else.

Binds 127.0.0.1 only. This is a device-local API with no authentication, and it
can start applications and power the machine off — it must never be reachable
from the network. If that ever needs to change, it needs a real auth story
first, not a bind-address change.
"""

from __future__ import annotations

import json
import logging
import mimetypes
from pathlib import Path
from typing import Any

from aiohttp import WSMsgType, web

from .modules.apps import AppsModule
from .modules.audio import AudioModule
from .modules.brightness import BrightnessModule
from .modules.net import NetModule
from .modules.power import PowerModule
from .modules.settingsmod import SettingsModule
from .modules.system import SystemModule
from .modules.windows import WindowsModule
from .settings import Settings
from .wm.sway import SwayWM

log = logging.getLogger(__name__)

MODULES = (
    AppsModule, WindowsModule, NetModule, AudioModule,
    BrightnessModule, PowerModule, SystemModule, SettingsModule,
)


class Hub:
    """Owns the modules, the RPC table, and the set of connected clients."""

    def __init__(self, shell_dir: Path, state_dir: Path) -> None:
        self.shell_dir = shell_dir
        self.settings = Settings(state_dir / "settings.json")
        self.settings.load()
        self.wm = SwayWM()
        self.modules: list = [cls(self) for cls in MODULES]
        self.methods: dict[str, Any] = {}
        self.clients: set[web.WebSocketResponse] = set()

        for module in self.modules:
            for name, handler in module.rpc().items():
                self.methods[f"{module.name}.{name}"] = handler

    async def start(self) -> None:
        for module in self.modules:
            await module.start()
        log.info("touchd ready — %d methods", len(self.methods))

    async def stop(self) -> None:
        for module in self.modules:
            try:
                await module.stop()
            except Exception:  # noqa: BLE001 — shutdown must not raise
                log.exception("error stopping %s", module.name)

    async def broadcast(self, event: str, data: Any) -> None:
        if not self.clients:
            return
        payload = json.dumps({"event": event, "data": data})
        # Iterate a copy: a send to a client that has gone away raises, and
        # removing from the live set mid-iteration would skip the next client.
        for ws in list(self.clients):
            try:
                await ws.send_str(payload)
            except Exception:  # noqa: BLE001
                self.clients.discard(ws)

    async def dispatch(self, method: str, params: dict) -> Any:
        handler = self.methods.get(method)
        if handler is None:
            raise KeyError(f"unknown method: {method}")
        return await handler(**params)


# ── HTTP ──────────────────────────────────────────────────────────────

async def handle_index(request: web.Request) -> web.StreamResponse:
    return _serve_file(request.app["hub"].shell_dir / "index.html")


async def handle_static(request: web.Request) -> web.StreamResponse:
    hub: Hub = request.app["hub"]
    rel = request.match_info["path"]
    target = (hub.shell_dir / rel).resolve()
    # Containment check. The shell bundle is trusted, but this handler takes a
    # path straight off the wire, and "../../etc/shadow" must not resolve to a
    # readable file just because the daemon happens to run as the user who owns
    # the session.
    if not str(target).startswith(str(hub.shell_dir.resolve())):
        raise web.HTTPForbidden()
    return _serve_file(target)


async def handle_icon(request: web.Request) -> web.StreamResponse:
    """Serve an application's icon from wherever the theme keeps it.

    The shell cannot read /usr/share/icons itself — it is a web page — so the
    daemon hands over the one file that .desktop entry pointed at.
    """
    hub: Hub = request.app["hub"]
    apps = next(m for m in hub.modules if m.name == "apps")
    path = apps.icon_path(request.match_info["app_id"])
    if path is None or not path.exists():
        raise web.HTTPNotFound()
    return _serve_file(path)


def _serve_file(path: Path) -> web.StreamResponse:
    if not path.is_file():
        raise web.HTTPNotFound()
    ctype, _ = mimetypes.guess_type(path.name)
    return web.FileResponse(
        path,
        headers={
            "Content-Type": ctype or "application/octet-stream",
            # The bundle is replaced by `make deploy` while the shell may be
            # sitting on the old copy. Caching it would mean a deploy that
            # visibly changes nothing until the profile is wiped.
            "Cache-Control": "no-store",
        },
    )


async def handle_health(request: web.Request) -> web.Response:
    """Readiness probe. bin/linuxtouch-shell polls this before starting
    Chromium, so the user never sees a 'site cannot be reached' page at boot."""
    return web.json_response({"ok": True})


async def handle_nav(request: web.Request) -> web.Response:
    """Global navigation, POSTed by bin/linuxtouch-nav from a sway keybind.

    The daemon decides what each action means, which is why "Start again from
    home opens the switcher" needs no state in the keybind script: the shell
    receives the raw action and applies its own context.
    """
    hub: Hub = request.app["hub"]
    action = request.match_info["action"]
    if action not in ("home", "back", "switcher"):
        raise web.HTTPBadRequest()

    # Whether the shell was ALREADY focused has to be sampled before raising
    # it, and only the compositor knows. The shell cannot answer this itself:
    # while an app covers the screen the shell keeps rendering whatever screen
    # it was last on, so "am I on the home screen?" and "is the user looking at
    # me?" are different questions. Without this, pressing Start from inside an
    # app opened the switcher instead of going home, because the shell's last
    # screen happened to be home.
    was_focused = await hub.wm.shell_focused()

    # Raise the shell first, then tell it what to show. The other order leaves
    # the shell rendering a switcher nobody can see yet.
    await hub.wm.focus_shell()

    await hub.broadcast("nav", {"action": action, "shell_focused": was_focused})
    return web.json_response({"ok": True})


async def handle_ws(request: web.Request) -> web.WebSocketResponse:
    hub: Hub = request.app["hub"]
    ws = web.WebSocketResponse(heartbeat=30)
    await ws.prepare(request)
    hub.clients.add(ws)
    log.info("shell connected (%d client(s))", len(hub.clients))

    try:
        async for message in ws:
            if message.type is not WSMsgType.TEXT:
                continue
            await _handle_message(hub, ws, message.data)
    finally:
        hub.clients.discard(ws)
        log.info("shell disconnected (%d client(s))", len(hub.clients))
    return ws


async def _handle_message(hub: Hub, ws: web.WebSocketResponse, raw: str) -> None:
    try:
        request = json.loads(raw)
        call_id = request.get("id")
        method = request["method"]
        params = request.get("params") or {}
    except (json.JSONDecodeError, KeyError, AttributeError):
        await ws.send_str(json.dumps({"ok": False, "error": "malformed request"}))
        return

    try:
        result = await hub.dispatch(method, params)
        await ws.send_str(json.dumps({"id": call_id, "ok": True, "result": result}))
    except Exception as exc:  # noqa: BLE001
        # Every failure is reported to the caller rather than logged and
        # dropped. A shell that gets no reply hangs its spinner forever, which
        # is indistinguishable from a crashed daemon from the user's side.
        log.warning("%s failed: %s", method, exc)
        await ws.send_str(json.dumps({"id": call_id, "ok": False, "error": str(exc)}))


def build_app(shell_dir: Path, state_dir: Path) -> web.Application:
    hub = Hub(shell_dir, state_dir)
    app = web.Application()
    app["hub"] = hub
    app.add_routes([
        web.get("/", handle_index),
        web.get("/healthz", handle_health),
        web.get("/ws", handle_ws),
        web.get("/icon/{app_id}", handle_icon),
        web.post("/nav/{action}", handle_nav),
        web.get("/{path:.*}", handle_static),
    ])

    async def _on_start(_: web.Application) -> None:
        await hub.start()

    async def _on_stop(_: web.Application) -> None:
        await hub.stop()

    app.on_startup.append(_on_start)
    app.on_cleanup.append(_on_stop)
    return app
