# Assembly

Assumes the aluminum parts are machined and tapped
([machining.md](machining.md)), the inserts and cradle are printed
([printing.md](printing.md)), and the parts in
[bill-of-materials.md](bill-of-materials.md) have arrived.

## 1. Finish the aluminum

1. Break the tabs, file the nubs flush, and deburr every edge, including
   inside the notches and around the window.
2. Open the bezel's 11 screw holes from 1/8" to 3.4 mm (#29 drill). If you
   skipped the long-reach drill file, drill the back's 10 floor-boss holes
   at 2.5 mm by hand. Then tap all 21 holes in the back M3 (see
   [machining.md](machining.md#back-tub-one-setup)).
3. Check that all 21 tapped holes take an M3 screw by hand.
3. Optional: bead-blast and anodize. Mask or chase the threads afterwards.

## 2. Test-fit before wiring anything

- Drop each printed insert into its notch. Each should slide in by hand and
  sit flush with the rim. If one does not, see [printing.md](printing.md#fit).
- Lay the screen face down in the bezel. It should drop into the pocket with
  a little clearance all round.
- Find where the screen's FPC ribbon leaves the panel. If it leaves from an
  edge rather than the back, file a relief into the back's rim at that
  point, so the rim does not pinch it.
- Check that the 200 mm FPC cable reaches from the screen's connector to the
  board's `LCD` connector, on the underside of the board near the 40-pin
  header.

## 3. Battery pack

1. Build the 4S2P pack in the printed cradle: two columns of four cells,
   with the series and parallel links spot-welded across the 10 mm centre gap
   and the ends. See [power.md](power.md#pack-assembly).
2. Wire the balance leads and the main leads to the DALY BMS, following the
   BMS's own diagram. Connect B− first and the balance leads in order.
3. Measure each series group's voltage at the BMS before connecting anything
   else.
4. Put the 7.5 A fuse and XT30 connector on the BMS output (P+ / P−).

## 4. Electronics

The space above the battery, top left, has four M3 tray bosses. A printed
tray for the BMS, IP2368, regulator and Pololu switch is not designed yet. Use
double-sided foam tape for now, or design a plate that screws to those bosses.
The open area above the board, top right, also has room.

1. **Charger:** the IP2368's USB-C port faces the left-wall insert, behind
   the slot. Its BAT+ / BAT− go to the pack connector.
2. **Master switch:** pack → Pololu Mini Pushbutton Switch SV → S13V25F12
   input. Put the switch's button where you can reach it with the lid off, or
   wire its pushbutton terminals to a second switch.
3. **12 V to the board:** S13V25F12 output → Q6A power header, pin 1 = +12 V
   and pin 2 = GND. **Check polarity with a meter before connecting.**
   Reversed polarity damages the board.
4. **Power button:** fit the 12 mm momentary button in the left insert, and
   wire it across header pin 3 (`PWR`) and pin 2 (`GND`).
5. Before plugging into the board, power up and measure 12.0 V at the header
   plug.

## 5. Board and screen

1. Plug the FPC into the board's `LCD` connector, on the board's underside,
   **before** the board goes in.
2. Fit the board to the four standoffs with M3 × 6 screws. Its ports should
   line up with the right and bottom notches, about 0.5 mm short of where the
   inserts will sit.
3. Optional: put a thermal pad between the underside of the board, below the
   SoC, and the floor.
4. Fit the battery cradle with its two M3 × 6 screws, through the
   counterbores in the centre gap.
5. Drop in the three inserts.

## 6. Close up

1. Run 0.3 mm foam tape around the border of the screen's back face, where
   the back's rim will press.
2. Lay the bezel face down on a soft cloth, with the screen face down in its
   pocket. Plug the FPC into the screen.
3. Lower the back onto the bezel, routing the FPC so it is not pinched.
4. Fit the 11 M3 × 12 lid screws through the bezel's counterbores. Tighten
   them in a cross pattern, snug only: they thread into aluminum.

## 7. First power-on

1. Charge the pack from a USB-C PD charger, watching it, on a fireproof
   surface. Confirm it stops at 16.8 V, or check the charger's end state.
2. Switch on the master switch, then press the 12 mm button. The board
   should boot, and the screen should come up once the Display 10 FHD overlay
   is enabled in Radxa OS. See Radxa's
   [MIPI DSI guide](https://docs.radxa.com/en/dragon/q6a/hardware-use/mipi-dsi).
3. Run a sustained load and watch the SoC temperature.
