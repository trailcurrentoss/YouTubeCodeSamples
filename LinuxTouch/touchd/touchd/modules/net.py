"""Networking, via NetworkManager's nmcli.

Runs inside the graphical session, so polkit already grants these operations to
the logged-in user and no custom polkit rules are needed. That is the main
reason touchd is a user service — see bin/linuxtouch-session.
"""

from __future__ import annotations

import asyncio
import logging

from ..proc import CommandError, run
from .base import Module

log = logging.getLogger(__name__)

# -t gives colon-separated output with literal colons backslash-escaped, which
# matters because SSIDs and security strings both routinely contain them.
def _split(line: str) -> list[str]:
    fields, current, escaped = [], "", False
    for char in line:
        if escaped:
            current += char
            escaped = False
        elif char == "\\":
            escaped = True
        elif char == ":":
            fields.append(current)
            current = ""
        else:
            current += char
    fields.append(current)
    return fields


class NetModule(Module):
    name = "net"

    def __init__(self, hub) -> None:
        super().__init__(hub)
        self._task: asyncio.Task | None = None
        self._last: dict | None = None

    def rpc(self):
        return {
            "status": self.status,
            "scan": self.scan,
            "connect": self.connect,
            "forget": self.forget,
            "setRadio": self.set_radio,
        }

    async def start(self) -> None:
        self._task = asyncio.create_task(self._poll())

    async def stop(self) -> None:
        if self._task:
            self._task.cancel()

    async def _poll(self) -> None:
        """Push status changes to the status bar.

        Polling rather than subscribing to NetworkManager's D-Bus signals: this
        is one cheap command every five seconds against a dependency touchd
        already has, versus a D-Bus client for a single indicator.
        """
        while True:
            try:
                current = await self.status()
                if current != self._last:
                    self._last = current
                    await self.emit("status", current)
            except asyncio.CancelledError:
                raise
            except Exception:  # noqa: BLE001
                log.exception("network poll failed")
            await asyncio.sleep(5)

    async def status(self) -> dict:
        state = {"connected": False, "ssid": None, "signal": 0, "ip": None,
                 "type": None, "radio": True}
        try:
            out = await run(
                ["nmcli", "-t", "-f", "TYPE,STATE,CONNECTION,DEVICE",
                 "device", "status"], timeout=5)
        except CommandError as exc:
            log.warning("nmcli device status: %s", exc)
            return state

        device = None
        for line in out.splitlines():
            dev_type, dev_state, connection, name = (_split(line) + ["", "", "", ""])[:4]
            if dev_state == "connected" and dev_type in ("wifi", "ethernet"):
                # Ethernet wins if both are up: it is the one the user is
                # actually getting their packets from.
                if state["type"] != "ethernet":
                    state.update(connected=True, ssid=connection, type=dev_type)
                    device = name
                if dev_type == "ethernet":
                    break

        if device:
            state["ip"] = await self._ip_for(device)
        if state["type"] == "wifi":
            state["signal"] = await self._signal()
        state["radio"] = await self._radio_enabled()
        return state

    async def _ip_for(self, device: str) -> str | None:
        try:
            out = await run(["nmcli", "-t", "-f", "IP4.ADDRESS",
                             "device", "show", device], timeout=5)
        except CommandError:
            return None
        for line in out.splitlines():
            if ":" in line:
                return _split(line)[1].split("/")[0] or None
        return None

    async def _signal(self) -> int:
        try:
            out = await run(["nmcli", "-t", "-f", "IN-USE,SIGNAL",
                             "device", "wifi", "list"], timeout=8)
        except CommandError:
            return 0
        for line in out.splitlines():
            in_use, signal = (_split(line) + ["", ""])[:2]
            if in_use.strip() == "*":
                return int(signal or 0)
        return 0

    async def _radio_enabled(self) -> bool:
        try:
            out = await run(["nmcli", "radio", "wifi"], timeout=5)
        except CommandError:
            return True
        return out.strip() == "enabled"

    async def scan(self, rescan: bool = True) -> list[dict]:
        # --rescan yes can take 10+ seconds on a busy band; the timeout has to
        # allow for that or the picker reports failure on a working radio.
        argv = ["nmcli", "-t", "-f", "IN-USE,SSID,SIGNAL,SECURITY",
                "device", "wifi", "list"]
        if rescan:
            argv += ["--rescan", "yes"]
        try:
            out = await run(argv, timeout=25)
        except CommandError as exc:
            log.warning("wifi scan failed: %s", exc)
            return []

        known = await self._known_ssids()
        seen: dict[str, dict] = {}
        for line in out.splitlines():
            in_use, ssid, signal, security = (_split(line) + ["", "", "", ""])[:4]
            if not ssid:
                continue  # hidden network; nothing to show and nothing to tap
            entry = {
                "ssid": ssid,
                "signal": int(signal or 0),
                "secure": security.strip() not in ("", "--"),
                "security": security.strip(),
                "active": in_use.strip() == "*",
                "known": ssid in known,
            }
            # Same SSID on 2.4 and 5 GHz appears twice; keep the stronger.
            if ssid not in seen or entry["signal"] > seen[ssid]["signal"]:
                seen[ssid] = entry
        return sorted(seen.values(), key=lambda n: -n["signal"])

    async def _known_ssids(self) -> set[str]:
        try:
            out = await run(["nmcli", "-t", "-f", "NAME,TYPE",
                             "connection", "show"], timeout=5)
        except CommandError:
            return set()
        return {
            _split(line)[0]
            for line in out.splitlines()
            if len(_split(line)) > 1 and "wireless" in _split(line)[1]
        }

    async def connect(self, ssid: str, password: str | None = None) -> dict:
        argv = ["nmcli", "device", "wifi", "connect", ssid]
        if password:
            argv += ["password", password]
        try:
            # 45s: association, DHCP, and NetworkManager's own retry all fit
            # inside one call, and a shorter timeout reports failure on a join
            # that then succeeds a second later.
            await run(argv, timeout=45)
        except CommandError as exc:
            # nmcli's stderr is written for humans and is the most useful thing
            # the picker can show, so pass it through rather than flattening it
            # to "connection failed".
            return {"ok": False, "error": exc.stderr or str(exc)}
        current = await self.status()
        await self.emit("status", current)
        return {"ok": True, "status": current}

    async def forget(self, ssid: str) -> dict:
        try:
            await run(["nmcli", "connection", "delete", "id", ssid], timeout=10)
        except CommandError as exc:
            return {"ok": False, "error": exc.stderr or str(exc)}
        return {"ok": True}

    async def set_radio(self, enabled: bool) -> dict:
        await run(["nmcli", "radio", "wifi", "on" if enabled else "off"], timeout=10)
        return {"ok": True}
