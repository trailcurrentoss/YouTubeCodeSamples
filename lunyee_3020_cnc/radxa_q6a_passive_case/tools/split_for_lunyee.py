#!/usr/bin/env python3
"""Split FreeCAD grbl_post G-code into one file per tool for a LUNYEE 3020 Nova.

The 3020 Nova's 710 W trim router is switched and speed-set by hand: the
controller cannot start it or set its speed. So every file this writes:

  * is named for the speed-dial setting the tool needs (..._DIAL-3.nc),
  * repeats that setting in its header,
  * stops with M0 before the first cut, so you can set the dial and switch
    the router on, then press Cycle Start,
  * lifts to the clearance height and stops with M5/M2 at the end.

GRBL rejects M6, so tools are split into separate files instead: change the
bit, re-zero Z on the plate top, run the next file.

Dial speeds are not published by LUNYEE. The table below assumes the six
steps are spread evenly over the advertised 6,000-30,000 rpm range. Measure
yours with a tachometer and edit DIAL_RPM; the feeds were chosen so that
+/-20 % of spindle speed still gives a usable chipload.

Usage:
  python3 split_for_lunyee.py <posted.nc> --part <name> --out <dir>
"""
import argparse
import math
import os
import re

DIAL_RPM = {1: 6000, 2: 10800, 3: 15600, 4: 20400, 5: 25200, 6: 30000}
RAPID_MM_MIN = 3000.0          # estimate only; the 3020 Nova tops out at 5000
CLEARANCE_Z = 5.0

TC_START = re.compile(r"^\(Begin operation: (TC_\S+)\)\s*$")
TC_LABEL = re.compile(r"^\(TC: (.*)\)\s*$")
S_WORD = re.compile(r"\bM3 S(\d+(?:\.\d+)?)")
POSTAMBLE = "(Begin postamble)"


def closest_dial(rpm):
    return min(DIAL_RPM, key=lambda d: abs(DIAL_RPM[d] - rpm))


def slug(label, tool_no):
    lab = label.lower()
    if "drill" in lab:
        m = re.search(r"(\d+(?:[.p]\d+)?)\s*mm", lab)      # "2.5mm" or "2p5mm"
        return "T%d-%smm-drill" % (tool_no, m.group(1).replace("p", ".") if m else "")
    if "1/4" in lab or "quarter" in lab:
        return "T%d-quarter-inch-endmill" % tool_no
    if "1/8" in lab or "eighth" in lab:
        return "T%d-eighth-inch-endmill" % tool_no
    return "T%d-tool" % tool_no


def run_time(lines):
    """Minutes, straight-line approximation of feeds plus rapids."""
    x = y = z = 0.0
    f = 100.0
    t = 0.0
    for l in lines:
        if l.startswith("("):
            continue
        m = dict(re.findall(r"([XYZF])(-?[\d.]+)", l))
        nx, ny, nz = float(m.get("X", x)), float(m.get("Y", y)), float(m.get("Z", z))
        if "F" in m:
            f = float(m["F"])
        d = math.dist((x, y, z), (nx, ny, nz))
        if re.match(r"G0?0\b", l):
            t += d / RAPID_MM_MIN
        elif re.match(r"G0?[123]\b", l):
            t += d / f
        x, y, z = nx, ny, nz
    return t


def split(path):
    """Return [(tc_label, tool_no, rpm, body_lines)] in program order."""
    lines = open(path).read().splitlines()
    sections, cur = [], None
    for i, l in enumerate(lines):
        if l.strip() == POSTAMBLE:
            break
        m = TC_START.match(l)
        if m:
            cur = {"label": m.group(1), "tool": None, "rpm": None, "body": []}
            sections.append(cur)
            continue
        if cur is None:
            continue
        lm = TC_LABEL.match(l)
        if lm:
            cur["label"] = lm.group(1)
        tm = re.match(r"^\( M6 T(\d+) \)", l)
        if tm:
            cur["tool"] = int(tm.group(1))
        sm = S_WORD.search(l)
        if sm and cur["rpm"] is None:
            cur["rpm"] = float(sm.group(1))
            continue
        if l.startswith("(Begin toolchange)") or l.startswith("( M6") or l.startswith("(Finish operation: TC"):
            continue
        cur["body"].append(l)
    return sections


def write(sections, part, out_dir, source, zero):
    os.makedirs(out_dir, exist_ok=True)
    written = []
    n = len(sections)
    for k, s in enumerate(sections, start=1):
        dial = closest_dial(s["rpm"])
        name = "%s_%dof%d_%s_DIAL-%d.nc" % (part, k, n, slug(s["label"], s["tool"]), dial)
        minutes = run_time(s["body"])
        ops = [m.group(1) for m in (re.match(r"^\(Begin operation: (.*)\)", l) for l in s["body"]) if m]
        head = [
            "(%s)" % name,
            "(Generated from %s by split_for_lunyee.py)" % os.path.basename(source),
            "(Machine: LUNYEE 3020 Nova, 710 W trim router, manual speed dial)",
            "(TOOL T%d: %s)" % (s["tool"], s["label"]),
            "(SPINDLE: SET THE TRIMMER DIAL TO %d  [about %d rpm; program asks for %d])"
            % (dial, DIAL_RPM[dial], s["rpm"]),
            "(ZERO: %s)" % zero,
            "(Operations: %s)" % ", ".join(ops),
            "(Estimated run time: %d min)" % round(minutes),
            "G17 G90",
            "G21",
            "G54",
            "G0 Z%.3f" % CLEARANCE_Z,
            "M3 S%d" % s["rpm"],
            "(Set the trimmer dial to %d and switch it ON, then press Cycle Start)" % dial,
            "M0",
        ]
        tail = ["G0 Z%.3f" % CLEARANCE_Z, "M5", "(Switch the trimmer OFF)", "G17 G90", "M2", ""]
        p = os.path.join(out_dir, name)
        with open(p, "w") as fh:
            fh.write("\n".join(head + s["body"] + tail))
        written.append((p, dial, minutes))
    return written


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("posted", help="G-code posted by FreeCAD's grbl post processor")
    ap.add_argument("--part", required=True, help="file name prefix, e.g. base or lid")
    ap.add_argument("--out", required=True, help="output directory")
    ap.add_argument("--zero", default="X0 Y0 = front-left corner of the plate, Z0 = top of the plate",
                    help="work-zero description written into each file header")
    a = ap.parse_args()
    secs = split(a.posted)
    if not secs:
        raise SystemExit("no tool sections found in %s" % a.posted)
    for p, dial, minutes in write(secs, a.part, a.out, a.posted, a.zero):
        print("%-60s dial %d  ~%d min" % (os.path.basename(p), dial, round(minutes)))


if __name__ == "__main__":
    main()
