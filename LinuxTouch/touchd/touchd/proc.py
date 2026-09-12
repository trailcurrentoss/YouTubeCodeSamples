"""Subprocess helpers.

Every shell-out in LinuxTouch goes through here, for three reasons: a single
place to enforce a timeout, a single place that logs what was run, and a single
place to audit when asking "what does this daemon actually execute?"

Nothing in this module ever takes a string command line. `run()` takes an argv
list and never touches a shell, so a network name containing `;` or `$(...)`
is data rather than syntax. Several callers pass user-supplied SSIDs and
passwords straight through.
"""

from __future__ import annotations

import asyncio
import logging
import shutil

log = logging.getLogger(__name__)


class CommandError(RuntimeError):
    """A command exited non-zero, or was not installed, or timed out."""

    def __init__(self, argv: list[str], returncode: int, stderr: str):
        self.argv = argv
        self.returncode = returncode
        self.stderr = stderr.strip()
        super().__init__(
            f"{argv[0]} exited {returncode}"
            + (f": {self.stderr}" if self.stderr else "")
        )


async def run(argv: list[str], *, timeout: float = 10.0, check: bool = True) -> str:
    """Run argv, return stdout.

    Raises CommandError on non-zero exit when check is True, and always on a
    missing binary or a timeout. The timeout is not optional anywhere: `nmcli
    dev wifi list --rescan yes` can sit for tens of seconds on a busy band, and
    a hung call here would stall the whole asyncio loop's ability to answer the
    shell.
    """
    if shutil.which(argv[0]) is None:
        raise CommandError(argv, 127, f"{argv[0]} is not installed")

    log.debug("run: %s", " ".join(argv))
    proc = await asyncio.create_subprocess_exec(
        *argv,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )
    try:
        out, err = await asyncio.wait_for(proc.communicate(), timeout=timeout)
    except asyncio.TimeoutError:
        # Kill rather than terminate: these are short-lived query tools, none
        # of which have cleanup worth waiting for, and a TERM that is ignored
        # would leave the process to be reaped at daemon exit.
        proc.kill()
        await proc.wait()
        raise CommandError(argv, -1, f"timed out after {timeout}s") from None

    stdout = out.decode("utf-8", "replace")
    if check and proc.returncode != 0:
        raise CommandError(argv, proc.returncode or -1, err.decode("utf-8", "replace"))
    return stdout


def have(binary: str) -> bool:
    """True if a binary is on PATH.

    Used by modules to degrade honestly: a device without `gammastep` reports
    that brightness control is unavailable, rather than offering a slider that
    silently does nothing.
    """
    return shutil.which(binary) is not None
