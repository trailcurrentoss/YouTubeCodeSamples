# Hardware

The firmware targets exactly one board: the **Elecrow CrowPanel 1.28"
HMI ESP32 Rotary Display** (240×240 round IPS, touch, rotary knob). Nothing
here is written to be portable to other panels — the pin map, panel driver
and touch setup are specific to it. The Makerfabs MaTouch 2.1" version of
this pad lives next door in `../makerfab-2_1-rotary-encoder-esp32s3`.

| Part | Detail |
|---|---|
| SoC | ESP32-S3R8 — 8 MB **octal** PSRAM, 16 MB quad flash (despite "ESP32" in the product name) |
| Display | 1.28" round IPS, 240×240, GC9A01, 4-wire SPI at 80 MHz |
| Touch | CST816D capacitive (CST816S register map, I²C `0x15`), interrupt-driven |
| Input | Rotary ring (EC3501, 30 detents) with a push switch — the whole panel is the button |
| USB | One USB-C on the ESP32-S3's native USB (no UART bridge) |
| Also fitted | 5 × WS2812 around the ring (GPIO48) and a power LED (GPIO40) — not used by this firmware |

Vendor sources: the Elecrow GitHub repository
`CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-240-240-IPS-Round-Touch-Knob-Screen`
(`example/Arduino/RotaryScreen_1_28/`) and the Elecrow wiki, which agree on
every pin. Every value here was verified on hardware in the TrailCurrent
Capstan firmware, which runs on the same board.

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

## Things this board does that you would not expect

**Two GPIOs have to be high before the panel does anything.** The vendor
firmware drives GPIO1 and GPIO2 high in `setup()` without comment. Without
them the panel ignores every byte: black screen, no error. The display
driver sets them first.

**Red and blue are swapped unless you ask for BGR.**
`CONFIG_CROWPANEL_LCD_BGR` defaults on. Do not derive it from the vendor's
LovyanGFX `rgb_order` (false on this board); that flag does not map onto
esp_lcd's element order the way its name suggests. Check colour against a
deliberate red/green/blue target — a screen of UI chrome will not show a
red/blue swap.

**The display is mirrored, the touch is not.** The panel needs
`CONFIG_CROWPANEL_LCD_MIRROR_X`, but the touch layer already reports in the
corrected orientation, so the touch mirror options stay off. Tying the two
together gives a perfect picture with back-to-front touch. The mirror is
applied through esp_lvgl_port's rotation settings; setting it on the panel
directly is silently overwritten when LVGL attaches.

**The whole panel is the ring's push switch.** Pressing the glass firmly
also clicks the ring. The firmware treats a contact during which the
switch closed as a *push*, and ignores it as a tap — see
[firmware architecture](firmware-architecture.md#touch-vs-push).

**One detent is 4 quadrature counts, not the datasheet's 2.** The EC3501
datasheet says "1 pulse per 2 detents", which would give 2; its own
section 4-2 (30 pulses per revolution per phase) gives 4, and the board
agrees with 4. `CONFIG_CROWPANEL_ENCODER_STEPS_PER_DETENT` defaults to 4.
Ring direction is correct as wired (`CONFIG_CROWPANEL_ENCODER_INVERT` off),
the opposite of the MaTouch.

**The ring phases have external pull-ups.** The internal ones stay off;
adding them would only weaken the edges.

**Draw buffers live in internal RAM.** Two 16 KB partial buffers, DMA-capable.
Capstan tried PSRAM buffers on this board: the SPI flush then ran slowly
enough to trip the task watchdog.

## Screen layout at 240×240

The 2.1" pad's screens are redrawn for the smaller glass rather than
scaled (see [GUI](gui.md)). The keys are 60 px — about 6.5 mm on this
panel — and carry an icon and a name; the shortcut line printed on the
2.1" keys would be 8 px tall here, so it is dropped. Every key's shortcut
is still listed in [App mappings](apps.md).

## Recovery

If a firmware never gets far enough to start USB, `scripts/flash.sh`
cannot ask it to reboot into the ROM loader. Hold the board's **BOOT**
(GPIO0) strap low while resetting or powering it, and the chip comes up in
the ROM loader on its built-in USB-Serial/JTAG port and can be flashed
normally — see [building and flashing](building-and-flashing.md#recovery).
Where BOOT and RESET are on your board revision is in Elecrow's wiki for
this product; this repository has not recorded it.
