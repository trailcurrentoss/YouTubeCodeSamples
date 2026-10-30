# Bill of materials

Everything to buy, apart from the aluminum and cutting tools, which are in
[tools-and-stock.md](tools-and-stock.md). The reasons for the power parts
are in [power.md](power.md).

Prices are what the listings showed on 2026-10-17. Links marked ✓ were
checked and showed the stated product that day. Links marked ⚠ could not be
checked (the site blocked automated access, or the page was blank), so
confirm the part number before ordering.

> **Availability:** in October 2026 the Dragon Q6A and the Display 10 FHD
> were out of stock at every seller checked. Expect a backorder.

## Compute and display

| Qty | Part | Notes | Approx. price | Link |
|---|---|---|---|---|
| 1 | Radxa Dragon Q6A | Any RAM size. Pick a variant with eMMC/UFS, or add an NVMe 2230. | from $95 | [Arace](https://arace.tech/products/radxa-dragon-q6a) ✓ · [Ameridroid](https://ameridroid.com/products/radxa-dragon-q6a) ✓ · [Radxa resellers](https://radxa.com/products/dragon/q6a) ✓ |
| 1 | Radxa Display 10 FHD (SKU AD003) | 10.1", 1200×1920, MIPI DSI, 10-point touch. Ameridroid lists the 39-to-40-pin FPC cable in the box. | $65–95 | [Arace](https://arace.tech/products/display-10-fhd) ✓ · [Ameridroid](https://ameridroid.com/products/10-1-fhd-display-for-rock) ✓ |
| 0–1 | Radxa FPC cable, 39P to 40P, 200 mm (AC013-200) | Spare. It is only 200 mm long, so check the route before you machine. | $2 | [Arace](https://arace.tech/products/radxa-display-fpc-cable-39p-to-40p) ✓ |
| 0–1 | NVMe SSD, M.2 2230 | Optional storage. It fits the board's own M.2 slot. | varies | — |

Radxa's [Q6A accessories page](https://docs.radxa.com/en/dragon/q6a/accessories)
lists the Display 10 FHD as compatible.

## Battery

| Qty | Part | Notes | Approx. price | Link |
|---|---|---|---|---|
| 8 | **Samsung 35E** 18650, 3500 mAh | Best runtime. Maximum charge is 2 A per cell. | $6 each | [18650 Battery Store](https://www.18650batterystore.com/products/samsung-35e-18650-3500mah-8a-battery) ✓ |
| *or* 8 | Molicel P30B 18650, 3000 mAh | Takes higher charge current. 86 Wh, under the 100 Wh air-travel limit. | $7 each | [18650 Battery Store](https://www.18650batterystore.com/products/molicel-p30b-18650-3000mah-36a-battery) ✓ |
| *or* 8 | Molicel P28A 18650, 2800 mAh | The most robust cell, with the least capacity | $6 each | [18650 Battery Store](https://www.18650batterystore.com/products/molicel-p28a) ✓ · [Battery Junction](https://batteryjunction.com/molicel-inr18650-p28a) ⚠ |
| 1 roll | Pure nickel strip, 0.15 × 8 mm | Spot-welded series and parallel links | ~$7 | [18650 Battery Store, nickel strip](https://18650batterystore.com/products/3p-pure-nickel-strip-1kg-roll-for-battery-spot-welding-18650-21700) ⚠ |
| 1 set | Fish-paper insulator rings and Kapton tape | One ring on every cell's positive end | ~$5 | any battery supplier |

Buy all eight cells at the same time, from the same seller and batch.

## Protection, charging and regulation

| Qty | Part | Notes | Approx. price | Link |
|---|---|---|---|---|
| 1 | **DALY 4S 15 A Li-ion BMS with balancing** (DL-J05G-L4S15ATJ) | 61 × 35 × 9 mm. Check that the over-voltage threshold printed on the board is **4.25 V**. | ~$9 | [TinyTronics](https://www.tinytronics.nl/en/power/bms-and-chargers/bms/daly-bms-li-ion-li-po-protection-circuit-4s-15a-with-balancing) ✓ (EU; ships internationally) |
| 1 | **IP2368 100 W USB-C PD charge module, 4S** | One USB-C port, PD sink and CC/CV charger. Set the charge current to 4 A or less. Generic Chinese module. | ~$16 | [Newegg Business](https://www.neweggbusiness.com/p/9B-27U-00HC-00022) ✓ |
| *or* 1 | TI USB-PD-CHG-EVM-01 (BQ25798 + TPS25751) | Upgrade path: a proper charge-while-running power path, configured over I²C. Larger, and much more expensive. | ~$150–180 | [TI](https://www.ti.com/tool/USB-PD-CHG-EVM-01) ✓ · [DigiKey](https://www.digikey.com/en/products/detail/texas-instruments/USB-PD-CHG-EVM-01/14004336) ⚠ |
| 1 | **Pololu S13V25F12**, 12 V 2.5 A buck-boost regulator | 22.9 × 22.9 × 9.7 mm. 2.8–22 V in, 85–95 % efficient. 30 W covers Radxa's 24 W recommendation. | $19 | [Pololu #4984](https://www.pololu.com/product/4984) ✓ |
| 1 | Pololu Mini Pushbutton Power Switch SV | Hard master on/off. 4.5–40 V, about 4 A. | $5 | [Pololu #2809](https://www.pololu.com/product/2809) ✓ |

## Switch, fuse and wiring

| Qty | Part | Notes | Approx. price | Link |
|---|---|---|---|---|
| 1 | 12 mm momentary anti-vandal pushbutton | For the Ø12.2 hole in the left insert. It goes across the Q6A's `PWR` and `GND` header pins. | $4–10 | [E-Switch PV5 series](https://www.e-switch.com/product/pv5-series-long-life-anti-vandal-switch/) ✓ · [DigiKey PV5S24018](https://www.digikey.com/en/products/detail/e-switch/PV5S24018/4028819) ⚠ · [Maplin (UK)](https://pro.maplin.co.uk/products/off-on-momentary-12mm-vandal-resistant-push-switch-spst) ✓ |
| 1 | Inline MINI blade fuse holder, rated 32 V DC | Use McMaster **8110K1**, not 8110K813, which is rated for 12 V only | — | [McMaster inline fuse holders](https://www.mcmaster.com/inline-fuse-holders) ✓ |
| 1 | MINI blade fuse, 7.5 A, 32 V (Littelfuse 0297007) | 7.5 A if the charge current also passes through the fuse | <$1 | [Waytek](https://www.waytekwire.com/product/littelfuse-mini-automotive-fuse-5a) ⚠ |
| 1 pack | XT30 connectors, 5 pairs | Pack disconnect | $5 | [goBILDA 3803-0102-0505](https://www.gobilda.com/xt30-connector-pack-fh-mc-x-5-mh-fc-x-5) ✓ |
| 1 | Mating plug for the Q6A 3-pin power header | Radxa's docs do not give the pitch. Measure the header or check the schematic. | — | — |
| — | 18 AWG silicone wire (pack and charger), 22 AWG (button, balance) | | — | — |

## Enclosure hardware

| Qty | Part | Notes | Link |
|---|---|---|---|
| 11 | M3 × 12 low-head socket cap screw, DIN 7984, stainless | Lid screws | [McMaster 92855A313](https://www.mcmaster.com/products/92855A313/) ⚠ (part number from the series numbering; check it) |
| 6 (+4) | M3 × 6 low-head socket cap screw, DIN 7984, stainless | 4 for the board, 2 for the battery cradle, plus 4 for a future electronics tray | [McMaster 92855A307](https://www.mcmaster.com/product/92855A307/) ✓ |
| 1 roll | Foam tape, about 0.3 mm thick, 5 mm wide | Around the screen border, under the back's rim | any |
| 1 | Thermal pad, 1.5–3 mm | Optional: board underside to the floor, for cooling | any |
| ~100 g | PETG or ASA filament | Four printed parts | any |

## Rough cost

| Group | Approx. |
|---|---|
| Board + display | $160–345 |
| Cells and pack materials (35E) | ~$60 |
| BMS, charger, regulator, switch | ~$50 |
| Wiring, fuse, fasteners, tape | ~$30 |
| **Electronics total** | **~$300–485** |

The aluminum plate and cutting tools are extra. See
[tools-and-stock.md](tools-and-stock.md).
