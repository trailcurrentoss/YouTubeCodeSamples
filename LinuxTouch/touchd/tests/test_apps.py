"""Tests for .desktop parsing.

These cover the parts that silently produce a broken home screen rather than an
error: a field code left on the command line, an entry for an uninstalled
package, and an entry that asked not to be shown.
"""

from __future__ import annotations

import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from touchd.modules.apps import _exec_argv, _parse  # noqa: E402


def write(tmp_path: Path, body: str, name: str = "test.desktop") -> Path:
    path = tmp_path / name
    path.write_text(body)
    return path


class TestExecArgv:
    def test_strips_standalone_field_codes(self):
        # A literal "%U" on the command line is a filename to most programs.
        assert _exec_argv("thunderbird %U") == ["thunderbird"]
        assert _exec_argv("geany %F") == ["geany"]

    def test_strips_embedded_field_codes_but_keeps_the_flag(self):
        # Dropping the whole token here would lose --file= entirely.
        assert _exec_argv("viewer --file=%f") == ["viewer", "--file="]

    def test_keeps_ordinary_arguments(self):
        assert _exec_argv("chromium --new-window %U") == ["chromium", "--new-window"]

    def test_respects_quoting(self):
        assert _exec_argv('sh -c "echo hello"') == ["sh", "-c", "echo hello"]

    def test_unbalanced_quotes_do_not_raise(self):
        # A malformed Exec line must skip that entry, not take out the scan.
        assert _exec_argv('broken "unclosed') == []


class TestParse:
    def test_minimal_entry(self, tmp_path):
        entry = _parse(write(tmp_path, """
[Desktop Entry]
Type=Application
Name=Test App
Exec=/bin/sh
"""))
        assert entry is not None
        assert entry["name"] == "Test App"
        assert entry["argv"] == ["/bin/sh"]
        assert entry["terminal"] is False

    @pytest.mark.parametrize("line", ["NoDisplay=true", "Hidden=true"])
    def test_hidden_entries_are_skipped(self, tmp_path, line):
        assert _parse(write(tmp_path, f"""
[Desktop Entry]
Type=Application
Name=Hidden
Exec=/bin/sh
{line}
""")) is None

    def test_non_application_types_are_skipped(self, tmp_path):
        assert _parse(write(tmp_path, """
[Desktop Entry]
Type=Link
Name=A link
Exec=/bin/sh
""")) is None

    def test_missing_binary_is_skipped(self, tmp_path):
        # Debian leaves entries behind for removed packages often enough that
        # skipping this check puts dead icons on the grid.
        assert _parse(write(tmp_path, """
[Desktop Entry]
Type=Application
Name=Gone
Exec=/usr/bin/definitely-not-installed-12345
""")) is None

    def test_failing_tryexec_is_skipped(self, tmp_path):
        assert _parse(write(tmp_path, """
[Desktop Entry]
Type=Application
Name=Gone
TryExec=/usr/bin/definitely-not-installed-12345
Exec=/bin/sh
""")) is None

    def test_terminal_flag_is_read(self, tmp_path):
        entry = _parse(write(tmp_path, """
[Desktop Entry]
Type=Application
Name=Top
Exec=/bin/sh
Terminal=true
"""))
        assert entry["terminal"] is True

    def test_no_desktop_entry_section(self, tmp_path):
        assert _parse(write(tmp_path, "[Something Else]\nName=x\n")) is None

    def test_undecodable_file_does_not_raise(self, tmp_path):
        path = tmp_path / "bad.desktop"
        path.write_bytes(b"[Desktop Entry]\nName=\xff\xfe binary\nExec=/bin/sh\n")
        # Must return something or None, but never propagate an exception: one
        # bad file cannot be allowed to empty the whole app grid.
        _parse(path)
