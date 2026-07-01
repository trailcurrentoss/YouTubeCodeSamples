#!/usr/bin/env bash
#
# Bring up a Qualcomm Genie NPU LLM server on a Radxa Dragon Q6A.
#
# Starts from a stock Radxa OS R2 (or newer) image, downloads the
# NPU-optimized Llama 3.2 1B bundle, installs the TrailCurrent Peregrine
# genie_server.py + systemd unit, and starts the service. Result: an
# Ollama-compatible HTTP endpoint on http://localhost:11434 serving the
# model from the Hexagon NPU at ~12 tokens/s.
#
# Run as the `radxa` user (not root). Requires sudo for the systemd
# install step and internet access for the model + repo files.
#
# Usage:
#     bash install-genie-npu.sh

set -eu

CTX_LENGTH="${CTX_LENGTH:-1024}"
MODEL_REPO="radxa/Llama3.2-1B-${CTX_LENGTH}-qairt-v68"
MODEL_DIR_NAME="Llama3.2-1B-${CTX_LENGTH}-qairt-v68"
MODEL_DIR="${HOME}/${MODEL_DIR_NAME}"

PEREGRINE_RAW="https://raw.githubusercontent.com/trailcurrentoss/TrailCurrentPeregrine/main"

if [[ "${EUID}" -eq 0 ]]; then
    echo "ERROR: run this script as the 'radxa' user, not root." >&2
    exit 1
fi

echo "==> [1/7] Installing btop (for visualizing CPU load during chats)"
sudo apt-get update -qq
sudo apt-get install -y -qq btop

echo "==> [2/7] Installing modelscope"
pip3 install --quiet --break-system-packages modelscope
export PATH="${HOME}/.local/bin:${PATH}"

echo "==> [3/7] Downloading ${MODEL_REPO} (~1.3 GB)"
cd "${HOME}"
modelscope download --model "${MODEL_REPO}" --local-dir "./${MODEL_DIR_NAME}"

echo "==> [4/7] Fetching genie_server.py from TrailCurrent Peregrine"
curl -fsSL -o "${HOME}/genie_server.py" "${PEREGRINE_RAW}/src/genie_server.py"

echo "==> [5/7] Installing systemd unit (retargeted for user '${USER}')"
curl -fsSL "${PEREGRINE_RAW}/config/genie-server.service" \
  | sed -e "s|/home/trailcurrent|${HOME}|g" \
        -e "s|User=trailcurrent|User=${USER}|g" \
        -e "s|Llama3.2-1B-1024-v68|${MODEL_DIR_NAME}|g" \
  | sudo tee /etc/systemd/system/genie-server.service >/dev/null

echo "==> [6/7] Enabling and starting genie-server"
sudo systemctl daemon-reload
sudo systemctl enable --now genie-server

echo "==> [7/7] Waiting for NPU warmup (up to 120s)"
if ! sudo timeout 120 journalctl -u genie-server -f -n 200 --no-pager 2>/dev/null \
     | grep -qm1 "Warmup done"; then
    echo
    echo "ERROR: warmup did not complete within 120s."
    echo "  Status: sudo systemctl status genie-server"
    echo "  Logs:   sudo journalctl -u genie-server -n 100 --no-pager"
    exit 1
fi

cat <<EOF

NPU LLM is live on http://localhost:11434/
Model:  ${MODEL_REPO}
Serves on every boot (systemd enabled).

Try it:
  curl -s http://localhost:11434/api/chat \\
    -d '{"messages":[{"role":"user","content":"Tell me a joke"}]}'

Watch NPU/CPU load side-by-side in another terminal:
  btop
EOF
