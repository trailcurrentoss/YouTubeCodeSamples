#!/bin/bash
# provision.sh — turn a stock Raspberry Pi OS install into a LinuxTouch device.
#
# Run this ON the device, as root:
#
#     sudo scripts/provision.sh
#     sudo scripts/provision.sh --user pi        # explicit session user
#     sudo scripts/provision.sh --dry-run        # show what would change
#
# Idempotent: safe to re-run after every change, and re-running is the intended
# way to pick up a new shell or daemon during development. Nothing here is
# destructive except the config.txt overlay line, which is backed up first.
#
# The stock desktop is left completely intact. LinuxTouch is installed as an
# additional session, and the only thing that changes about the existing setup
# is which session the display manager logs into by default. Setting
# `autologin-session=labwc` in the lightdm drop-in gets the Pi desktop back.
set -euo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DRY_RUN=0
SESSION_USER="${SUDO_USER:-}"

PREFIX_SHARE=/usr/share/linuxtouch
PREFIX_BIN=/usr/local/bin
CONFIG_TXT=/boot/firmware/config.txt

PACKAGES=(
    sway                    # the compositor; see docs/architecture.md for why not labwc
    python3-aiohttp         # touchd's only third-party Python dependency
    gammastep               # brightness, since this panel has no backlight
    device-tree-compiler    # to build the GT911 overlay
    curl                    # bin/linuxtouch-nav POSTs with it
)

log()  { printf '\033[1;32m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m!! \033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31mxx \033[0m %s\n' "$*" >&2; exit 1; }

run() {
    if [ "$DRY_RUN" = 1 ]; then
        printf '   would run: %s\n' "$*"
    else
        "$@"
    fi
}

while [ $# -gt 0 ]; do
    case "$1" in
        --user)    SESSION_USER="${2:-}"; shift 2 ;;
        --user=*)  SESSION_USER="${1#*=}"; shift ;;
        --dry-run) DRY_RUN=1; shift ;;
        -h|--help) sed -n '2,14p' "$0"; exit 0 ;;
        *)         die "unknown option: $1" ;;
    esac
done

[ "$(id -u)" = 0 ] || die "must run as root: sudo $0"

# The session user is who lightdm logs in and who owns the shell's Chromium
# profile. It is asked for rather than defaulted to "pi", because a default
# that happens to match the author's own account is exactly the kind of local
# detail that has no business in a tracked file.
if [ -z "$SESSION_USER" ]; then
    die "cannot tell which user should own the session.
Pass it explicitly:  sudo $0 --user <username>
(SUDO_USER was empty, which happens when running as root directly.)"
fi
id "$SESSION_USER" >/dev/null 2>&1 || die "no such user: $SESSION_USER"

# ── 0. sanity checks, warnings only ──────────────────────────────────

log "Checking the platform"
MODEL="$(tr -d '\0' < /proc/device-tree/model 2>/dev/null || echo unknown)"
printf '   model:  %s\n' "$MODEL"
case "$MODEL" in
    *"Raspberry Pi 5"*) ;;
    *) warn "LinuxTouch is developed and tested on a Pi 5 only. Continuing anyway." ;;
esac

if ! grep -q '^VERSION_CODENAME=trixie' /etc/os-release 2>/dev/null; then
    warn "not Debian trixie; package names may differ"
fi

# 90%-full cards are the normal state of a stock Pi OS image on a small SD, and
# the package install below needs room. Checked because "apt failed with no
# space" halfway through provisioning leaves a half-installed device.
AVAIL_MB=$(($(df --output=avail -k / | tail -1) / 1024))
printf '   free:   %s MB on /\n' "$AVAIL_MB"
[ "$AVAIL_MB" -lt 300 ] && die "less than 300 MB free on /; expand the filesystem first (raspi-config)"
[ "$AVAIL_MB" -lt 800 ] && warn "less than 800 MB free; consider expanding the filesystem"

# ── 1. packages ──────────────────────────────────────────────────────

MISSING=()
for pkg in "${PACKAGES[@]}"; do
    dpkg-query -W -f='${Status}' "$pkg" 2>/dev/null | grep -q 'install ok installed' \
        || MISSING+=("$pkg")
done

if [ ${#MISSING[@]} -gt 0 ]; then
    log "Installing: ${MISSING[*]}"
    run apt-get update
    run env DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends "${MISSING[@]}"
else
    log "All packages already installed"
fi

command -v chromium >/dev/null 2>&1 \
    || warn "chromium is not installed; the shell cannot start without it"

# ── 2. GT911 touch overlay ───────────────────────────────────────────
#
# The vendor overlay works. This one exists only to drop the phantom 0x14 node
# that fails on every boot — see overlays/linuxtouch-gt911-overlay.dts.

log "Building and installing the GT911 overlay"
if [ "$DRY_RUN" = 0 ]; then
    dtc -@ -I dts -O dtb -o /tmp/linuxtouch-gt911.dtbo \
        "$REPO/overlays/linuxtouch-gt911-overlay.dts" 2>/dev/null \
        || die "dtc failed to build the overlay"
    install -m644 /tmp/linuxtouch-gt911.dtbo /boot/firmware/overlays/
    rm -f /tmp/linuxtouch-gt911.dtbo
fi

if [ -f "$CONFIG_TXT" ]; then
    if grep -q '^dtoverlay=linuxtouch-gt911' "$CONFIG_TXT"; then
        log "config.txt already uses linuxtouch-gt911"
    else
        log "Switching config.txt to the LinuxTouch overlay"
        run cp -n "$CONFIG_TXT" "$CONFIG_TXT.linuxtouch.bak"
        if grep -q '^dtoverlay=waveshare-35dpi' "$CONFIG_TXT"; then
            # Replace the -5b line, and comment out any -4b line. The vendor
            # .dtbo files stay installed: a replacement panel strapped to 0x14
            # needs them, and reverting is then a one-line edit.
            run sed -i \
                -e 's/^dtoverlay=waveshare-35dpi-5b/dtoverlay=linuxtouch-gt911/' \
                -e 's/^dtoverlay=waveshare-35dpi-4b/#dtoverlay=waveshare-35dpi-4b/' \
                "$CONFIG_TXT"
        else
            warn "no waveshare-35dpi overlay line found; appending ours"
            run bash -c "printf '\ndtoverlay=linuxtouch-gt911\n' >> '$CONFIG_TXT'"
        fi
    fi
    grep -q '^dtparam=i2c_arm=on' "$CONFIG_TXT" \
        || warn "dtparam=i2c_arm=on is missing from config.txt; touch will not work"
    grep -q '^dtoverlay=dwc2,dr_mode=host' "$CONFIG_TXT" \
        || warn "dtoverlay=dwc2,dr_mode=host is missing; the keyboard may not enumerate"
    grep -q '^dtparam=cooling_fan=on' "$CONFIG_TXT" \
        || warn "dtparam=cooling_fan=on is missing; the Active Cooler may not be detected"
else
    warn "$CONFIG_TXT not found; skipping boot configuration"
fi

# ── 3. the shell, the daemon, and the helpers ────────────────────────

log "Installing the shell bundle and daemon to $PREFIX_SHARE"
run install -d "$PREFIX_SHARE"
run rm -rf "$PREFIX_SHARE/shell" "$PREFIX_SHARE/touchd"
run cp -r "$REPO/shell" "$PREFIX_SHARE/shell"
run cp -r "$REPO/touchd" "$PREFIX_SHARE/touchd"
# Python bytecode from a previous version confuses nothing, but shipping it is
# untidy and it is owned by root inside a tree the daemon reads as a user.
run find "$PREFIX_SHARE/touchd" -name __pycache__ -type d -prune -exec rm -rf {} +

log "Installing helper commands to $PREFIX_BIN"
for helper in linuxtouch-nav linuxtouch-shell linuxtouch-session; do
    run install -m755 "$REPO/bin/$helper" "$PREFIX_BIN/$helper"
done

log "Installing the compositor config to /etc/linuxtouch/sway/config"
run install -d /etc/linuxtouch/sway
run install -m644 "$REPO/wm/sway/config" /etc/linuxtouch/sway/config

# ── 4. system configuration ──────────────────────────────────────────

log "Installing the session entry, sysctl, and logind drop-ins"
run install -m644 "$REPO/system/linuxtouch.desktop" /usr/share/wayland-sessions/linuxtouch.desktop
run install -m644 "$REPO/system/60-linuxtouch-sysrq.conf" /etc/sysctl.d/60-linuxtouch-sysrq.conf
run install -d /etc/systemd/logind.conf.d
run install -m644 "$REPO/system/70-linuxtouch-power.conf" /etc/systemd/logind.conf.d/70-linuxtouch-power.conf

# Apply the sysrq change now rather than waiting for a reboot: until it takes
# effect, Alt+Select can hard-reboot the device.
run sysctl --quiet -p /etc/sysctl.d/60-linuxtouch-sysrq.conf

log "Setting LinuxTouch as the default session for $SESSION_USER"
#
# THIS EDITS /etc/lightdm/lightdm.conf DIRECTLY, and it has to.
#
# LightDM's config precedence is the OPPOSITE of systemd's: it reads
#   1. /usr/share/lightdm/lightdm.conf.d/*.conf
#   2. /etc/lightdm/lightdm.conf.d/*.conf
#   3. /etc/lightdm/lightdm.conf          <-- LAST, and therefore WINS
#
# So a drop-in in lightdm.conf.d cannot override the main file. Raspberry Pi OS
# ships `autologin-session=rpd-labwc` in lightdm.conf, which silently beat a
# drop-in that looked completely correct: provisioning reported success, the
# board rebooted, and it came back into the stock desktop with no error
# anywhere. Verified on hardware 2026-09-05.
#
# The original file is backed up once, and the rollback is documented below.
LIGHTDM_CONF=/etc/lightdm/lightdm.conf
if [ -f "$LIGHTDM_CONF" ]; then
    run cp -n "$LIGHTDM_CONF" "$LIGHTDM_CONF.linuxtouch.bak"
    if [ "$DRY_RUN" = 0 ]; then
        # Replace the keys if present, append under [Seat:*] if not. The user
        # name is substituted at install time rather than shipped in a tracked
        # file, because a local account name has no business in the repository.
        for key_value in \
            "autologin-user=$SESSION_USER" \
            "autologin-session=linuxtouch" \
            "user-session=linuxtouch"
        do
            key="${key_value%%=*}"
            if grep -qE "^[#[:space:]]*${key}=" "$LIGHTDM_CONF"; then
                sed -i -E "s|^[#[:space:]]*${key}=.*|${key_value}|" "$LIGHTDM_CONF"
            else
                sed -i "/^\[Seat:\*\]/a ${key_value}" "$LIGHTDM_CONF"
            fi
        done
    fi
    # A drop-in from an earlier version of this script would be dead weight
    # that looks authoritative. Remove it rather than leave it to mislead.
    run rm -f /etc/lightdm/lightdm.conf.d/60-linuxtouch.conf
else
    warn "$LIGHTDM_CONF not found; cannot set the default session"
fi

log "Installing the touchd user service"
run install -d /etc/systemd/user
run install -m644 "$REPO/systemd/touchd.service" /etc/systemd/user/touchd.service
# Deliberately NOT enabled. sway starts it from an exec_always, after importing
# WAYLAND_DISPLAY and SWAYSOCK into the systemd user manager — enabling it would
# start it earlier than that and it would come up blind. See systemd/touchd.service.

# ── done ─────────────────────────────────────────────────────────────

cat <<EOF

$(log "Provisioning complete")

Installed:
  $PREFIX_SHARE/{shell,touchd}
  $PREFIX_BIN/linuxtouch-{nav,shell,session}
  /etc/linuxtouch/sway/config
  /usr/share/wayland-sessions/linuxtouch.desktop
  /etc/sysctl.d/60-linuxtouch-sysrq.conf
  /etc/systemd/logind.conf.d/70-linuxtouch-power.conf
  /etc/lightdm/lightdm.conf                        (session user: $SESSION_USER;
                                                    original at .linuxtouch.bak)
  /etc/systemd/user/touchd.service                 (started by sway, not enabled)
  /boot/firmware/overlays/linuxtouch-gt911.dtbo

A reboot is needed, because the overlay change and the logind drop-in both only
take effect at boot:

    sudo reboot

After it comes back, check:

    dmesg | grep -i goodix        # one "ID 911" line, and NO -EBUSY lines
    systemctl --user status touchd
    swaymsg -t get_outputs        # HDMI-A-1 at 640x480

You do not have to reboot to try it — restarting the display manager switches
the session immediately (it will close anything open on the panel):

    sudo systemctl restart lightdm

If the screen stays black, switch back to the stock desktop from an SSH session:

    sudo cp /etc/lightdm/lightdm.conf.linuxtouch.bak /etc/lightdm/lightdm.conf
    sudo systemctl restart lightdm
EOF
