# App mappings

Each supported app gets eight on-screen keys (numbered clockwise from 12
o'clock) and its own meaning for the ring. All shortcuts are the apps'
**defaults on a US keyboard layout**. Keys are drawn by the EEZ project and
sent by `main/app_model.c` — the two lists must stay in the same order.

## Supported apps

| App | Ring turn | Ring push | Setup |
|---|---|---|---|
| [FreeCAD](#freecad) | rotate the view about X / Y / Z | cycle the axis | [add-on](#freecad-setup) |
| [Blender](#blender) | rotate the selection about X / Y / Z | cycle the axis | — |
| [Kdenlive](#kdenlive) | scrub 5 frames (or 1 second) | cut at the playhead | — |
| [VSCodium / VS Code](#vscodium) | scroll the code | lines ↔ pages | — |
| [GIMP](#gimp) | zoom in / out | fit image in window | — |
| [Inkscape](#inkscape) | zoom in / out | zoom to page | — |
| [LibreOffice](#libreoffice) | move 3 lines / a page | lines ↔ pages | — |
| [Firefox](#firefox) | scroll the page | lines ↔ pages | — |
| [Chromium / Google Chrome](#chromium) | scroll the page | lines ↔ pages | — |
| [anything else](#other-apps) | — | — | idle clock |

The companion decides which page to show from the focused window's X11
class; the matching rules are in [companion](companion.md#how-it-works).

## FreeCAD

| # | Key | Sends |
|---|---|---|
| 0 | Fit All | `V`, then `F` |
| 1 | Iso | `0` |
| 2 | Front | `1` |
| 3 | Top | `2` |
| 4 | Right | `3` |
| 5 | Hide/Show | `Space` |
| 6 | Undo | `Ctrl+Z` |
| 7 | Redo | `Ctrl+Y` |

| Ring | Does | Sends |
|---|---|---|
| Turn | Rotate the view about the active axis, 5°/detent | `F14`/`F15`/`F16` for X/Y/Z; with `Shift` for the other direction |
| Push | Cycle the active axis X → Y → Z | nothing (handled on the pad) |

FreeCAD has no keyboard command for "rotate the view N degrees", so the
ring needs the small **RotaryHid add-on** in [`freecad/`](../freecad/),
installed once — see [FreeCAD setup](#freecad-setup) below. The on-screen
keys work without it.

## Blender

| # | Key | Sends |
|---|---|---|
| 0 | Grab | `G` |
| 1 | Rotate | `R` |
| 2 | Scale | `S` |
| 3 | Edit Mode | `Tab` |
| 4 | Frame Sel | `Numpad .` |
| 5 | Camera | `Numpad 0` |
| 6 | Undo | `Ctrl+Z` |
| 7 | Render | `F12` |

| Ring | Does | Sends |
|---|---|---|
| Turn | Rotate the selection about the active axis, 5°/detent | `R` `<axis>` `[-]` `5` `Enter` — an exact, typed rotation |
| Push | Cycle the active axis X → Y → Z | nothing (handled on the pad) |

The rotation goes to whatever Blender considers active, so the mouse
pointer should be over the 3D viewport. Each detent is one undo step.

## Kdenlive

| # | Key | Sends |
|---|---|---|
| 0 | Play | `Space` |
| 1 | Set In | `I` |
| 2 | Set Out | `O` |
| 3 | Select | `S` |
| 4 | Razor | `X` |
| 5 | Spacer | `M` |
| 6 | Undo | `Ctrl+Z` |
| 7 | Render | `Ctrl+Enter` (shown as "Ctrl+Ret") |

| Input | Does | Sends |
|---|---|---|
| Turn | Scrub 5 frames per click, or one second in second mode | `→` ×5 / `←` ×5, or `Shift+→` / `Shift+←` |
| Push | Cut the clip at the playhead; adds a red mark to the strip | `Shift+R` |
| Tap the centre | Toggle the step between Frame and Second | nothing (handled on the pad) |

Kdenlive's arrow keys move exactly one frame and it has no "N frames"
shortcut, so frame mode presses the arrow several times per click. The
count is `CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT` (default 5; set 1 for
single-frame precision), and the pad's step pill shows it ("Step - 5
Frames"). The timecode and marks assume 30 fps (`CONFIG_ROTARY_KDENLIVE_FPS`)
and count from 0 at power-up.

## VSCodium

Also applies to Microsoft VS Code, which uses the same default keybindings.

| # | Key | Sends |
|---|---|---|
| 0 | Palette | `F1` (Command Palette) |
| 1 | Open | `Ctrl+P` (open a file by name) |
| 2 | Find | `Ctrl+F` |
| 3 | Go to Def | `F12` |
| 4 | Rename | `F2` (rename symbol) |
| 5 | Comment | `Ctrl+/` |
| 6 | Terminal | ``Ctrl+` `` (toggle the integrated terminal) |
| 7 | Save | `Ctrl+S` |

| Ring | Does | Sends |
|---|---|---|
| Turn | Scroll the editor 3 lines per click, without moving the cursor | `Ctrl+↓` ×3 / `Ctrl+↑` ×3 |
| Turn (page mode) | Scroll one page per click | `Alt+PgDn` / `Alt+PgUp` |
| Push | Toggle between line and page scrolling | nothing (handled on the pad) |

The lines per click are `CONFIG_ROTARY_VSCODIUM_LINES_PER_DETENT`
(default 3). The pad shows the current step in its pill ("Step - 3 Lines" /
"Step - Page"). Scrolling acts on the editor that has keyboard focus.

## GIMP

| # | Key | Sends |
|---|---|---|
| 0 | Move | `M` |
| 1 | Rect Sel | `R` (Rectangle Select) |
| 2 | Crop | `Shift+C` |
| 3 | Brush | `P` (Paintbrush) |
| 4 | Eraser | `Shift+E` |
| 5 | Text | `T` |
| 6 | Undo | `Ctrl+Z` |
| 7 | Redo | `Ctrl+Y` |

| Ring | Does | Sends |
|---|---|---|
| Turn | Zoom in / out, one step per click | `Keypad +` / `Keypad −` |
| Push | Fit the image in the window | `Ctrl+Shift+J` |

## Inkscape

| # | Key | Sends |
|---|---|---|
| 0 | Selector | `S` |
| 1 | Node | `N` |
| 2 | Rect | `R` |
| 3 | Ellipse | `E` |
| 4 | Pen | `B` (Bézier pen) |
| 5 | Text | `T` |
| 6 | Undo | `Ctrl+Z` |
| 7 | Redo | `Ctrl+Y` |

| Ring | Does | Sends |
|---|---|---|
| Turn | Zoom in / out, one step per click | `Keypad +` / `Keypad −` |
| Push | Zoom to fit the page | `5` |

Keypad `+`/`−` are used rather than the main-row keys because the main `+`
needs Shift on a US layout; both apps bind the keypad keys to zoom by
default. The pad shows no zoom level — it cannot know the app's actual zoom.

## LibreOffice

One page for Writer, Calc and Impress — these shortcuts are the same in
all three.

| # | Key | Sends |
|---|---|---|
| 0 | Bold | `Ctrl+B` |
| 1 | Italic | `Ctrl+I` |
| 2 | Underline | `Ctrl+U` |
| 3 | Find | `Ctrl+H` (Find & Replace) |
| 4 | Save | `Ctrl+S` |
| 5 | Print | `Ctrl+P` |
| 6 | Undo | `Ctrl+Z` |
| 7 | Redo | `Ctrl+Y` |

| Ring | Does | Sends |
|---|---|---|
| Turn | Move 3 lines per click | `↓` ×3 / `↑` ×3 |
| Turn (page mode) | Move one page per click | `PgDn` / `PgUp` |
| Push | Toggle between line and page steps | nothing (handled on the pad) |

LibreOffice has no key that scrolls the view without moving the cursor, so
the ring moves the **text cursor** in Writer and the **active cell** in
Calc, and the view follows. The lines per click are
`CONFIG_ROTARY_LIBREOFFICE_LINES_PER_DETENT` (default 3). VSCodium and
LibreOffice each remember their own line/page mode.

## Firefox

| # | Key | Sends |
|---|---|---|
| 0 | Back | `Alt+←` |
| 1 | Forward | `Alt+→` |
| 2 | Reload | `F5` |
| 3 | New Tab | `Ctrl+T` |
| 4 | Close Tab | `Ctrl+W` |
| 5 | Find | `Ctrl+F` |
| 6 | Sidebar | `Ctrl+B` (bookmarks sidebar) |
| 7 | History | `Ctrl+H` (history sidebar) |

## Chromium

Also applies to Google Chrome, which uses the same shortcuts.

| # | Key | Sends |
|---|---|---|
| 0 | Back | `Alt+←` |
| 1 | Forward | `Alt+→` |
| 2 | Reload | `F5` |
| 3 | New Tab | `Ctrl+T` |
| 4 | Close Tab | `Ctrl+W` |
| 5 | Bookmark | `Ctrl+D` (bookmark this page) |
| 6 | History | `Ctrl+H` (history page) |
| 7 | Download | `Ctrl+J` (downloads page) |

**The ring, in both browsers:**

| Ring | Does | Sends |
|---|---|---|
| Turn | Scroll the page 3 steps per click | `↓` ×3 / `↑` ×3 |
| Turn (page mode) | Scroll one screen per click | `PgDn` / `PgUp` |
| Push | Toggle between line and page steps | nothing (handled on the pad) |

Arrow keys scroll the page as long as no text field has focus — click on
the page itself first if the ring moves a text cursor instead. The steps
per click are `CONFIG_ROTARY_BROWSER_LINES_PER_DETENT` (default 3); each
browser remembers its own line/page mode. The ← / → on the Back and Forward
keys come from Font Awesome, merged into the shortcut font because Roboto
Mono has no arrows.

## Other apps

Any other focused window shows the idle screen — an analog clock with "No
supported app" — and the ring and keys send nothing. The clock gets the
time from the companion; see [GUI](gui.md#screens).

## About the numbers on the pad

The angle (FreeCAD, Blender) and the timecode (Kdenlive) are the pad's own
**tally of what it has sent**. Nothing comes back from the app, so moving
the view with the mouse or clicking the timeline makes them drift. They
reset when the pad restarts.

## FreeCAD setup

Install the add-on once per computer:

1. In FreeCAD, **File → Open…** the repository's
   `freecad/RotaryHid_Install.FCMacro`.
2. Press **Run** (▶), or **Macro → Execute macro**.

It copies the add-on into FreeCAD's `Mod` folder (so it loads at every
start) and switches it on immediately. Full walk-through, checks and
removal: [Linux setup, part 3](linux-setup.md#part-3--freecad-add-on).

**What it does.** It registers keyboard shortcuts on FreeCAD's main window
for each axis and direction:

| Pad sends | FreeCAD sees (X11) | Rotation |
|---|---|---|
| `F14` / `Shift+F14` | `Launch (5)` / `Shift+Launch (5)` | about world X, + / − |
| `F15` / `Shift+F15` | `Launch (6)` / `Shift+Launch (6)` | about world Y, + / − |
| `F16` / `Shift+F16` | `Launch (7)` / `Shift+Launch (7)` | about world Z, + / − |

On Windows and macOS the same keys arrive as plain `F14`–`F16`; the add-on
registers both names, so it works unchanged there. Each shortcut orbits the
camera about the chosen *world* axis through the current focal point, so
the model appears to turn in place.

**Why an add-on and not macros bound in Tools → Customize.** On X11,
F14–F16 arrive as `XF86Launch5`–`7`, which Qt names "Launch (5)"… FreeCAD
stores shortcuts with spaces removed, turning that into "Launch(5)", which
Qt cannot parse — so those keys cannot be bound through FreeCAD's own
shortcut settings at all. The add-on registers them with Qt directly.

Why F14–F16 and not F13: on X11, F13 arrives as `XF86Tools`, which GNOME
grabs to open Settings. F14–F16 are left alone by a stock desktop.

**Rotation step.** 5° per click by default. To change it: **Tools → Edit
parameters**, `BaseApp → Preferences → Mod → RotaryHid`, add a float
`StepDeg`. Set the firmware's `CONFIG_ROTARY_STEP_DEG` to the same value so
the angle on the pad matches the view.

**No-install alternative.** FreeCAD's own **Shift+←/→** rotates the view
90° about the screen axis. It needs nothing installed, but it is a quarter
turn per click about one axis only, so the pad does not use it.
