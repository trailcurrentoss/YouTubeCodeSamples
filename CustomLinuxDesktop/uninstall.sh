#!/usr/bin/env bash
# Remove TrailShell. Leaves per-user settings in ~/.config/trailshell alone
# unless --purge is given.
set -euo pipefail
PREFIX="${PREFIX:-/usr/local}"
PURGE=0
[ "${1:-}" = "--purge" ] && PURGE=1

if [ "$(id -u)" -ne 0 ]; then exec sudo -E "$0" "$@"; fi

rm -rf "$PREFIX/lib/trailshell"
rm -f /usr/bin/trailshell /usr/bin/trailshell-session
rm -f /usr/share/xsessions/trailshell.desktop
rm -rf /usr/share/trailshell

conf=/etc/lightdm/lightdm.conf
if [ -f "$conf.trailshell-backup" ]; then
    mv "$conf.trailshell-backup" "$conf"
    echo "restored $conf"
fi

if [ "$PURGE" -eq 1 ]; then
    for home in /home/*; do
        rm -rf "$home/.config/trailshell"
    done
    echo "purged per-user settings"
fi
echo "TrailShell removed."
