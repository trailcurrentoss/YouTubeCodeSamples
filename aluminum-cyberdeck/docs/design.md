# Design

How the enclosure is laid out, why it is shaped the way it is, and how to
change it.

## The constraint that shapes everything

A 3-axis machine only cuts from above. It can mill a pocket into the top of a
plate, but it cannot cut a hole in the side of a wall without the part being
turned on its side and re-fixtured.

So every port opening in this enclosure is a **notch cut straight down from
the rim**. Seen from above, each notch is T-shaped:

```
        outside of case
   ─────────┐       ┌─────────    ← outer skin, 2.5 mm
            │       │
      ┌─────┘       └─────┐       ← flange pocket, 7 mm
      │                   │
   ───┘                   └───    ← inside face of the wall
        inside of case
```

The narrow part goes through the outer skin; that is the visible window. The
wider part sits behind it, inside the wall. A 3D-printed insert of the same
T shape slides down into the notch from above:

- The outer skin catches the insert's flange, so it cannot be pushed out of
  the case.
- The notch floor carries it from below.
- The front bezel sits on the rim and clamps it from above once the lid is
  screwed down.

Each insert carries cutouts sized for the connectors behind it. If a cable
does not fit, reprint the insert rather than re-machining aluminum.

The notch corners that face outward keep the radius the 1/4" cutter leaves
(3.175 mm). The inserts have 3.5 mm chamfers on those corners so they clear it.

## Layout

The tablet lies flat with the screen facing **+Z**. X is the long axis
(landscape), Y the short axis, and the origin is the centre of the tablet.

```
 +Y ┌───────────────────────────────────────────────────────┐
    │ ┌──────────────┐                                       │
    │ │ charger, BMS │                                       │
 L  │ │ 12 V reg.    │                                       │  R
 E ▓│ └──────────────┘                                       │  I
 F ▓│ ┌───────────────────────────┐                          │  G
 T  │ │ 4S2P 18650 cradle         │               ┌─────────┐│▓ H
    │ │ (2 × 4 cells)             │               │ Radxa   ││▓ T
    │ │                           │               │ Q6A     ││▓
    │ └───────────────────────────┘               └─────────┘│
 -Y └─────────────────────────────────────────▓▓▓▓▓▓────────┘
   -X                     BOTTOM                           +X
   ▓ = port notch with printed insert
```

- **Right wall:** two USB-A stacks (4 ports) and Gigabit Ethernet.
- **Bottom wall:** the board's USB-C, HDMI and 3.5 mm audio.
- **Left wall:** the USB-C charge input and a 12 mm power button.

The board sits in the lower-right corner, so both of its connector edges face
a wall. Its connectors stop 0.5 mm short of the inserts. No cable leaves the
case and comes back in, and the screen connects over its own MIPI DSI ribbon.

## Stack-up (Z)

| Z (mm) | What is there |
|---|---|
| 0 | Outside of the back |
| 2.5 | Floor (cavity bottom, notch floors) |
| 2.5–7.0 | Board standoffs (Ø6.5); the board's lowest underside part is at 3.5 |
| 7.0 | Underside of the Q6A PCB |
| 24.45 | Top of the USB stacks, the tallest part |
| 25.5 | Back rim, parting line |
| 25.85 | Back of the screen (0.35 mm foam tape below it) |
| 31.8 | Front of the screen glass |
| 33.3 | Front face of the bezel (1.5 mm lip over the glass) |

The thickness is set by the Q6A's USB stack. Nothing else could be thinner
without moving the board or dropping the stacked USB ports.

## Fastening

Eleven M3 low-head screws (DIN 7984) go through counterbores in the front
bezel into M3 tapped holes in the back's rim. The rim is 9.5 mm wide: the
screw line is 4 mm in from the outside, and the inner 1.5 mm overlaps the
screen's edge and clamps it against the bezel lip.

The screw positions avoid all three notches. The screw nearest the board
corner is moved along the bottom edge (`s_brx`) so it stays clear of the
right-hand notch's flange.

## The parametric model

`AluminumCyberdeck.FCStd` is built with PartDesign. Every sketch is fully
constrained, and every dimension is bound to an alias in the `Design`
spreadsheet. Change a cell, recompute, and the parts follow.

### Main inputs

| Alias | Default | Meaning |
|---|---|---|
| `scr_w`, `scr_h`, `scr_t` | 246.16, 155, 5.95 | Screen outline and thickness |
| `act_w`, `act_h` | 216.58, 135.36 | Screen active area (sets the window) |
| `win_dx`, `win_dy` | 0, 0 | Window offset, if the active area is not centred |
| `scr_clr` | 0.25 | Screen pocket clearance per side |
| `tape` | 0.35 | Foam tape behind the screen |
| `lip` | 1.5 | Bezel lip over the glass |
| `border` | 8 | Bezel width outside the screen pocket |
| `overlap` | 1.5 | How far the back rim overlaps the screen |
| `floor` | 2.5 | Back floor thickness |
| `back_h` | 25.5 | Height of the back tub |
| `stand_h` | 4.5 | Board standoff height |
| `skin`, `ear` | 2.5, 5 | Notch outer skin and flange length |
| `pm_r`, `pm_f` | 3, 4.5 | Window margin around the right and bottom port groups |
| `ins_clr` | 0.15 | Printed insert clearance per side |
| `scr_e`, `cb_d`, `cb_h` | 4, 5.8, 2.3 | Lid screw inset and counterbore |

Derived values are in the lower half of the sheet: `out_w`, `out_h`,
`total_t`, `wall`, the board position (`brd_x0`, `brd_y0`) and the notch
extents. The board position comes from the port overhangs measured on the
official Radxa STEP model, so the connectors always end up 0.5 mm from the
inserts.

### Using the 8" screen instead

The Radxa Display 8 HD is 199.22 × 127 × 5.9 mm, with a 172.22 × 107.64 mm
active area. Set those five values and recompute. At that size the battery
cradle no longer fits beside the board, so the battery has to be rearranged,
for example 3S2P or a different cradle layout.

### After any change

1. Recompute the document.
2. In each CAM job, check that every operation's base faces still point at
   the right features. Operations select faces by number, and a big change can
   renumber them.
3. Regenerate the operations and post again.
4. Export the STLs again if an insert or the cradle changed.

## Reference parts

The `References` group holds parts that are not manufactured. They are there
for interference checks:

- `REF_Radxa_Dragon_Q6A_envelope`: bounding boxes of every component on the
  official Radxa STEP (v1.21), deliberately a little oversized
- `REF_Radxa_Display_10_FHD`: the screen outline
- `REF_Cell_*`: the eight 18650 cells
- `REF_ChargerBoard`, `REF_ChargerUSBC`: a placeholder 50 × 30 mm charge board

With these, the model reports **zero** interferences between all parts.
