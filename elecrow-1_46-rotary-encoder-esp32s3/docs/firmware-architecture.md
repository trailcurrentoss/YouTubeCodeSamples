# Firmware architecture

```
components/crowpanel_board/ hardware: panel, touch, ring, LED ring, backlight, LVGL setup
components/rotary_usb/      USB: HID keyboard + serial (companion protocol, log)
main/app_model.c            behaviour: focused app, axis, tallies, input -> keystrokes
main/clock.c                time of day for the idle clock (companion sync + esp_timer)
main/ui_glue.c              drives the EEZ screens from app_model
main/actions.c, vars.c      the C half of the EEZ project's actions and variables
main/main.c                 boot order and wiring
main/ui/                    EEZ Studio's export -- generated, never edited
```

The layers only call downward: `main` knows about the board and USB
components; neither component knows about the other or about the UI.

## Boot order

1. **NVS.**
2. **`crowpanel_board_init()`** — backlight PWM (dark), LVGL task, panel
   rails + JD9855 panel, CST816D/T touch, ring. Touch failing to start is logged and
   tolerated; the ring can still drive the pad.
3. **UI**, under the display lock — `ui_init()`, `ui_glue_init()` (dark
   theme, idle screen, 250 ms status timer) and a 50 ms `lv_timer` calling
   `ui_tick()`.
4. **`rotary_usb_init()`** — TinyUSB comes up and the log is mirrored onto
   the USB serial port. It starts after the UI because the companion can
   send a focus change the moment the port opens.
5. **Backlight on**, so the panel never shows an unpainted frame.

## Threads and locking

| Context | Runs | Holds display lock |
|---|---|---|
| LVGL task (core 1) | rendering, touch & ring reads, ring callbacks, LVGL events, `lv_timer`s | yes, always |
| TinyUSB task | USB stack, serial RX → `on_focus()` / `on_time()` in `main.c` | takes it for the model / clock update |
| `usb_hid` task | types queued keystrokes | never touches LVGL |

Everything in `app_model.c` and `ui_glue.c` therefore runs with the lock
held. Anything that wants to call into them from another task takes
`crowpanel_board_lock()` first, as `on_focus()` does.

## Input flow

```
ring turn ─► PCNT ─► board_encoder.c ─► on_rotate() ─► app_model_rotate()
ring push ─► GPIO41 ─► board_encoder.c ─► on_press() ─► app_model_push()
touch key ─► LVGL CLICKED ─► action_key_tap(userData=index) ─► app_model_key_tap()
companion ─► CDC RX ─► rotary_usb.c ─► on_focus() ─► app_model_set_app()
                                    └─► on_time()  ─► clock_set()
                                    app_model ─► rotary_usb_type() ─► usb_hid task ─► host
                                    app_model ─► changed callback ─► ui_glue (screens, chips, marks)
```

**The ring is relative only.** The driver emits detents, never a position,
and zeroes the hardware counter on every read. Anything bounded (the
Kdenlive playhead at frame 0) is clamped on the *displayed* value, so
turning past a limit and back never needs "winding back". See the comment
block at the top of `board_encoder.c`.

**Keystrokes are queued, all-or-nothing.** One ring click can be several
keystrokes — Blender's `R Z 5 Enter`, five arrow presses for a Kdenlive
scrub, three `Ctrl+↓` for a VSCodium scroll. `rotary_usb_type()` either
queues the whole click or none of it, so a full queue can never leave an
app half-way through a command. When it refuses, the model does not count
the click either, so the on-screen tally never claims a rotation or scrub
that was not sent.

## Touch vs push

The whole panel is the ring's button: a firm press on the glass is both a
touch contact and a switch closure. `board_touch.c` and `board_encoder.c`
share two flags — "finger is down" and "the switch closed during this
contact" — and `crowpanel_board_touch_contact_pushed()` reports the second.
`actions.c` checks it and drops the tap, because the push has already done
its job (changed axis, cut in Kdenlive, switched line/page scrolling in
VSCodium, LibreOffice and the browsers, fit the view in GIMP or Inkscape). A light tap never closes the
switch and works normally.

## What the pad does *not* know

**The time**, until the companion tells it: there is no battery-backed
clock. The idle clock hides its hands until the first `time` line, then
counts on its own and is corrected every minute.


The angle and the Kdenlive timecode are **tallies of what the pad has
sent**, not readings from the app. Nothing flows back from the app, so
using the mouse or the app's own controls makes them drift. They reset
when the pad reboots.

## The one geometry exception

EEZ Studio owns the position, size and font of every widget it authors, and
nothing in `main/` may override them — otherwise the EEZ Studio canvas and
the device disagree. The Kdenlive **cut marks** are the exception by
design: they are objects created at runtime inside the EEZ-authored strip,
and where they sit *is* the data (where you cut). They take their colour
from the EEZ `CutMark` style, so the theme still owns how they look.
