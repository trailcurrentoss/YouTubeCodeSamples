"""The shell itself: builds every surface and wires them together.

Window policy, in one place, because it is the thing that makes a desktop feel
like a phone rather than a shrunken PC:

  * Apps are always full-screen. At 640x480 there is no useful overlap, and a
    title bar plus borders would eat 10% of the canvas to let you do something
    you would never want to do. Openbox is configured to undecorate and
    maximise everything (see session/openbox-rc.xml).
  * The shell reserves the top 36 px and bottom 30 px with EWMH struts, so
    "maximised" automatically means the 640x414 canvas between them.
  * The launcher *is* the desktop. It shows when nothing is running and when
    you press Home, and hides whenever an app takes the screen.

Every touch affordance has a keyboard equivalent (see keymap.py), because the
PocketTerm35's keyboard is the primary input, not an accessory.
"""
from __future__ import annotations

import logging

from . import _gi  # noqa: F401
from gi.repository import GLib, Gtk

from . import (banner, bar, keymap, launcher, nav, notifications, powermenu,
               panner, providers, settings, shade, shortcuts, style,
               switcher, theme as T, wm, x11)

log = logging.getLogger("trailshell.shell")


class Shell:
    def __init__(self) -> None:
        style.apply()
        self.X = x11.get()

        #: Enforces "apps are full-screen, chromeless and immovable". See wm.py
        #: for why openbox's own config is not sufficient on its own.
        self.policy = wm.WindowPolicy(self.X)
        #: True while the launcher is deliberately on top. Without this, the
        #: _NET_ACTIVE_WINDOW change caused by raising the launcher races
        #: _on_windows_changed, which then immediately hides it again.
        self._home_showing = False

        self.sys = providers.System()
        self.sys.dnd = bool(settings.get("dnd", False))

        self.store = notifications.NotificationStore()
        self.server = notifications.NotificationServer(self.store)

        # --- surfaces ---
        self.launcher = launcher.Launcher(self.sys, on_launch=self._on_app_launched)
        self.switcher = switcher.Switcher(on_activated=self._leave_home)
        self.shortcuts = shortcuts.Shortcuts()
        self.power = powermenu.PowerMenu(on_logout=self._logout)
        self.panner = panner.Panner(on_pan=self._pan)
        self.policy.on_oversize_changed = self.panner.set_visible
        self.shade = shade.Shade(
            self.sys, self.store, self.server,
            on_open_settings=self._open_settings,
            on_show_shortcuts=self.shortcuts.show,
            on_open_launcher=self.go_home,
            on_power=self.power.show,
        )
        self.banner = banner.Banner(on_tap=self._on_banner_tap,
                                    on_expired=lambda _n: None)
        self.bar = bar.StatusBar(
            self.sys,
            on_tap=self.shade.open,
            on_pull_begin=self.shade.begin_pull,
            on_pull_update=self.shade.update_pull,
            on_pull_end=self.shade.end_pull,
        )
        self.nav = nav.NavBar(on_back=self.go_back,
                              on_home=self.go_home,
                              on_recents=self.show_recents)

        self.store.changed.connect(self._on_store_changed)
        self.store.posted.connect(self._on_notification_posted)

    # -- startup ----------------------------------------------------------
    def start(self) -> None:
        self.bar.show()
        self.nav.show()
        self.shade.show_chrome_hint()
        self.server.start()
        self.go_home()
        self.X.watch(self._on_windows_changed)
        self.policy.start()
        self._bind_keys()
        log.info("TrailShell ready (theme=%s, scale=%.2f)", T.theme_name(), T.UI_SCALE)

    def stop(self) -> None:
        self.X.ungrab_all()
        self.server.stop()

    # -- hotkeys ----------------------------------------------------------
    def _bind_keys(self) -> None:
        actions = {
            "home": self.go_home,
            "launcher": self.go_home,
            "recents": self.show_recents,
            "last_app": self.switcher.cycle,
            "back": self.go_back,
            "shade": self.shade.toggle,
            "close": self.close_current,
            "pan_up": lambda: self._pan(-1),
            "pan_down": lambda: self._pan(+1),
            "vol_up": lambda: self._nudge_volume(+0.05),
            "vol_down": lambda: self._nudge_volume(-0.05),
            "vol_mute": self.sys.audio.toggle_mute,
            "dim_up": lambda: self._nudge_dim(+0.1),
            "dim_down": lambda: self._nudge_dim(-0.1),
            "theme": self._toggle_theme,
            "screenshot": self.screenshot,
            "shortcuts": self.shortcuts.toggle,
        }
        resolved: dict[str, str] = {}
        for b in keymap.BINDINGS:
            cb = actions.get(b.action)
            if cb is None:
                continue
            # Try each alias until one grabs; a key another client already owns
            # falls through to the Ctrl+Alt fallback rather than silently dying.
            for accel in b.accels:
                if self.X.bind(accel, cb):
                    resolved[b.action] = accel
                    break
            else:
                log.warning("no accelerator available for %s", b.action)

        # Dedicated hardware keys are bound separately from the Super aliases,
        # because the nav-bar legend must reflect that *this key* works.
        hardware: dict[str, str] = {}
        for action, (label, accel) in keymap.HARDWARE_NAV.items():
            cb = actions.get(action)
            if not accel or cb is None:
                continue
            if self.X.bind(accel, cb):
                hardware[action] = label
                log.info("hardware key %s -> %s", accel, action)
            else:
                log.warning("hardware key %s (%s) did not bind", accel, label)
        self.shortcuts.set_resolved(resolved)
        # Advertise a hardware key on the nav bar only if that key itself
        # grabbed - never because the action is reachable via Super.
        self.nav.set_legends(hardware)
        log.info("bound %d/%d shortcuts", len(resolved), len(keymap.BINDINGS))

    # -- navigation -------------------------------------------------------
    def _hide_overlays(self, keep=()) -> bool:
        """Close whatever shell surface is on top. True if something closed."""
        closed = False
        for name, is_open, close in (
            ("shade", lambda: self.shade.win.get_visible(), self.shade.close),
            ("shortcuts", lambda: self.shortcuts.visible, self.shortcuts.hide),
            ("switcher", lambda: self.switcher.visible, self.switcher.hide),
            ("power", lambda: self.power.visible, self.power.hide),
        ):
            if name not in keep and is_open():
                close()
                closed = True
        return closed

    def go_home(self) -> None:
        self._hide_overlays()
        self.banner.hide_all()
        self.panner.set_visible(False)
        was_home = self._home_showing and self.launcher.win.get_visible()
        self._home_showing = True
        if was_home:
            # Home while already home resets search and scroll, the way a
            # phone's home button returns you to the first page.
            self.launcher.reset()
        else:
            self.launcher.show()
        self.launcher.win.present()

    def go_back(self) -> None:
        if self._hide_overlays():
            return
        # With full-screen apps and no universal X "back", the useful meaning
        # of Back is "return to what I was looking at before".
        wins = list(reversed(self.X.list_windows()))
        if len(wins) >= 2:
            self.X.activate(wins[1].xid)
        elif self.launcher.win.get_visible():
            return
        else:
            self.go_home()

    def show_recents(self) -> None:
        self._hide_overlays(keep=("switcher",))
        if self.switcher.visible:
            self.switcher.hide()
        else:
            self.switcher.show()

    def _pan(self, steps: int) -> None:
        """Scroll the focused window when it is taller than the canvas."""
        self.policy.pan_active(steps)

    def close_current(self) -> None:
        info = self.X.active_window()
        if info:
            self.X.close(info.xid)

    # -- window tracking --------------------------------------------------
    def _on_app_launched(self, _app) -> None:
        # Give the app a moment to map before we get out of its way.
        GLib.timeout_add(600, self._hide_launcher_if_apps)

    def _leave_home(self) -> None:
        """Give the screen back to whatever app is running."""
        self._home_showing = False
        self.launcher.hide()

    def _hide_launcher_if_apps(self) -> bool:
        if self.X.list_windows():
            self._leave_home()
        return False

    def _on_windows_changed(self) -> None:
        # Every new or re-configured window gets the full-screen policy applied
        # before anything else looks at it.
        self.policy.sync()

        wins = self.X.list_windows()
        if not wins:
            # Last app closed: fall back to the launcher rather than a blank
            # root window, which would look like a crash.
            self._home_showing = True
            if not self.launcher.win.get_visible():
                self.launcher.show()
        elif not self._home_showing:
            # Only auto-hide when the user has not explicitly asked for Home.
            active = next((w for w in wins if w.is_active), None)
            if active and self.launcher.win.get_visible():
                self.launcher.hide()
        if self.switcher.visible:
            self.switcher._rebuild()

    # -- notifications ----------------------------------------------------
    def _on_store_changed(self, store) -> None:
        self.bar.set_notification_count(len(store.items))

    def _on_notification_posted(self, notif) -> None:
        if self.sys.dnd and not notif.is_critical:
            return          # recorded in the shade, but no heads-up
        if self.shade.win.get_visible():
            return          # already looking at the list
        self.banner.post(notif)

    def _on_banner_tap(self, _notif) -> None:
        self.shade.open()

    # -- system actions ---------------------------------------------------
    def screenshot(self) -> None:
        self.shade.take_screenshot()

    def _nudge_volume(self, delta: float) -> None:
        self.sys.audio.set_volume(self.sys.audio.volume + delta)

    def _nudge_dim(self, delta: float) -> None:
        self.sys.brightness.set(self.sys.brightness.value + delta)

    def _toggle_theme(self) -> None:
        settings.set("theme", T.toggle_theme())

    def _logout(self) -> None:
        from gi.repository import Gtk as _Gtk
        self.stop()
        _Gtk.main_quit()

    def _open_settings(self) -> None:
        # No bespoke settings app yet; hand off to the Pi's own control centre,
        # which is at least a real, complete one.
        providers.run_fire(["rpcc"])
