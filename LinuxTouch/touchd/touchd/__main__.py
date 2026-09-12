"""touchd entry point.

    python3 -m touchd --shell ../shell --port 8720

Paths are arguments rather than constants so the same daemon runs from a git
checkout during development and from /usr/share/linuxtouch on the device, with
no build step and no environment-specific branch in the code.
"""

from __future__ import annotations

import argparse
import logging
import os
from pathlib import Path

from aiohttp import web

from .server import build_app

DEFAULT_PORT = 8720


def _default_state_dir() -> Path:
    base = os.environ.get("XDG_STATE_HOME") or (Path.home() / ".local/state")
    return Path(base) / "linuxtouch"


def main() -> None:
    parser = argparse.ArgumentParser(prog="touchd", description="LinuxTouch daemon")
    parser.add_argument(
        "--shell", type=Path,
        default=Path(__file__).resolve().parents[2] / "shell",
        help="directory containing the shell bundle (index.html and friends)",
    )
    parser.add_argument("--state", type=Path, default=_default_state_dir(),
                        help="where settings.json is written")
    parser.add_argument("--port", type=int,
                        default=int(os.environ.get("LINUXTOUCH_PORT", DEFAULT_PORT)))
    parser.add_argument("--verbose", "-v", action="store_true")
    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.INFO,
        # No timestamp: this runs under systemd, which adds its own, and two
        # timestamps per line on a journal read on a 640x480 screen is noise.
        format="%(levelname)-7s %(name)s: %(message)s",
    )

    if not (args.shell / "index.html").is_file():
        # Fail loudly at startup rather than serving 404s to a blank screen and
        # leaving the user to guess whether the daemon or the shell is broken.
        raise SystemExit(f"no shell bundle at {args.shell} (index.html missing)")

    args.state.mkdir(parents=True, exist_ok=True)
    app = build_app(args.shell.resolve(), args.state.resolve())

    # 127.0.0.1 only, deliberately. See server.py.
    web.run_app(app, host="127.0.0.1", port=args.port, print=None,
                access_log=None)


if __name__ == "__main__":
    main()
