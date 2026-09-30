# Hardware

The firmware targets exactly one board: the **Makerfabs MaTouch ESP32-S3
Rotary IPS Display with Touch 2.1" (ST7701)**. Nothing here is written to
be portable to other panels — the pin map, panel init table and touch setup
are specific to it.

| Part | Detail |
|---|---|
| SoC | ESP32-S3R8 — 8 MB **octal** PSRAM, 16 MB quad flash |
| Display | 2.1" round IPS, 480×480, ST7701S, 16-bit RGB565 parallel bus + 3-wire SPI for init |
| Touch | CST826 capacitive (CST816S register map, I²C `0x15`), **polled** |
| Input | Rotary ring (quadrature) with a push switch — the whole panel is the button |
| USB | One USB-C on the ESP32-S3's native USB (no UART bridge) |
| Buttons | **Flash** (GPIO0 / BOOT) and **Reset** |

Vendor sources: the Makerfabs GitHub repository
`MaTouch-ESP32-S3-Rotary-IPS-Display-with-Touch-2.1-ST7701` and the
Makerfabs wiki. Where the two disagree, `components/matouch_board/src/board_pins.h`
says which one was right and how it was verified.

## Pin map

The authoritative list, with provenance for every value, is
[`components/matouch_board/src/board_pins.h`](../components/matouch_board/src/board_pins.h).
Summary:

| Function | GPIO |
|---|---|
| LCD SPI sideband CS / SCK / SDA | 1 / 46 / 0 |
| LCD DE / VSYNC / HSYNC / PCLK | 2 / 42 / 3 / 45 |
| LCD data (16 lines) | 11 15 12 16 21 · 39 7 47 8 48 9 · 4 41 5 40 6 |
| Backlight (LEDC PWM) | 38 |
| Touch I²C SDA / SCL | 17 / 18 |
| Touch INT / RST | not connected |
| Ring A / B / push | 13 / 10 / 14 |
| UART0 TX / RX (log fallback) | 43 / 44 |

## Things this board does that you would not expect

**The colour channels are labelled both ways round.** The vendor firmware
and the vendor wiki name the same five data pins R0–R4 and B0–B4
respectively. The pin map follows the wiki and
`CONFIG_MATOUCH_LCD_SWAP_RB` (default on, verified on hardware) swaps the
groups. Fix a colour swap with that option, never by editing the pin table.

**The stock ST7701 init table shows nothing.** `esp_lcd_st7701`'s default
sequence leaves this glass black with no error. The panel needs its own
gate-driver registers — see
[`st7701_matouch_init.h`](../components/matouch_board/src/st7701_matouch_init.h).

**Touch has no interrupt or reset line.** TP_INT only reaches GPIO0 through
an unpopulated resistor, and TP_RST has a pull-up and no GPIO. The
controller is polled over I²C.

**The whole panel is the ring's push switch.** Pressing the glass firmly
also clicks the ring. The firmware treats a contact during which the
switch closed as a *push*, and ignores it as a tap — see
[firmware architecture](firmware-architecture.md#touch-vs-push).

**Touch is a little off.** The TrailCurrent Capstan project measured about
20 px of offset on this panel. The 84 px keys here are large enough that it
does not matter; small targets would need a calibration step.

**Ring direction is reversed.** Raw counts go backwards on this board;
`CONFIG_MATOUCH_ENCODER_INVERT` defaults on. One detent is 4 quadrature
counts (`CONFIG_MATOUCH_ENCODER_STEPS_PER_DETENT`).

## Recovery

If a firmware never gets far enough to start USB, hold **Flash** while
pressing **Reset**. The chip comes up in the ROM loader on its built-in
USB-Serial/JTAG port and can be flashed normally — see
[building and flashing](building-and-flashing.md#recovery).
