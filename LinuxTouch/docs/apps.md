# Applications

Two kinds of application run on LinuxTouch, and the difference matters only to
whoever is writing them. To the user, and to the switcher, they are identical:
fullscreen windows that Start and Select navigate between.

---

## 1. Ordinary Debian applications

Mail clients, browsers, editors, viewers — anything with a `.desktop` file.

**Nothing is required to make one appear.** `apt install thunderbird` puts
Thunderbird on the home screen at the next `apps.rescan`, with its own icon from
whatever theme provides it. There is no whitelist, no manifest, and no per-app
support code anywhere in this repository. That is what makes "full Linux" a
literal claim rather than a marketing one.

What LinuxTouch does to them:

| | |
|---|---|
| **Fullscreen** | A sway `for_window` rule forces it, Wayland and XWayland alike. No title bar, no borders. |
| **Touch** | libinput delivers real touch events; GTK3/4 and Qt5/6 handle them natively, including kinetic scrolling. X11 apps get pointer emulation through XWayland. |
| **Keyboard** | Every key reaches the app verbatim, including A/B/X/Y/L/R. LinuxTouch grabs only `Pause` and `Print`. |
| **Dialogs** | Open floating and centred at 560×380 rather than fullscreen, so a confirmation is visibly attached to something. |

### The honest limitation

A 640×480 screen is smaller than any desktop application was designed for. Menus
will be dense, some dialogs will need scrolling, and a few apps with fixed
minimum window sizes will be clipped. LinuxTouch does not pretend otherwise and
does not try to rescale them, because a downscaled app is an unreadable app on a
3.5" panel.

Where an application is genuinely unusable, the answer is a first-party app that
does the one thing that was needed — which is what the next section is for.

### Console programs

`Terminal=true` entries are launched inside `x-terminal-emulator` (Debian's
alternatives symlink — `lxterminal` on this image), so `btop`, `htop`, and
friends are grid apps like anything else.

---

## 2. First-party LinuxTouch apps

For things no desktop application does well at this size. The read-only FreeCAD
viewer is the first of these.

**They are web apps in their own Chromium window.** Not screens inside the
shell — separate windows, so they appear in the switcher, can be closed, and
cannot take the home screen down with them when they crash.

### Why a web app rather than GTK or Qt

- It reuses `shell/css/shell.css`, so a first-party app looks like part of the
  system without re-implementing the design.
- It can be developed against `make mock` on a workstation.
- For a 3D viewer specifically, the browser is where the mature libraries are.

### The pattern

1. Put the app under `apps/<name>/` with its own `index.html`.
2. Serve it from touchd by adding the directory to the static route, or ship it
   as its own directory under `/usr/share/linuxtouch/apps/`.
3. Add a `.desktop` file so it appears on the grid like anything else:

```ini
[Desktop Entry]
Type=Application
Name=CAD Viewer
Comment=Read-only STEP and FCStd viewer
Icon=applications-engineering
Exec=linuxtouch-app cad-viewer
Categories=Graphics;Viewer;
```

4. `linuxtouch-app` is a thin wrapper around the same Chromium invocation as
   `bin/linuxtouch-shell`, pointed at the app's URL with a distinct
   `--user-data-dir`.

Because it launches through a `.desktop` file, it is discovered, hidden, and
launched by exactly the same code path as Thunderbird. There is no "first-party"
branch in `modules/apps.py`, and there should never be one.

### Notes for the FreeCAD viewer specifically

Read-only viewing does not need FreeCAD. FreeCAD itself is a large Qt
application whose UI is unusable at 640×480, and its Python API is not something
to run on the critical path of a touch UI.

The tractable shape is a browser-side viewer: parse STEP with a WASM OCCT
importer, render with WebGL, and drive the camera from touch gestures — one
finger to orbit, two to pan and zoom. Model files come from the filesystem via a
touchd method rather than a file input, since there is no file picker worth
using at this size.

That is a design sketch, not a built thing. Nothing in this repository
implements it yet.

---

## Hiding apps

61 `.desktop` entries were present on the stock image, and a good number of them
are Raspberry Pi OS configuration tools that make no sense on a handheld.
**Settings → Apps** toggles any entry's visibility on the grid. It hides, it does
not uninstall: the app stays on the system, and the hidden list is stored in
`settings.json` under `hidden_apps`.
