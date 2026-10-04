#!/bin/bash
# Flash the Rotary Macro Pad over its one USB-C cable.
#
# While the firmware runs, the port is a USB keyboard + serial port (TinyUSB)
# and esptool cannot reset it. This script asks the firmware to reboot into
# the ROM loader first -- it writes `bootloader` to the pad's serial port --
# then waits for the ROM's USB-Serial/JTAG port and runs `idf.py flash`.
#
# If the pad is already in the ROM loader (a board fresh from the factory, a
# previous attempt, or BOOT held during reset), it flashes straight away.
#
#   scripts/flash.sh              build if needed, flash, then open the monitor
#   scripts/flash.sh --no-monitor
#
# Ports are found by USB identity under /dev/serial/by-id, never hardcoded.
set -euo pipefail

cd "$(dirname "$0")/.."

MONITOR=1
[ "${1:-}" = "--no-monitor" ] && MONITOR=0

command -v idf.py >/dev/null || { echo "idf.py not found -- source ESP-IDF first: . \$IDF_PATH/export.sh" >&2; exit 1; }

app_port()  { ls /dev/serial/by-id/*Rotary_HID* 2>/dev/null | head -n1 || true; }
rom_port()  { ls /dev/serial/by-id/*Espressif_USB_JTAG* 2>/dev/null | head -n1 || true; }

port="$(rom_port)"
if [ -z "$port" ]; then
    app="$(app_port)"
    if [ -z "$app" ]; then
        echo "No pad found. Plug it in; if it is stuck, hold BOOT while pressing RESET." >&2
        exit 1
    fi
    echo "Asking the firmware on $app to reboot into the ROM loader..."
    stty -F "$app" 115200 raw -echo 2>/dev/null || true
    printf 'bootloader\n' > "$app"

    for _ in $(seq 1 50); do
        port="$(rom_port)"
        [ -n "$port" ] && break
        sleep 0.2
    done
    if [ -z "$port" ]; then
        echo "The ROM loader did not appear. Hold BOOT while pressing RESET, then re-run." >&2
        exit 1
    fi
fi

echo "Flashing via $port"
if [ "$MONITOR" -eq 1 ]; then
    # After the flash the app starts TinyUSB, so the monitor must follow the
    # pad to its new port -- the ROM port is gone by then.
    idf.py -p "$port" flash
    echo "Waiting for the firmware's serial port..."
    for _ in $(seq 1 50); do
        app="$(app_port)"
        [ -n "$app" ] && break
        sleep 0.2
    done
    [ -n "${app:-}" ] && exec idf.py -p "$app" monitor --no-reset
    echo "Firmware port did not appear; flash completed anyway." >&2
else
    idf.py -p "$port" flash
fi
