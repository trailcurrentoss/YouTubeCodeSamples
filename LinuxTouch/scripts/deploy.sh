#!/bin/bash
# deploy.sh — push the shell and daemon to a board and restart them.
#
# The fast development loop: no image rebuild, no reflash, no reboot. Copies the
# working tree to the device, runs provision.sh there (which is idempotent), and
# restarts touchd so the change is live.
#
#     make deploy
#     make deploy DEVICE=user@host
#     scripts/deploy.sh --device user@host --shell-only
#
# --shell-only skips provision.sh entirely and just replaces the web bundle,
# which is the common case while working on the UI and takes about a second.
set -euo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SHELL_ONLY=0

# dev-env.sh parses --device/--key/--port and leaves everything else in "$@".
# shellcheck source=scripts/dev-env.sh
. "$REPO/scripts/dev-env.sh" "$@"

for arg in "$@"; do
    [ "$arg" = "--shell-only" ] && SHELL_ONLY=1
done

log() { printf '\033[1;32m==>\033[0m %s\n' "$*"; }

log "Target: $DEVICE"

# Copy into the user's home rather than straight into /usr: the SSH user is not
# root, and asking for a password mid-rsync is worse than a two-step copy.
STAGE="linuxtouch-deploy"

log "Copying the working tree"
# --delete so a file removed locally actually disappears on the device; a stale
# JS module left behind is a bug that only reproduces on hardware.
rsync -az --delete \
    --exclude '__pycache__' --exclude '*.pyc' \
    --exclude 'scripts/dev.env' --exclude '.git' \
    -e "ssh $SSH_OPTS" \
    "$REPO"/ "$DEVICE:$STAGE/"

if [ "$SHELL_ONLY" = 1 ]; then
    log "Installing the shell bundle only"
    # shellcheck disable=SC2029  # $STAGE is ours, and must expand locally
    ssh $SSH_OPTS "$DEVICE" "
        set -e
        sudo rm -rf /usr/share/linuxtouch/shell
        sudo cp -r '$STAGE/shell' /usr/share/linuxtouch/shell
    "
else
    log "Running provision.sh on the device (sudo may prompt)"
    # -t allocates a TTY so sudo can prompt. Without it, a board whose sudo
    # needs a password fails with "no tty present" and no explanation.
    # shellcheck disable=SC2029
    ssh -t $SSH_OPTS "$DEVICE" "sudo '$STAGE/scripts/provision.sh' --user \"\$USER\""
fi

log "Restarting touchd"
# `|| true`: on a board that has not rebooted since the first provision, the
# user service may not be loaded yet, and that is not a deploy failure.
ssh $SSH_OPTS "$DEVICE" 'systemctl --user restart touchd 2>/dev/null || true'

log "Reloading the shell"
# swaymsg reload restarts exec_always, which restarts Chromium against the new
# bundle. Silently skipped when sway is not the running session.
ssh $SSH_OPTS "$DEVICE" 'swaymsg reload 2>/dev/null || true'

log "Done. Recent daemon output:"
ssh $SSH_OPTS "$DEVICE" 'journalctl --user -u touchd -n 15 --no-pager 2>/dev/null || true'
