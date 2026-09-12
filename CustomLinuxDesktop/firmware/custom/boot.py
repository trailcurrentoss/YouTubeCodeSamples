# PocketTerm35 keyboard - boot configuration.
#
# The stock firmware unconditionally ran:
#
#     import storage
#     storage.disable_usb_drive()
#
# which is why no CIRCUITPY drive ever appeared and why changing the keymap
# required BOOTSEL plus picotool. That is a slow, risky loop for what is
# ultimately a text file.
#
# This version keeps the drive hidden during normal use - so the keyboard is
# only ever a keyboard, and there is no volume to accidentally write to or
# corrupt - but exposes it when SELECT is held during power-up. Hold SELECT,
# plug in, and CIRCUITPY appears; then code.py is editable directly.
#
# SELECT is matrix row 6, column 3:
#     row_pins = [GP16, GP10, GP11, GP12, GP13, GP14, GP15]   -> row 6 = GP15
#     col_pins = [GP0 .. GP9]                                 -> col 3 = GP3
#
# The whole check is wrapped in try/except and defaults to hiding the drive, so
# any failure here leaves exactly the stock behaviour rather than a bricked
# keyboard.
import storage

expose_drive = False
try:
    import board
    import digitalio
    import time

    col = digitalio.DigitalInOut(board.GP3)     # column 3
    col.direction = digitalio.Direction.OUTPUT
    col.value = True

    row = digitalio.DigitalInOut(board.GP15)    # row 6
    row.direction = digitalio.Direction.INPUT
    row.pull = digitalio.Pull.DOWN

    time.sleep(0.02)                            # let the line settle
    expose_drive = bool(row.value)              # SELECT held?

    # Release the pins so code.py can claim them for the real matrix scan.
    col.value = False
    col.deinit()
    row.deinit()
except Exception as exc:      # noqa: BLE001 - never let boot.py fail the board
    print("boot.py key check failed, hiding drive: %s" % exc)
    expose_drive = False

if expose_drive:
    print("SELECT held at boot - exposing CIRCUITPY for editing")
else:
    storage.disable_usb_drive()
