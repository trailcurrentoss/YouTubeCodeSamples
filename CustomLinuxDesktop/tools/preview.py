"""Run one shell component on the live X display for a few seconds, screenshot
it, then exit. Lets us validate visuals against the real 640x480 panel without
taking over the session.

  python3 tools/preview.py bar shots/01-bar.png
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.environ.setdefault("DISPLAY", ":0")
os.environ.setdefault("XDG_RUNTIME_DIR", "/run/user/1000")

import gi
gi.require_version("Gtk", "3.0"); gi.require_version("Gdk", "3.0")
from gi.repository import Gdk, GLib, Gtk

from trailshell import providers, style, theme as T

what = sys.argv[1] if len(sys.argv) > 1 else "bar"
out = sys.argv[2] if len(sys.argv) > 2 else f"shots/{what}.png"
hold = float(sys.argv[3]) if len(sys.argv) > 3 else 2.5

theme_arg = os.environ.get("TS_THEME")
if theme_arg:
    T.set_theme(theme_arg)
style.apply()
system = providers.System()
widgets = []

if what == "bar":
    from trailshell.bar import StatusBar
    b = StatusBar(system)
    b.set_notification_count(3)
    b.show()
    widgets.append(b)
elif what == "nav":
    from trailshell.nav import NavBar
    n = NavBar()
    n.show()
    widgets.append(n)
elif what == "shade":
    from trailshell.bar import StatusBar
    from trailshell.shade import Shade
    from trailshell.notifications import NotificationStore
    store = NotificationStore()
    store.demo()
    sh = Shade(system, store)
    b = StatusBar(system, on_tap=sh.open)
    b.set_notification_count(len(store.items))
    b.show(); sh.open(animate=False)
    widgets += [b, sh]
elif what == "launcher":
    from trailshell.launcher import Launcher
    lc = Launcher(system)
    lc.show()
    widgets.append(lc)
elif what == "power":
    from trailshell.powermenu import PowerMenu
    pm = PowerMenu()
    pm.show()
    widgets.append(pm)
elif what == "shortcuts":
    from trailshell.shortcuts import Shortcuts
    from trailshell import keymap
    sc = Shortcuts()
    sc.set_resolved({b.action: b.accels[0] for b in keymap.BINDINGS})
    sc.show()
    widgets.append(sc)
elif what == "switcher":
    from trailshell.switcher import Switcher
    sw = Switcher()
    sw.show()
    widgets.append(sw)
else:
    sys.exit("unknown component: %s" % what)


def grab():
    Gdk.flush()
    root = Gdk.get_default_root_window()
    pb = Gdk.pixbuf_get_from_window(root, 0, 0, T.SCREEN_W, T.SCREEN_H)
    os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
    pb.savev(out, "png", [], [])
    print("wrote", out)
    Gtk.main_quit()
    return False


GLib.timeout_add(int(hold * 1000), grab)
Gtk.main()
