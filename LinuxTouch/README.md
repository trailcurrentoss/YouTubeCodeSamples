# LinuxTouch

A touch-first Linux shell for the [Waveshare PocketTerm35](https://docs.waveshare.com/PocketTerm35)
— a Raspberry Pi 5 handheld with a 3.5" 640×480 optically-bonded touch panel and a
67-key QWERTY keyboard.

The goal is the Ubuntu Touch / postmarketOS idea applied to Raspberry Pi OS: **full
Debian underneath, phone-shaped on top.** Not a kiosk running one app, and not a
scaled-down desktop with 12 px checkboxes. A home screen with an icon grid, one app
fullscreen at a time, a card switcher, and everything reachable by thumb.

Ordinary Debian applications are first-class citizens here. Mail clients, browsers,
and text editors are discovered from their `.desktop` files and launched fullscreen —
there is no curated whitelist, and nothing is recompiled.

> **Status: in development.** Every claim in [docs/hardware.md](docs/hardware.md) is
> measured on a live unit and says so. Everything else is marked as a target until it
> has run on hardware. Nothing here is a finished product yet.

---

## What it looks like

```
┌────────────────────────────────────────┐  640×480
│ 09:41        LinuxTouch        ▂▄ 🔊   │  status bar (28 px)
├────────────────────────────────────────┤
│    ▢         ▢         ▢         ▢     │
│  Files    Browser    Mail     Editor   │  paged icon grid
│                                        │  4 × 3 = 12 per page
│    ▢         ▢         ▢         ▢     │
│  Terminal  Settings   Music    Photos  │
│                                        │
│    ▢         ▢         ▢         ▢     │
│   CAD      Calc      Clock     Notes   │
├────────────────────────────────────────┤
│              ● ○ ○                     │  page dots
└────────────────────────────────────────┘
   Start = Home        Select = Back
```

## How it is put together

Three processes replace the Raspberry Pi OS desktop session.

| Piece | What it is | Job |
|---|---|---|
| **sway** | wlroots compositor, every window forced fullscreen, no borders | Hosts real apps. Owns the two global nav keys. |
| **touchd** | Python asyncio daemon, WebSocket API on localhost | Single source of truth for apps, windows, network, audio, power. Also serves the shell bundle. |
| **shell** | Chromium `--app` window loading that bundle | The home screen, switcher, and settings. Just another fullscreen window. |

The shell is deliberately *not* privileged and *not* a compositor plugin. It is a web
app that asks `touchd` for everything, which means you can develop it in a desktop
browser at 640×480 with `make mock` and never touch the hardware until the end.

See [docs/architecture.md](docs/architecture.md) for the reasoning, including why the
compositor is sway rather than labwc and how to swap it.

## Controls

The device has a real QWERTY keyboard, so **there is no on-screen keyboard anywhere in
LinuxTouch** — a D-pad-walked letter grid is strictly slower than typing. See
[docs/controls.md](docs/controls.md).

Global navigation is bound at the compositor level to exactly two keys:

| Key | Reports as | Action |
|---|---|---|
| **Start** | `KEY_PAUSE` → `Pause` | **Home** — and again from home, the switcher |
| **Select** | `KEY_SYSRQ` → `Print` | **Back** — leave the app, it keeps running |

Those two are the only buttons on this hardware that carry no character. A/B/X/Y/L/R
are the literal letter keys, so binding them globally would make it impossible to type
the letter "a" in a mail client. They are never grabbed. This is the single most
important lesson carried over from
[TrailCurrentTracer](../../Product/TrailCurrentTracer/docs/controls.md), where it was
learned the hard way.

## Getting started

You need a PocketTerm35 with a Pi 5, running Raspberry Pi OS (Trixie, 64-bit), and
SSH access to it.

```bash
# 1. Point the dev scripts at your board. This writes scripts/dev.env,
#    which is gitignored — your board's address stays on your bench.
make dev-env

# 2. Install LinuxTouch on the board. Idempotent; safe to re-run.
#    Prints the privileged commands for you to run rather than running them.
make provision

# 3. Push shell + daemon changes and restart, without reflashing anything.
make deploy
```

To work on the UI without hardware, `make mock` serves the shell in a 640×480 frame
against a fake daemon.

[docs/building.md](docs/building.md) covers both paths: the provisioning script above,
and the `rpi-image-gen` build that turns the same layers into a flashable `.img.xz`.

## Repository layout

```
docs/           Verified hardware findings, controls, architecture, app authoring
overlays/       GT911 device-tree overlay (vendor -5b, minus the phantom 0x14 node)
wm/sway/        Compositor config: fullscreen rules, global nav keybinds
touchd/         The daemon. WM adapter + one module per subsystem.
shell/          The web shell. Status bar, grid, switcher, settings screens.
systemd/        Session, daemon, and shell units
scripts/        provision.sh, deploy, dev.env handling
image/          rpi-image-gen layers for a flashable image
```

## Prior art and credit

- **Hardware layer** — the GT911 overlay, the `config.txt` block, the button keymap,
  and the boot-trim approach are ported from
  [TrailCurrentTracer](../../Product/TrailCurrentTracer) (MIT, same author), where they
  were verified on this same panel. Tracer is a single-purpose field tool built on
  `cage`; LinuxTouch is a general-purpose shell, which is why the compositor and the
  input model differ.
- **Waveshare** — [PocketTerm35 documentation](https://docs.waveshare.com/PocketTerm35)
  and the `3.5HDMI_E_DTBO` overlay archive.
- [`pocketterm35-kali`](https://github.com/darkLabz001/pocketterm35-kali) — independent
  confirmation that the GT911 answers at `0x5d`, not `0x14`.

## License

MIT — see [LICENSE](LICENSE).
