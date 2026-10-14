/*
 * The UART to the ESP32 co-processor. Moves text lines both ways and knows
 * nothing about what they say, beyond that each starts with a capital -- the protocol lives in main/bt_link.c and in
 * coprocessor/main/main.c.
 */

#include "board_internal.h"

#include <string.h>

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "board_pins.h"
#include "knob_board.h"

static const char *TAG = "board.coproc";

#define LINE_MAX_LEN 160            /* the ESP32 sends short lines only */

static knob_board_coproc_line_cb_t s_cb;
static void *s_ctx;
static SemaphoreHandle_t s_tx_lock;

static void rx_task(void *arg)
{
    (void)arg;
    char line[LINE_MAX_LEN];
    size_t n = 0;
    bool overflow = false;
    uint8_t buf[64];
    for (;;) {
        const int got = uart_read_bytes(BOARD_COPROC_UART, buf, sizeof(buf), pdMS_TO_TICKS(100));
        for (int i = 0; i < got; i++) {
            const char c = (char)buf[i];
            if (c == '\r') {
                continue;
            }
            if (c != '\n') {
                /* Every line starts with an upper-case keyword. Anything
                 * before it is junk -- typically the byte the ESP32's TX pin
                 * glitches out while it is being configured at boot. */
                if (n == 0 && !(c >= 'A' && c <= 'Z')) {
                    continue;
                }
                if (n < sizeof(line) - 1) {
                    line[n++] = c;
                } else {
                    overflow = true;
                }
                continue;
            }
            line[n] = '\0';
            if (n && !overflow && s_cb) {
                s_cb(line, s_ctx);
            }
            n = 0;
            overflow = false;
        }
    }
}

esp_err_t knob_board_coproc_start(knob_board_coproc_line_cb_t cb, void *ctx)
{
    if (s_tx_lock) {
        return ESP_ERR_INVALID_STATE;
    }
    const uart_config_t uc = {
        .baud_rate = BOARD_COPROC_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    /* An 8 KB TX buffer: a sound upload (~3.6 KB) or a run of streamed audio
     * should not block the caller while it goes out on the wire. */
    esp_err_t err = uart_driver_install(BOARD_COPROC_UART, 1024, 8192, 0, NULL, 0);
    if (err == ESP_OK) {
        err = uart_param_config(BOARD_COPROC_UART, &uc);
    }
    if (err == ESP_OK) {
        err = uart_set_pin(BOARD_COPROC_UART, BOARD_COPROC_TX, BOARD_COPROC_RX,
                           UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "UART setup failed: %s", esp_err_to_name(err));
        return err;
    }
    s_cb = cb;
    s_ctx = ctx;
    s_tx_lock = xSemaphoreCreateMutex();
    xTaskCreate(rx_task, "coproc_rx", 3072, NULL, 4, NULL);
    ESP_LOGI(TAG, "link on UART%d (TX %d, RX %d) at %d", BOARD_COPROC_UART,
             BOARD_COPROC_TX, BOARD_COPROC_RX, BOARD_COPROC_BAUD);
    return ESP_OK;
}

void knob_board_coproc_send(const char *line, size_t len)
{
    if (!s_tx_lock) {
        return;
    }
    xSemaphoreTake(s_tx_lock, portMAX_DELAY);
    uart_write_bytes(BOARD_COPROC_UART, line, len);
    uart_write_bytes(BOARD_COPROC_UART, "\n", 1);
    xSemaphoreGive(s_tx_lock);
}
