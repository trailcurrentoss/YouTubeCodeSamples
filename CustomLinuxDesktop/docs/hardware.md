# PocketTerm35 hardware notes

Everything here was measured on the device rather than assumed, because most of
it drives a design decision in the shell.

## Panel

| | |
|---|---|
| Model | Waveshare 3.5" DPI LCD (`dtoverlay=waveshare-35dpi-5b`) |
| Resolution | 640 x 480, 4:3 |
| Active area | ~71.1 x 53.3 mm |
| Density | **~228 PPI** (9.0 px/mm) |
| Connection | DPI via `vc4-kms-v3d`, appears to X as `HDMI-1` |
| Backlight | **No `/sys/class/backlight` node** |

228 PPI on 640 x 480 is the single most important fact about this device. It
means naive 96 DPI UI is physically microscopic, *and* that there are very few
pixels to spend once things are sized correctly. Every layout number in
`trailshell/theme.py` is derived from millimetres and this density.

`xrandr` reports the physical size as `640mm x 480mm`, which is wrong (it is
the pixel count in millimetres). Do not trust it — `theme.PX_PER_MM` is the
authority.

### Backlight

There is no kernel backlight device. The real backlight is controlled by the
Pico keyboard firmware via its own function keys (`Fn +` / `Fn -`). The shell
therefore **does not** try to own it. What the shade offers instead is a
software *dim* layer (`xrandr --brightness`, an X gamma ramp) that composes on
top of the hardware backlight and can go darker than the firmware's minimum.

## Touch

| | |
|---|---|
| Device | Goodix Capacitive TouchScreen (i2c `1-005d`) |
| Bus/IDs | `0018:0416:038f` |
| Properties | `INPUT_PROP_DIRECT` — a true touchscreen, not a touchpad |
| Multitouch | Yes: `ABS_MT_SLOT`, `ABS_MT_POSITION_X/Y`, `ABS_MT_TRACKING_ID` |
| Driver | `xf86-input-libinput` |

Because it is a real multitouch direct device, X delivers XI2 touch events and
GTK3 turns them into `GdkEventTouch`. That is what makes
`Gtk.ScrolledWindow.set_kinetic_scrolling(True)` give genuine iOS/Android-style
flick-and-glide scrolling, and it is why the shade and launcher are built from
GTK widgets rather than drawn with Cairo.

Any widget that wants a horizontal swipe must use `Gtk.GesturePan` with
`Orientation.HORIZONTAL`, not `Gtk.GestureDrag` — otherwise it swallows
vertical flicks and breaks scrolling. See `shade.NotifCard`.

Real touch events are necessary but not sufficient. GTK's `kinetic-scrolling`
still loses the sequence to any child that handles button events, and does
nothing at all for a pointer drag, so the shell does its own drag-scrolling in
`ui.attach_drag_scroll()`. Do not re-enable `set_kinetic_scrolling(True)`
alongside it — they will both move the adjustment.

## Keyboard

A Pico-based USB HID device (`My Company My Custom Pico Keyboard`), presenting
a standard `pc105` / `us` XKB layout with Super on `mod4`.

**There is no on-screen keyboard and there should never be one.** This inverts
the usual phone-shell assumption: the keyboard is the primary input and touch
is the convenient secondary. Consequences that shaped the design:

- The launcher's search field is focused on open, so typing filters instantly.
- Every touch affordance has a key equivalent — see `trailshell/keymap.py`.
- The dedicated **START** and **SELECT** buttons emit `Pause` and `Print` — the
  only two keys on this keyboard that send something a person would never type.
  The shell binds them to Home and Recents.
- The gamepad cluster sends plain `a b x y l r` and the arrow keys, so **none
  of it can be bound** without changing the firmware.
- `Fn +`/`Fn -` (backlight) and `Fn e`/`Fn r` (volume) are handled *entirely
  inside the keyboard firmware* as PWM outputs and **never reach Linux at
  all** — no keysym is sent, so there is nothing for the shell to bind.

The firmware was read off the device and is fully documented in
[keyboard-firmware.md](keyboard-firmware.md), including the complete key
matrix and which keycodes are free (`F13`-`F24`).

## Consequences of a 640x410 app canvas

Some applications declare a minimum window size larger than the canvas and will
not go below it. Measured on this device:

| Window | Size | Minimum |
|---|---|---|
| GIMP main window | 640x503 | clamps at 503 tall; refuses 410 |
| GIMP "Welcome to GIMP 3.0.4" | 653x684 | larger than the whole 640x480 screen |
| lxterminal | 639x408 | resizes only in 8x17 character cells |
| Chromium | 640x414 | fits exactly |

Two lessons are baked into `trailshell/wm.py`:

* Respect `WM_NORMAL_HINTS` increments, or terminals cause an endless resize
  war (measured: 47 re-placements before the fix, 1 after).
* When a client refuses to shrink, stop asking and make it pannable instead.
  Panning must move the **WM frame** directly — GIMP re-centres its dialog when
  the window manager moves it, so `_NET_MOVERESIZE_WINDOW` is silently undone.

Raising `Xft.dpi` for applications makes this worse, because minimum sizes scale
with the font. The session defaults to 144 and it is configurable via `app_dpi`
in `~/.config/trailshell/settings.json`; lowering it makes more apps fit at the
cost of smaller text.

## Power

`upower` exposes only the synthetic `DisplayDevice`, reporting
`power supply: no` and a `battery-missing` icon: **there is no battery**. The
status bar hides the battery indicator entirely rather than showing 0%. The
detection in `providers.Power` is written so a battery added later is picked up
automatically.

## Audio

PipeWire + WirePlumber; default sink is `Built-in Audio Digital Stereo (HDMI)`.
Controlled with `wpctl`.

**There are two independent volumes.** `Fn`+`E`/`Fn`+`R` drive an analogue
level on the keyboard's GP18 PWM, ahead of the amplifier; PipeWire's volume is
separate and the two multiply. The shade's slider controls only the PipeWire
one, because the hardware one is not visible to Linux. Note `wpctl` writes RTKit warnings to stderr on this
machine — `providers.run_async` uses `STDERR_SILENCE` so they never reach the
log.

## Boot configuration

The overlays already present in `/boot/firmware/config.txt` that matter:

```
dtoverlay=vc4-kms-v3d
dtoverlay=waveshare-35dpi-5b     # the panel
dtoverlay=dwc2,dr_mode=host      # USB host, for the keyboard
dtparam=i2c_arm=on               # the Goodix touch controller
max_framebuffers=2
disable_fw_kms_setup=1
disable_overscan=1
```

TrailShell requires **X11**, not Wayland. The touchscreen and keyboard are
working under X on this device and the shell is built against X11 EWMH
(struts, `_NET_CLIENT_LIST`, `XGrabKey`); do not switch the session to labwc.
