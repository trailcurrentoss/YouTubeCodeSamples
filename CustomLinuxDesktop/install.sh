#!/usr/bin/env bash
# Install TrailShell system-wide and register it as a selectable X session.
#
# Deliberately NON-DESTRUCTIVE: it adds a session next to the ones already on
# the machine and never changes the default. You pick TrailShell from the
# LightDM greeter's session menu, and the stock Raspberry Pi desktop stays
# exactly where it was as a fallback. Use --set-default to opt in to it being
# the default, and ./uninstall.sh to remove it.
set -euo pipefail
cd "$(dirname "$0")"

PREFIX="${PREFIX:-/usr/local}"
LIBDIR="$PREFIX/lib/trailshell"
SHAREDIR="/usr/share/trailshell"
XSESSIONS="/usr/share/xsessions"
SET_DEFAULT=0

for arg in "$@"; do
    case "$arg" in
        --set-default) SET_DEFAULT=1 ;;
        -h|--help) sed -n '2,12p' "$0"; exit 0 ;;
        *) echo "unknown option: $arg" >&2; exit 2 ;;
    esac
done

if [ "$(id -u)" -ne 0 ]; then
    echo "This installs system-wide; re-running under sudo." >&2
    exec sudo -E "$0" "$@"
fi

echo "==> Checking runtime dependencies"
missing=()
while read -r pkg; do
    [ -z "$pkg" ] && continue
    dpkg -s "$pkg" >/dev/null 2>&1 || missing+=("$pkg")
done < <(grep -vE '^\s*(#|$)' packages.runtime | sed 's/[[:space:]]*#.*//' | tr -d ' ')
if [ ${#missing[@]} -gt 0 ]; then
    echo "Missing packages: ${missing[*]}" >&2
    echo "Run ./install-deps.sh first." >&2
    exit 1
fi

echo "==> Installing the shell to $LIBDIR"
install -d "$LIBDIR"
rm -rf "$LIBDIR/trailshell"
cp -r trailshell "$LIBDIR/trailshell"
find "$LIBDIR/trailshell" -name '__pycache__' -type d -exec rm -rf {} + 2>/dev/null || true

echo "==> Installing session data to $SHAREDIR"
install -d "$SHAREDIR"
install -m 0644 session/openbox-rc.xml "$SHAREDIR/openbox-rc.xml"

echo "==> Installing launchers"
# A wrapper rather than a PYTHONPATH export in the .desktop, so the shell can
# also be run by hand for debugging.
cat > /usr/bin/trailshell <<WRAP
#!/bin/sh
exec env PYTHONPATH="$LIBDIR\${PYTHONPATH:+:\$PYTHONPATH}" python3 -m trailshell "\$@"
WRAP
chmod 0755 /usr/bin/trailshell

# Copied verbatim: it invokes /usr/bin/trailshell, which carries PYTHONPATH,
# rather than being rewritten at install time.
install -m 0755 session/trailshell-session /usr/bin/trailshell-session

echo "==> Registering the X session"
install -d "$XSESSIONS"
install -m 0644 session/trailshell.desktop "$XSESSIONS/trailshell.desktop"

if [ "$SET_DEFAULT" -eq 1 ]; then
    echo "==> Making TrailShell the default session"
    # Delegated so there is exactly one implementation of the four-place
    # session-selection dance. Setting only lightdm.conf's user-session (which
    # is what this script used to do) has no effect when the user has a
    # remembered session or autologin is configured - both outrank it.
    "$(dirname "$0")/tools/set-session.sh" trailshell
fi

GREETER=$(grep -E '^\s*greeter-session=' /etc/lightdm/lightdm.conf 2>/dev/null | tail -1 | cut -d= -f2)

cat <<DONE

TrailShell installed.

  Try it over the current session:   trailshell
  Make it your desktop:              sudo ./tools/set-session.sh trailshell
  Go back at any time:               sudo ./tools/set-session.sh rpd-x

DONE

if [ "$GREETER" = "pi-greeter-x" ] || [ "$GREETER" = "pi-greeter" ]; then
    cat <<'WARN'
NOTE: this machine's greeter (pi-greeter) has no session picker, so logging
out will NOT offer you a choice. Use tools/set-session.sh to switch.

WARN
fi

cat <<'DONE2'
The Raspberry Pi desktop stays installed. If TrailShell ever fails to start,
the session script hands the display back to it automatically and logs why to
~/.local/share/trailshell/session.log

Remove with ./uninstall.sh
DONE2
