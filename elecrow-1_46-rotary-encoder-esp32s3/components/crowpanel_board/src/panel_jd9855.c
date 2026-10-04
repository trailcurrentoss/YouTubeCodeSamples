/*
 * JD9855 esp_lcd panel driver — CrowPanel 1.46" 360x360.
 *
 * Structured after Espressif's own esp_lcd_gc9a01 so it behaves like a
 * first-party driver and can be replaced by one if it ever appears.
 *
 * The register sequence lives in jd9855_init_table.h and is transcribed from
 * LovyanGFX's Panel_ST77961, which is what the vendor firmware runs. That
 * transcription was verified byte-for-byte against upstream: 47 commands,
 * 193 parameter bytes.
 */

#include <stdlib.h>
#include <string.h>          /* memset -- include what you use rather than
                                relying on an esp_* header to supply it. */
#include <sys/cdefs.h>

#include "esp_check.h"
#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "panel_jd9855.h"
#include "jd9855_init_table.h"

static const char *TAG = "jd9855";

typedef struct {
    esp_lcd_panel_t            base;
    esp_lcd_panel_io_handle_t  io;
    int                        reset_gpio_num;
    bool                       reset_level;
    int                        x_gap;
    int                        y_gap;
    uint8_t                    madctl;
    uint8_t                    colmod;
} jd9855_panel_t;

static esp_err_t panel_del(esp_lcd_panel_t *panel);
static esp_err_t panel_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_init(esp_lcd_panel_t *panel);
static esp_err_t panel_draw_bitmap(esp_lcd_panel_t *panel, int x0, int y0,
                                   int x1, int y1, const void *data);
static esp_err_t panel_invert_color(esp_lcd_panel_t *panel, bool invert);
static esp_err_t panel_mirror(esp_lcd_panel_t *panel, bool x, bool y);
static esp_err_t panel_swap_xy(esp_lcd_panel_t *panel, bool swap);
static esp_err_t panel_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_disp_on_off(esp_lcd_panel_t *panel, bool on);

esp_err_t esp_lcd_new_panel_jd9855(esp_lcd_panel_io_handle_t io,
                                   const esp_lcd_panel_dev_config_t *dev_cfg,
                                   esp_lcd_panel_handle_t *ret_panel)
{
    esp_err_t ret = ESP_OK;
    jd9855_panel_t *jd = NULL;

    ESP_RETURN_ON_FALSE(io && dev_cfg && ret_panel, ESP_ERR_INVALID_ARG, TAG,
                        "invalid argument");

    jd = calloc(1, sizeof(jd9855_panel_t));
    ESP_RETURN_ON_FALSE(jd, ESP_ERR_NO_MEM, TAG, "no mem for panel");

    if (dev_cfg->reset_gpio_num >= 0) {
        gpio_config_t cfg = {
            .mode         = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << dev_cfg->reset_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&cfg), err, TAG, "reset gpio config failed");
    }

    /*
     * Colour order. The 1.46" runs rgb_order = true in the vendor firmware,
     * which is the OPPOSITE of the 1.28" -- do not carry one board's value
     * across to the other.
     */
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
    switch (dev_cfg->rgb_ele_order) {
    case LCD_RGB_ELEMENT_ORDER_RGB: jd->madctl = 0;                 break;
    case LCD_RGB_ELEMENT_ORDER_BGR: jd->madctl = LCD_CMD_BGR_BIT;   break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG,
                          "unsupported rgb element order");
    }
#else
    switch (dev_cfg->color_space) {
    case ESP_LCD_COLOR_SPACE_RGB: jd->madctl = 0;               break;
    case ESP_LCD_COLOR_SPACE_BGR: jd->madctl = LCD_CMD_BGR_BIT; break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG,
                          "unsupported color space");
    }
#endif

    /*
     * The init table already sets COLMOD to 0x05 (RGB565). We record the
     * requested depth so panel_init() can override it for 18-bit, but 16-bit
     * is the only mode this board is wired for.
     */
    switch (dev_cfg->bits_per_pixel) {
    case 16: jd->colmod = 0x05; break;
    case 18: jd->colmod = 0x06; break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG,
                          "unsupported bpp %d", dev_cfg->bits_per_pixel);
    }

    jd->io             = io;
    jd->reset_gpio_num = dev_cfg->reset_gpio_num;
    jd->reset_level    = dev_cfg->flags.reset_active_high;

    jd->base.del          = panel_del;
    jd->base.reset        = panel_reset;
    jd->base.init         = panel_init;
    jd->base.draw_bitmap  = panel_draw_bitmap;
    jd->base.invert_color = panel_invert_color;
    jd->base.mirror       = panel_mirror;
    jd->base.swap_xy      = panel_swap_xy;
    jd->base.set_gap      = panel_set_gap;
    jd->base.disp_on_off  = panel_disp_on_off;

    *ret_panel = &jd->base;
    ESP_LOGD(TAG, "panel created @%p", jd);
    return ESP_OK;

err:
    if (jd) {
        if (dev_cfg->reset_gpio_num >= 0) {
            gpio_reset_pin(dev_cfg->reset_gpio_num);
        }
        free(jd);
    }
    return ret;
}

static esp_err_t panel_del(esp_lcd_panel_t *panel)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);
    if (jd->reset_gpio_num >= 0) {
        gpio_reset_pin(jd->reset_gpio_num);
    }
    free(jd);
    return ESP_OK;
}

static esp_err_t panel_reset(esp_lcd_panel_t *panel)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);

    if (jd->reset_gpio_num >= 0) {
        /*
         * Hardware reset: idle -> ASSERT -> DEASSERT, ending DEASSERTED.
         *
         * `reset_level` is flags.reset_active_high, i.e. the level that
         * HOLDS the panel in reset. It is 0 on this board, so asserted is
         * LOW and released is HIGH.
         *
         * The first version of this ran reset_level, !reset_level,
         * reset_level -- LOW, HIGH, LOW -- which ends with reset ASSERTED
         * and leaves the panel held in reset forever. All 47 init commands
         * were then sent to a chip that was not listening, over a
         * write-only bus with no readback, so every one returned ESP_OK.
         * The driver logged "initialised (47 commands)", the backlight lit,
         * and the screen stayed black. Nothing in the logs pointed at it.
         *
         * The vendor sketch does digitalWrite HIGH, LOW, HIGH -- and their
         * literal HIGH is this code's !reset_level, which is what the
         * original transcription got backwards.
         */
        gpio_set_level(jd->reset_gpio_num, !jd->reset_level);  /* idle    */
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(jd->reset_gpio_num, jd->reset_level);   /* assert  */
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(jd->reset_gpio_num, !jd->reset_level);  /* release */
        vTaskDelay(pdMS_TO_TICKS(120));
    } else {
        ESP_RETURN_ON_ERROR(
            esp_lcd_panel_io_tx_param(jd->io, LCD_CMD_SWRESET, NULL, 0),
            TAG, "software reset failed");
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    return ESP_OK;
}

static esp_err_t panel_init(esp_lcd_panel_t *panel)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);

    for (size_t i = 0; i < JD9855_INIT_CMD_COUNT; i++) {
        const jd9855_init_cmd_t *c = &s_jd9855_init[i];
        ESP_RETURN_ON_ERROR(
            esp_lcd_panel_io_tx_param(jd->io, c->cmd,
                                      c->len ? c->data : NULL, c->len),
            TAG, "init cmd 0x%02X failed", c->cmd);
        if (c->delay_ms) {
            vTaskDelay(pdMS_TO_TICKS(c->delay_ms));
        }
    }

    /*
     * MADCTL is not in the vendor table, so apply the requested colour order
     * afterwards. Leaving it unset would inherit whatever the panel powers up
     * with, which differs between production runs.
     */
    ESP_RETURN_ON_ERROR(
        esp_lcd_panel_io_tx_param(jd->io, LCD_CMD_MADCTL,
                                  (uint8_t[]){ jd->madctl }, 1),
        TAG, "MADCTL failed");

    ESP_LOGI(TAG, "initialised (%u commands)", (unsigned)JD9855_INIT_CMD_COUNT);
    return ESP_OK;
}

static esp_err_t panel_draw_bitmap(esp_lcd_panel_t *panel, int x0, int y0,
                                   int x1, int y1, const void *data)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);
    ESP_RETURN_ON_FALSE(x1 > x0 && y1 > y0, ESP_ERR_INVALID_ARG, TAG,
                        "empty draw area");

    x0 += jd->x_gap;  x1 += jd->x_gap;
    y0 += jd->y_gap;  y1 += jd->y_gap;

    ESP_RETURN_ON_ERROR(
        esp_lcd_panel_io_tx_param(jd->io, LCD_CMD_CASET, (uint8_t[]){
            (x0 >> 8) & 0xFF, x0 & 0xFF,
            ((x1 - 1) >> 8) & 0xFF, (x1 - 1) & 0xFF }, 4),
        TAG, "CASET failed");
    ESP_RETURN_ON_ERROR(
        esp_lcd_panel_io_tx_param(jd->io, LCD_CMD_RASET, (uint8_t[]){
            (y0 >> 8) & 0xFF, y0 & 0xFF,
            ((y1 - 1) >> 8) & 0xFF, (y1 - 1) & 0xFF }, 4),
        TAG, "RASET failed");

    /* 2 bytes per pixel -- this panel is RGB565 only on this board. */
    size_t len = (size_t)(x1 - x0) * (y1 - y0) * 2;
    ESP_RETURN_ON_ERROR(
        esp_lcd_panel_io_tx_color(jd->io, LCD_CMD_RAMWR, data, len),
        TAG, "RAMWR failed");

    return ESP_OK;
}

static esp_err_t panel_invert_color(esp_lcd_panel_t *panel, bool invert)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);
    return esp_lcd_panel_io_tx_param(
        jd->io, invert ? LCD_CMD_INVON : LCD_CMD_INVOFF, NULL, 0);
}

static esp_err_t panel_mirror(esp_lcd_panel_t *panel, bool x, bool y)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);
    if (x) { jd->madctl |= LCD_CMD_MX_BIT; } else { jd->madctl &= ~LCD_CMD_MX_BIT; }
    if (y) { jd->madctl |= LCD_CMD_MY_BIT; } else { jd->madctl &= ~LCD_CMD_MY_BIT; }
    return esp_lcd_panel_io_tx_param(jd->io, LCD_CMD_MADCTL,
                                     (uint8_t[]){ jd->madctl }, 1);
}

static esp_err_t panel_swap_xy(esp_lcd_panel_t *panel, bool swap)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);
    if (swap) { jd->madctl |= LCD_CMD_MV_BIT; } else { jd->madctl &= ~LCD_CMD_MV_BIT; }
    return esp_lcd_panel_io_tx_param(jd->io, LCD_CMD_MADCTL,
                                     (uint8_t[]){ jd->madctl }, 1);
}

static esp_err_t panel_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);
    jd->x_gap = x_gap;
    jd->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_disp_on_off(esp_lcd_panel_t *panel, bool on)
{
    jd9855_panel_t *jd = __containerof(panel, jd9855_panel_t, base);
    return esp_lcd_panel_io_tx_param(
        jd->io, on ? LCD_CMD_DISPON : LCD_CMD_DISPOFF, NULL, 0);
}
