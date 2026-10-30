# Tools and stock

What you need to machine the two aluminum parts. How each tool is used is in
[machining.md](machining.md).

Prices are what the listings showed on 2026-10-17. Links marked ✓ were
checked and showed the stated product that day. Links marked ⚠ could not be
checked (the site blocks automated access), so confirm the size before
ordering.

## Endmills

**Every tool here has a 1/4" or 1/8" shank.** Your collet only needs to hold
those two sizes.

| T | Need | Recommended | Alternative |
|---|---|---|---|
| **T1** roughing | 1/4" 3-flute carbide square endmill for aluminum, **1-1/4" flute length**. The outer profile is 28.9 mm deep. | Lakeshore Carbide 1/4" Extended Length Variable Flute for Aluminum, ZrN (360014LX): 1-1/4" LOC, 3" OAL, $58.06. [Lakeshore](https://www.lakeshorecarbide.com/14extendedlengthvariablefluteendmillforaluminumzrn.aspx) ✓ | Helical H45AL-L-30250 (#03197): 1-1/4" LOC, 3" OAL, ZPLUS, $73.50. [Helical](https://www.helicaltool.com/products/tool-details-03197) ✓. Uncoated #03195, $62.30. [Helical](https://www.helicaltool.com/products/tool-details-03195) ✓ |
| **T2** finishing | The same tool as T1, a **second one**, kept sharp for the finishing passes | Same as T1 | Same as T1 |
| **T5** counterbores | 1/8" 2–3 flute carbide square endmill, standard length. The counterbores are 2.3 mm deep. | Lakeshore Carbide 1/8" Variable 3 Flute for Aluminum, ZrN (360018X): 1/2" LOC, $20.33. [Lakeshore](https://www.lakeshorecarbide.com/18variablefluteendmillforaluminumzrn.aspx) ✓ | Helical H45AL-SR-30125 (#03032): 3/8" LOC, $29.00. [Helical](https://www.helicaltool.com/products/tool-details-03032) ✓ |

**Buy two 1/4" endmills** (T1 and T2), and a third as a spare if you can.
The roughing tool works for about 2.5 hours on the back alone. Using a
separate, sharp tool for the finishing passes keeps the screen pocket and
walls on size.

**Flute length matters.** The 1/4" tools need the 1-1/4" length:
- The outer profile is 28.9 mm deep, measured from the top of the 1-1/8"
  plate.
- The walls are 23 mm deep.

A standard 1/4" endmill has a 3/4" or 1" flute, and is too short for both.

## Drills

All of these have a **1/8" shank**. A plain 2.5 mm jobber drill has a 2.5 mm
shank, so it will not fit a 1/8" collet.

| T | Need | Option | Link |
|---|---|---|---|
| **T3** rim tap holes (11) | 2.5 mm carbide drill, 1/8" shank. Any length will do: the holes are open from above and 8 mm deep. | Makera TiN-coated carbide drill, 2.5 × 12 mm, 1/8" shank, 10-pack | [Core Electronics (AU)](https://core-electronics.com.au/makera-tin-coated-drill-bit-for-metal-3-175mm-shank-2-5mm-x-12mm.html) ✓ · [Makera store](https://www.makera.com/products/tin-coating-drill-bit-1-8-shank) ⚠ (check the 2.5 mm size is offered) |
| **T6** floor tap holes (10) | 2.5 mm carbide drill, 1/8" shank, **at least 2-1/2" (63 mm) long**, so it can stick out 27 mm and still be gripped | 2.5 mm × 1/8" shank × 13/16" flute × 2-1/2" OAL, AlTiN, solid carbide | [Butler Bros 1466455](https://shop.butlerbros.com/catalogue/product/1466455) ⚠ |
| | *No long drill?* | Skip file 4 and drill these 10 holes by hand on a drill press, with an ordinary 2.5 mm jobber centred on each boss | TTC 2.50 mm HSS jobber: [Travers 01-042-180](https://www.travers.com/product/ttc-production-01-042-180-jobbers-drill-01-042-180) ⚠ |
| **T4** lid-screw holes (11) | 1/8" (3.175 mm) carbide drill, 1/8" shank | RedLine RD22108: 1/8" 2-flute carbide stub drill, AlTiN, 3/4" flute, 2" OAL, $27.50 | [RedLine Tools](https://www.redlinetools.com/itemdetail/RD22108) ✓ |
| *by hand* | #29 (3.454 mm) drill, to open the lid holes to M3 clearance after machining | #29 M35 cobalt jobber, 135° split point | [Travers 01-021-229](https://www.travers.com/product/to-01-021-229-jobbers-drill-01-021-229) ⚠ |

Why T4 is 1/8" and not 3.4 mm: I found no 3.4 mm drill with a 1/8" shank. A
1/8" hole already passes an M3 screw (3.0 mm). Opening it to 3.4 mm by hand
gives room for any positioning error between the bezel and the back.

## Taps and tapping

All 21 holes in the back are tapped M3 × 0.5 by hand after machining.

| Item | Use | Link |
|---|---|---|
| **M3 × 0.5 spiral-flute tap, semi-bottoming** (YMW 374615, Travers 12-014-279) | All 21 holes. It pulls chips up out of the blind holes. $29.47. | [Travers](https://www.travers.com/product/ymw-374615-spiral-fluted-tap-12-014-279) ✓ |
| *Optional:* M3 × 0.5 spiral-point plug tap (YG-1, Travers 12-679-350) | Faster in the 11 rim holes, but it pushes chips forward and the rim holes are only 8 mm deep. Use the spiral-flute tap unless you deepen those holes (`tap_d` in the spreadsheet) to 11 mm. $24.64. | [Travers](https://www.travers.com/product/yg-spiral-pointed-tap-12-679-350) ✓ |
| Tap wrench, 1.6–4.7 mm (Starrett 93A, Travers 57-051-200) | Holds the tap. $60.37. | [Travers](https://www.travers.com/product/starrett-93a-tap-wrench-57-051-200) ✓ |
| Spring tap guide (TTC 20-MTG, Travers 57-062-050) | Keeps the tap square. $14.99. | [Travers](https://www.travers.com/product/ttc-20-mtg-tap-wrench-57-062-050) ✓ |
| Cutting fluid for aluminum | Any tapping fluid made for aluminum | — |

## Optional finishing tools

| Item | Use | Link |
|---|---|---|
| 90° chamfer mill, 1/4" shank (Lakeshore 14CHAM45) | A small edge break on the outer edges, as an extra CAM pass. $38.39. | [Lakeshore](https://www.lakeshorecarbide.com/1490degreeincludedanglechamfertool.aspx) ✓ |
| Deburring handle with blades (Noga NG9300, Travers 99-000-174) | Hand deburring the notches, window and holes | [Travers](https://www.travers.com/product/noga-ng9300-deburring-set-99-000-174) ⚠ |

## Workholding

| Item | Use | Link |
|---|---|---|
| Low-profile edge or toe clamps, ×4 | Clamp the stock margin, outside the profile path (beyond ±146 mm in X and ±101 mm in Y from the centre). Use whatever fits your table. | Threaded-insert tables: Mitee-Bite Pitbull 26050 ([Travers 83-010-827](https://www.travers.com/product/mitee-bite-26050-vise-edge-clamp-83-010-827) ⚠). Shapeoko-style tables: [Carbide 3D Essential Clamps](https://shop.carbide3d.com/products/essential-clamps) ⚠ |
| 3 mm dowel pins, ×2 (e.g. 3 × 16 mm) | Re-register the bezel for setup B through two of its screw holes | [Travers 72-790-203](https://www.travers.com/product/ttc-72-790-203-dowel-pin-72-790-203) ⚠ |
| Double-sided tape | Extra hold and damping under the plates | — |
| Sacrificial spoilboard | Every outer profile cuts 0.3 mm into it | — |

## Aluminum stock

Both parts are 6061-T6 or T651 plate. Each blank needs **at least 292.7 ×
201.5 mm**: the part plus 15 mm of clamping and tab room on every side.

| Part | Thickness | Size to order | Where |
|---|---|---|---|
| **Back tub** | **1-1/8" (28.575 mm)**, what the CAM is set up for | 12" × 12" (or 12" × 9" from a vendor that cuts to a −0 tolerance) | OnlineMetals 1.125" 6061-T651, cut to size ([pid 23015](https://www.onlinemetals.com/en/buy/aluminum/1-125-aluminum-plate-6061-t651/pid/23015) ⚠) · [Metals Depot P3118-6061](https://www.metalsdepot.com/aluminum-products/6061-aluminum-sheet-plate) ⚠ |
| Back tub, alternative | 1-1/4" (31.75 mm) | 12" × 12", **$263.52** | [Speedy Metals 61p1.25](https://www.speedymetals.com/p-2416-1-14-6061-t651-aluminum-plate.aspx) ✓. Set `stock_back_t` to 31.75 mm and post again (see below). |
| **Front bezel** | **3/8" (9.525 mm)** | 12" × 12", **$85.75** | [Speedy Metals 61p.375](https://www.speedymetals.com/p-2410-38-6061-t651-aluminum-plate.aspx) ✓ · OnlineMetals cut to size ([pid 1249](https://www.onlinemetals.com/en/buy/aluminum/0-375-aluminum-plate-6061-t651/pid/1249) ⚠) |

On sizes and tolerances:
- **Do not order 12" × 8".** At 203.2 mm it leaves 1.7 mm of margin, and
  saw-cut vendors (Speedy Metals lists ±1/4") can deliver it narrower than the
  part needs. 12" × 12" is safe from any vendor, and the offcut is useful.
- **SendCutSend** stocks 6061 only up to 0.750" thick, so it cannot supply
  the back. It can cut a 3/8" bezel blank.

### If your plate is a different thickness

The CAM stock heights are spreadsheet cells:

| Alias | Default | Meaning |
|---|---|---|
| `stock_back_t` | 28.575 mm | Back plate thickness. The facing pass takes it down to 25.5 mm. |
| `stock_front_t` | 9.525 mm | Front plate thickness. The facing pass takes it down to 7.8 mm. |

Measure your plate, enter the real thickness, recompute, post the jobs
again (CAM → Post Process), then split the files with
`tools/split_by_tool.py` (see [machining.md](machining.md#changing-the-cam)).
The facing and roughing operations start at the new stock top automatically.
Thicker plate only adds passes: 1-1/4" plate adds about four 1 mm facing
passes on the back's rim, plus one extra roughing level.

## Cost summary

| Group | Approx. |
|---|---|
| Endmills: 2 × 1/4" ($116), 1/8" ($20) | ~$136 |
| Drills: 2.5 mm short and long, 1/8", #29 | ~$60 |
| Tap, wrench, guide | ~$105 |
| Aluminum (1-1/4" + 3/8", 12" × 12" each) | ~$350 |
| **Total** | **~$650** |

Clamps, the spoilboard and the optional chamfer mill and deburring tools are
extra.
