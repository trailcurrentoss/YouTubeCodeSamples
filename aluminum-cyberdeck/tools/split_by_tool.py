#!/usr/bin/env python3
"""Split FreeCAD grbl_post G-code into one file per tool, for two kinds of machine.

FreeCAD posts a whole CAM job as one file with M6 tool changes. Stock GRBL
rejects M6, and many senders do not handle it, so this writes one file per
tool instead: change the bit, re-zero Z, run the next file.

Two variants are written for every tool:

  GRBL_SPD   Spindle speed is set by the program (M3 S...). With --max-rpm, a
             speed above your spindle's maximum is capped, and the feeds in
             that file are scaled by the same ratio so the chipload is kept.

  MAKITA     For a router with a manual speed dial (Makita RT0701C, e.g. on a
             LowRider). The programmed speed is mapped to the closest dial
             setting, the feeds are scaled to that dial's real speed, and the
             dial number is put in the file name and header.

Usage:
  python3 split_by_tool.py <posted.nc> [more.nc ...] --out <dir> [--max-rpm N]
"""
import argparse
import math
import os
import re

# Makita RT0701C dial speeds, tachometer-measured (forum.v1e.com/t/makita-rt0701c-rpms/40651)
MAKITA_DIAL = {1: 9600, 2: 12000, 3: 16500, 4: 21500, 5: 26500, 6: 29500}

TC_START = re.compile(r"^\(Begin operation: TC[:_ ]\s*(.*)\)\s*$")
POSTAMBLE = "(Begin postamble)"
F_WORD = re.compile(r"F(-?\d+(?:\.\d+)?)")


def slug(tc_label, tool_no):
    lab = tc_label.lower()
    dia = re.search(r"(\d+(?:/\d+)?(?:\.\d+)?)\s*(in|mm)", lab)
    d = (dia.group(1).replace("/", "-") + dia.group(2)) if dia else "tool"
    if "drill" in lab:
        kind = "drill"
    elif "rough" in lab:
        kind = "endmill-rough"
    elif "finish" in lab:
        kind = "endmill-finish"
    else:
        kind = "endmill"
    return "T%d-%s-%s" % (tool_no, d, kind)


def feed_time(lines):
    """Minutes of feed moves (G1/G2/G3), straight-line approximation."""
    x = y = z = 0.0
    f = 1000.0
    t = 0.0
    for l in lines:
        if l.startswith("("):
            continue
        m = dict(re.findall(r"([XYZF])(-?[\d.]+)", l))
        nx, ny, nz = float(m.get("X", x)), float(m.get("Y", y)), float(m.get("Z", z))
        if "F" in m:
            f = float(m["F"])
        if re.match(r"G0?[123]\b", l):
            t += math.dist((x, y, z), (nx, ny, nz)) / f
        x, y, z = nx, ny, nz
    return t


def scale_feeds(lines, ratio):
    if abs(ratio - 1.0) < 1e-9:
        return list(lines)
    out = []
    for l in lines:
        if l.startswith("("):
            out.append(l)
        else:
            out.append(F_WORD.sub(lambda m: "F%.3f" % (float(m.group(1)) * ratio), l))
    return out


def split(path):
    lines = open(path).read().splitlines()
    starts = [i for i, l in enumerate(lines) if TC_START.match(l)]
    post = next(i for i, l in enumerate(lines) if l.strip() == POSTAMBLE)
    preamble = [l for l in lines[:starts[0]] if not l.startswith("(Output Time")]
    postamble = lines[post:]
    sections = []
    for k, s in enumerate(starts):
        e = starts[k + 1] if k + 1 < len(starts) else post
        body = lines[s:e]
        label = TC_START.match(body[0]).group(1)
        tool = rpm = None
        for l in body:
            m = re.match(r"^M6 T(\d+)", l)
            if m:
                tool = int(m.group(1))
            m = re.match(r"^M3 S(\d+(?:\.\d+)?)", l)
            if m and rpm is None:
                rpm = float(m.group(1))
        ops = [re.match(r"^\(Begin operation: (.*)\)", l).group(1) for l in body
               if l.startswith("(Begin operation:") and "TC:" not in l]
        sections.append(dict(label=label, tool=tool, rpm=rpm, body=body, ops=ops))
    return preamble, sections, postamble


def write_variant(base, out, preamble, sec, k, n, postamble, variant, target, dial=None):
    ratio = target / sec["rpm"]
    body = []
    for l in sec["body"]:
        if re.match(r"^M6 T\d+", l):
            body.append("(Tool T%d is installed before this file starts; no M6)" % sec["tool"])
        elif re.match(r"^M3 S", l):
            body.append("M3 S%d" % round(target))
        else:
            body.append(l)
    body = scale_feeds(body, ratio)
    if variant == "GRBL_SPD":
        tag = "GRBL_SPD-%drpm" % round(target)
        spindle = "Spindle speed is set by the program: S%d." % round(target)
    else:
        tag = "MAKITA-dial%d" % dial
        spindle = ("Set the Makita dial to %d (%d rpm) and switch the router ON before "
                   "starting. Switch it OFF when the file ends." % (dial, target))
    name = "%s__%dof%d_%s__%s.nc" % (base, k, n, slug(sec["label"], sec["tool"]), tag)
    hdr = [
        "(%s)" % name,
        "(Part file %d of %d. Tool: %s)" % (k, n, sec["label"]),
        "(%s)" % spindle,
        "(Feeds scaled x%.3f from the CAM's %d rpm to keep the chipload.)" % (ratio, round(sec["rpm"])),
        "(Before running: install this tool and re-zero Z. Z0 is the stock bottom / spoilboard: probe the stock top and set Z to the stock thickness. Keep X0 Y0.)",
        "(Operations: %s)" % ", ".join(sec["ops"]),
        "(Approx. feed time: %.0f min)" % feed_time(body),
    ]
    with open(os.path.join(out, name), "w") as fh:
        fh.write("\n".join(hdr + preamble + body + postamble) + "\n")
    return name


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="+")
    ap.add_argument("--out", required=True)
    ap.add_argument("--max-rpm", type=float, default=None,
                    help="cap for the GRBL_SPD variant (your spindle's maximum / $30)")
    a = ap.parse_args()
    for sub in ("grbl_spd", "makita"):
        os.makedirs(os.path.join(a.out, sub), exist_ok=True)
    for f in a.files:
        base = os.path.splitext(os.path.basename(f))[0]
        pre, secs, post = split(f)
        n = len(secs)
        for k, s in enumerate(secs, 1):
            tgt = min(s["rpm"], a.max_rpm) if a.max_rpm else s["rpm"]
            g = write_variant(base, os.path.join(a.out, "grbl_spd"), pre, s, k, n, post, "GRBL_SPD", tgt)
            dial = min(MAKITA_DIAL, key=lambda d: abs(MAKITA_DIAL[d] - s["rpm"]))
            m = write_variant(base, os.path.join(a.out, "makita"), pre, s, k, n, post, "MAKITA",
                              MAKITA_DIAL[dial], dial)
            print(g)
            print(m)


if __name__ == "__main__":
    main()
