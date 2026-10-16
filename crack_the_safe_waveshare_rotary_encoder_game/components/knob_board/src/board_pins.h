/*
 * Waveshare ESP32-S3-Knob-Touch-LCD-1.8 -- ESP32-S3 side.
 *
 * Every pin is cited to where it was read. "Demo" is Waveshare's ESP-IDF demo
 * package (ESP32-S3-Knob-Touch-LCD-1.8-Demo/ESP-IDF/<example>/main/
 * user_config.h); "Sch p.N" is page N of the board schematic. Where the two
 * disagree, the schematic wins and the disagreement is noted.
 *
 * The board has a SECOND MCU, an ESP32-U4WDH, on the other orientation of the
 * USB-C plug. It owns the second knob switch pair and the PCM5100A's
 * soft-mute line, and it has the Classic Bluetooth the S3 lacks. This game
 * gives it its own firmware (coprocessor/) and talks to it over a UART --
 * see board_coproc.c.
 */
#pragma once

#include "driver/gpio.h"

/* ---- LCD: ST77916, 360x360 round, QSPI on SPI2 (Demo 08, Sch p.2) ------- */
#define BOARD_LCD_HOST        SPI2_HOST
#define BOARD_LCD_H_RES       360
#define BOARD_LCD_V_RES       360
#define BOARD_LCD_CS          GPIO_NUM_14
#define BOARD_LCD_PCLK        GPIO_NUM_13
#define BOARD_LCD_D0          GPIO_NUM_15
#define BOARD_LCD_D1          GPIO_NUM_16
#define BOARD_LCD_D2          GPIO_NUM_17
#define BOARD_LCD_D3          GPIO_NUM_18
#define BOARD_LCD_RST         GPIO_NUM_21
/* LCD_TE is on the panel connector but no-connect on the S3 (Sch p.2), so
 * there is no tearing sync to wait on. */

/* Backlight: AO3400 low-side switch with a 10k pull-down, so active HIGH and
 * dark while the pin floats at reset (Sch p.1). PWM per Demo 08's
 * lcd_bl_pwm_bsp: LEDC low-speed, 50 kHz, 8-bit. */
#define BOARD_LCD_BL          GPIO_NUM_47
#define BOARD_LCD_BL_FREQ_HZ  50000

/* ---- I2C0: CST816 touch @0x15 and DRV2605 haptics @0x5A (Sch p.2/p.4) --- */
#define BOARD_I2C_PORT        I2C_NUM_0
#define BOARD_I2C_SDA         GPIO_NUM_11
#define BOARD_I2C_SCL         GPIO_NUM_12
#define BOARD_I2C_HZ          300000      /* Demo; 5.1k external pull-ups */
#define BOARD_TOUCH_ADDR      0x15
#define BOARD_TOUCH_INT       GPIO_NUM_9
#define BOARD_TOUCH_RST       GPIO_NUM_10
#define BOARD_HAPTIC_ADDR     0x5A

/* ---- Knob: Alps SSCM110100 bidirectional detector switch (Sch p.1) -----
 *
 * NOT a quadrature encoder. Two independent contacts to ground, each with a
 * 10k pull-up: one closes once per detent clockwise, the other once per
 * detent anticlockwise. Demo 04 decodes it by polling, and counts a RISING
 * edge (contact opening) after a debounced low on A as right, on B as left.
 */
#define BOARD_KNOB_A          GPIO_NUM_8   /* EC1_A -- clockwise */
#define BOARD_KNOB_B          GPIO_NUM_7   /* EC1_B -- anticlockwise */

/* ---- Audio (Demo 07, Sch p.2/p.5) --------------------------------------
 *
 * The PCM5100A DAC sits behind a CH445P 4PDT switch whose select line is the
 * S3's GPIO0 -- which is ALSO the BOOT strap (10k pull-up). HIGH routes the
 * S3's I2S to the DAC, LOW the ESP32's. GPIO0 is high at reset, so the S3
 * owns the DAC from power-on until something drives it low.
 *
 * No MCLK: the DAC's SCK is tied to GND, so it runs its PLL from BCK. FMT is
 * tied to GND, which is standard I2S (Philips) framing -- NOT the MSB-aligned
 * framing Demo 07 configures, which would put every sample a bit late.
 *
 * The DAC's XSMT (soft mute) is driven by the ESP32's IO32, not by the S3.
 * The factory ESP32 firmware holds it un-muted; replace that firmware and
 * the S3's audio goes silent with nothing wrong on this side.
 */
#define BOARD_AUDIO_ROUTE     GPIO_NUM_0    /* CH445P IN: 1 = S3 owns the DAC */
#define BOARD_I2S_BCLK        GPIO_NUM_39
#define BOARD_I2S_WS          GPIO_NUM_40
#define BOARD_I2S_DOUT        GPIO_NUM_41

/* ---- UART to the ESP32 co-processor (Sch p.2/p.3) ----------------------
 *
 * Net names are from the S3's side: ESP32S3_TX runs S3 GPIO38 -> ESP32 IO18,
 * ESP32S3_RX runs ESP32 IO23 -> S3 GPIO48. No series resistors, so getting
 * the direction wrong puts two drivers on one wire. */
#define BOARD_COPROC_UART     UART_NUM_1
#define BOARD_COPROC_TX       GPIO_NUM_38
#define BOARD_COPROC_RX       GPIO_NUM_48
#define BOARD_COPROC_BAUD     921600   /* fast enough to stream audio (bt_link.c) */

/* ---- Battery sense (Sch p.2/p.4, Demo 01) ------------------------------
 *
 * BATT_ADC is the midpoint of R62/R63, 10k/10k, on the "5V" net: the USB
 * supply when plugged in, the cell otherwise. The charger has no status line
 * to the S3; see board_battery.c. */
#define BOARD_BATT_ADC        GPIO_NUM_1
#define BOARD_BATT_ADC_CHANNEL ADC_CHANNEL_0   /* GPIO1 on ADC1 */
#define BOARD_BATT_DIVIDER    2

/* MSM261D4030H1CPM PDM microphone, L/R tied to GND (left slot). PDM RX only
 * exists on I2S0. */
#define BOARD_PDM_CLK         GPIO_NUM_45
#define BOARD_PDM_DIN         GPIO_NUM_46
