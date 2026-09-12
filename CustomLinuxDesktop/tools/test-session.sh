#!/usr/bin/env bash
# Run the REAL TrailShell session (openbox + shell, our own openbox-rc.xml) on
# an isolated 640x480 Xvfb display, drive it, and screenshot each state.
#
# This is the honest test: unlike tools/drive.sh it shares nothing with the
# host's LXDE session, so there is no lxpanel stealing hotkeys, no rpd-rc.xml
# putting title bars on windows, and no competing struts.
set -u
cd "$(dirname "$0")/.."

DISP="${DISP:-:77}"
PREFIX="shots/sess"
export XDG_RUNTIME_DIR=/run/user/1000

command -v Xvfb >/dev/null || { echo "Xvfb missing - run ./install-deps.sh --dev"; exit 1; }

cleanup() {
    [ -n "${SHELL_PID:-}" ] && kill -TERM "$SHELL_PID" 2>/dev/null
    [ -n "${OB_PID:-}" ] && kill -TERM "$OB_PID" 2>/dev/null
    pkill -x lxterminal 2>/dev/null
    [ -n "${XPID:-}" ] && kill -TERM "$XPID" 2>/dev/null
    wait 2>/dev/null
}
trap cleanup EXIT

echo "==> Xvfb on $DISP at 640x480"
Xvfb "$DISP" -screen 0 640x480x24 -nolisten tcp >/tmp/xvfb.log 2>&1 &
XPID=$!
sleep 2
export DISPLAY="$DISP"
xdpyinfo | grep -m1 dimensions || { echo "Xvfb did not come up"; exit 1; }

# The shell binds F13-F18; xkb calls those keycodes XF86Tools/XF86Launch5-9,
# so the session renames them. Do the same here or the test is not testing the
# real thing.
xmodmap - <<'XMODMAP' 2>/dev/null || true
keycode 191 = F13
keycode 192 = F14
keycode 193 = F15
keycode 194 = F16
keycode 195 = F17
keycode 196 = F18
XMODMAP

echo "==> openbox with TrailShell's own config"
xsetroot -solid "#000000"
openbox --config-file session/openbox-rc.xml >/tmp/openbox.log 2>&1 &
OB_PID=$!
for _ in $(seq 1 20); do xprop -root _NET_SUPPORTING_WM_CHECK >/dev/null 2>&1 && break; sleep 0.2; done

echo "==> TrailShell"
python3 -m trailshell -v >/tmp/trailshell-session.log 2>&1 &
SHELL_PID=$!
sleep 4

shot() { python3 tools/shot.py "${PREFIX}-$1" >/dev/null 2>&1 && echo "  $1"; }

shot 01-home.png

echo "==> launch an app (checks undecorated + maximised into the strut canvas)"
lxterminal >/dev/null 2>&1 &
sleep 3.5
shot 02-app.png
python3 - <<'PY'
import os, sys; sys.path.insert(0, os.getcwd())
import gi; gi.require_version('Gtk','3.0')
from trailshell import x11, theme as T
for w in x11.get().list_windows():
    x, y, ww, hh = w.geometry
    ok = (y == T.APP_Y and hh == T.APP_H and ww == T.SCREEN_W)
    print("  window %-22s geom=%dx%d+%d+%d  %s"
          % (w.wm_class, ww, hh, x, y,
             "FITS THE APP CANVAS" if ok else "!! expected %dx%d+0+%d" % (T.SCREEN_W, T.APP_H, T.APP_Y)))
PY

echo "==> notification (exercises the D-Bus server end to end)"
notify-send -u normal "Storage" "Root filesystem 91% full" 2>/dev/null
sleep 1.5
shot 03-banner.png

echo "==> shade"
xdotool key super+n; sleep 1.5; shot 04-shade.png
xdotool key Escape; sleep 0.8

echo "==> recents"
xdotool key super+Tab; sleep 1.5; shot 05-recents.png
xdotool key Escape; sleep 0.6

echo "==> shortcuts"
xdotool key super+slash; sleep 1.5; shot 06-shortcuts.png
xdotool key Escape; sleep 0.6

echo "==> dark theme"
xdotool key super+d; sleep 1.0; xdotool key super+n; sleep 1.5; shot 07-shade-dark.png
xdotool key Escape; sleep 0.5; xdotool key super+h; sleep 1.0; shot 08-home-dark.png
xdotool key super+d; sleep 0.5

echo
echo "=== errors ==="
grep -E "ERROR|Traceback|Exception" /tmp/trailshell-session.log | head -20 || echo "  none"
echo "=== shortcuts bound ==="
grep -E "bound .*/.* shortcuts|no accelerator" /tmp/trailshell-session.log
echo "=== hardware keys (these were TAKEN on the LXDE desktop) ==="
grep -E "hardware key|did not bind" /tmp/trailshell-session.log || echo "  (none logged)"
