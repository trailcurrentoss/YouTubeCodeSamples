# LinuxTouch architecture

Three processes replace the Raspberry Pi OS desktop session.

```
       ┌──────────────────────────────────────────────┐
       │  sway  (wlroots compositor)                  │
       │  · every window fullscreen, no borders       │
       │  · binds Pause + Print, nothing else         │
       │  · IPC socket: window tree, focus, close     │
       └───┬──────────────────────┬───────────────────┘
           │ Wayland              │ Wayland / XWayland
   ┌───────▼────────┐   ┌─────────▼──────────────────┐
   │ shell          │   │ Thunderbird, Chromium,     │
   │ (Chromium app) │   │ Geany, our FreeCAD viewer… │
   └───────┬────────┘   └────────────────────────────┘
           │ WebSocket (127.0.0.1:8720)
   ┌───────▼─────────────────────────────────────────┐
   │ touchd  — asyncio daemon                        │
   │  wm/sway.py │ apps │ net │ audio │ brightness   │
   │             │      │ nmcli│wpctl │ gammastep    │
   └─────────────────────────────────────────────────┘
```

## Why the shell is a web app and not a compositor plugin

The shell is an ordinary unprivileged Chromium window that talks to `touchd` over a
WebSocket. It has no special access to the compositor and asks the daemon for
everything.

That buys three things:

1. **You can develop it on a laptop.** `make mock` serves it in a 640×480 frame against
   a fake daemon. Most of the UI work never touches hardware.
2. **Animation is free.** iOS-grade page transitions and card physics are CSS, not
   hand-written interpolation against a scene graph.
3. **First-party apps use the same stack.** The read-only FreeCAD viewer is a web app in
   its own Chromium `--app` window — see [apps.md](apps.md).

The cost is roughly 250 MB resident for the shell's Chromium process, which is
affordable on an 8 GB Pi 5 and would not be on a Pi Zero.

## Why sway, and how to swap it

**The switcher is what picks the compositor.** To show cards for running apps, focus
one, and close another, the shell needs to enumerate toplevel windows and map them back
to the processes it launched.

| | sway | labwc |
|---|---|---|
| Window list with **PID** per window | `swaymsg -t get_tree` | not available |
| Live window events | `swaymsg -t subscribe '["window"]'` | not available |
| Focus / close a specific window | `[con_id=N] focus` / `kill` | via `wlr-foreign-toplevel` only |
| Force every window fullscreen | `for_window` rule | window rules in `rc.xml` |
| Global keybind above a focused app | `bindsym` | `<keybind>` |

labwc is the Raspberry Pi OS default and is already installed, which is a real
argument. But it has no IPC: the same features mean writing a `wlr-foreign-toplevel`
protocol client, and that protocol carries **no PID**, so "which window did my launch
produce?" becomes guesswork against app IDs and titles. sway hands that over as JSON.

To keep this from being a one-way door, every compositor call in `touchd` goes through
one interface — [`touchd/wm/base.py`](../touchd/wm/base.py):

```python
class WindowManager:
    async def windows(self) -> list[Window]: ...
    async def focus(self, wid: str) -> None: ...
    async def close(self, wid: str) -> None: ...
    async def subscribe(self, cb) -> None: ...
```

`wm/sway.py` implements it in about 150 lines. A `wm/labwc.py` would be a drop-in
replacement, and nothing above the adapter knows which compositor is running.

## Why not Phosh, Plasma Mobile, or cage

- **Phosh / Plasma Mobile** are built around a portrait phone screen and a
  touch keyboard. This device is landscape 640×480 with a physical keyboard — the two
  assumptions that shape those shells are both wrong here, and fighting them costs more
  than the app grid is worth.
- **cage** hosts exactly one fullscreen client with no window management at all. It is
  the right choice for a single-purpose kiosk — which is why
  [Tracer](../../../Product/TrailCurrentTracer) uses it — and it cannot express "home
  screen plus switchable apps".

## touchd

Python 3.13 + `aiohttp`. One process, no threads beyond asyncio, no database.

- **Serves the shell bundle** over HTTP on `127.0.0.1:8720`, so the shell has an origin
  and `fetch()` works normally.
- **WebSocket at `/ws`** carrying JSON:
  - request: `{"id": 7, "method": "apps.launch", "params": {"id": "geany.desktop"}}`
  - reply: `{"id": 7, "ok": true, "result": {...}}`
  - push: `{"event": "wm.windows", "data": [...]}`
- **Modules** own one subsystem each and are the only thing that shells out:

  | Module | Backs onto |
  |---|---|
  | `apps` | XDG `.desktop` discovery, `gio launch` |
  | `wm` | the adapter above |
  | `net` | `nmcli` |
  | `audio` | `wpctl` (PipeWire) |
  | `brightness` | `gammastep -b` (no kernel backlight exists) |
  | `power` | `systemctl poweroff` / `reboot`, both requiring `confirm` |
  | `system` | model, uptime, temp, memory, disk |
  | `settings` | one JSON file in `/var/lib/linuxtouch` |

  The rule is that **the shell never shells out and never parses command output.** If a
  new capability needs a subprocess, it belongs in a module.

## Global navigation, end to end

There is no evdev grabbing anywhere in LinuxTouch. The path is:

```
Start pressed → RP2040 sends KEY_PAUSE → sway matches `bindsym Pause`
   → sway runs `linuxtouch-nav home`
   → that POSTs to touchd
   → touchd focuses the shell window via the WM adapter
   → touchd pushes {"event": "nav", "data": {"action": "home"}}
   → the shell shows the home screen
```

The compositor is the only component that sees raw keys, which is precisely why
letters stay available to applications. See [controls.md](controls.md).

## What runs at boot

| What | Started by | Notes |
|---|---|---|
| sway | lightdm, via `/usr/share/wayland-sessions/linuxtouch.desktop` | A real logind session — see below |
| `touchd.service` | sway `exec_always` | A systemd **user** unit, started after the environment import |
| shell | sway `exec_always` | Chromium `--app`, inside a supervision loop |

**sway does not respawn `exec` children.** labwc has `lwrespawn` for this; sway
has no equivalent, so `bin/linuxtouch-shell` wraps Chromium in its own restart
loop. Without it, a Chromium crash leaves the user looking at the wallpaper with
no way back except SSH. The same script also kills any previous shell using its
profile first, because `exec_always` fires again on every `swaymsg reload` — and
`make deploy` reloads sway, which otherwise leaves two shells tiled side by side
with neither fullscreen.

**Why a display-manager session rather than a systemd system unit.** It looks
tidier to run sway from a system unit with `User=` and seatd, the way a kiosk
usually does. That costs more than it saves here, because LinuxTouch changes
network settings and powers the device off, and both are polkit-gated. polkit
grants those to a user with an *active local session* — which a system unit is
not. As a system service, `nmcli` fails with "not authorized" and
`systemctl poweroff` waits on a password prompt the device can never show. Going
through lightdm makes the session active and local, so both simply work, with no
custom polkit rules to write or maintain.

**Why sway starts touchd rather than `WantedBy=graphical-session.target`.** A
systemd user unit does not inherit the compositor's environment. Started from
the target, touchd comes up with no `SWAYSOCK` and no `WAYLAND_DISPLAY`, and the
result is two symptoms that look nothing like an environment problem: the app
switcher is permanently empty (every `swaymsg` call fails), and tapping a grid
icon does nothing (the launched app has no compositor to connect to and exits
immediately). Both were observed on hardware before `wm/sway/config` gained its
`systemctl --user import-environment` line. touchd is still its own unit rather
than a child of sway, so a shell crash leaves the network and power controls up.

**Identifying the shell's own window.** Chromium ignores `--class` on Wayland
and names its window after the URL it was given
(`chrome-127.0.0.1__-Default`). Rather than spread that fragile string around,
`wm/sway/config` applies a `linuxtouch_shell` **mark** to it in one place, and
touchd matches the mark. Without this, "Home" would silently focus nothing, and
the home screen would appear as a closeable card in its own switcher.
