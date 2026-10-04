# Torsion Table CAD

FreeCAD CAM jobs for cutting the ribs of a **torsion box table** on a
LowRider 4 (LR4) CNC router. A torsion box is a grid of thin ribs sandwiched
between two skins. It is light, and it stays flat because the grid resists
twisting, which makes it a good base for a CNC or a workbench.

The ribs are 3" (76.2 mm) strips cut from two 4×8 sheets of ¾" plywood.

## What's in this folder

| File | In the repo? | What it is |
|---|---|---|
| `lr4_cam.FCStd` | Yes (Git LFS) | The CAM file: stock, tools, toolpaths and the parameter spreadsheet for every sheet. |
| `lr4_table_clean.FCStd` | **No** | The full table model the CAM file takes its rib shapes from. At about 180 MB it is too large to keep in git. |

`lr4_cam.FCStd` doesn't hold the rib shapes itself. It **links** to them in
`lr4_table_clean.FCStd`. Without that file next to it, FreeCAD opens the CAM
file but reports missing link targets and the jobs have nothing to cut.

### Download the full model

Download **`TorsionTable.zip`** from the
[torsion-table-release](https://github.com/trailcurrentoss/YouTubeCodeSamples/releases/tag/torsion-table-release) page. It contains both
`lr4_table_clean.FCStd` and `lr4_cam.FCStd`, already side by side. Unzip it
and open `lr4_cam.FCStd` from there. If you only cloned this repo, you'll
still need the zip for the table model.

### Getting the CAM file out of Git LFS

You only need this if you're using the copy in the repo rather than the zip.
`.FCStd` files are stored with [Git LFS](https://git-lfs.com/). If
`lr4_cam.FCStd` is only a few hundred bytes after cloning, you have the LFS
pointer instead of the real file:

```bash
git lfs install
git lfs pull
```

## The three jobs

Open the file and look under the **CAM** jobs in the tree view. There is one
job per piece of stock:

| Job | Stock | Parts |
|---|---|---|
| **Coupon - joint fit test** | A band across the top of sheet 1 | One cross rib and one long rib, cut first to check that the slots fit |
| **Sheet 1 - 12 cross ribs** | 4×8 sheet | 12 cross ribs, one per lane |
| **Sheet 2 - 8 long-B + 8 long-A** | 4×8 sheet | 16 long ribs |

**Cut the coupon first.** Check that the two test pieces slot together before
you commit a full sheet. The coupon uses up the top of sheet 1, so the sheet 1
ribs start below it (from Y = 380 mm).

Each job runs the same operations in order:

1. **SCORE.** A shallow pass that traces every outline, so you can see the
   layout on the board before cutting through.
2. **STOP.** A pause to drive hold-down screws. Put them where the score lines
   show they won't be hit, so parts stay put once they are cut free.
3. **CUT.** The full-depth cut, with dogbones in the inside corners so the
   rib slots accept a square-edged mating rib.

## Parameters

Everything adjustable lives in the **CAM parameters** spreadsheet, and the
jobs read their values from it. Change a value there, recompute, and the
toolpaths follow.

| Setting | Value | Notes |
|---|---|---|
| Sheet size | 1219.2 × 2438.4 mm | 4×8 ft |
| Material thickness | 19.05 mm | ¾" plywood |
| Rib height | 76.2 mm | 3" |
| Gap between parts | 10 mm | |
| Tool | 6.35 mm single-flute O-flute | ¼" |
| Spindle | 12,000 rpm | Router speed dial at 2 |
| Feed, sheet 1 | 3000 mm/min XY, 600 mm/min plunge | |
| Feed, sheet 2 | 4500 mm/min XY, 900 mm/min plunge | |
| Step-down | 2 mm | The tested maximum. Don't raise it. |
| Cut-through pad | 2 mm | Extra depth below the part |
| Surface variance | 14 mm | **Measured on one specific table. Re-measure yours.** |

### Measure your own surface variance

Z zero is set at the **highest** point of the spoilboard. The surface variance
setting says how far below that the lowest point sits, and the cut depth adds
it on top so the low spots still cut all the way through. The 14 mm here is
for one particular spoilboard and is almost certainly wrong for yours. Measure
the low spot on your machine, enter it (with a little padding), and recompute.
A flatter spoilboard means fewer passes and less wasted depth.

## Opening it

1. Install [FreeCAD](https://www.freecad.org/) 1.0 or newer.
2. Download and unzip `TorsionTable.zip` from the
   [release page](https://github.com/trailcurrentoss/YouTubeCodeSamples/releases/tag/torsion-table-release).
3. Open `lr4_cam.FCStd`, then switch to the **CAM** workbench.
4. Change any values you need in **CAM parameters**, then press **Recompute**.
5. Select a job and use **Post Process** to write G-code for your controller.

Check every toolpath in the simulator before running it on the machine.
