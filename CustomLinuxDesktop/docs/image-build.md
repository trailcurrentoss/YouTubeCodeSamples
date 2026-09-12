# Notes for building a Raspberry Pi image

Collected so the working configuration can be reproduced rather than
rediscovered.

## Base

Raspberry Pi OS (Debian 13 "trixie"), **64-bit**, Pi 5. Verified on
`Linux 6.18.39+rpt-rpi-2712`, Python 3.13.

## Packages

`packages.runtime` is the authoritative list and is read directly by
`install-deps.sh`, so the image build should read the same file rather than
keeping its own copy:

```sh
apt-get install -y --no-install-recommends \
  $(grep -vE '^\s*(#|$)' packages.runtime | sed 's/[[:space:]]*#.*//')
```

`packages.dev` is for development only and must **not** go into the image.

The one package that is easy to miss and fails confusingly is
**`python3-gi-cairo`**. Without it every `DrawingArea` raises
`TypeError: Couldn't find foreign struct converter for 'cairo.Context'` at draw
time, which looks like a shell bug rather than a missing dependency.

## Boot overlays

Required in `/boot/firmware/config.txt` — see `docs/hardware.md` for why:

```
dtparam=i2c_arm=on
dtoverlay=vc4-kms-v3d
dtoverlay=waveshare-35dpi-5b
dtoverlay=dwc2,dr_mode=host
max_framebuffers=2
disable_fw_kms_setup=1
disable_overscan=1
display_auto_detect=1
```

## Session

`install.sh` places:

| Path | Contents |
|---|---|
| `/usr/local/lib/trailshell/trailshell/` | the Python package |
| `/usr/bin/trailshell` | debug launcher (runs over an existing session) |
| `/usr/bin/trailshell-session` | session entry point used by the greeter |
| `/usr/share/trailshell/openbox-rc.xml` | window-manager policy |
| `/usr/share/xsessions/trailshell.desktop` | the selectable session |

For an image you almost certainly want TrailShell to be the default:

```sh
./install.sh --set-default
```

which sets `user-session=trailshell` under `[Seat:*]` in
`/etc/lightdm/lightdm.conf`, backing up the original first.

**Keep the stock Raspberry Pi desktop installed in the image.** It costs little
and is the recovery path if the shell ever fails to start on a new panel
revision — the greeter's session menu is the escape hatch.

## Display manager

LightDM with `lightdm-gtk-greeter`. The greeter itself is not styled for this
panel yet; its default text is small at 228 PPI. Either theme it or enable
autologin in the image.

## Things that must stay X11

The shell uses EWMH struts, `_NET_CLIENT_LIST_STACKING` and `XGrabKey`. It will
not run on Wayland/labwc, and — more importantly — the panel and touchscreen
are known-good under X on this hardware.
