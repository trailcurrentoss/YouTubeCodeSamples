#!/usr/bin/env bash
# Back up the PocketTerm35 keyboard's RP2040/RP2350 firmware.
#
# READ-ONLY. This script never writes to the microcontroller. It only
# identifies the device and copies its flash off, so there is always a way
# back to the keyboard that shipped with the machine.
#
# Run it with the keyboard already in BOOTSEL mode (see docs/keyboard-firmware.md).
set -uo pipefail
cd "$(dirname "$0")/.."

OUT_DIR="firmware/backup"
STAMP=$(date +%Y%m%d-%H%M%S)

# BOOTSEL vendor/product IDs, per Raspberry Pi's USB device documentation.
RP2040_BOOT="2e8a:0003"
RP2350_BOOT="2e8a:000f"

say()  { printf '%s\n' "$*"; }
fail() { printf 'ERROR: %s\n' "$*" >&2; exit 1; }

say "== Looking for a device in BOOTSEL mode =="
LSUSB=$(lsusb 2>/dev/null)
printf '%s\n' "$LSUSB" | grep -iE "2e8a:" || true

CHIP=""
if printf '%s\n' "$LSUSB" | grep -qi "$RP2040_BOOT"; then
    CHIP="RP2040"
elif printf '%s\n' "$LSUSB" | grep -qi "$RP2350_BOOT"; then
    CHIP="RP2350"
fi

if [ -z "$CHIP" ]; then
    say ""
    say "No RP2040/RP2350 in BOOTSEL mode found."
    say "The keyboard currently enumerates as:"
    printf '%s\n' "$LSUSB" | grep -i "1209:0001" || say "  (not found either - is it plugged in?)"
    fail "Put the keyboard into BOOTSEL first - see docs/keyboard-firmware.md"
fi
say "Found: $CHIP in BOOTSEL mode"

command -v picotool >/dev/null || fail "picotool is not installed (sudo apt install picotool)"

mkdir -p "$OUT_DIR"

say ""
say "== The BOOTSEL mass-storage volume =="
# INFO_UF2.TXT identifies the exact board and bootloader build. Worth keeping:
# it is the only in-band record of what this hardware actually is.
MNT=$(lsblk -o LABEL,MOUNTPOINT -nr 2>/dev/null | awk '$1=="RPI-RP2"||$1=="RP2350"{print $2}' | head -1)
if [ -n "${MNT:-}" ] && [ -d "$MNT" ]; then
    say "mounted at $MNT"
    for f in INFO_UF2.TXT INDEX.HTM; do
        [ -f "$MNT/$f" ] && { say "--- $f ---"; cat "$MNT/$f"; cp "$MNT/$f" "$OUT_DIR/$STAMP-$f"; }
    done
else
    say "(not auto-mounted; that is fine, picotool talks to it directly over USB)"
fi

say ""
say "== Device info (picotool) =="
sudo picotool info -a 2>&1 | tee "$OUT_DIR/$STAMP-info.txt" || \
    say "picotool info failed - continuing to the save attempt anyway"

say ""
say "== Saving flash =="
# --all reads the entire flash, not just the program region, so anything the
# firmware persists (settings, a keymap blob) comes along too.
UF2="$OUT_DIR/$STAMP-keyboard-full.uf2"
if sudo picotool save -a "$UF2" 2>&1 | tee "$OUT_DIR/$STAMP-save.log"; then
    say "saved: $UF2"
else
    say "full save failed; trying just the program region"
    UF2="$OUT_DIR/$STAMP-keyboard-program.uf2"
    sudo picotool save "$UF2" 2>&1 | tee -a "$OUT_DIR/$STAMP-save.log" || \
        fail "picotool could not read the flash (see $OUT_DIR/$STAMP-save.log)"
fi

if [ -f "$UF2" ]; then
    sha256sum "$UF2" | tee "$OUT_DIR/$STAMP-keyboard.sha256"
    ls -lh "$UF2"
fi

say ""
say "== Done - NOTHING WAS WRITTEN TO THE KEYBOARD =="
say "Backup in $OUT_DIR/"
say ""
say "To put the keyboard back into normal typing mode:"
say "    sudo picotool reboot"
say "  or just unplug and replug it / power-cycle the machine."
