#!/usr/bin/env bash
# Switch which X session LightDM starts, and switch back.
#
#   sudo ./tools/set-session.sh trailshell     # use TrailShell
#   sudo ./tools/set-session.sh rpd-x          # back to the Raspberry Pi desktop
#   ./tools/set-session.sh --show              # what is set right now
#
# LightDM picks a session from FOUR places, and setting only one of them does
# nothing, which is exactly the trap this script exists to avoid:
#
#   1. autologin-session  in lightdm.conf   - wins when autologin fires (boot)
#   2. ~/.dmrc                              - the user's last-used session
#   3. AccountsService XSession=            - the same thing, cached by
#                                             accountsservice; the greeter
#                                             prefers this over .dmrc
#   4. user-session       in lightdm.conf   - the fallback default, and the
#                                             LOWEST priority of the four
#
# This machine's greeter is pi-greeter, which has no session picker at all, so
# "log out and choose it from the menu" is not an option here.
set -euo pipefail

CONF=/etc/lightdm/lightdm.conf
TARGET_USER="${SUDO_USER:-$USER}"
USER_HOME=$(getent passwd "$TARGET_USER" | cut -d: -f6)
AS_FILE="/var/lib/AccountsService/users/$TARGET_USER"

show() {
    echo "user:                 $TARGET_USER"
    echo "autologin-session:    $(grep -E '^\s*autologin-session=' "$CONF" 2>/dev/null | tail -1 | cut -d= -f2 || echo '<unset>')"
    echo "user-session:         $(grep -E '^\s*user-session=' "$CONF" 2>/dev/null | tail -1 | cut -d= -f2 || echo '<unset>')"
    echo "~/.dmrc Session:      $(grep -E '^Session=' "$USER_HOME/.dmrc" 2>/dev/null | cut -d= -f2 || echo '<unset>')"
    if [ -r "$AS_FILE" ]; then
        echo "AccountsService:      $(grep -E '^XSession=' "$AS_FILE" | cut -d= -f2 || echo '<unset>')"
    else
        echo "AccountsService:      <needs root to read>"
    fi
    echo
    echo "available sessions:   $(ls /usr/share/xsessions/*.desktop 2>/dev/null | xargs -n1 basename | sed 's/\.desktop//' | tr '\n' ' ')"
}

if [ "${1:-}" = "--show" ] || [ $# -eq 0 ]; then
    show
    exit 0
fi

SESSION="$1"
[ -f "/usr/share/xsessions/$SESSION.desktop" ] || {
    echo "No such session: $SESSION" >&2
    echo "Available: $(ls /usr/share/xsessions/*.desktop | xargs -n1 basename | sed 's/\.desktop//' | tr '\n' ' ')" >&2
    exit 2
}

if [ "$(id -u)" -ne 0 ]; then exec sudo -E "$0" "$@"; fi

set_key() {  # set_key <key> <value>  - in the [Seat:*] section
    local key="$1" val="$2"
    if grep -qE "^\s*#?\s*$key=" "$CONF"; then
        sed -i "s|^\s*#\?\s*$key=.*|$key=$val|" "$CONF"
    else
        sed -i "/^\[Seat:\*\]/a $key=$val" "$CONF"
    fi
}

cp -n "$CONF" "$CONF.trailshell-backup" 2>/dev/null || true

echo "==> lightdm.conf"
set_key user-session "$SESSION"
# Only touch autologin-session if autologin is actually configured; adding it
# otherwise would silently enable autologin.
if grep -qE '^\s*autologin-user=' "$CONF"; then
    set_key autologin-session "$SESSION"
    echo "    (autologin is enabled for $(grep -E '^\s*autologin-user=' "$CONF" | cut -d= -f2))"
fi

echo "==> ~/.dmrc"
printf '[Desktop]\nSession=%s\n' "$SESSION" > "$USER_HOME/.dmrc"
chown "$TARGET_USER" "$USER_HOME/.dmrc"

echo "==> AccountsService"
mkdir -p "$(dirname "$AS_FILE")"
if [ -f "$AS_FILE" ]; then
    if grep -qE '^XSession=' "$AS_FILE"; then
        sed -i "s|^XSession=.*|XSession=$SESSION|" "$AS_FILE"
    else
        sed -i "/^\[User\]/a XSession=$SESSION" "$AS_FILE"
    fi
else
    printf '[User]\nXSession=%s\nSystemAccount=false\n' "$SESSION" > "$AS_FILE"
fi

echo
show
echo
echo "Reboot, or restart the display manager:  sudo systemctl restart lightdm"
