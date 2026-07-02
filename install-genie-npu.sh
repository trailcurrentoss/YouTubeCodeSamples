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

echo "==> [1/8] Installing btop + jq (btop for CPU visualization, jq for stream parsing)"
sudo apt-get update -qq
sudo apt-get install -y -qq btop jq

echo "==> [2/8] Installing modelscope"
pip3 install --quiet --break-system-packages modelscope
export PATH="${HOME}/.local/bin:${PATH}"

echo "==> [3/8] Downloading ${MODEL_REPO} (~1.3 GB)"
cd "${HOME}"
modelscope download --model "${MODEL_REPO}" --local-dir "./${MODEL_DIR_NAME}"

# The modelscope bundle was retuned in mid-2026 to prioritize first-token
# latency: poll:true busy-spins on shared memory waiting for the DSP, and
# perf_profile:burst keeps the DSP pinned at max power. On the Q6A that
# saturates three big cores at idle (~180% CPU, thermal throttling). Revert
# both to the earlier tuning so genie-server idles near 0% CPU.
echo "==> [4/8] Patching HTP config for quiet idle (poll=false, perf_profile=balanced)"
sed -i 's/"perf_profile": "burst"/"perf_profile": "balanced"/' \
    "${MODEL_DIR}/htp_backend_ext_config.json"
sed -i 's/"poll": true/"poll": false/' \
    "${MODEL_DIR}/htp-model-config-llama32-1b-gqa.json"

echo "==> [5/8] Fetching genie_server.py from TrailCurrent Peregrine"
curl -fsSL -o "${HOME}/genie_server.py" "${PEREGRINE_RAW}/src/genie_server.py"

echo "==> [6/8] Installing systemd unit (retargeted for user '${USER}')"
curl -fsSL "${PEREGRINE_RAW}/config/genie-server.service" \
  | sed -e "s|/home/trailcurrent|${HOME}|g" \
        -e "s|User=trailcurrent|User=${USER}|g" \
        -e "s|Llama3.2-1B-1024-v68|${MODEL_DIR_NAME}|g" \
  | sudo tee /etc/systemd/system/genie-server.service >/dev/null

echo "==> [7/8] Enabling and starting genie-server"
sudo systemctl daemon-reload
sudo systemctl enable --now genie-server

echo "==> [8/8] Waiting for NPU warmup (up to 120s)"
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

Try it (streams token-by-token like ChatGPT):
  curl -sN http://localhost:11434/api/chat \\
    -d '{"messages":[{"role":"user","content":"Explain what a Large Language Model is and how it works in lay person terms."}],"stream":true}' \\
    | jq --unbuffered -j '.response'
  echo

Watch NPU/CPU load side-by-side in another terminal:
  btop
EOF
