# 3D-printed parts

Four printed parts, all in [`print/`](../print/).

| File | Fits | Cutouts |
|---|---|---|
| `insert_right_usb_rj45.stl` | Right-wall notch | Two USB-A stacks (open at the top), RJ45 |
| `insert_bottom_usbc_hdmi_audio.stl` | Bottom-wall notch | USB-C slot, HDMI, Ø8 audio |
| `insert_left_charge_power.stl` | Left-wall notch | Charge USB-C slot, Ø12.2 power button |
| `battery_cradle_4s2p_18650.stl` | Floor, left half | Eight cell troughs, two M3 screw pockets |

## Settings

| Setting | Value |
|---|---|
| Material | PETG or ASA. Avoid PLA: a sunny dashboard or a warm SoC will soften it. |
| Layer height | 0.2 mm |
| Walls | 4 |
| Infill | 30–40 % gyroid |
| Supports | None |

## Orientation

- **Port inserts: upright, as modelled.** Flange end down, the end that meets
  the rim up. This needs no supports. The cutouts print as short bridges (the
  widest is the 21 mm HDMI opening), and the USB opening is open at the top.
- **Battery cradle:** flat side down, troughs facing up.

## Fit

The inserts are 0.15 mm undersize on every face that meets aluminum
(`ins_clr` in the spreadsheet). The insert should slide in by hand and sit
level with the rim.

- **Too tight:** raise `ins_clr` to 0.2–0.25, recompute, and export the STL
  again from FreeCAD (select the body, File → Export).
- **Too loose:** the lid still clamps it, but you can lower `ins_clr`.

Before printing all three, print one insert and test it in its notch.

## Port openings

The board's connectors sit about 10 mm behind the outside face. The cutouts
are sized for the connector plus room for a slim plug:

| Port | Opening |
|---|---|
| USB-A stack | Stack outline + 0.5 mm, open at the top |
| RJ45 | Jack outline + 0.5 mm |
| USB-C (board and charge) | 13 × 7.5 mm slot |
| HDMI | 21 × 11 mm |
| Audio | Ø8 mm |

If your cables have bulky plugs, widen the opening in the matching
`sk_Ins*Ports` sketch and export again. The aluminum window behind the
insert is larger than the cutouts, so there is room to grow them.
