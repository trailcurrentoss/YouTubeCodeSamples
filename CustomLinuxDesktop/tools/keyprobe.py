"""Log every key X receives, so we can learn what the PocketTerm35's Fn layer
actually emits without touching the Pico firmware.

Run it, press the keys in question, then Ctrl+C (or wait for the timeout).
Anything that prints is handled by the kernel/X and is therefore something the
shell *can* bind. Keys that adjust the hardware but print nothing are handled
entirely inside the keyboard firmware, and the shell must leave them alone.

  python3 tools/keyprobe.py [seconds]
"""
import os, sys, time
os.environ.setdefault("DISPLAY", ":0")
import gi
gi.require_version("Gtk", "3.0"); gi.require_version("Gdk", "3.0")
from gi.repository import Gdk, GLib, Gtk

SECS = int(sys.argv[1]) if len(sys.argv) > 1 else 30
OUT = "/tmp/keyprobe.txt"
seen = []

win = Gtk.Window(title="TrailShell key probe")
win.set_default_size(640, 480)
win.set_keep_above(True)
lbl = Gtk.Label()
lbl.set_markup(
    "<span size='18000'>Press <b>START</b>, then <b>SELECT</b>.\n\n"
    "Then <b>Fn + -</b>, <b>Fn + +</b>,\n"
    "<b>Fn + e</b>, <b>Fn + r</b>.\n\n"
    f"Closing in {SECS}s.</span>")
lbl.set_justify(Gtk.Justification.CENTER)
win.add(lbl)


def describe(ev):
    name = Gdk.keyval_name(ev.keyval)
    mods = []
    for flag, tag in ((Gdk.ModifierType.CONTROL_MASK, "Ctrl"),
                      (Gdk.ModifierType.MOD1_MASK, "Alt"),
                      (Gdk.ModifierType.SHIFT_MASK, "Shift"),
                      (Gdk.ModifierType.SUPER_MASK, "Super"),
                      (Gdk.ModifierType.MOD5_MASK, "AltGr")):
        if ev.state & flag:
            mods.append(tag)
    return "keycode=%-4d keyval=0x%04x  %-28s mods=%s" % (
        ev.hardware_keycode, ev.keyval, name, "+".join(mods) or "-")


def on_key(_w, ev):
    line = describe(ev)
    print(line, flush=True)
    seen.append(line)
    lbl.set_markup(
        "<span size='16000'><tt>" +
        GLib.markup_escape_text("\n".join(seen[-8:])) +
        "</tt></span>")
    return True


def on_key_release(_w, ev):
    # Some firmware sends a key only on release, or sends a different code
    # there; recording both halves avoids concluding "nothing reached X".
    line = "RELEASE " + describe(ev)
    seen.append(line)
    return True


win.connect("key-press-event", on_key)
win.connect("key-release-event", on_key_release)
win.connect("destroy", Gtk.main_quit)
win.show_all()
win.present()
win.grab_focus()
try:
    win.get_window().focus(Gdk.CURRENT_TIME)
except Exception:
    pass
GLib.timeout_add_seconds(SECS, lambda: (Gtk.main_quit(), False)[1])
try:
    Gtk.main()
except KeyboardInterrupt:
    pass
report = ["=== %d key events captured ===" % len(seen)] + seen
if not seen:
    report.append("NOTHING REACHED X - those keys are handled entirely in the")
    report.append("keyboard firmware and cannot be bound by the shell.")
with open(OUT, "w") as fh:
    fh.write("\n".join(report) + "\n")
print("\n".join(report))
print("\nAlso written to %s" % OUT)
