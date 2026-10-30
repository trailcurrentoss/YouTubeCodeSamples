# Aluminum Cyberdeck Tablet

A 10.1" tablet built around the **Radxa Dragon Q6A**. The two shell halves
are CNC-machined from aluminum and the port covers are 3D printed. It runs
from eight 18650 cells charged over USB-C. The model, the printed parts and
the CAM toolpaths are all in one parametric FreeCAD file, and the G-code is
already posted.

It is designed for a **3-axis machine**, which cannot cut holes in a side
wall. Every port opening is a notch milled straight down from the rim. Each
notch is T-shaped seen from above, and a printed insert of the same shape
slides down into it. The wall's outer skin stops the insert being pushed out,
and the front bezel clamps it in place once the lid is screwed on.

| | |
|---|---|
| Outside size | 262.7 × 171.5 × 33.3 mm |
| Aluminum parts | Back tub (~690 g) and front bezel (~170 g), 6061-T6 |
| Screen | [Radxa Display 10 FHD](https://docs.radxa.com/en/accessories/display/lcd-10-fhd/lcd-10-fhd-product): 10.1", 1920×1200, 10-point touch, MIPI DSI |
| Battery | 4S2P 18650, 14.4 V nominal, 80–100 Wh depending on the cells |
| Runtime | About 10 h of typical use, 6 h under heavy load (Samsung 35E cells; [estimate](docs/power.md#runtime-estimate)) |
| Charging | USB-C PD, about 2 h with a 65 W charger |
| Exposed ports | 4× USB-A, Gigabit Ethernet, HDMI, USB-C, 3.5 mm audio, USB-C charge input, power button |

No cable leaves the case and comes back in. The screen connects over its own
MIPI DSI ribbon, and the board sits in a corner so its connectors face the
walls directly.

## Documentation

| Page | What is in it |
|---|---|
| [Design](docs/design.md) | Layout, how the T-slot inserts work, the stack-up, the parametric spreadsheet, switching to the 8" screen |
| [Bill of materials](docs/bill-of-materials.md) | Everything to buy, with links: board, screen, cells, BMS, USB-C charger, 12 V regulator, switches and fasteners |
| [Power](docs/power.md) | Battery and charging circuit, wiring, settings to check before the first charge, runtime estimate |
| [Tools and stock](docs/tools-and-stock.md) | Endmills, drills, taps, workholding and aluminum plate, with links |
| [Machining](docs/machining.md) | Work zero, feeds and speeds, and each setup and operation for both parts |
| [3D printing](docs/printing.md) | The four printed parts, settings, orientation and fit |
| [Assembly](docs/assembly.md) | Building the pack, wiring, fitting the board and screen, closing up, first power-on |

## Files

| Path | What it is |
|---|---|
| `AluminumCyberdeck.FCStd` | Model, printed parts, reference components and the three CAM jobs |
| [`gcode/grbl_spd/`](gcode/grbl_spd/) | G-code, one file per tool, with spindle speed set by the program (e.g. a Genmitsu on GRBL) |
| [`gcode/makita/`](gcode/makita/) | The same files for a hand-set Makita RT0701C router (e.g. a LowRider). The dial setting is in each file name. |
| [`tools/split_by_tool.py`](tools/split_by_tool.py) | Rebuilds both G-code sets from FreeCAD's posted output, with an optional spindle speed cap |
| [`step/`](step/) | STEP files of both aluminum parts, for other CAM software |
| [`print/`](print/) | STL files of the three port inserts and the battery cradle |

## Build at a glance

1. **Buy** the electronics ([bill of materials](docs/bill-of-materials.md)),
   plus the plate and tools ([tools and stock](docs/tools-and-stock.md)).
   The Q6A and the screen were back-ordered at the time of writing.
2. **Machine** the back (one setup, ~2.8 h) and the bezel (two setups,
   ~1.5 h) with 1/4" and 1/8" shank tools only, then tap 21 M3 holes
   ([machining](docs/machining.md)).
3. **Print** three port inserts and the battery cradle in PETG or ASA
   ([printing](docs/printing.md)).
4. **Build** the 4S2P pack, wire the BMS, charger and 12 V regulator, fit the
   board and screen, and close up with 11 M3 screws
   ([power](docs/power.md), [assembly](docs/assembly.md)).

## Status

The model has been checked in FreeCAD:
- **No overlaps** between the parts and the reference components,
  including the real connector envelopes from Radxa's STEP model.
- **Every port opening** is clear through both the aluminum and its
  insert.
- **Every sketch** is fully constrained, and the model regenerates from the
  spreadsheet.

Not yet done:

- **No toolpath has been run on a machine.** Simulate the files and cut a
  trial in wax or MDF before aluminum.
- **Screen ribbon exit and window position:** Radxa publishes no mechanical
  drawing of the panel. Check where the FPC leaves it and whether the active
  area is centred ([design](docs/design.md#main-inputs)).
- **Electronics tray:** the BMS, charger and regulator have four mounting
  bosses but no designed tray yet.
- **Cooling:** nothing conducts the SoC's heat to the case yet
  ([power](docs/power.md#heat)).
- **Port recess:** the connectors sit about 10 mm behind the surface. Slim
  plugs fit, but bulky ones may not.

## Sources

- Board geometry: Radxa's official Dragon Q6A 3D STEP model, v1.21
  ([Radxa docs](https://docs.radxa.com/en/dragon/q6a))
- [Q6A MIPI DSI supported displays](https://docs.radxa.com/en/dragon/q6a/hardware-use/mipi-dsi)
  and [power header](https://docs.radxa.com/en/dragon/q6a/hardware-use/power-header)
- [Radxa Display 10 FHD](https://docs.radxa.com/en/accessories/display/lcd-10-fhd/lcd-10-fhd-product)
- Q6A power measurements: [Jeff Geerling's sbc-reviews](https://github.com/geerlingguy/sbc-reviews/issues/85)
  and [SBCwiki](https://sbcwiki.com/news/articles/radxa-dragon-q6a-full-review/)
