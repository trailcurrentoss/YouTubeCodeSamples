"""Render every glyph to a PNG contact sheet at real shell sizes.

Offscreen, so it works over SSH with no X server. Used to eyeball icon
legibility at the exact pixel sizes the status bar and nav bar use.
"""
import sys, os, math
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import cairo
import gi
gi.require_version("Gtk", "3.0"); gi.require_version("Gdk", "3.0")
from trailshell import icons, theme as T

SIZES = [20, 24, 30, 44]      # status bar, nav bar, qs tile, big
names = icons.names()
COLS = len(SIZES) + 1
PAD, LABEL_W, ROW_H = 16, 150, 60
W = LABEL_W + sum(s + 40 for s in SIZES) + PAD * 2
H = PAD * 2 + 40 + ROW_H * len(names)

surf = cairo.ImageSurface(cairo.FORMAT_ARGB32, W, H)
cr = cairo.Context(surf)
cr.set_source_rgb(0, 0, 0); cr.paint()

cr.select_font_face("DejaVu Sans"); cr.set_font_size(14)
cr.set_source_rgb(0.68, 0.71, 0.74)
x = PAD + LABEL_W
for s in SIZES:
    cr.move_to(x, PAD + 22); cr.show_text(f"{s}px")
    x += s + 40

for i, n in enumerate(names):
    y = PAD + 40 + i * ROW_H
    cr.set_source_rgb(0.68, 0.71, 0.74); cr.set_font_size(15)
    cr.move_to(PAD, y + ROW_H / 2 + 5); cr.show_text(n)
    x = PAD + LABEL_W
    for s in SIZES:
        icons.draw(cr, n, x, y + (ROW_H - s) / 2, s, T.C["text"])
        x += s + 40

out = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "shots", "iconsheet.png")
surf.write_to_png(out)
print("wrote", out, f"{W}x{H}")
