/*
 * Elecrow CrowPanel 1.46" Rotary — 360x360 JD9855 pin map.
 *
 * Source: github.com/Elecrow-RD/CrowPanel-1.46inch-HMI-ESP32-Rotary-Display
 *         (branch `master`), example/V1.0/Arduino/
 *         RotaryScreen_1_46_Code_Core3_LVGL9/RotaryScreen_1_46.h. Every pin
 *         was cross-checked against the Eagle schematic netlist (the
 *         .sch files under Eagle_SCH&PCB) and verified on hardware in the
 *         TrailCurrent Capstan firmware.
 *
 * Despite "ESP32" in the product name this is an ESP32-S3R8 (8 MB octal
 * PSRAM, 16 MB flash).
 *
 * Pin-for-pin identical to the 1.28" for display, touch, encoder and
 * backlight. What differs is the panel controller (JD9855, not GC9A01), the
 * resolution, colour inversion, the encoder part, and the LED ring. Do not
 * carry a value across from the 1.28" because the pins match -- several of
 * the Kconfig defaults are deliberately different (see Kconfig).
 *
 * If you change a number here, cite where it came from. A wrong GPIO in this
 * file presents as a dead panel or a dead encoder, and the debugging goes
 * looking at the driver.
 */
#pragma once

/* 4-wire SPI panel. Plain SPI at 80 MHz -- not QSPI, despite the form
 * factor. */
#define BOARD_LCD_SPI_HOST      SPI2_HOST
#define BOARD_LCD_SPI_SCK       10
#define BOARD_LCD_SPI_MOSI      11
#define BOARD_LCD_SPI_DC        3
#define BOARD_LCD_SPI_CS        9
#define BOARD_LCD_RST           14
#define BOARD_LCD_SPI_HZ        (80 * 1000 * 1000)
#define BOARD_LCD_INVERT        0    /* cfg.invert = false (the 1.28" is true) */

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

/* Touch: the panel datasheet says CST816D, the vendor firmware uses a CST816T
 * library. Same family, same 0x15 address, same register map as the CST816S,
 * so esp_lcd_touch_cst816s drives it. INT and RST are wired. */
#define BOARD_TOUCH_I2C_SDA     6
#define BOARD_TOUCH_I2C_SCL     7
#define BOARD_TOUCH_I2C_ADDR    0x15
#define BOARD_TOUCH_INT         5
#define BOARD_TOUCH_RST         13

/*
 * Rotary ring. The part is NOT documented by the vendor, and it is NOT the
 * 1.28"'s EC3501: it counts 2 quadrature edges per detent where the 1.28"
 * counts 4 (verified on hardware -- see CROWPANEL_ENCODER_STEPS_PER_DETENT).
 * Decoded by PCNT. The phases have EXTERNAL pull-ups -- the vendor uses bare
 * INPUT on both -- so the internal ones stay off.
 */
#define BOARD_ENC_A             45
#define BOARD_ENC_B             42
#define BOARD_ENC_BTN           41   /* Active low. */

/*
 * WS2812 LED ring, 8 LEDs, GRB. Same vendor source as above (LED_PIN,
 * LED_NUM, and `digitalWrite(17, HIGH)` in setup()).
 */
#define BOARD_WS2812_GPIO       48
#define BOARD_WS2812_COUNT      8
#define BOARD_WS2812_EN         17   /* Ring supply; dark unless driven high. */
/* Which half of the ring each LED is on, seen from the front, in chain
 * order: -1 left, +1 right, 0 neither. Mapped on hardware in TrailCurrent
 * Capstan by lighting each index its own colour: 0 at 2 o'clock, 1 at 4,
 * 2 at 5, 3 at 7, 4 at 8, 5 at 10, 6 at 11, 7 at 1. The pair at 5 and 7 are
 * the bottom centre and belong to neither side. */
#define BOARD_WS2812_SIDE       { +1, +1, 0, 0, -1, -1, -1, +1 }

/*
 * Not driven by this firmware:
 *   GPIO40  power LED, active low
 *   GPIO18  battery sense (ADC2_CH7, through a 1K/1K divider)
 *   GPIO15  charge status, active low
 *   GPIO38/39  auxiliary I2C header
 *
 * GPIO43/44 are UART0 and carry the ESP console -- see sdkconfig.defaults.
 * On this board GPIO43 (U0TXD) also drives the vendor's demo "bulb" LED, so
 * that LED flickers with log output. Harmless, and expected.
 */
