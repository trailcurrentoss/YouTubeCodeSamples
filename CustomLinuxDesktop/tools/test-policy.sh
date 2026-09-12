#!/usr/bin/env bash
# Verify the window policy locks apps to the app canvas, including a client
# that draws its own decorations and remembers a windowed size (Chromium).
set -u
cd "$(dirname "$0")/.."
export DISPLAY=:0 XDG_RUNTIME_DIR=/run/user/1000

python3 -m trailshell -v >/tmp/policy.log 2>&1 &
PID=$!
sleep 4

check() {
python3 - <<'PY'
import os, sys; sys.path.insert(0, os.getcwd())
import gi; gi.require_version('Gtk','3.0')
from trailshell import x11, theme as T
wins = x11.get().list_windows()
if not wins:
    print("  (no windows)")
for w in wins:
    x, y, ww, hh = w.geometry
    # A window is correct if it fills the canvas as closely as its own size
    # hints permit, and never leaves the canvas.
    inside = x >= -2 and y >= T.APP_Y - 2 and (x + ww) <= T.SCREEN_W + 2 and (y + hh) <= T.APP_Y + T.APP_H + 2
    fills = ww >= T.SCREEN_W - 16 and hh >= T.APP_H - 20
    print("  %-18s %4dx%-4d+%-4d+%-4d  %s" % (
        (w.wm_class or '?')[:18], ww, hh, x, y,
        "LOCKED TO CANVAS" if (inside and fills) or w.is_dialog
        else "!! WRONG (canvas is %dx%d at y=%d)" % (T.SCREEN_W, T.APP_H, T.APP_Y)))
PY
}

echo "== lxterminal =="
lxterminal >/dev/null 2>&1 & sleep 3; check

echo "== chromium (client-side decorations, remembers a windowed size) =="
chromium --no-first-run --disable-session-crashed-bubble about:blank >/dev/null 2>&1 &
sleep 8; check

echo "== now try to drag a window away and see if it snaps back =="
W=$(xdotool search --class lxterminal 2>/dev/null | tail -1)
if [ -n "$W" ]; then
  xdotool windowmove "$W" 120 200; sleep 1.2
  echo "  after forcing it to +120+200:"; check
fi

pkill -x chromium 2>/dev/null
pkill -x lxterminal 2>/dev/null
sleep 1
kill -TERM $PID 2>/dev/null; sleep 1.5
echo "== errors =="; grep -E "ERROR|Traceback" /tmp/policy.log | head -5 || echo "  none"
echo "== policy log =="; grep -E "adopting|placing" /tmp/policy.log | head -12
