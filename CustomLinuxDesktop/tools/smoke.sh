#!/usr/bin/env bash
set -u
cd "$(dirname "$0")/.."
export DISPLAY=:0 XDG_RUNTIME_DIR=/run/user/1000
python3 -m trailshell -v >/tmp/smoke.log 2>&1 &
PID=$!
sleep 4
python3 tools/shot.py shots/dev/72-home.png >/dev/null 2>&1
lxterminal >/dev/null 2>&1 & sleep 3
python3 tools/shot.py shots/dev/73-app.png >/dev/null 2>&1
xdotool key super+h; sleep 1.5
python3 tools/shot.py shots/dev/74-home-again.png >/dev/null 2>&1
pkill -x lxterminal; sleep 1
kill -TERM $PID 2>/dev/null; sleep 2
kill -0 $PID 2>/dev/null && kill -9 $PID
echo "== errors =="; grep -E "ERROR|Traceback" /tmp/smoke.log | head -5 || echo "  none"
echo "== policy =="; grep -E "adopting|placing" /tmp/smoke.log | head -5
