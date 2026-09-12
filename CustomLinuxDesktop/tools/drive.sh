#!/usr/bin/env bash
# Drive the running shell with synthetic input and capture each state.
# Non-destructive: draws over the existing session, cleans up after itself.
set -u
cd "$(dirname "$0")/.."
export DISPLAY=:0 XDG_RUNTIME_DIR=/run/user/1000
shot() { python3 tools/shot.py "shots/$1" >/dev/null 2>&1; echo "  captured $1"; }

python3 -m trailshell -v > /tmp/trailshell.log 2>&1 &
PID=$!
sleep 4

echo "1. type a search query"
xdotool type --delay 60 "fire"; sleep 1.2; shot 20-search.png
xdotool key Escape; sleep 0.6

echo "2. open the shade via Super+n"
xdotool key super+n; sleep 1.2; shot 21-shade.png
xdotool key Escape; sleep 0.8

echo "3. launch a real app (tests EWMH struts)"
lxterminal & APP=$!
sleep 3.5; shot 22-app.png

echo "4. recents via Super+Tab"
xdotool key super+Tab; sleep 1.2; shot 23-recents.png

echo "5. shortcuts sheet"
xdotool key Escape; sleep 0.5; xdotool key super+slash; sleep 1.2; shot 24-shortcuts.png

echo "cleanup"
pkill -x lxterminal 2>/dev/null
kill -TERM $PID 2>/dev/null; wait $PID 2>/dev/null
echo "=== errors in log ==="
grep -E "ERROR|Traceback|Exception" /tmp/trailshell.log | head -20 || echo "  none"
