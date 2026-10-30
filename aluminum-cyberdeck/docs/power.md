# Power: battery, charging and runtime

The Radxa Dragon Q6A runs on **12 V**. This build uses eight 18650 cells in a
**4S2P** pack (14.4 V nominal, 12.0–16.8 V), charged from a USB-C PD port on
the left wall. A buck-boost regulator holds the board at a steady 12 V from a
full pack down to an empty one.

To buy the parts, see [bill-of-materials.md](bill-of-materials.md).

## Architecture

```
 8 × 18650 (4S2P)
   │  balance taps (JST-XH 5-pin)
   ▼
 DALY 4S 15 A BMS with balancing ──── protects and balances the cells
   │  P+ / P−
 7.5 A MINI fuse on P+
   │
 XT30 pack connector
   ├──────────────────────────────┐
   │                              │
 IP2368 100 W USB-C PD          Pololu Mini Pushbutton Power Switch SV
 charge module (BAT+/BAT−)        (hard master on/off)
   ▲                              │
   │ USB-C port in the            ▼
   │ left-wall insert           Pololu S13V25F12 buck-boost, 12 V / 2.5 A
                                  │
                                Q6A 3-pin power header
                                  Pin 1 = 12 V   Pin 2 = GND
                                  Pin 3 = PWR ── 12 mm momentary button ── GND
                                  │
                                39-pin FPC ── Radxa Display 10 FHD
                                (the screen is powered through the board)
```

### Why each part

- **The BMS** stops the pack being over-charged, over-discharged or shorted,
  and balances the four series groups. The chargers below do not balance, so
  the BMS has to.
- **The IP2368 module** is a USB-C PD sink with a buck-boost CC/CV charger
  for 4S packs, up to 100 W. One USB-C port charges the pack from any PD
  charger.
- **The buck-boost regulator** matters because the pack spends part of its
  life below 12 V under load. A plain buck converter would drop out near
  empty; the S13V25F12 holds 12 V from 2.8 V to 22 V in.
- **Two switches:**
  - The **Pololu pushbutton switch** is a hard master cut-off. In storage the
    regulator and board draw nothing.
  - The **12 mm panel button** in the left insert goes across the Q6A's `PWR`
    and `GND` header pins. A short press boots the board; pressed while it is
    running, it opens the shutdown menu.

### Charge while running

With the IP2368 wired straight to the pack and the tablet running, the load
current can stop the charger ever seeing its end-of-charge current. It then
holds the pack at 16.8 V instead of finishing. That is safe, but not ideal
for cell life.

For proper charge-while-running, replace the IP2368 with **TI's
USB-PD-CHG-EVM-01** (BQ25798 charger + TPS25751 PD controller). It has a
power-path `SYS` output: feed the switch and regulator from `SYS` instead of
from the pack. It costs far more and is larger, so it may need its own tray.

## Settings to check before the first charge

1. **Charge current.** The Samsung 35E allows 2 A of charge per cell, which
   is **4 A for a 2P pack**. Set or confirm the IP2368 charge current at 4 A
   or less. At 100 W it could push about 6 A. Molicel P28A and P30B tolerate
   more.
2. **BMS over-voltage threshold.** Read the value printed on the DALY board
   you receive. It must be **4.25 V per cell** for a 16.8 V charger. Listings
   disagree, and a 4.15 V version trips before charging finishes.
3. **Polarity at the Q6A header.** Reversed polarity damages the board.
   Radxa's docs do not give the header's connector pitch, so measure it, or
   check the schematic, before buying a mating plug.
4. **One input only.** Never plug a supply into the Q6A's own USB-C port
   while it is running from the header.

## Pack assembly

- Spot-weld **0.15 × 8 mm pure nickel strip** (Ni200). Each link carries only
  about 2.5 A. Do not solder to the cells.
- Use fish-paper insulator rings on every positive end, and Kapton tape
  between the parallel groups.
- The printed cradle holds the cells in two columns of four, with a 10 mm
  gap between the columns for the series links and the two cradle screws.
- **No-weld option:** eight Keystone 1043 single-cell holders, wired as
  4S2P. They are bulkier, and will not fit the printed cradle as designed.

Only build a lithium pack if you are comfortable doing it safely. Use matched
new cells from a reputable seller, and charge the first time on a fireproof
surface while you watch it.

## Runtime estimate

### Power draw

The figures below are at the Q6A's 12 V input, including the display.

| Scenario | Board | Display | Total |
|---|---|---|---|
| Idle desktop | 2.9 W ([measured][geerling]) | ~2 W at 50 % brightness | **5 W** |
| Typical: browsing, desktop, Wi-Fi | ~4.5 W (estimate) | ~3.4 W at full brightness | **8 W** |
| Heavy: CPU/GPU load or local LLM | ~10 W ([measured][geerling], [SBCwiki][sbcwiki]) | ~3.4 W + ~0.6 W NVMe | **14 W** |
| Radxa's design maximum | — | — | **24 W** |

- **Board:** Jeff Geerling measured 2.9 W idle, 9.3 W under stress-ng,
  10.1 W running HPL, and about 7.9 W during LLM inference. SBCwiki measured
  a peak of 10.17 W.
- **Display:** Radxa's datasheet gives the backlight as 12.8 V at
  180–200 mA, about 2.6 W at the LEDs. The display figures above add boost
  losses and about 0.4 W for logic and touch. They are estimates, not
  measurements.

### Usable energy

Pack energy = 8 cells × rated Ah × 3.6 V. Usable energy applies 0.92 (the
BMS cuts off at about 2.7–3.0 V, plus ageing) and 0.90 (regulator, BMS and
wiring losses).

| Cell | Pack energy | Usable at 12 V | Idle 5 W | Typical 8 W | Heavy 14 W | Max 24 W |
|---|---|---|---|---|---|---|
| Samsung 35E (3500 mAh) | 100.8 Wh | 83.5 Wh | **16.7 h** | **10.4 h** | **6.0 h** | 3.5 h |
| Molicel P30B (3000 mAh) | 86.4 Wh | 71.5 Wh | 14.3 h | 8.9 h | 5.1 h | 3.0 h |
| Molicel P28A (2800 mAh) | 80.6 Wh | 66.8 Wh | 13.4 h | 8.4 h | 4.8 h | 2.8 h |

Expect **about 10 hours of normal use** on Samsung 35E cells. The figures
assume no fan and no power-hungry USB devices. A USB SSD or other peripherals
come straight out of this budget.

**Air travel:** at 100.8 Wh the 35E pack is just over the 100 Wh limit for
carry-on batteries that need no airline approval. If you fly with it, use
P30B or P28A cells (80–86 Wh).

### Charge time

Charging a 100 Wh pack at 4 A (about 60 W into the pack) takes about
**2 hours** from empty. That needs a 65 W or larger USB-C PD charger. A 30 W
charger takes about 4 hours.

## Heat

The board's peak draw is about 10 W, and the case is a 860 g aluminum shell
with no vents. The obvious way to cool it is a thermal pad between the
underside of the board, under the SoC, and the floor. That is not modelled
yet. Watch SoC temperatures under sustained load before relying on it.

[geerling]: https://github.com/geerlingguy/sbc-reviews/issues/85
[sbcwiki]: https://sbcwiki.com/news/articles/radxa-dragon-q6a-full-review/
