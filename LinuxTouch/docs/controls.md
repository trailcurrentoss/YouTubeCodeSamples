# LinuxTouch controls

Two independent input paths, both mandatory:

1. **Touch** — everything must be reachable by thumb, with no exceptions.
2. **Buttons** — the D-pad, Start, and Select must be able to drive the shell.

That redundancy is not a courtesy. The buttons hang off the RP2040, which can strand
itself in BOOTSEL from an accidental double-tap of the case-back RESET button, killing
every button at once. The GT911 is on I²C and survives that fault, so **touch is the
recovery path** — see [hardware.md](hardware.md#the-rp2040-is-a-single-point-of-failure-for-input).

---

## The rule that shapes everything

**Six of the twelve controls are literal QWERTY letters.** `A B X Y L R` are
`KEY_A KEY_B KEY_X KEY_Y KEY_L KEY_R`. This device runs a mail client, a browser, and
a text editor — so if any of those letters were grabbed globally, the user could not
type the letter "a" in an email.

**Only `Start` (`KEY_PAUSE`) and `Select` (`KEY_SYSRQ`) carry no character.** They are
the only two buttons that can safely mean the same thing in every context, including
inside a text field.

So the entire global control scheme is those two keys, and nothing else:

| Button | Keysym | Global action |
|---|---|---|
| **Start** | `Pause` | **Home.** Pressed again while already home: **app switcher**. |
| **Select** | `Print` | **Back.** Leaves the app running and returns home. |

Both are bound in [`wm/sway/config`](../wm/sway/config) at the **compositor** level, so
they fire whether the focused window is the shell, Thunderbird, or a terminal. Nothing
else is globally bound. Ever.

### Why this is simpler than Tracer

[TrailCurrentTracer](../../../Product/TrailCurrentTracer/docs/controls.md) needed a
*modal* input daemon — a `nav` mode where letters were swallowed as buttons and a
`text` mode where they passed through — because Tracer's kiosk was the only thing on
screen and had to serve both purposes from one keyboard.

LinuxTouch does not need that, and deliberately does not have it. Focus does the job
that mode did:

- **The shell has focus** → the shell's JavaScript interprets arrows, Start, Select,
  and optionally letters as shortcuts.
- **An app has focus** → the app receives every key verbatim, exactly as it would on a
  desktop. The shell is not even in the input path.

There is no mode to get stuck in, no daemon owning a mode flag, and no way for the
device to end up typing "b" when the user meant Back. Dropping the modal design is the
main benefit of moving from a single-app kiosk to a real compositor.

---

## Inside the shell

Only while the shell itself holds focus:

| Input | Action |
|---|---|
| D-pad | Move selection in the grid / scroll a list |
| **Start** | Open the selected app |
| **Select** | Back one level; from the top level, nothing |
| Swipe left/right | Change grid page |
| Swipe up on a switcher card | Close that app |
| Tap | Everything a button can do |

Letters are available as shortcuts inside the shell **only when no text field has
focus**, and the hint bar never advertises them — Tracer's legend taught users "A =
Open", which stopped being true the moment they edited anything. LinuxTouch's hint bar
always leads with Start and Select, because those never change meaning.

The one text field in the shell is the Wi-Fi password box. While it has focus, every
letter types, and `Select` is the guaranteed way out. `Esc` and `Enter` are real keys
on the keyboard and also work.

---

## The power button

The physical power button must never hard-kill the unit on a short press. The image
sets:

```ini
HandlePowerKey=ignore
HandlePowerKeyLongPress=poweroff
```

A short press arrives at the shell as an event and raises a **"Shut down?"
confirmation**; `power.shutdown` refuses to act without `confirm: true`. A long press
still powers off at the logind level, as the escape hatch. This is a handheld that gets
carried in a bag, and a brushed button that kills it silently is a bug.

---

## No on-screen keyboard — decided

**Do not build one, anywhere in LinuxTouch.** The device has a real QWERTY keyboard, so
an on-screen keyboard would be strictly slower — D-pad-walking a grid to pick letters
you could simply type — and at 640×480 it would cover most of the screen.

This applies to every text input in the product: Wi-Fi passwords, search boxes, and any
first-party app we write. They all take input from the physical keyboard.

The corollary is that third-party apps get no OSK either. An app that expects one will
still work, because the hardware keyboard is a real HID keyboard as far as it knows.

---

## Re-capturing the keymap

The mapping lives in the RP2040's CircuitPython `code.py`, so reflashing the Pico can
move it. [`touchd/tools/capture_keymap.py`](../touchd/tools/capture_keymap.py) records
keycodes on the device without `sudo` and without grabbing the keyboard. The default
map is data, not code: [`touchd/keymap.default.json`](../touchd/keymap.default.json).

**Never hardcode an event device number.** The keyboard was `event0` on Tracer's card
and `event1` on this one — allocation order is not identity. Match on device name.
