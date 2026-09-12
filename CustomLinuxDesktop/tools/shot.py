"""Grab the live X root window (or a region) to a PNG.

Uses GDK directly so it needs nothing beyond python3-gi, and works over SSH
against the running :0 display.

  python3 tools/shot.py shots/out.png [x y w h]
"""
import sys, os
import gi
gi.require_version("Gtk", "3.0"); gi.require_version("Gdk", "3.0")
from gi.repository import Gdk, GdkPixbuf

os.environ.setdefault("DISPLAY", ":0")
out = sys.argv[1] if len(sys.argv) > 1 else "shots/root.png"
root = Gdk.get_default_root_window()
if root is None:
    sys.exit("no X display (is DISPLAY=%s right?)" % os.environ.get("DISPLAY"))
w, h = root.get_width(), root.get_height()
x = y = 0
if len(sys.argv) >= 6:
    x, y, w, h = (int(v) for v in sys.argv[2:6])
pb = Gdk.pixbuf_get_from_window(root, x, y, w, h)
if pb is None:
    sys.exit("capture failed")
os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
pb.savev(out, "png", [], [])
print("wrote %s (%dx%d)" % (out, pb.get_width(), pb.get_height()))
