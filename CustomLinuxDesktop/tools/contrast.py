"""WCAG 2.1 contrast audit of both palettes.

Legibility is a hard requirement on this device, so the palette is checked
rather than eyeballed. Body text must clear 4.5:1; the shell's smallest type
(TEXT_XS, 20 px) is not "large text" by WCAG's definition at this DPI, so the
3:1 large-text allowance is deliberately not used for anything but icons.
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import gi; gi.require_version("Gtk", "3.0")
from trailshell import theme as T


def lin(c):
    c /= 255.0
    return c / 12.92 if c <= 0.04045 else ((c + 0.055) / 1.055) ** 2.4


def lum(hexstr):
    h = hexstr.lstrip("#")
    r, g, b = (int(h[i:i+2], 16) for i in (0, 2, 4))
    return 0.2126 * lin(r) + 0.7152 * lin(g) + 0.0722 * lin(b)


def ratio(a, b):
    la, lb = lum(a), lum(b)
    hi, lo = max(la, lb), min(la, lb)
    return (hi + 0.05) / (lo + 0.05)


# (ink, ground, label, minimum)
CHECKS = [
    ("text",          "bg",          "body text on background",      4.5),
    ("text",          "surface",     "body text on surface",         4.5),
    ("text",          "surface_alt", "body text on card",            4.5),
    ("text_dim",      "bg",          "secondary text on background", 4.5),
    ("text_dim",      "surface_alt", "secondary text on card",       4.5),
    ("text_mute",     "bg",          "muted text on background",     4.5),
    ("accent",        "bg",          "accent text/icon on bg",       4.5),
    ("accent",        "surface_alt", "accent text on card",          4.5),
    ("danger",        "bg",          "error text on background",     4.5),
    ("danger",        "surface_alt", "error text on card",           4.5),
    ("info",          "bg",          "info text on background",      4.5),
    ("success",       "bg",          "success text on background",   4.5),
    ("on_accent",     "accent",      "label on accent fill",         4.5),
    ("on_accent",     "accent_press","label on pressed accent",      4.5),
    ("accent",        "accent_soft", "accent ink on accent tint",    4.5),
    # 1.4.11: control boundaries need 3:1. Plain dividers are decorative and
    # carry no minimum, so `border` is intentionally not audited as text.
    ("border_strong", "bg",          "control outline on bg",        3.0),
    ("border_strong", "surface_alt", "control outline on card",      3.0),
]

fails = 0
for name in ("light", "dark"):
    T.set_theme(name if name != T.theme_name() else name)
    if T.theme_name() != name:
        T.set_theme(name)
    print("\n=== %s ===" % name.upper())
    for ink, ground, label, minimum in CHECKS:
        r = ratio(T.C[ink], T.C[ground])
        ok = r >= minimum
        fails += 0 if ok else 1
        print("  %-6s %-30s %-9s on %-9s %5.2f:1  (min %.1f)"
              % ("OK  " if ok else "FAIL", label, T.C[ink], T.C[ground], r, minimum))

print("\n%d failure(s)" % fails)
sys.exit(1 if fails else 0)
