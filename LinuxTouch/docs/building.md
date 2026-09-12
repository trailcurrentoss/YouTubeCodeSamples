# Building and installing

Two paths, in the order you will actually want them.

---

## 1. Provisioning a running board

This is the development path and the one that works today. It converts a stock
Raspberry Pi OS install into a LinuxTouch device without reflashing anything.

**Requirements:** a PocketTerm35 with a Pi 5, Raspberry Pi OS Trixie 64-bit, and
SSH access.

```bash
make dev-env      # asks for the board's address; writes gitignored scripts/dev.env
make deploy       # rsync the tree, run provision.sh on the board, restart touchd
```

`scripts/provision.sh` is idempotent — re-running it is the intended way to pick
up changes. It:

| Step | What it does |
|---|---|
| Checks | Pi 5, trixie, and **at least 300 MB free** (refuses below that) |
| Installs | `sway`, `python3-aiohttp`, `gammastep`, `device-tree-compiler`, `curl` |
| Overlay | Compiles `linuxtouch-gt911.dtbo` and switches `config.txt` to it (backing the file up first) |
| Files | Shell and daemon to `/usr/share/linuxtouch`, helpers to `/usr/local/bin`, sway config to `/etc/linuxtouch` |
| System | `kernel.sysrq=0`, logind power-key handling, the wayland-session entry |
| Session | A lightdm drop-in making LinuxTouch the autologin session |

Then reboot. **The overlay change and the logind drop-in only take effect at
boot.**

### If the screen stays black

You still have SSH. The rollback is one line, and provision.sh prints it:

```bash
sudo sed -i 's/^autologin-session=.*/autologin-session=labwc/' \
    /etc/lightdm/lightdm.conf.d/60-linuxtouch.conf && sudo reboot
```

The stock Raspberry Pi desktop is never removed — LinuxTouch is installed
*alongside* it as an additional session, and that line switches back.

### Faster iteration

```bash
make deploy-shell   # replaces only the web bundle; about a second
make logs           # follow touchd's journal on the board
```

For UI work that needs no hardware at all, `make mock` serves the shell against
fixtures on your workstation. Resize the window to exactly 640×480 — the shell
has no responsive layout, deliberately.

### Do not run this while `apt` is busy

provision.sh installs packages, so it will fail on the dpkg lock if an
`apt upgrade` or PackageKit run is in flight. It reports that clearly rather than
forcing the lock. Wait for the other job to finish.

---

## 2. A flashable image

**Not built yet.** This section is the plan, not a description of something that
exists.

The toolchain will be **`rpi-image-gen`**, not pi-gen — TrailCurrent already has
a mature rpi-image-gen setup, and
[TrailCurrentTracer](../../../Product/TrailCurrentTracer/docs/building.md) proved
it out on this exact hardware: declarative layer YAML, a Docker-less
reproducible build, bake-time verification, first-boot hooks, and a splash
pipeline. Upstream ships a trixie minbase targeting rpi5, which is what
LinuxTouch needs. Choosing pi-gen would mean rebuilding that groundwork.

Pin the checkout. Tracer pins `RPIIG_REF` deliberately, because upstream changes
its own interface (`-D` became `-S`) and following main breaks the build on
someone else's schedule.

The layer has to carry, at minimum:

- the `config.txt` block from [hardware.md](hardware.md#boot-configuration-as-shipped),
  including `dtoverlay=dwc2,dr_mode=host` and `dtparam=cooling_fan=on` with its
  fan curve — without the latter the Active Cooler is not detected on this
  chassis and the board idles at 66–70 °C
- `linuxtouch-gt911.dtbo` and the `dtoverlay=` line
- everything `provision.sh` installs
- `console=tty1` **removed** from `cmdline.txt`, so kernel output does not scroll
  across the panel during boot
- a 640×480 splash, rather than the 1920×1080 one the upstream splash layer
  assumes

Output should be `.img.xz` plus a `.sha256`.

---

## What is verified, and what is not

Measured on hardware on 2026-09-04:

- ✅ The overlay compiles and the GT911 comes up at `0x5d` as `event7`, `PROP=2`
- ✅ touchd serves, discovers applications, and answers RPC — 34 apps found on
  the stock image after filtering
- ✅ The shell renders at 640×480 with real icons from the system icon themes
- ✅ sway's `for_window` fullscreen rule fires (`fullscreen_mode=1`, filling the
  output)
- ✅ Chromium launches with the shell's flag set

Found and fixed during that session, all of which would have been black-screen
or dead-button bugs on a deployed unit:

- `window_type` is an enum, not a regex — `[window_type="dialog|menu"]` made the
  criteria empty and sway then refused to load the entire config
- Chromium ignores `--class` on Wayland, so the shell is identified by a sway
  mark instead
- Chromium's `--start-fullscreen` raced sway's rule and left the shell *not*
  fullscreen; sway owns fullscreen now
- a systemd user unit inherits neither `SWAYSOCK` nor `WAYLAND_DISPLAY`, so sway
  must `import-environment` before starting touchd

Then verified again on **the real seat** (lightdm → sway owning the display),
2026-09-05:

- ✅ sway runs as the session at `HDMI-A-1 640x480`, no desktop panel anywhere
- ✅ The shell is a single window, `fullscreen=1`, `rect 640×480`, carrying the
  `linuxtouch_shell` mark
- ✅ touchd starts from sway's exec chain and answers `/healthz`
- ✅ **sway sees the touchscreen**: `1046:911:Goodix_Capacitive_TouchScreen
  type=touch`
- ✅ End to end: `apps.launch` → the app maps fullscreen and focused →
  `wm.windows` reports it with its PID, shell correctly excluded → `linuxtouch-nav
  home` (what the `Pause` keybind runs) returns the shell to fullscreen with the
  app dropped to the background

A further four bugs were found and fixed on the real seat, each invisible in
nested testing:

- LightDM reads `/etc/lightdm/lightdm.conf` **after** `lightdm.conf.d/`, so the
  main file wins — the drop-in provisioning wrote was silently ignored and the
  board booted straight back into the stock desktop
- sway fires `exec_always` lines **concurrently**, so starting touchd raced the
  environment import; they are one chained shell command now
- sway does **not** respawn `exec` children, and `exec_always` re-fires on every
  `swaymsg reload` — together these produced duplicate shells, then a runaway
  that opened 34 windows, because a relaunched Chromium hands off to the
  instance owning the profile and exits instantly
- only one window per workspace can be fullscreen in sway, so a launched app
  revokes the shell's — `focus` now always re-asserts `fullscreen enable`

Still unverified:

- ⏳ **Touch actually driving the UI.** The compositor sees the device; whether
  taps land on the right targets needs a finger, not SSH.
- ⏳ `Pause`/`Print` **as physical key presses** while an app holds focus. The
  command they run is verified; the binding itself needs the buttons pressed.
- ⏳ `gammastep -b` as a brightness control under sway
- ⏳ Hit-target usability inside real mail clients and editors at this size
- ⏳ Boot time
