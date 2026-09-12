#!/usr/bin/env bash
# Install TrailShell's dependencies from the package manifests.
#   ./install-deps.sh          runtime only
#   ./install-deps.sh --dev    runtime + development/test tooling
set -euo pipefail
cd "$(dirname "$0")"

read_manifest() { grep -vE '^\s*(#|$)' "$1" | sed 's/[[:space:]]*#.*//' | tr -d ' '; }

PKGS=$(read_manifest packages.runtime)
if [ "${1:-}" = "--dev" ]; then
    PKGS="$PKGS $(read_manifest packages.dev)"
fi

# shellcheck disable=SC2086
echo "Installing: $(echo $PKGS | tr '\n' ' ')"
sudo apt-get update
# shellcheck disable=SC2086
sudo apt-get install -y --no-install-recommends $PKGS
echo "OK - dependencies installed."
