# Machining

Both aluminum parts are cut on a 3-axis machine from 6061-T6 plate, using
only **1/4" and 1/8" shank tools**. The back is one setup. The front bezel is
two setups, with the part flipped between them. To buy the tools and plate,
see [tools-and-stock.md](tools-and-stock.md).

## G-code files

There is **one file per tool**: change the bit, re-zero Z, run the next file.
Stock GRBL rejects `M6` tool changes, and many senders do not handle them, so
none of the files contains one.

Every file comes in two versions:

| Folder | For | Spindle |
|---|---|---|
| [`gcode/grbl_spd/`](../gcode/grbl_spd/) | A machine whose spindle speed is set by the program, e.g. a Genmitsu running GRBL | `M3 S…` at the speed in the file name |
| [`gcode/makita/`](../gcode/makita/) | A machine with a hand-set router, e.g. a LowRider with a Makita RT0701C | You set the dial number in the file name and switch the router on and off yourself |

File names read `<part>__<k>of<n>_<tool>__<variant>.nc`, for example
`back_op1_inside_up__1of5_T1-1-4in-endmill-rough__MAKITA-dial3.nc`. Run them
in order of `k`. Each file's header repeats the tool, the spindle setting,
the operations it contains and its approximate cutting time.

### Back tub: `back_op1_inside_up`, one setup

| # | Tool | Operations | GRBL_SPD | Makita | Time |
|---|---|---|---|---|---|
| 1 | T1 1/4" rougher | Face the rim; adaptive rough of the cavity and port notches | 18 000 rpm | dial 3 | ~142 min |
| 2 | T2 1/4" finisher | Finish the walls, bosses and notch outlines | 18 000 rpm | dial 3 | ~5 min |
| 3 | T3 2.5 mm drill | 11 lid-screw tap holes in the rim | 5000 rpm | dial 1 | ~1 min |
| 4 | T6 2.5 mm drill, long reach | 10 tap holes in the floor bosses (see below) | 5000 rpm | dial 1 | ~1 min |
| 5 | T1 1/4" rougher | Widen the outer slot, then cut the outer profile with tabs | 18 000 rpm | dial 3 | ~18 min |

### Front bezel, setup A: `front_opA_inside_up`

| # | Tool | Operations | GRBL_SPD | Makita | Time |
|---|---|---|---|---|---|
| 1 | T1 1/4" rougher | Face; adaptive rough of the screen pocket; clear the window through the lip | 18 000 rpm | dial 3 | ~79 min |
| 2 | T2 1/4" finisher | Finish the screen pocket with dogbone corners; finish the window edge | 18 000 rpm | dial 3 | ~3 min |
| 3 | T4 1/8" drill | 11 lid-screw holes, Ø3.175 through | 6000 rpm | dial 1 | ~1 min |
| 4 | T1 1/4" rougher | Widen the outer slot, then cut the outer profile with tabs | 18 000 rpm | dial 3 | ~7 min |

### Front bezel, setup B: `front_opB_outside_up`

| # | Tool | Operations | GRBL_SPD | Makita | Time |
|---|---|---|---|---|---|
| 1 | T5 1/8" endmill | 11 counterbores, Ø5.8 × 2.3 mm (helical) | 20 000 rpm | dial 4 | ~1 min |

The times are feed moves only. Rapids, pecking and your tool changes add to
them. The Makita versions take about 8 % longer, because dial 3 is 16 500
rpm and the feeds are scaled down to match.

### Speeds and feeds in the two versions

The CAM is programmed for 18 000 rpm on the 1/4" tools. The two versions
differ only in spindle speed and feed:

- **GRBL_SPD** uses the programmed speeds. It assumes the spindle can reach
  18 000–20 000 rpm and that GRBL's `$30` equals the spindle's maximum
  speed. If yours tops out lower, see below.
- **Makita** uses the closest dial setting. The feeds in that file are
  scaled by (dial rpm ÷ programmed rpm), so each cutting edge takes the same
  size of chip. The dial speeds are tachometer readings of an RT0701C
  ([V1 Engineering forum](https://forum.v1e.com/t/makita-rt0701c-rpms/40651)):

| Dial | 1 | 2 | 3 | 4 | 5 | 6 |
|---|---|---|---|---|---|---|
| rpm | 9 600 | 12 000 | 16 500 | 21 500 | 26 500 | 29 500 |

With the Makita, the controller's `M3`/`M5` do nothing. Set the dial and
switch the router on before you start each file, and off when it finishes.
The drill files ask for dial 1 (9600 rpm), the router's slowest speed; the
feeds are scaled up so carbide drills cut cleanly at that speed.

### Spindle slower than 18 000 rpm

Many small Genmitsu spindles stop at 10 000–12 000 rpm. Regenerate the
GRBL_SPD files with a speed cap, and the feeds are scaled down by the same
ratio:

```bash
# post the three jobs from FreeCAD first (CAM → Post Process), then:
python3 tools/split_by_tool.py back_op1_inside_up.nc front_opA_inside_up.nc \
    front_opB_outside_up.nc --out gcode --max-rpm 12000
```

[`tools/split_by_tool.py`](../tools/split_by_tool.py) also rebuilds the
Makita files, so run it after any change to the CAM.

## Before you cut

- **Work zero:** X0 Y0 is the **centre of the stock**. Z0 is the **bottom of
  the stock**, the spoilboard surface. Probe the top of the stock and set Z
  to the stock thickness: 28.575 mm for the back, 9.525 mm for the front. If
  your plate measures differently, set `stock_back_t` / `stock_front_t` in the
  spreadsheet and post again (see
  [tools-and-stock.md](tools-and-stock.md#if-your-plate-is-a-different-thickness)).
- **Re-zero Z after every tool change,** at the start of each file. Keep X0
  Y0.
- Every outer profile goes **0.3 mm into the spoilboard**. Use a sacrificial
  spoilboard.
- **No toolpath has been run on a machine yet.** Simulate each file in your
  sender, or in FreeCAD (CAM → Simulator), and cut one trial part in wax, MDF
  or HDPE first.
- Use air blast or mist, especially in the adaptive passes.

## Tools

All shanks are 1/4" or 1/8".

| T | Tool | Used for | CAM speed | Feed / plunge |
|---|---|---|---|---|
| 1 | 1/4" 3-flute carbide square endmill, **1-1/4" flute length** (roughing) | Facing, adaptive roughing, window clearing, slot widening, outer profiles | 18 000 rpm | 1000 / 200 mm/min |
| 2 | 1/4" 3-flute carbide square endmill, 1-1/4" flute length (finishing; same spec as T1, kept sharp) | Wall and boss finishing, notch outlines, screen pocket with dogbones, window edge | 18 000 rpm | 900 / 200 mm/min |
| 3 | 2.5 mm carbide drill, 1/8" shank | 11 rim tap holes in the back | 5000 rpm | 120 mm/min, 1.5 mm pecks |
| 4 | 1/8" carbide drill, 1/8" shank | 11 lid-screw holes in the front | 6000 rpm | 150 mm/min, 1.5 mm pecks |
| 5 | 1/8" 2–3 flute carbide square endmill | 11 counterbores | 20 000 rpm | 500 / 120 mm/min |
| 6 | 2.5 mm carbide drill, 1/8" shank, **long reach (≥ 27 mm stick-out)** | 10 tap holes in the floor bosses | 5000 rpm | 120 mm/min, 1.5 mm pecks |

T1 and T2 are the same tool. One does all the roughing; the other stays
sharp for the finishing passes, which set the final size.

Adaptive roughing runs at 20 % stepover (1.27 mm) with a 6.5 mm depth of cut
on the back, and 6.3 mm on the bezel, entering on a 3° helix. That is
conservative for a 1/4" carbide endmill in 6061. If your machine chatters,
lower the step-down in the operation and post again.

**Why the profiles have two passes:** a 1/4" tool cutting a 1/4"-wide slot
29 mm deep packs with chips. Each outer profile therefore starts with a
**widening pass** 3 mm further out, which stops just above the tabs. The
final pass then follows the part outline through to the spoilboard and
leaves the tabs. The slot is 9.35 mm wide for most of its depth.

### Reach for the floor holes (T6)

The standoff and tray bosses are on the floor, 6–8 mm from walls that rise
to 25.5 mm. To keep the collet nut above the rim, the drill must **stick out
at least 27 mm** from the collet. A common 38 mm-long 1/8"-shank drill cannot
do that and still be gripped safely.

- **With a long drill** (2-1/2" OAL or more; see
  [tools-and-stock.md](tools-and-stock.md#drills)): run file 4.
- **Without one:** skip file 4. After machining, drill the 10 holes by hand
  on a drill press with an ordinary 2.5 mm jobber drill, centred on each
  boss. The board's mounting holes are Ø3.5, so a little error is fine.

The rim holes (file 3) are open from above, so any short drill works there.

## Back tub: one setup

**Stock:** 6061-T6 plate, 1-1/8" (28.575 mm) thick, at least 292.7 × 201.5
mm. Order 12" × 12".

**Workholding:** low-profile toe clamps on the stock margin, outside the
profile path. The widening pass comes within 137.5 mm of X0 and 92 mm of Y0
(tool centre), so keep clamps beyond ±146 mm in X and ±101 mm in Y. Double-sided
tape under the middle of the plate also helps.

What the files do:

- **Facing** covers only the rim, where the bezel seats. The cavity and the
  outside of the part are cut away anyway, so the plate's top surface stays
  everywhere else until those cuts reach it.
- **Roughing** (adaptive, T1) clears the cavity and all three port notches
  from the top of the plate down to the 2.5 mm floor, in four passes, leaving
  0.3 mm on the walls. It steers around the 10 bosses.
- **Finishing** (T2) takes the walls, boss sides and notch outlines to size.
  Inside corners are 4 mm radius; notch corners are the 1/4" tool's radius.
- **Profile** (T1): a widening pass, then the outline through to −0.3 mm with
  six tabs, 8 mm wide and 1.2 mm high (`tab_h` = 1.8 mm above the final depth).

**After the machine:** break the tabs, file the nubs flush, and deburr. If
you skipped file 4, drill the 10 floor holes now. Then tap all 21 holes
M3 × 0.5 with a **spiral-flute, semi-bottoming tap**. Use a tap guide to keep
it square, use cutting fluid, and back the tap out often to clear chips:

- **Rim (11):** 8 mm deep. The M3 × 12 lid screws use 6.5 mm of thread.
- **Standoffs (4)** and **tray/battery bosses (6):** 6 mm and 3.7 mm deep.
  These are shallow blind holes, so go gently near the bottom.

## Front bezel: setup A, inside face up

**Stock:** 3/8" (9.525 mm) 6061-T6 plate, at least 292.7 × 201.5 mm. Order
12" × 12".

The pocket that holds the screen, the window, and the screw holes are all
machined from the inside. Mount the plate so the face that will be the
**outside of the tablet is down on the spoilboard**. The model is flipped
end over end about the long (X) axis for this setup.

- **Facing** takes the inside face down to 7.8 mm thick.
- **Pocket roughing** (adaptive) clears the 246.7 × 155.5 mm screen pocket
  down to the 1.5 mm lip.
- **Window clearing** (adaptive) mills the window through the lip to −0.3 mm.
  The window is milled away entirely, so there is no slug to catch.
- **Pocket finishing** (T2) has a dogbone dress-up, which adds corner reliefs
  so the square-cornered screen seats fully. The window edge is finished to a
  3.5 mm corner radius.
- **Screw holes:** T4 drills 11 holes, 1/8" (3.175 mm) through. **Open them
  to 3.4 mm (#29) by hand afterwards.** An M3 screw passes 3.175 mm, but 3.4
  gives room for positioning error between the two parts.
- **Profile:** a widening pass, then the outline with six tabs.

The bezel is a thin frame once the window is cleared. Keep the clamps on the
stock margin and do not over-tighten them.

## Front bezel: setup B, outside face up

Break the bezel out of its frame, file the tabs flush, open the 11 holes to
3.4 mm, and turn it over so the outside face is up.

**Registration:** the counterbores must line up with the holes drilled in
setup A. Put X0 Y0 at the **centre of the part**. The easiest way is two 3 mm
dowel pins in the spoilboard, at the positions of two diagonally opposite
screw holes, with the bezel dropped onto them: for example X +126.16,
Y +80.58 and X −126.16, Y −80.58, in setup B coordinates. The bottom-right
corner has no hole there. Or edge-find the outline and use its centre. Clamp
at the edges.

**Stock height:** 7.8 mm. Z0 is the spoilboard.

The counterbores are only 2.3 mm deep, so no move goes below Z 0. That is
expected.

## Changing the CAM

All three jobs are live in FreeCAD's CAM workbench (`Job_Back_Op1_InsideUp`,
`Job_Front_OpA_InsideUp`, `Job_Front_OpB_OutsideUp`). Double-click an
operation to edit it. To make new files:

1. **Post each job.** Use CAM → Post Process, or post from a script. Each job
   posts with `--tool-change`, so the tool sections are marked.
2. **Split the output.** Run `tools/split_by_tool.py` on the three posted
   files, as shown above.
3. **Check the tabs.** After any change to an outer profile, open
   `DressupTag` / `DressupTag001` and confirm that six tabs are enabled.
   FreeCAD disables tabs that no longer sit on the toolpath, and gives no
   warning. In the profile file you can confirm them in the G-code: there
   should be moves at Z 1.5.

If you change the spreadsheet, see [design.md](design.md#after-any-change).
The two front jobs use a flipped or offset copy of the bezel, with each
job's stock placed by hand. If you change the bezel's thickness, check that
each front job's stock still sits at Z 0 to the stock top.
