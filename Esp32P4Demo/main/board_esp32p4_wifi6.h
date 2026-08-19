/*
 * Board support definitions — Waveshare ESP32-P4-WIFI6 (SKU 31647).
 *
 * Bare board variant: no MIPI-DSI display, no MIPI-CSI camera fitted.
 *
 * Module:  ESP32-P4NRW32
 *   - 32 MB in-package HEX PSRAM
 *   - 32 MB onboard NOR flash
 *   - ESP32-C6-MINI-1 companion radio over SDIO (Wi-Fi 6 / BLE 5)
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------------
 * SDMMC slot allocation — the ESP32-P4 has two slots and this board uses both:
 *
 *   Slot 0  ->  microSD card         (pins below)
 *   Slot 1  ->  ESP32-C6 Wi-Fi radio (esp_hosted hardcodes SDMMC_HOST_SLOT_1)
 *
 * Do not move the card to slot 1. SDMMC_HOST_DEFAULT() selects slot 1, so
 * forgetting to override it makes the card and the radio fight over the same
 * slot; the symptom is a Wi-Fi failure that looks like broken C6 firmware.
 * -------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 * microSD (TF) card slot — SDMMC slot 0, 4-bit, UHS-I capable.
 *
 * These are the ESP32-P4's dedicated high-speed SDMMC pins. The card's VDD and
 * bus IO rail are fed from the SoC's internal LDO channel 4 (VO4), so the power
 * control driver must be brought up before the card is probed.
 * -------------------------------------------------------------------------- */
#define BSP_SD_CLK              GPIO_NUM_43
#define BSP_SD_CMD              GPIO_NUM_44
#define BSP_SD_D0               GPIO_NUM_39
#define BSP_SD_D1               GPIO_NUM_40
#define BSP_SD_D2               GPIO_NUM_41
#define BSP_SD_D3               GPIO_NUM_42

/* No card-detect or write-protect signal is routed on this board. */
#define BSP_SD_CD               GPIO_NUM_NC
#define BSP_SD_WP               GPIO_NUM_NC

/* On-chip LDO channel powering the SDMMC IO rail. */
#define BSP_SD_PWR_LDO_CHAN_ID  4

/* --------------------------------------------------------------------------
 * I2C bus (shared with the 40-pin header, ID EEPROM, and any HAT)
 * -------------------------------------------------------------------------- */
#define BSP_I2C_SDA             GPIO_NUM_7
#define BSP_I2C_SCL             GPIO_NUM_8

/* --------------------------------------------------------------------------
 * Audio — ES8311 codec on the shared I2C bus, audio data over I2S.
 *
 * The onboard SMD microphone is ANALOG: it feeds the ES8311's ADC, and the P4
 * reads the digitised result on I2S DIN. It is not a PDM microphone, so the
 * codec must be configured with es8311_microphone_config(dev, false).
 *
 * MCLK must be supplied on its own pin at 384x the sample rate; the codec
 * cannot derive its clock from SCLK in this wiring.
 * -------------------------------------------------------------------------- */
#define BSP_I2S_MCLK            GPIO_NUM_13
#define BSP_I2S_BCLK            GPIO_NUM_12
#define BSP_I2S_WS              GPIO_NUM_10
#define BSP_I2S_DOUT            GPIO_NUM_9   /* to codec DAC (playback) */
#define BSP_I2S_DIN             GPIO_NUM_11  /* from codec ADC (microphone) */

/* Speaker amplifier enable. Held low while recording so the amplifier cannot
 * feed the speaker back into the microphone. */
#define BSP_AUDIO_PA_EN         GPIO_NUM_53

#define BSP_ES8311_I2C_ADDR     0x18

/* --------------------------------------------------------------------------
 * ESP32-C6 companion radio — SDIO slot 1.
 *
 * These match the defaults that the esp_hosted component itself ships for
 * ESP32-P4 (CONFIG_ESP_HOSTED_SDIO_PIN_*), which the board follows, so Wi-Fi
 * needs no pin overrides. They are listed here for reference — esp_hosted owns
 * the actual configuration, so do not try to drive these pins directly.
 * -------------------------------------------------------------------------- */
#define BSP_C6_SDIO_CLK         GPIO_NUM_18
#define BSP_C6_SDIO_CMD         GPIO_NUM_19
#define BSP_C6_SDIO_D0          GPIO_NUM_14
#define BSP_C6_SDIO_D1          GPIO_NUM_15
#define BSP_C6_SDIO_D2          GPIO_NUM_16
#define BSP_C6_SDIO_D3          GPIO_NUM_17
#define BSP_C6_RESET            GPIO_NUM_54

/* --------------------------------------------------------------------------
 * Record push button — user-fitted, on the 40-pin header.
 *
 * Wired between header pin 35 (GPIO20) and ground, so it is active-low with
 * the internal pull-up enabled.
 * -------------------------------------------------------------------------- */
#define BSP_RECORD_BUTTON       GPIO_NUM_20

/* --------------------------------------------------------------------------
 * Filesystem mount point
 * -------------------------------------------------------------------------- */
#define BSP_SD_MOUNT_POINT      "/sdcard"

#ifdef __cplusplus
}
#endif
