"""Entry point: ``python3 -m trailshell``."""
from __future__ import annotations

import argparse
import logging
import os
import signal
import sys

from . import _gi  # noqa: F401
from gi.repository import GLib, Gtk


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(prog="trailshell", description="TrailShell desktop")
    ap.add_argument("-v", "--verbose", action="store_true", help="debug logging")
    ap.add_argument("--version", action="store_true")
    args = ap.parse_args(argv)

    if args.version:
        from . import __version__
        print(f"trailshell {__version__}")
        return 0

    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.INFO,
        format="%(asctime)s %(levelname)-5s %(name)s: %(message)s",
        datefmt="%H:%M:%S",
    )
    log = logging.getLogger("trailshell")

    if not os.environ.get("DISPLAY"):
        log.error("no DISPLAY set - TrailShell is an X11 shell")
        return 1

    from .shell import Shell

    shell = Shell()
    shell.start()

    def shutdown(*_a):
        log.info("shutting down")
        shell.stop()
        Gtk.main_quit()
        return GLib.SOURCE_REMOVE

    # Unix signals have to go through the GLib loop; a bare handler would run
    # between GTK frames and leave X grabs behind.
    GLib.unix_signal_add(GLib.PRIORITY_DEFAULT, signal.SIGINT, shutdown)
    GLib.unix_signal_add(GLib.PRIORITY_DEFAULT, signal.SIGTERM, shutdown)

    try:
        Gtk.main()
    except KeyboardInterrupt:
        shutdown()
    return 0


if __name__ == "__main__":
    sys.exit(main())
