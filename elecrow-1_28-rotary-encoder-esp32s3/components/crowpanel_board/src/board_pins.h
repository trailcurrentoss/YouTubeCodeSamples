/*
 * Elecrow CrowPanel 1.28" Rotary — 240x240 GC9A01 pin map.
 *
 * Source: github.com/Elecrow-RD/CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-
 *         240-240-IPS-Round-Touch-Knob-Screen (branch `master`),
 *         example/Arduino/RotaryScreen_1_28/ — the LovyanGFX LGFX class.
 *         The wiki pin table agrees on every value. Verified on hardware in
 *         the TrailCurrent Capstan firmware.
 *
 * Despite "ESP32" in the product name this is an ESP32-S3R8 (8 MB octal
 * PSRAM, 16 MB flash), confirmed by reading the silicon over USB.
 *
 * If you change a number here, cite where it came from. A wrong GPIO in this
 * file presents as a dead panel or a dead encoder, and the debugging goes
 * looking at the driver.
 */
#pragma once

/* 4-wire SPI panel. */
#define BOARD_LCD_SPI_HOST      SPI2_HOST
#define BOARD_LCD_SPI_SCK       10
#define BOARD_LCD_SPI_MOSI      11
#define BOARD_LCD_SPI_DC        3
#define BOARD_LCD_SPI_CS        9
#define BOARD_LCD_RST           14
#define BOARD_LCD_SPI_HZ        (80 * 1000 * 1000)
#define BOARD_LCD_INVERT        1    /* cfg.invert = true */

/*
 * TWO RAILS MUST BE HIGH BEFORE ANY PANEL TRAFFIC.
 *
 * The vendor firmware drives GPIO1 and GPIO2 high in setup() with no
 * explanation. Without them the panel accepts no data and the board looks
 * dead -- no error, just a black screen.
 */
#define BOARD_LCD_RAIL_A        1
#define BOARD_LCD_RAIL_B        2

/* Backlight, LEDC-dimmable. */
#define BOARD_BL_GPIO           46
#define BOARD_BL_PWM_HZ         5000
#define BOARD_BL_PWM_BITS       8

/* Touch: CST816D. Same register map and 0x15 address as the CST816S. Unlike
 * the MaTouch, INT and RST are wired, so the controller is interrupt-driven. */
#define BOARD_TOUCH_I2C_SDA     6
#define BOARD_TOUCH_I2C_SCL     7
#define BOARD_TOUCH_I2C_ADDR    0x15
#define BOARD_TOUCH_INT         5
#define BOARD_TOUCH_RST         13

/*
 * Rotary ring: EC3501 C15H30P3, 30 detents, contact chatter <= 5 ms.
 * Decoded by PCNT, full quadrature (4 edges per cycle). The phases have
 * EXTERNAL pull-ups -- the vendor uses bare INPUT on both -- so the internal
 * ones stay off.
 */
#define BOARD_ENC_A             45
#define BOARD_ENC_B             42
#define BOARD_ENC_BTN           41   /* Active low. */

/*
 * WS2812 LED ring, 5 LEDs, GRB. Same vendor source as above. No supply
 * enable on this board (the 1.46" gates its ring on GPIO17); the ring is
 * live as soon as the board is powered.
 */
#define BOARD_WS2812_GPIO       48
#define BOARD_WS2812_COUNT      5
/* Which half of the ring each LED is on, seen from the front, in chain
 * order: -1 left, +1 right, 0 neither. Mapped on hardware in TrailCurrent
 * Capstan by lighting each index its own colour: 0 at 4 o'clock, 1 at 1,
 * 2 at 11, 3 just shy of 9, 4 at 6. The one at 6 is the bottom centre and
 * belongs to neither side. */
#define BOARD_WS2812_SIDE       { +1, +1, -1, -1, 0 }

/*
 * Not driven by this firmware:
 *   GPIO40  power LED, active low
 * GPIO43/44 (UART0) are free on this board and carry the ESP console -- see
 * sdkconfig.defaults.
 */
