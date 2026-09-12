"""A freedesktop notification server, plus the shell's banner and history.

TrailShell owns ``org.freedesktop.Notifications`` on the session bus, so any
normal Linux app that calls ``notify-send`` or libnotify lands in the shade.
The RPi session's notification-daemon is not started, so there is no contest
for the name.

Presentation follows the phone pattern rather than the desktop one:
  * a banner drops in under the status bar, sized for a glance, and leaves;
  * the notification persists in the shade until dismissed;
  * Do Not Disturb suppresses the banner but still records the notification.
"""
from __future__ import annotations

import logging
import time
from dataclasses import dataclass, field

from . import _gi  # noqa: F401  (pins GI versions)
from gi.repository import Gio, GLib

log = logging.getLogger("trailshell.notifications")

BUS_NAME = "org.freedesktop.Notifications"
OBJ_PATH = "/org/freedesktop/Notifications"

INTROSPECTION = """
<node>
  <interface name="org.freedesktop.Notifications">
    <method name="Notify">
      <arg type="s" name="app_name"     direction="in"/>
      <arg type="u" name="replaces_id"  direction="in"/>
      <arg type="s" name="app_icon"     direction="in"/>
      <arg type="s" name="summary"      direction="in"/>
      <arg type="s" name="body"         direction="in"/>
      <arg type="as" name="actions"     direction="in"/>
      <arg type="a{sv}" name="hints"    direction="in"/>
      <arg type="i" name="expire_timeout" direction="in"/>
      <arg type="u" name="id"           direction="out"/>
    </method>
    <method name="CloseNotification">
      <arg type="u" name="id" direction="in"/>
    </method>
    <method name="GetCapabilities">
      <arg type="as" name="capabilities" direction="out"/>
    </method>
    <method name="GetServerInformation">
      <arg type="s" name="name"        direction="out"/>
      <arg type="s" name="vendor"      direction="out"/>
      <arg type="s" name="version"     direction="out"/>
      <arg type="s" name="spec_version" direction="out"/>
    </method>
    <signal name="NotificationClosed">
      <arg type="u" name="id"/>
      <arg type="u" name="reason"/>
    </signal>
    <signal name="ActionInvoked">
      <arg type="u" name="id"/>
      <arg type="s" name="action_key"/>
    </signal>
  </interface>
</node>
"""

# CloseNotification reasons from the spec.
REASON_EXPIRED, REASON_DISMISSED, REASON_CLOSED, REASON_UNDEFINED = 1, 2, 3, 4

URGENCY_LOW, URGENCY_NORMAL, URGENCY_CRITICAL = 0, 1, 2

#: How long a banner stays on screen when the app does not specify.
DEFAULT_TIMEOUT_MS = 4500
#: Critical notifications never auto-dismiss; the user has to act.
MAX_HISTORY = 50


@dataclass
class Notification:
    id: int
    app_name: str = ""
    app_icon: str = ""
    summary: str = ""
    body: str = ""
    actions: list[str] = field(default_factory=list)
    urgency: int = URGENCY_NORMAL
    timeout_ms: int = -1
    created: float = field(default_factory=time.time)

    @property
    def age_str(self) -> str:
        secs = max(0, int(time.time() - self.created))
        if secs < 60:
            return "now"
        if secs < 3600:
            return f"{secs // 60}m"
        if secs < 86400:
            return f"{secs // 3600}h"
        return f"{secs // 86400}d"

    @property
    def is_critical(self) -> bool:
        return self.urgency == URGENCY_CRITICAL


class Signal:
    def __init__(self) -> None:
        self._subs = []

    def connect(self, fn):
        self._subs.append(fn)

    def emit(self, *a):
        for fn in list(self._subs):
            try:
                fn(*a)
            except Exception:
                log.exception("notification signal handler failed")


class NotificationStore:
    """Newest-first history of live notifications."""

    def __init__(self) -> None:
        self.items: list[Notification] = []
        self.changed = Signal()
        self.posted = Signal()      # emitted with the Notification to banner

    def add(self, n: Notification) -> None:
        self.items = [i for i in self.items if i.id != n.id]
        self.items.insert(0, n)
        del self.items[MAX_HISTORY:]
        self.changed.emit(self)
        self.posted.emit(n)

    def remove(self, nid: int) -> bool:
        before = len(self.items)
        self.items = [i for i in self.items if i.id != nid]
        if len(self.items) != before:
            self.changed.emit(self)
            return True
        return False

    def clear(self) -> list[int]:
        ids = [i.id for i in self.items]
        self.items = []
        self.changed.emit(self)
        return ids

    def demo(self) -> None:
        """Seed plausible notifications for offline visual checks."""
        samples = [
            ("Network Manager", "Connected to ISoWIhOGp7", "Signal 82%, IPv4 assigned", URGENCY_NORMAL),
            ("System Update", "14 packages can be upgraded", "Includes a kernel security fix", URGENCY_NORMAL),
            ("Storage", "Root filesystem 91% full", "Free space to avoid write failures", URGENCY_CRITICAL),
        ]
        for i, (app, summ, body, urg) in enumerate(samples, start=1):
            self.items.append(Notification(id=9000 + i, app_name=app, summary=summ,
                                           body=body, urgency=urg,
                                           created=time.time() - i * 420))
        self.changed.emit(self)


class NotificationServer:
    """Owns org.freedesktop.Notifications and feeds a NotificationStore."""

    def __init__(self, store: NotificationStore) -> None:
        # Do Not Disturb is applied by the shell when it decides whether to
        # raise a banner, not here: a suppressed notification must still be
        # recorded and still get a valid id back on the bus.
        self.store = store
        self._next_id = 1
        self._conn: Gio.DBusConnection | None = None
        self._reg_id = 0
        self._owner_id = 0
        self._node = Gio.DBusNodeInfo.new_for_xml(INTROSPECTION)

    def start(self) -> None:
        self._owner_id = Gio.bus_own_name(
            Gio.BusType.SESSION, BUS_NAME,
            # REPLACE lets a shell restart take the name back from its own
            # previous instance instead of silently running deaf.
            Gio.BusNameOwnerFlags.REPLACE | Gio.BusNameOwnerFlags.ALLOW_REPLACEMENT,
            self._on_bus_acquired, self._on_name_acquired, self._on_name_lost,
        )

    def stop(self) -> None:
        if self._reg_id and self._conn:
            self._conn.unregister_object(self._reg_id)
            self._reg_id = 0
        if self._owner_id:
            Gio.bus_unown_name(self._owner_id)
            self._owner_id = 0

    # -- bus plumbing -----------------------------------------------------
    def _on_bus_acquired(self, conn, _name):
        self._conn = conn
        try:
            self._reg_id = conn.register_object(
                OBJ_PATH, self._node.interfaces[0], self._on_call, None, None
            )
        except GLib.Error as exc:
            log.error("cannot export notification object: %s", exc.message)

    def _on_name_acquired(self, _conn, name):
        log.info("owning %s", name)

    def _on_name_lost(self, _conn, name):
        log.warning("lost %s - another notification daemon took over", name)

    def _on_call(self, _conn, _sender, _path, _iface, method, params, invocation):
        try:
            if method == "Notify":
                invocation.return_value(GLib.Variant("(u)", (self._notify(params),)))
            elif method == "CloseNotification":
                nid = params.unpack()[0]
                if self.store.remove(nid):
                    self._emit_closed(nid, REASON_CLOSED)
                invocation.return_value(None)
            elif method == "GetCapabilities":
                invocation.return_value(GLib.Variant("(as)", (
                    ["body", "body-markup", "actions", "persistence", "icon-static"],
                )))
            elif method == "GetServerInformation":
                invocation.return_value(GLib.Variant(
                    "(ssss)", ("TrailShell", "TrailCurrent", "0.1.0", "1.2")))
            else:
                invocation.return_error_literal(
                    Gio.dbus_error_quark(), Gio.DBusError.UNKNOWN_METHOD, method)
        except Exception:
            log.exception("D-Bus call %s failed", method)
            invocation.return_error_literal(
                Gio.dbus_error_quark(), Gio.DBusError.FAILED, "internal error")

    def _notify(self, params) -> int:
        app_name, replaces_id, app_icon, summary, body, actions, hints, expire = params.unpack()

        urgency = URGENCY_NORMAL
        raw = hints.get("urgency")
        if isinstance(raw, int):
            urgency = max(URGENCY_LOW, min(URGENCY_CRITICAL, raw))

        nid = replaces_id or self._alloc_id()
        n = Notification(
            id=nid, app_name=app_name or "System", app_icon=app_icon or "",
            summary=summary or "", body=body or "",
            actions=list(actions), urgency=urgency,
            timeout_ms=expire if expire and expire > 0 else (
                -1 if urgency == URGENCY_CRITICAL else DEFAULT_TIMEOUT_MS),
        )
        log.info("notify [%s] %s", n.app_name, n.summary)
        self.store.add(n)
        return nid

    def _alloc_id(self) -> int:
        nid = self._next_id
        self._next_id += 1
        return nid

    # -- outbound signals -------------------------------------------------
    def _emit_closed(self, nid: int, reason: int) -> None:
        if not self._conn:
            return
        try:
            self._conn.emit_signal(None, OBJ_PATH, BUS_NAME, "NotificationClosed",
                                   GLib.Variant("(uu)", (nid, reason)))
        except GLib.Error as exc:
            log.debug("NotificationClosed emit failed: %s", exc.message)

    def dismiss(self, nid: int) -> None:
        """Called by the shade when the user swipes a notification away."""
        if self.store.remove(nid):
            self._emit_closed(nid, REASON_DISMISSED)

    def dismiss_all(self) -> None:
        for nid in self.store.clear():
            self._emit_closed(nid, REASON_DISMISSED)

    def invoke(self, nid: int, action_key: str) -> None:
        if not self._conn:
            return
        try:
            self._conn.emit_signal(None, OBJ_PATH, BUS_NAME, "ActionInvoked",
                                   GLib.Variant("(us)", (nid, action_key)))
        except GLib.Error as exc:
            log.debug("ActionInvoked emit failed: %s", exc.message)
