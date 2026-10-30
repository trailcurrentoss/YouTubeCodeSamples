#include "sx1262.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "sdkconfig.h"

static const char *TAG = "sx1262";

/* --- Socket pins --- */

#define PIN_CLK  8
#define PIN_MISO 7
#define PIN_MOSI 6
#define PIN_NSS  10
#define PIN_BUSY 9

/* {IRQ, NRST} per board revision: V1.0/V1.1, then V1.2. */
static const int PINMAPS[2][2] = { {53, 54}, {27, 28} };

/* --- Commands --- */

#define CMD_SET_STANDBY        0x80
#define CMD_SET_RX             0x82
#define CMD_SET_TX             0x83
#define CMD_SET_RF_FREQ        0x86
#define CMD_CALIBRATE          0x89
#define CMD_SET_PACKET_TYPE    0x8A
#define CMD_SET_MOD_PARAMS     0x8B
#define CMD_SET_PKT_PARAMS     0x8C
#define CMD_SET_TX_PARAMS      0x8E
#define CMD_SET_BUF_BASE       0x8F
#define CMD_SET_PA_CONFIG      0x95
#define CMD_SET_REGULATOR      0x96
#define CMD_SET_DIO3_TCXO      0x97
#define CMD_CALIBRATE_IMAGE    0x98
#define CMD_SET_DIO2_RF_SWITCH 0x9D
#define CMD_SET_DIO_IRQ        0x08
#define CMD_CLEAR_IRQ          0x02
#define CMD_GET_IRQ            0x12
#define CMD_GET_RX_BUF_STATUS  0x13
#define CMD_GET_PKT_STATUS     0x14
#define CMD_GET_STATUS         0xC0
#define CMD_WRITE_REG          0x0D
#define CMD_READ_REG           0x1D
#define CMD_WRITE_BUF          0x0E
#define CMD_READ_BUF           0x1E

#define REG_VERSION     0x0320
#define REG_SYNC_WORD   0x0740
#define REG_RX_GAIN     0x08AC
#define REG_OCP         0x08E7
#define REG_TX_CLAMP    0x08D8

#define IRQ_MASK  (SX_IRQ_TX_DONE | SX_IRQ_RX_DONE | SX_IRQ_PREAMBLE | SX_IRQ_HEADER_VALID | \
                   SX_IRQ_HEADER_ERR | SX_IRQ_CRC_ERR | SX_IRQ_TIMEOUT)
#define DIO1_MASK (SX_IRQ_TX_DONE | SX_IRQ_RX_DONE | SX_IRQ_HEADER_ERR | SX_IRQ_CRC_ERR | SX_IRQ_TIMEOUT)

static spi_device_handle_t s_dev;
static SemaphoreHandle_t   s_irq_sem;
static int s_irq_pin = -1;
static int s_nrst_pin = -1;
static sx1262_modem_t s_modem;

/* --- SPI plumbing --- */

static bool wait_busy(int ms)
{
    int64_t end = esp_timer_get_time() + ms * 1000LL;
    while (gpio_get_level(PIN_BUSY)) {
        if (esp_timer_get_time() > end) return false;
        vTaskDelay(1);
    }
    return true;
}

static bool xfer(const uint8_t *tx, uint8_t *rx, size_t len)
{
    if (!wait_busy(100)) {
        ESP_LOGW(TAG, "BUSY stuck high before cmd 0x%02X", tx[0]);
        return false;
    }
    spi_transaction_t t = { .length = len * 8, .rxlength = rx ? len * 8 : 0, .tx_buffer = tx, .rx_buffer = rx };
    gpio_set_level(PIN_NSS, 0);
    esp_err_t err = spi_device_polling_transmit(s_dev, &t);
    gpio_set_level(PIN_NSS, 1);
    return err == ESP_OK;
}

/* Write-only command: the status bytes clocked back are not needed. */
static bool cmd(const uint8_t *buf, size_t len)
{
    return xfer(buf, NULL, len);
}

#define CMD(...) do { const uint8_t _b[] = { __VA_ARGS__ }; cmd(_b, sizeof(_b)); } while (0)

static void read_reg(uint16_t addr, uint8_t *out, size_t n)
{
    uint8_t tx[24] = { CMD_READ_REG, addr >> 8, addr & 0xFF, 0 };
    uint8_t rx[24];
    xfer(tx, rx, 4 + n);
    memcpy(out, rx + 4, n);
}

static void write_reg(uint16_t addr, const uint8_t *in, size_t n)
{
    uint8_t tx[24] = { CMD_WRITE_REG, addr >> 8, addr & 0xFF };
    memcpy(tx + 3, in, n);
    cmd(tx, 3 + n);
}

static void IRAM_ATTR dio1_isr(void *arg)
{
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(s_irq_sem, &woken);
    if (woken) portYIELD_FROM_ISR();
}

/* --- Pin-map detection --- */

static bool chip_present(void)
{
    uint8_t ver[16] = {0};
    read_reg(REG_VERSION, ver, sizeof(ver));
    ESP_LOGI(TAG, "version string \"%.15s\"", (const char *)ver);
    return memcmp(ver, "SX126", 5) == 0;
}

/*
 * Arm a 1 ms RX timeout routed to DIO1 and see which candidate pin goes
 * high. Only inputs are touched, so whatever the other pair is wired to on
 * this revision is never driven.
 */
static int detect_pinmap(void)
{
    for (int i = 0; i < 2; i++) {
        gpio_reset_pin(PINMAPS[i][0]);
        gpio_set_direction(PINMAPS[i][0], GPIO_MODE_INPUT);
        gpio_pulldown_en(PINMAPS[i][0]);
    }

    CMD(CMD_SET_STANDBY, 0x00);
    CMD(CMD_SET_DIO_IRQ, SX_IRQ_TIMEOUT >> 8, SX_IRQ_TIMEOUT & 0xFF,
        SX_IRQ_TIMEOUT >> 8, SX_IRQ_TIMEOUT & 0xFF, 0, 0, 0, 0);
    CMD(CMD_CLEAR_IRQ, 0xFF, 0xFF);
    vTaskDelay(pdMS_TO_TICKS(2));

    int before[2] = { gpio_get_level(PINMAPS[0][0]), gpio_get_level(PINMAPS[1][0]) };
    CMD(CMD_SET_RX, 0x00, 0x00, 0x40);      /* 64 * 15.625 us = 1 ms */
    vTaskDelay(pdMS_TO_TICKS(20));
    int after[2] = { gpio_get_level(PINMAPS[0][0]), gpio_get_level(PINMAPS[1][0]) };
    CMD(CMD_CLEAR_IRQ, 0xFF, 0xFF);

    for (int i = 0; i < 2; i++) gpio_pulldown_dis(PINMAPS[i][0]);

    for (int i = 0; i < 2; i++) {
        if (!before[i] && after[i]) return i;
    }
    ESP_LOGW(TAG, "DIO1 not seen on GPIO%d or GPIO%d (before %d/%d after %d/%d)",
             PINMAPS[0][0], PINMAPS[1][0], before[0], before[1], after[0], after[1]);
    return -1;
}

static void hard_reset(void)
{
    gpio_reset_pin(s_nrst_pin);
    gpio_set_direction(s_nrst_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(s_nrst_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    gpio_set_level(s_nrst_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    wait_busy(200);
}

/* --- Public API --- */

esp_err_t sx1262_init(void)
{
    gpio_config_t io = { .pin_bit_mask = 1ULL << PIN_NSS, .mode = GPIO_MODE_OUTPUT };
    gpio_config(&io);
    gpio_set_level(PIN_NSS, 1);
    io = (gpio_config_t){ .pin_bit_mask = 1ULL << PIN_BUSY, .mode = GPIO_MODE_INPUT };
    gpio_config(&io);

    spi_bus_config_t bus = {
        .mosi_io_num = PIN_MOSI, .miso_io_num = PIN_MISO, .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1, .quadhd_io_num = -1, .max_transfer_sz = 300,
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO), TAG, "spi bus");
    spi_device_interface_config_t dc = {
        .clock_speed_hz = 8 * 1000 * 1000, .mode = 0, .spics_io_num = -1, .queue_size = 1,
    };
    ESP_RETURN_ON_ERROR(spi_bus_add_device(SPI2_HOST, &dc, &s_dev), TAG, "spi dev");

    if (!wait_busy(500) || !chip_present()) {
        ESP_LOGE(TAG, "no SX126x on the wireless-module socket");
        return ESP_ERR_NOT_FOUND;
    }

#if CONFIG_MESH_SOCKET_V1_1
    int map = 0;
#elif CONFIG_MESH_SOCKET_V1_2
    int map = 1;
#else
    int map = detect_pinmap();
    if (map < 0) return ESP_ERR_NOT_FOUND;
#endif
    s_irq_pin = PINMAPS[map][0];
    s_nrst_pin = PINMAPS[map][1];
    ESP_LOGI(TAG, "socket pin map %s: IRQ=GPIO%d NRST=GPIO%d",
             map == 0 ? "V1.0/V1.1" : "V1.2", s_irq_pin, s_nrst_pin);

    hard_reset();
    if (!chip_present()) return ESP_ERR_INVALID_RESPONSE;

    s_irq_sem = xSemaphoreCreateBinary();
    gpio_config_t irq = {
        .pin_bit_mask = 1ULL << s_irq_pin, .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&irq);
    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;
    gpio_isr_handler_add(s_irq_pin, dio1_isr, NULL);
    return ESP_OK;
}

int sx1262_irq_pin(void) { return s_irq_pin; }
int sx1262_nrst_pin(void) { return s_nrst_pin; }

static uint8_t bw_code(float khz)
{
    if (khz < 9)   return 0x00;  /* 7.8 */
    if (khz < 13)  return 0x08;  /* 10.4 */
    if (khz < 18)  return 0x01;  /* 15.6 */
    if (khz < 26)  return 0x09;  /* 20.8 */
    if (khz < 36)  return 0x02;  /* 31.25 */
    if (khz < 52)  return 0x0A;  /* 41.7 */
    if (khz < 90)  return 0x03;  /* 62.5 */
    if (khz < 200) return 0x04;  /* 125 */
    if (khz < 400) return 0x05;  /* 250 */
    return 0x06;                 /* 500 */
}

static void calibrate_image(uint32_t hz)
{
    uint32_t mhz = hz / 1000000;
    if (mhz > 900)      CMD(CMD_CALIBRATE_IMAGE, 0xE1, 0xE9);
    else if (mhz > 850) CMD(CMD_CALIBRATE_IMAGE, 0xD7, 0xDB);
    else if (mhz > 770) CMD(CMD_CALIBRATE_IMAGE, 0xC1, 0xC5);
    else if (mhz > 460) CMD(CMD_CALIBRATE_IMAGE, 0x75, 0x81);
    else                CMD(CMD_CALIBRATE_IMAGE, 0x6B, 0x6F);
}

esp_err_t sx1262_configure(const sx1262_modem_t *m)
{
    s_modem = *m;

    CMD(CMD_SET_STANDBY, 0x00);
    /* Elecrow module: TCXO on DIO3 (3.3 V, 5 ms start-up) and the antenna
     * RX/TX switch on DIO2, matching Meshtastic's elecrow_panel variant. */
    CMD(CMD_SET_DIO3_TCXO, 0x07, 0x00, 0x01, 0x40);
    CMD(CMD_SET_DIO2_RF_SWITCH, 0x01);
    CMD(CMD_CALIBRATE, 0x7F);
    vTaskDelay(pdMS_TO_TICKS(10));
    wait_busy(100);
    CMD(CMD_SET_REGULATOR, 0x01);
    CMD(CMD_SET_PACKET_TYPE, 0x01);
    calibrate_image(m->freq_hz);
    vTaskDelay(pdMS_TO_TICKS(5));

    uint32_t frf = (uint32_t)(((uint64_t)m->freq_hz << 25) / 32000000ULL);
    CMD(CMD_SET_RF_FREQ, frf >> 24, frf >> 16, frf >> 8, frf);
    CMD(CMD_SET_BUF_BASE, 0x00, 0x00);

    float symbol_ms = (float)(1 << m->sf) / m->bw_khz;
    uint8_t ldro = symbol_ms > 16.0f;
    CMD(CMD_SET_MOD_PARAMS, m->sf, bw_code(m->bw_khz), m->cr - 4, ldro);
    CMD(CMD_SET_PKT_PARAMS, m->preamble >> 8, m->preamble & 0xFF, 0x00, 0xFF, 0x01, 0x00);

    uint8_t sw[2] = { (m->sync_word & 0xF0) | 0x04, ((m->sync_word & 0x0F) << 4) | 0x04 };
    write_reg(REG_SYNC_WORD, sw, 2);
    if (m->rx_boosted) write_reg(REG_RX_GAIN, (const uint8_t[]){ 0x96 }, 1);

    /* High-power PA up to +22 dBm, 140 mA OCP, errata 15.2 TX clamp fix. */
    int8_t dbm = m->tx_dbm > 22 ? 22 : (m->tx_dbm < -9 ? -9 : m->tx_dbm);
    CMD(CMD_SET_PA_CONFIG, 0x04, 0x07, 0x00, 0x01);
    CMD(CMD_SET_TX_PARAMS, (uint8_t)dbm, 0x04);
    write_reg(REG_OCP, (const uint8_t[]){ 0x38 }, 1);
    uint8_t clamp;
    read_reg(REG_TX_CLAMP, &clamp, 1);
    clamp |= 0x1E;
    write_reg(REG_TX_CLAMP, &clamp, 1);

    CMD(CMD_SET_DIO_IRQ, IRQ_MASK >> 8, IRQ_MASK & 0xFF, DIO1_MASK >> 8, DIO1_MASK & 0xFF, 0, 0, 0, 0);
    CMD(CMD_CLEAR_IRQ, 0xFF, 0xFF);

    ESP_LOGI(TAG, "modem %.3f MHz BW%.1f SF%u CR4/%u preamble %u sync 0x%02X %d dBm LDRO %u",
             m->freq_hz / 1e6, m->bw_khz, m->sf, m->cr, m->preamble, m->sync_word, dbm, ldro);
    return ESP_OK;
}

void sx1262_start_rx(void)
{
    CMD(CMD_SET_PKT_PARAMS, s_modem.preamble >> 8, s_modem.preamble & 0xFF, 0x00, 0xFF, 0x01, 0x00);
    CMD(CMD_CLEAR_IRQ, 0xFF, 0xFF);
    CMD(CMD_SET_RX, 0xFF, 0xFF, 0xFF);      /* continuous */
}

esp_err_t sx1262_transmit(const uint8_t *data, size_t len, uint32_t timeout_ms)
{
    if (len > 255) return ESP_ERR_INVALID_SIZE;
    CMD(CMD_SET_STANDBY, 0x00);
    CMD(CMD_SET_PKT_PARAMS, s_modem.preamble >> 8, s_modem.preamble & 0xFF, 0x00, (uint8_t)len, 0x01, 0x00);

    uint8_t tx[258] = { CMD_WRITE_BUF, 0x00 };
    memcpy(tx + 2, data, len);
    cmd(tx, 2 + len);

    CMD(CMD_CLEAR_IRQ, 0xFF, 0xFF);
    xSemaphoreTake(s_irq_sem, 0);
    uint32_t t = timeout_ms * 64;           /* 15.625 us units */
    CMD(CMD_SET_TX, t >> 16, t >> 8, t);

    int64_t end = esp_timer_get_time() + (timeout_ms + 100) * 1000LL;
    while (esp_timer_get_time() < end) {
        sx1262_wait_irq(pdMS_TO_TICKS(50));
        uint16_t irq = sx1262_get_irq();
        if (irq & SX_IRQ_TX_DONE) {
            CMD(CMD_CLEAR_IRQ, 0xFF, 0xFF);
            return ESP_OK;
        }
        if (irq & SX_IRQ_TIMEOUT) break;
    }
    CMD(CMD_CLEAR_IRQ, 0xFF, 0xFF);
    CMD(CMD_SET_STANDBY, 0x00);
    return ESP_ERR_TIMEOUT;
}

bool sx1262_wait_irq(TickType_t ticks)
{
    if (gpio_get_level(s_irq_pin)) return true;
    return xSemaphoreTake(s_irq_sem, ticks) == pdTRUE;
}

uint16_t sx1262_get_irq(void)
{
    uint8_t tx[4] = { CMD_GET_IRQ }, rx[4] = {0};
    xfer(tx, rx, 4);
    return rx[2] << 8 | rx[3];
}

void sx1262_clear_irq(uint16_t mask)
{
    CMD(CMD_CLEAR_IRQ, mask >> 8, mask & 0xFF);
}

int sx1262_read_packet(uint8_t *buf, size_t cap, float *rssi, float *snr)
{
    uint8_t tx[4] = { CMD_GET_RX_BUF_STATUS }, rx[4];
    xfer(tx, rx, 4);
    size_t len = rx[2];
    uint8_t start = rx[3];
    if (len > cap) return -1;

    uint8_t ps_tx[5] = { CMD_GET_PKT_STATUS }, ps[5];
    xfer(ps_tx, ps, 5);
    if (rssi) *rssi = -ps[2] / 2.0f;
    if (snr) *snr = (int8_t)ps[3] / 4.0f;

    uint8_t rd_tx[258] = { CMD_READ_BUF, start, 0 }, rd[258];
    xfer(rd_tx, rd, 3 + len);
    memcpy(buf, rd + 3, len);
    return (int)len;
}

bool sx1262_receiving(void)
{
    uint16_t irq = sx1262_get_irq();
    return (irq & (SX_IRQ_PREAMBLE | SX_IRQ_HEADER_VALID)) && !(irq & SX_IRQ_RX_DONE);
}
