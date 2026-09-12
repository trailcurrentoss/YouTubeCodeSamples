"""Report what GTK actually receives from the panel.

If GTK sees GdkEventTouch, ScrolledWindow's built-in kinetic scrolling works.
If it only sees emulated button/motion events, it does not, and the shell has
to implement drag-scrolling itself.
"""
import os, sys
os.environ.setdefault("DISPLAY", ":0")
import gi
gi.require_version("Gtk", "3.0"); gi.require_version("Gdk", "3.0")
from gi.repository import Gdk, GLib, Gtk

SECS = int(sys.argv[1]) if len(sys.argv) > 1 else 20
seen = {}

win = Gtk.Window(title="touch probe")
win.set_default_size(640, 440)
win.move(0, 40)
win.set_keep_above(True)
lbl = Gtk.Label()
lbl.set_markup("<span size='22000'>Drag a finger up and down\nanywhere in this window</span>")
win.add(lbl)

MASK = (Gdk.EventMask.TOUCH_MASK | Gdk.EventMask.BUTTON_PRESS_MASK
        | Gdk.EventMask.BUTTON_RELEASE_MASK | Gdk.EventMask.POINTER_MOTION_MASK
        | Gdk.EventMask.SMOOTH_SCROLL_MASK)
win.add_events(MASK)

SOURCES = {}
for name in dir(Gdk.InputSource):
    if name.isupper():
        try: SOURCES[int(getattr(Gdk.InputSource, name))] = name
        except Exception: pass

def on_event(_w, ev):
    t = ev.type.value_name.replace("GDK_", "")
    dev = ev.get_source_device()
    src = "?"
    if dev is not None:
        try: src = SOURCES.get(int(dev.get_source()), str(dev.get_source()))
        except Exception: pass
    key = (t, src)
    seen[key] = seen.get(key, 0) + 1
    return False

win.connect("event", on_event)
win.connect("destroy", Gtk.main_quit)
win.show_all()
GLib.timeout_add_seconds(SECS, lambda: (Gtk.main_quit(), False)[1])
Gtk.main()

print("\n=== events GTK received ===")
if not seen:
    print("  (nothing - was the window touched?)")
for (t, src), n in sorted(seen.items(), key=lambda kv: -kv[1]):
    print("  %-24s source=%-24s x%d" % (t, src, n))
touch = any(t.startswith("TOUCH") for t, _ in seen)
print("\nGTK sees real touch events:", "YES" if touch else "NO")
print("=> ScrolledWindow kinetic scrolling will",
      "work" if touch else "NOT work; the shell must drag-scroll itself")
