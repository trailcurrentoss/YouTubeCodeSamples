#!/usr/bin/env bash
# Run the full shell against the *running* :0 session for a few seconds,
# screenshot it, then stop. Non-destructive: it never touches lightdm or the
# existing LXDE session, it just draws on top of it.
set -u
cd "$(dirname "$0")/.."
export DISPLAY=:0 XDG_RUNTIME_DIR=/run/user/1000
SECS="${1:-8}"
SHOT="${2:-shots/live.png}"

python3 -m trailshell -v > /tmp/trailshell.log 2>&1 &
PID=$!
sleep 3
for i in $(seq 1 $((SECS-3))); do sleep 1; done
python3 tools/shot.py "$SHOT" >/dev/null 2>&1
kill -TERM $PID 2>/dev/null
wait $PID 2>/dev/null
echo "=== log ==="
tail -40 /tmp/trailshell.log
echo "=== shot: $SHOT ==="
