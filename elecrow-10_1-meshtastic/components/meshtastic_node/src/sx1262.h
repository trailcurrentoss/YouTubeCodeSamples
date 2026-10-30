#pragma once

/*
 * SX1262 driver for the CrowPanel Advance P4 wireless-module socket.
 *
 * SPI, NSS and BUSY are the same on every board revision. IRQ (DIO1) and
 * NRST moved between V1.1 and V1.2, so sx1262_init() works out which pair
 * is wired by reading the chip and watching which candidate IRQ pin rises.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#define SX_IRQ_TX_DONE       0x0001
#define SX_IRQ_RX_DONE       0x0002
#define SX_IRQ_PREAMBLE      0x0004
#define SX_IRQ_HEADER_VALID  0x0010
#define SX_IRQ_HEADER_ERR    0x0020
#define SX_IRQ_CRC_ERR       0x0040
#define SX_IRQ_TIMEOUT       0x0200

typedef struct {
    uint32_t freq_hz;
    float    bw_khz;
    uint8_t  sf;
    uint8_t  cr;          /* denominator: 5..8 means 4/5..4/8 */
    uint16_t preamble;
    uint8_t  sync_word;   /* one-byte form, e.g. 0x2B */
    int8_t   tx_dbm;
    bool     rx_boosted;
} sx1262_modem_t;

esp_err_t sx1262_init(void);
int  sx1262_irq_pin(void);
int  sx1262_nrst_pin(void);

esp_err_t sx1262_configure(const sx1262_modem_t *m);
void sx1262_start_rx(void);

/* Blocks until TX done or timeout, then leaves the radio in standby. */
esp_err_t sx1262_transmit(const uint8_t *data, size_t len, uint32_t timeout_ms);

/* Waits for a DIO1 edge. Returns true if one arrived. */
bool sx1262_wait_irq(TickType_t ticks);

uint16_t sx1262_get_irq(void);
void     sx1262_clear_irq(uint16_t mask);

/* Reads the last received packet. Returns length, or -1. */
int sx1262_read_packet(uint8_t *buf, size_t cap, float *rssi, float *snr);

/* True while a preamble/header has been detected but no RxDone yet. */
bool sx1262_receiving(void);
