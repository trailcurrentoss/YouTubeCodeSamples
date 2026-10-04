#!/bin/bash
# Build the Rotary HID companion Debian package.
#
#   companion/packaging/linux/build-deb.sh
#
# Output: dist/rotary-hid-companion_<version>_all.deb (repo-relative).
# Needs only dpkg-deb and fakeroot, both present on any Debian/Ubuntu box.
# The version comes from __version__ in rotary_hid_companion.py, so there is
# one place to bump it.
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
COMPANION="$(cd "$HERE/../.." && pwd)"
ROOT="$(cd "$COMPANION/.." && pwd)"
PKG=rotary-hid-companion

VERSION="$(sed -n 's/^__version__ = "\(.*\)"/\1/p' "$COMPANION/rotary_hid_companion.py")"
[ -n "$VERSION" ] || { echo "no __version__ in rotary_hid_companion.py" >&2; exit 1; }

command -v dpkg-deb >/dev/null || { echo "dpkg-deb not found" >&2; exit 1; }
command -v fakeroot >/dev/null || { echo "fakeroot not found: sudo apt install fakeroot" >&2; exit 1; }

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

# ---- files ---------------------------------------------------------------
install -Dm755 "$COMPANION/rotary_hid_companion.py" \
    "$STAGE/usr/lib/$PKG/rotary_hid_companion.py"
mkdir -p "$STAGE/usr/bin"
ln -s "../lib/$PKG/rotary_hid_companion.py" "$STAGE/usr/bin/$PKG"

install -Dm644 "$HERE/rotary-hid-companion.service" \
    "$STAGE/usr/lib/systemd/user/rotary-hid-companion.service"
install -Dm644 "$HERE/70-rotary-hid.rules" \
    "$STAGE/usr/lib/udev/rules.d/70-rotary-hid.rules"

install -Dm644 "$ROOT/docs/companion.md" "$STAGE/usr/share/doc/$PKG/README.md"
install -Dm644 "$ROOT/LICENSE" "$STAGE/usr/share/doc/$PKG/copyright"

# ---- control -------------------------------------------------------------
mkdir -p "$STAGE/DEBIAN"
for s in postinst prerm postrm; do
    install -m755 "$HERE/$s" "$STAGE/DEBIAN/$s"
done

# mktemp -d makes the staging root 0700 and the umask may leave directories
# group-writable. Both would be recorded in the package -- a 0700 "./" entry
# can clamp permissions on / at install time -- so normalise explicitly.
chmod 0755 "$STAGE"
find "$STAGE" -type d -exec chmod 0755 {} +

INSTALLED_KB="$(du -sk --exclude=DEBIAN "$STAGE" | cut -f1)"
cat > "$STAGE/DEBIAN/control" <<EOF
Package: $PKG
Version: $VERSION
Architecture: all
Maintainer: TrailCurrent <trailcurrentopensource@gmail.com>
Depends: python3 (>= 3.8), python3-serial, x11-utils, systemd, udev
Section: utils
Priority: optional
Installed-Size: $INSTALLED_KB
Description: Companion for the Rotary Macro Pad (rotary touchscreen USB keyboard)
 Watches which window has focus on an X11 desktop and tells the Rotary
 Macro Pad over USB serial, so the pad shows FreeCAD, Blender or Kdenlive
 shortcut keys to match. Installs a systemd user service that starts with
 every desktop session and a udev rule that gives the logged-in user
 access to the pad without joining the dialout group.
EOF

mkdir -p "$ROOT/dist"
OUT="$ROOT/dist/${PKG}_${VERSION}_all.deb"
fakeroot dpkg-deb --build --root-owner-group "$STAGE" "$OUT" >/dev/null
echo "Built $OUT"
echo "Install:  sudo apt install $OUT"
