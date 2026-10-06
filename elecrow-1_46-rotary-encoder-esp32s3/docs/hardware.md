# Hardware

The firmware targets exactly one board: the **Elecrow CrowPanel 1.46"
HMI ESP32 Rotary Display** (360×360 round IPS, touch, rotary knob). Nothing
here is written to be portable to other panels — the pin map, panel driver
and touch setup are specific to it. Sibling versions of this pad live next
door: the CrowPanel 1.28" in `../elecrow-1_28-rotary-encoder-esp32s3` and
the Makerfabs MaTouch 2.1" in `../makerfab-2_1-rotary-encoder-esp32s3`.

| Part | Detail |
|---|---|
| SoC | ESP32-S3R8 — 8 MB **octal** PSRAM, 16 MB quad flash (despite "ESP32" in the product name) |
| Display | 1.46" round IPS, 360×360, JD9855, 4-wire SPI at 80 MHz |
| Touch | CST816D/T capacitive (CST816S register map, I²C `0x15`), interrupt-driven |
| Input | Rotary ring with a push switch — the whole panel is the button |
| USB | One USB-C on the ESP32-S3's native USB (no UART bridge) |
| LED ring | 8 × WS2812 around the ring (GPIO48, power enable GPIO17). Rests green; while the ring turns, the half on the side it turns towards goes white |
| Also fitted | A power LED (GPIO40), a battery connector with charge sensing — not used by this firmware |

Vendor sources: the Elecrow GitHub repository
`CrowPanel-1.46inch-HMI-ESP32-Rotary-Display`
(`example/V1.0/Arduino/RotaryScreen_1_46_Code_Core3_LVGL9/`) and the Elecrow
wiki. Every pin was also checked against the Eagle schematic netlist in that
repository, and the board layer here is the one the TrailCurrent Capstan
firmware runs on this board.

## Pin map

The authoritative list, with provenance, is
[`components/crowpanel_board/src/board_pins.h`](../components/crowpanel_board/src/board_pins.h).
Summary:

| Function | GPIO |
|---|---|
| LCD SPI SCK / MOSI / DC / CS | 10 / 11 / 3 / 9 |
| LCD reset | 14 |
| Panel rails (must be driven HIGH) | 1, 2 |
| Backlight (LEDC PWM) | 46 |
| Touch I²C SDA / SCL | 6 / 7 |
| Touch INT / RST | 5 / 13 |
| Ring A / B / push | 45 / 42 / 41 |
| UART0 TX / RX (log fallback) | 43 / 44 |

These are pin-for-pin the same as the 1.28". Almost nothing else is —
see the next section before copying any setting across.

## How it differs from the 1.28"

| | 1.28" | 1.46" |
|---|---|---|
| Controller | GC9A01 (Espressif driver) | JD9855 (driver in this repo) |
| Resolution | 240×240 | 360×360 |
| Colour inversion | on | off |
| Element order (`CROWPANEL_LCD_BGR`) | BGR | **RGB** |
| Display mirror (`CROWPANEL_LCD_MIRROR_X`) | on | **off** |
| Counts per detent | 4 | **2** |
| WS2812 ring | 5, no enable pin | 8, enable on GPIO17 |

## Things this board does that you would not expect

**There is no driver for the JD9855 anywhere.** Not in esp-iot-solution,
not in any public repository, and the panel datasheet has no initialisation
section. Elecrow's own firmware drives it through LovyanGFX's
`Panel_ST77961`, so
[`panel_jd9855.c`](../components/crowpanel_board/src/panel_jd9855.c) is an
esp_lcd driver built around that init table (47 commands, transcribed
byte-for-byte). Don't "tidy" the magic numbers: there is nothing to check
them against except a working panel. LovyanGFX's `Panel_ST77961`
*constructor* defaults to 360×390; that is wrong for this board, and the
init table's own column/row window (0–359) is the authority.

**Two GPIOs have to be high before the panel does anything.** The vendor
firmware drives GPIO1 and GPIO2 high in `setup()` without comment. Without
them the panel ignores every byte: black screen, no error. The display
driver sets them first.

**Colour order is RGB, the opposite of the 1.28".**
`CONFIG_CROWPANEL_LCD_BGR` defaults off. Do not derive it from the vendor's
LovyanGFX `rgb_order` (true on this board); on both CrowPanels that flag is
the inverse of what esp_lcd needs. Check colour against a deliberate
red/green/blue target — a screen of UI chrome will not show a red/blue swap.

**Orientation is applied by esp_lvgl_port, not the panel driver.** Mirror
settings go in the port's rotation config; setting them on the panel directly
is silently overwritten when LVGL attaches. The display mirror and the touch
mirror are independent options, because the touch layer is a separate device
whose axes depend on how *it* is bonded.

**Touch orientation has not been checked on this board.** Capstan, where the
rest of this board layer comes from, keeps touch switched off. The touch
mirror options default off; if taps land at their mirror image, flip
`CONFIG_CROWPANEL_TOUCH_MIRROR_X` / `_Y` — not the display mirror.

**The whole panel is the ring's push switch.** Pressing the glass firmly
also clicks the ring. The firmware treats a contact during which the
switch closed as a *push*, and ignores it as a tap — see
[firmware architecture](firmware-architecture.md#touch-vs-push).

**One detent is 2 quadrature counts.** The encoder part is not documented,
and it is not the 1.28"'s: at the 1.28"'s value of 4, it takes two clicks
to move once. `CONFIG_CROWPANEL_ENCODER_STEPS_PER_DETENT` defaults to 2
(verified on hardware). Ring direction (`CONFIG_CROWPANEL_ENCODER_INVERT`,
off) has not been confirmed on this board yet.

**The ring phases have external pull-ups.** The internal ones stay off;
adding them would only weaken the edges.

**Draw buffers are a fixed size in internal RAM.** Two 16 KB partial
buffers, DMA-capable — the same as the 1.28", not scaled up with the
resolution. Capstan measured both alternatives on this board: a buffer sized
as a fraction of the 360×360 screen left too little contiguous internal RAM
to start a task, and PSRAM buffers made the SPI flush slow enough to trip the
task watchdog.

**GPIO43 lights a demo LED.** It is also UART0 TX, which carries the log
(see `sdkconfig.defaults`), so that LED flickers while the firmware logs.
That is expected.

## Screen layout at 360×360

The pad's screens are redrawn for this glass rather than scaled (see
[GUI](gui.md)). The keys are 88 px and carry all three lines the 2.1" keys
do — icon, name and shortcut. Every shortcut is also listed in
[App mappings](apps.md).

## Recovery

If a firmware never gets far enough to start USB, `scripts/flash.sh`
cannot ask it to reboot into the ROM loader. Hold the board's **BOOT**
(GPIO0) strap low while resetting or powering it, and the chip comes up in
the ROM loader on its built-in USB-Serial/JTAG port and can be flashed
normally — see [building and flashing](building-and-flashing.md#recovery).
Where BOOT and RESET are on your board revision is in Elecrow's wiki for
this product; this repository has not recorded it.
