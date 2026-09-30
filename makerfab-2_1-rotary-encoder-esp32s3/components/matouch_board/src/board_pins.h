/*
 * Makerfabs MaTouch ESP32-S3 Rotary IPS 2.1" — 480x480 ST7701S pin map.
 *
 * Source: github.com/Makerfabs/MaTouch-ESP32-S3-Rotary-IPS-Display-with-Touch-
 *         2.1-ST7701, example/fw_test/fw_test.ino (Arduino_ESP32RGBPanel
 *         constructor), cross-checked against the wiki.makerfabs.com pin
 *         table. Verified on hardware in the TrailCurrent Capstan firmware.
 *
 * If you change a number here, cite where it came from. A wrong GPIO in this
 * file presents as a dead panel or a dead encoder, and the debugging goes
 * looking at the driver.
 */
#pragma once

/* 3-wire SPI sideband, used only to push the ST7701S init sequence. */
#define BOARD_LCD_SPI_CS        1
#define BOARD_LCD_SPI_SCK       46
#define BOARD_LCD_SPI_SDA       0
#define BOARD_LCD_RST           -1   /* Not connected on this board. */

/* 16-bit RGB565 parallel bus. */
#define BOARD_LCD_DE            2
#define BOARD_LCD_VSYNC         42
#define BOARD_LCD_HSYNC         3
#define BOARD_LCD_PCLK          45

/*
 * Colour channel order. The vendor firmware comments and the vendor wiki
 * label 11/15/12/16/21 and 4/41/5/40/6 opposite ways round. This table
 * follows the WIKI; CONFIG_MATOUCH_LCD_SWAP_RB (default y, verified on
 * hardware) swaps the two groups when they are handed to esp_lcd. Do not
 * rewire the table to fix a colour swap -- flip that option instead.
 */
#define BOARD_LCD_B0            11
#define BOARD_LCD_B1            15
#define BOARD_LCD_B2            12
#define BOARD_LCD_B3            16
#define BOARD_LCD_B4            21
#define BOARD_LCD_G0            39
#define BOARD_LCD_G1            7
#define BOARD_LCD_G2            47
#define BOARD_LCD_G3            8
#define BOARD_LCD_G4            48
#define BOARD_LCD_G5            9
#define BOARD_LCD_R0            4
#define BOARD_LCD_R1            41
#define BOARD_LCD_R2            5
#define BOARD_LCD_R3            40
#define BOARD_LCD_R4            6

/*
 * RGB timings from the vendor's Arduino_ST7701_RGBPanel constructor. 12 MHz
 * pclk is Arduino_GFX's default for octal PSRAM, not a vendor spec -- raise it
 * only with a frame-rate measurement to justify it.
 */
#define BOARD_LCD_PCLK_HZ       (12 * 1000 * 1000)
#define BOARD_LCD_HSYNC_FRONT   10
#define BOARD_LCD_HSYNC_PULSE   8
#define BOARD_LCD_HSYNC_BACK    50
#define BOARD_LCD_VSYNC_FRONT   10
#define BOARD_LCD_VSYNC_PULSE   8
#define BOARD_LCD_VSYNC_BACK    20
#define BOARD_LCD_HSYNC_POL     1
#define BOARD_LCD_VSYNC_POL     1

/* Backlight gates an S8050 NPN, so LEDC dimming works. */
#define BOARD_BL_GPIO           38
#define BOARD_BL_PWM_HZ         5000
#define BOARD_BL_PWM_BITS       8

/*
 * Touch: CST826 (NOT CST8266 as the product brief says). Same register map
 * and 0x15 address as the CST816S, so that driver is used.
 *
 * INT and RST are physically unusable: TP_INT reaches GPIO0 only through an
 * unpopulated resistor (and GPIO0 is the LCD SPI SDA), and TP_RST has a
 * pull-up with no GPIO drive. The controller is polled over I2C.
 */
#define BOARD_TOUCH_I2C_SDA     17
#define BOARD_TOUCH_I2C_SCL     18
#define BOARD_TOUCH_I2C_ADDR    0x15
#define BOARD_TOUCH_INT         -1
#define BOARD_TOUCH_RST         -1

/* Rotary ring. Decoded by PCNT, full quadrature (4 edges per cycle). */
#define BOARD_ENC_A             13
#define BOARD_ENC_B             10
#define BOARD_ENC_BTN           14   /* Active low. */
