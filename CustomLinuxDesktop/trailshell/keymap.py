"""The single definition of every keyboard shortcut in the shell.

Both the hotkey grabber (:mod:`__main__`) and the on-screen cheat sheet
(:mod:`shortcuts`) read this table, so the help can never drift from what the
keys actually do.

Why this table is unusually important on this device: the PocketTerm35 has no
on-screen keyboard and never will, because it has a real one. That inverts the
usual phone-shell assumption - here the keyboard is the *primary* input and
touch is the convenient secondary, so every touch affordance in the shell has
a key equivalent listed here, not just the "power user" ones.

Each entry lists several accelerators. They are tried in order and the first
one that actually grabs wins, so the Ctrl+Alt aliases act as a fallback for
keyboards whose Super key is missing or remapped by the firmware.
"""
from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class Binding:
    action: str
    accels: tuple[str, ...]
    label: str
    section: str
    #: Shown in the cheat sheet instead of the raw accelerator when the key is
    #: physically labelled differently on this keyboard.
    display: str = ""

    @property
    def shown(self) -> str:
        return self.display or self.accels[0].replace("+", " + ")


BINDINGS: tuple[Binding, ...] = (
    # --- getting around ---
    Binding("home",      ("Super+h", "Ctrl+Alt+h"),     "Home screen",          "Navigation"),
    Binding("launcher",  ("Super+a", "Ctrl+Alt+a"),     "All apps / search",    "Navigation"),
    Binding("recents",   ("Super+Tab", "Ctrl+Alt+Tab"), "Recent apps",          "Navigation"),
    Binding("last_app",  ("Alt+Tab",),                  "Switch to last app",   "Navigation"),
    Binding("back",      ("Alt+Left",),                 "Back",                 "Navigation"),
    Binding("shade",     ("Super+n", "Ctrl+Alt+n"),     "Notifications & quick settings", "Navigation"),

    # --- windows ---
    Binding("close",     ("Super+q", "Ctrl+Alt+q"),     "Close current app",    "Windows"),
    # For applications whose minimum size is larger than the 640x410 canvas
    # (GIMP clamps at 640x503), so the off-screen part is still reachable.
    Binding("pan_up",    ("Super+Up", "Ctrl+Alt+Up"),   "Scroll oversized window up",   "Windows"),
    Binding("pan_down",  ("Super+Down", "Ctrl+Alt+Down"), "Scroll oversized window down", "Windows"),

    # --- system ---
    Binding("vol_up",    ("XF86AudioRaiseVolume",),     "Volume up",            "System", "Fn + r"),
    Binding("vol_down",  ("XF86AudioLowerVolume",),     "Volume down",          "System", "Fn + e"),
    Binding("vol_mute",  ("XF86AudioMute",),            "Mute",                 "System"),
    Binding("dim_up",    ("XF86MonBrightnessUp",),      "Brighter",             "System", "Fn + +"),
    Binding("dim_down",  ("XF86MonBrightnessDown",),    "Dimmer",               "System", "Fn + -"),
    Binding("theme",     ("Super+d", "Ctrl+Alt+d"),     "Light / dark theme",   "System"),
    Binding("screenshot",("Super+p", "Ctrl+Alt+p"),     "Screenshot",           "System"),
    Binding("shortcuts", ("Super+slash", "Ctrl+Alt+slash"), "This shortcut list", "System", "Super + /"),
)

#: The PocketTerm35 has dedicated START and SELECT keys, bound to the two
#: navigation actions people reach for most. Because a key with a printed
#: legend is worthless if you cannot tell what it does, the nav bar draws the
#: matching legend next to the icon it triggers.
#:
#: Each entry is (legend, accelerator). The legend is drawn ONLY if that exact
#: accelerator was successfully grabbed - not merely if the action is reachable
#: some other way. Advertising "START" on screen because Super+h happened to
#: bind would be a lie about a key that does nothing.
#:
#: The accelerators below come from reading the keyboard's own firmware
#: (see docs/keyboard-firmware.md). Its matrix row 6 is:
#:
#:     FN  CTRL  L-ALT  PRINT_SCREEN  SPACE  PAUSE  R-ALT  WIN  FN
#:
#: so the two dedicated buttons flank the spacebar and emit Print and Pause -
#: the only keys on this keyboard that send something a person would never
#: type. SELECT sits left of the spacebar and START right of it, per the usual
#: convention; if that is backwards on your unit, swap these two lines.
#:
#: Note Print is commonly grabbed by a screenshot binding on other desktops,
#: which is exactly why the nav bar only draws a legend when the grab succeeds.
HARDWARE_NAV: dict[str, tuple[str, str]] = {
    "home":    ("START", "Pause"),
    "recents": ("SELECT", "Print"),
}

#: The six gamepad buttons, after the firmware remap (docs/keyboard-firmware.md).
#: They send F13-F18, which the session renames from xkb's misleading
#: "XF86Tools"/"XF86Launch5".."XF86Launch9" labels.
#:
#: These are deliberately NOT grabbed globally. A global grab would take them
#: away from every application, and the whole point of moving them off letters
#: was to give applications six real, unused keys. The shell only acts on them
#: while one of its own surfaces has focus.
#:
#: Semantics follow the console convention people already have: A confirms,
#: B goes back, shoulders page through lists.
GAMEPAD: dict[str, tuple[str, str]] = {
    # keysym    (button, what it does in a shell surface)
    "F18":      ("A", "Select / launch"),
    "F17":      ("B", "Back / close"),
    "F15":      ("X", "Close item"),
    "F16":      ("Y", "Notifications"),
    "F13":      ("L", "Page up"),
    "F14":      ("R", "Page down"),
}

#: Reverse lookup used by the surfaces.
GAMEPAD_ACTION = {
    "F18": "select", "F17": "back", "F15": "close_item",
    "F16": "shade",  "F13": "page_up", "F14": "page_down",
}

SECTIONS = ("Navigation", "Windows", "System")


def by_section() -> dict[str, list[Binding]]:
    out: dict[str, list[Binding]] = {s: [] for s in SECTIONS}
    for b in BINDINGS:
        out.setdefault(b.section, []).append(b)
    return out
