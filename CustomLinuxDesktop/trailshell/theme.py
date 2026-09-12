"""
TrailShell design tokens.

SINGLE SOURCE OF TRUTH for every colour, size and font in the shell.
Tuned for the PocketTerm35: Waveshare 3.5" DPI panel, 640x480, ~228 PPI.

Physical sizing rationale
-------------------------
The panel is 640 px across ~71.1 mm of glass => 9.0 px/mm (228 PPI).
Everything below is chosen in millimetres first, then converted, so the UI is
physically the same size as a modern phone's UI rather than accidentally
microscopic. Reference points:

  * A phone at 400 PPI renders 16sp body text at 40 px == 2.54 mm em.
    Matching that here needs ~23 px. We use 24 px and never go below 20 px,
    which gives ~1.7 mm cap height - comfortably readable for a 40+ reader
    wearing glasses at a typical 25-30 cm hold distance.
  * Android's minimum touch target is 48dp (7.6 mm); iOS uses 44pt (7.7 mm).
    We use 64 px (7.1 mm) for standard rows and never accept less than 56 px
    (6.2 mm) for a secondary control.

Bump UI_SCALE if the whole thing still reads too small on the glass; every
size below is derived from it.
"""

# --- panel geometry -------------------------------------------------------
SCREEN_W = 640
SCREEN_H = 480
PX_PER_MM = 9.0          # 640 px / 71.1 mm of active area

#: Global multiplier. 1.0 == the tuned default described above.
#: Raise toward 1.15 for more legibility, drop toward 0.9 for more content.
#: Read once at import - every size below is derived from it, so changing it
#: takes effect on the next shell start.
def _initial_scale() -> float:
    try:
        from .settings import get
        return float(get("ui_scale", 1.0))
    except Exception:
        return 1.0


UI_SCALE = _initial_scale()


def mm(value: float) -> int:
    """Millimetres -> physical pixels on this panel."""
    return round(value * PX_PER_MM * UI_SCALE)


def sp(px: float) -> int:
    """Scale a hand-tuned pixel value by UI_SCALE."""
    return round(px * UI_SCALE)


# --- chrome geometry ------------------------------------------------------
# The shell reserves a strut at top and bottom; apps get everything between.
STATUS_H = sp(36)        # 4.0 mm - tall enough to grab for the swipe-down shade
NAV_H = sp(34)           # 3.8 mm - back / home / recents, with key legends

#: Nav-bar key legends ("START", "SELECT"). Below the TEXT_XS floor on purpose
#: and the only place in the shell that goes there: these are 5-6 character
#: all-caps words, so every glyph is full cap height (~1.5 mm) with no
#: descenders and no reading required - you match the word on screen to the
#: word on the key. Body text never uses this.
TEXT_LEGEND = sp(18)
APP_Y = STATUS_H
APP_H = SCREEN_H - STATUS_H - NAV_H      # 418 px of app canvas

# --- touch targets --------------------------------------------------------
TOUCH_MIN = sp(56)       # 6.2 mm - absolute floor for any tappable thing
ROW_H = sp(64)           # 7.1 mm - standard list row / settings row
BTN_H = sp(64)
FAB = sp(72)

# --- launcher grid --------------------------------------------------------
# The app canvas is only 414 px tall once the status and nav bars take their
# struts, so the grid scrolls rather than paginates: 4 columns of 116 px cells
# shows two and a half rows, and a kinetic flick reaches the rest. Four columns
# (not five) because a 160 px cell fits ~13 characters of label at TEXT_XS,
# where a 128 px cell truncates "Chromium Web Browser" into nonsense.
GRID_COLS = 4
ICON_PX = sp(76)         # 8.4 mm icon - close to an iOS home-screen icon
CELL_W = SCREEN_W // GRID_COLS           # 160 px
CELL_H = sp(116)
SEARCH_H = sp(56)

# --- spacing --------------------------------------------------------------
SP_1 = sp(4)
SP_2 = sp(8)
SP_3 = sp(12)
SP_4 = sp(16)
SP_5 = sp(24)
SP_6 = sp(32)

# --- radii (mirrors the trailcurrent.com scale, scaled down for a 3.5") ----
RADIUS_SM = sp(8)
RADIUS_MD = sp(12)
RADIUS_LG = sp(16)
RADIUS_FULL = 9999

# --- type scale -----------------------------------------------------------
FONT = "Inter"
FONT_FALLBACK = "Inter, DejaVu Sans, sans-serif"
FONT_MONO = "DejaVu Sans Mono, monospace"

TEXT_XS = sp(20)         # hard floor - status bar meta, captions only
TEXT_SM = sp(22)
TEXT_BASE = sp(24)       # body / list rows
TEXT_LG = sp(28)
TEXT_XL = sp(32)         # screen titles
TEXT_2XL = sp(40)        # clock on the shade

WEIGHT_NORMAL = 400
WEIGHT_MEDIUM = 500
WEIGHT_SEMIBOLD = 600
WEIGHT_BOLD = 700

# --- colour ---------------------------------------------------------------
# Sourced from https://trailcurrent.com/css/variables.css.
#
# Light is the default: this panel is most often read outdoors and in daylight,
# where a light ground is far easier to see than a dark one. Dark is a first
# class alternative for night use in a vehicle, not an afterthought.
#
# Note on the accent colours in the LIGHT palette: the brand's #52a441,
# #48e6fe, #74fe00 and #ff5453 were designed to sit on dark or photographic
# grounds. On white they land at 3.1:1, 1.4:1, 1.6:1 and 2.9:1 contrast, all
# below the 4.5:1 needed for text. The light palette therefore uses darkened
# variants for anything inked onto the background, and keeps the pure brand
# colours for fills (where the text sits on top of the colour, not beside it).

BRAND_PRIMARY = "#52a441"
BRAND_PRIMARY_LIGHT = "#6ab85a"
BRAND_PRIMARY_DARK = "#3d7d31"
BRAND_SECONDARY = "#d0e2c7"

PALETTES = {
    "light": {
        # grounds
        "bg":            "#ffffff",
        "surface":       "#f8f9fa",   # --color-gray-100
        "surface_alt":   "#e9ecef",   # --color-gray-200
        "surface_hi":    "#dee2e6",   # --color-gray-300
        "scrim":         "rgba(0,0,0,0.45)",

        # lines: `border` is a decorative divider (WCAG sets no minimum for
        # those); `border_strong` outlines an actual control and must clear 3:1.
        "border":        "#dee2e6",
        "border_strong": "#83888f",   # 3.01:1 on the card, 3.57:1 on #ffffff

        # ink              (worst-case contrast noted)
        "text":          "#212529",   # 13.01:1 on the card ground
        "text_dim":      "#495057",   #  6.90:1 on the card ground
        "text_mute":     "#6c757d",   #  4.69:1 on #ffffff
        "on_accent":     "#ffffff",   #  4.83:1 on the accent fill

        # brand + state, darkened for use as ink on a light ground
        "accent":        "#3b782f",   # 4.53:1 on the card ground
        "accent_press":  "#2e5f25",   # pressed state - darker, per light-UI convention
        "accent_soft":   "#dff1dc",   # --color-primary-subtle, lifted to clear 4.5:1
        "info":          "#0a7d92",   # 4.81:1 on #ffffff
        "success":       "#3d8600",   # 4.56:1 on #ffffff
        "danger":        "#c62828",   # 5.62:1 on #ffffff
    },
    "dark": {
        # grounds
        "bg":            "#000000",   # --color-dark
        "surface":       "#14171a",
        "surface_alt":   "#212529",   # --color-gray-900
        "surface_hi":    "#343a40",   # --color-gray-800
        "scrim":         "rgba(0,0,0,0.72)",

        # lines
        "border":        "#343a40",
        "border_strong": "#656f78",   # 3.01:1 on the card, 4.10:1 on #000000

        # ink
        "text":          "#ebebeb",   # 12.94:1 on the card ground  --color-light
        "text_dim":      "#adb5bd",   #  7.43:1 on the card ground  --color-gray-400
        "text_mute":     "#6d767e",   #  4.54:1 on #000000
        "on_accent":     "#000000",   #  6.75:1 on the accent fill

        # brand + state - the palette's native home, used unmodified
        "accent":        BRAND_PRIMARY,        # #52a441, 4.95:1 on the card
        "accent_press":  BRAND_PRIMARY_LIGHT,  # #6ab85a - lighter, per dark-UI convention
        "accent_soft":   "#1b2a17",
        "info":          "#48e6fe",   # --color-info
        "success":       "#74fe00",   # --color-success
        "danger":        "#ff5453",   # --color-danger
    },
}

#: Live palette. Mutated **in place** on theme change so that modules which
#: did ``C = theme.C`` at import time keep seeing the current colours without
#: any of them needing to re-resolve the reference.
C: dict[str, str] = dict(PALETTES["light"])

_theme_name = "light"
_listeners: list = []


def theme_name() -> str:
    return _theme_name


def set_theme(name: str) -> None:
    """Switch palette and notify every listener. No-op for an unknown name."""
    global _theme_name
    if name not in PALETTES or name == _theme_name:
        return
    _theme_name = name
    C.clear()
    C.update(PALETTES[name])
    for fn in list(_listeners):
        try:
            fn(name)
        except Exception:
            import logging
            logging.getLogger("trailshell.theme").exception("theme listener failed")


def toggle_theme() -> str:
    set_theme("dark" if _theme_name == "light" else "light")
    return _theme_name


def on_theme_changed(fn) -> None:
    _listeners.append(fn)


# --- motion ---------------------------------------------------------------
# Short. A 3.5" screen has almost no distance to travel, and the Pi 5 drives
# this panel over DPI - long animations just read as lag.
DUR_FAST = 120           # ms
DUR_BASE = 200
DUR_SLOW = 320


def _apply_saved_theme() -> None:
    try:
        from .settings import get
        name = get("theme", "light")
    except Exception:
        name = "light"
    if name in PALETTES and name != _theme_name:
        set_theme(name)


_apply_saved_theme()
