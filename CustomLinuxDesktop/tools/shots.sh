#!/usr/bin/env bash
# Capture the documentation screenshot set from a fresh shell on the live :0.
# Lives in a file rather than an inline command so that pkill patterns cannot
# match the invoking shell's own command line and kill it.
set -u
cd "$(dirname "$0")/.."
export DISPLAY=:0 XDG_RUNTIME_DIR=/run/user/1000

pkill -x trailshell 2>/dev/null
sleep 1

shot() { python3 tools/shot.py "shots/$1" >/dev/null 2>&1 && echo "  $1"; }

python3 -m trailshell -v > /tmp/ts-shots.log 2>&1 &
PID=$!
sleep 4

shot 50-home.png

xdotool key super+slash; sleep 1.5; shot 51-shortcuts.png
xdotool key Escape; sleep 0.8

xdotool key super+n; sleep 1.3
xdotool mousemove 597 445 click 1; sleep 1.5; shot 52-power.png
xdotool key Escape; sleep 0.8

xdotool key super+d; sleep 1.0; shot 53-home-dark.png
xdotool key super+n; sleep 1.3; shot 54-shade-dark.png
xdotool key Escape; sleep 0.6
xdotool key super+d; sleep 0.6

kill -TERM "$PID" 2>/dev/null
sleep 2
if kill -0 "$PID" 2>/dev/null; then echo "ORPHAN - forcing"; kill -9 "$PID"; else echo "clean exit"; fi

echo "=== errors ==="
grep -E "ERROR|Traceback|Exception" /tmp/ts-shots.log | head -10 || echo "  none"
