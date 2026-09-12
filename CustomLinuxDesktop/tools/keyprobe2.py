"""Disambiguate START from SELECT.

The first probe proved two dedicated (non-typing) keys exist - Print (107) and
Pause (127) - but not which is which, because they were pressed interleaved.
This asks for one at a time and reports the winner per phase.
"""
import os, sys, time
os.environ.setdefault("DISPLAY", ":0")
import gi
gi.require_version("Gtk", "3.0"); gi.require_version("Gdk", "3.0")
from gi.repository import Gdk, GLib, Gtk

PHASES = [("START", 12), ("SELECT", 12)]
OUT = "/tmp/keyprobe2.txt"
hits = {name: {} for name, _ in PHASES}
phase = [0]

win = Gtk.Window(title="which key is which")
win.set_default_size(640, 440); win.move(0, 40); win.set_keep_above(True)
lbl = Gtk.Label(); lbl.set_justify(Gtk.Justification.CENTER)
win.add(lbl)


def render(extra=""):
    if phase[0] >= len(PHASES):
        lbl.set_markup("<span size='26000'>Done - thank you</span>")
        return
    name, _ = PHASES[phase[0]]
    lbl.set_markup(
        "<span size='34000'><b>Press %s</b></span>\n"
        "<span size='19000'>a few times\n\n%s</span>" % (name, extra))


def on_key(_w, ev):
    if phase[0] >= len(PHASES):
        return True
    name = PHASES[phase[0]][0]
    key = (ev.hardware_keycode, Gdk.keyval_name(ev.keyval))
    hits[name][key] = hits[name].get(key, 0) + 1
    render("captured: " + ", ".join("%s(%d)" % (k[1], k[0]) for k in hits[name]))
    return True


# Print's PRESS is grabbed by another client on this desktop, so count releases
# too - otherwise that key looks like it does not exist.
win.connect("key-press-event", on_key)
win.connect("key-release-event", on_key)
win.connect("destroy", Gtk.main_quit)
win.show_all(); win.present()
try:
    win.get_window().focus(Gdk.CURRENT_TIME)
except Exception:
    pass
render()


def advance():
    phase[0] += 1
    render()
    if phase[0] >= len(PHASES):
        GLib.timeout_add_seconds(2, lambda: (Gtk.main_quit(), False)[1])
        return False
    GLib.timeout_add_seconds(PHASES[phase[0]][1], advance)
    return False


GLib.timeout_add_seconds(PHASES[0][1], advance)
Gtk.main()

lines = []
for name, _ in PHASES:
    ranked = sorted(hits[name].items(), key=lambda kv: -kv[1])
    lines.append("%s:" % name)
    if not ranked:
        lines.append("   (nothing captured)")
    for (code, sym), n in ranked:
        lines.append("   keycode=%-4d %-14s x%d" % (code, sym, n))
open(OUT, "w").write("\n".join(lines) + "\n")
print("\n".join(lines))
