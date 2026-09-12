# Keyboard firmware

- `backup/` — verified read-only dumps of the keyboard's RP2040 flash. **Keep a
  copy of these off this machine.**
- `circuitpy/` — the CircuitPython filesystem extracted from the backup, for
  reference. `code.py` is the entire keyboard: matrix scan, `KEY_MAP`,
  `FN_MAP`, and the PWM handlers for backlight and audio.

Nothing here has been written back to the device. See
[../docs/keyboard-firmware.md](../docs/keyboard-firmware.md) for the full
procedure, the complete key matrix, and which keycodes are free.
