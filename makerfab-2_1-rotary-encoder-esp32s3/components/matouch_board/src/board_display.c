/*
 * Display bring-up: ST7701S on a 16-bit RGB565 parallel bus, initialised over
 * a 3-wire SPI sideband.
 *
 * board_display_init() returns an LVGL display owned by esp_lvgl_port. The
 * backlight is NOT switched on here -- matouch_board_init() leaves it dark
 * until the first screen has been drawn.
 */

#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_io_additions.h"
#include "esp_lcd_st7701.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "board_internal.h"
#include "board_pins.h"
#include "matouch_board.h"
#include "st7701_matouch_init.h"

static const char *TAG = "board.disp";

/* A Kconfig bool that is 'n' is UNDEFINED, not 0, so it cannot appear in a
 * runtime expression. Normalise it once. */
#ifdef CONFIG_MATOUCH_LCD_SWAP_RB
#  define MATOUCH_SWAP_RB 1
#else
#  define MATOUCH_SWAP_RB 0
#endif

static esp_lcd_panel_handle_t    s_panel;
static esp_lcd_panel_io_handle_t s_io;

esp_err_t board_display_init(lv_display_t **out_disp)
{
    const spi_line_config_t line_cfg = {
        .cs_io_type   = IO_TYPE_GPIO,
        .cs_gpio_num  = BOARD_LCD_SPI_CS,
        .scl_io_type  = IO_TYPE_GPIO,
        .scl_gpio_num = BOARD_LCD_SPI_SCK,
        .sda_io_type  = IO_TYPE_GPIO,
        .sda_gpio_num = BOARD_LCD_SPI_SDA,
        .io_expander  = NULL,
    };
    esp_lcd_panel_io_3wire_spi_config_t io_cfg =
        ST7701_PANEL_IO_3WIRE_SPI_CONFIG(line_cfg, 0);
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_3wire_spi(&io_cfg, &s_io),
                        TAG, "3-wire SPI io failed");

    /* esp_lcd's data_gpio_nums[] is blue-LSB first for RGB565: [0..4] blue,
     * [5..10] green, [11..15] red. See the colour note in board_pins.h. */
    const int blue[5] = { BOARD_LCD_B0, BOARD_LCD_B1, BOARD_LCD_B2,
                          BOARD_LCD_B3, BOARD_LCD_B4 };
    const int red[5]  = { BOARD_LCD_R0, BOARD_LCD_R1, BOARD_LCD_R2,
                          BOARD_LCD_R3, BOARD_LCD_R4 };
#if MATOUCH_SWAP_RB
    const int *lsb = red,  *msb = blue;
#else
    const int *lsb = blue, *msb = red;
#endif

    esp_lcd_rgb_panel_config_t rgb_cfg = {
        .clk_src           = LCD_CLK_SRC_DEFAULT,
        .psram_trans_align = 64,
        .data_width        = 16,
        .bits_per_pixel    = 16,
        .de_gpio_num       = BOARD_LCD_DE,
        .pclk_gpio_num     = BOARD_LCD_PCLK,
        .vsync_gpio_num    = BOARD_LCD_VSYNC,
        .hsync_gpio_num    = BOARD_LCD_HSYNC,
        .disp_gpio_num     = -1,
        .data_gpio_nums = {
            lsb[0], lsb[1], lsb[2], lsb[3], lsb[4],
            BOARD_LCD_G0, BOARD_LCD_G1, BOARD_LCD_G2,
            BOARD_LCD_G3, BOARD_LCD_G4, BOARD_LCD_G5,
            msb[0], msb[1], msb[2], msb[3], msb[4],
        },
        .timings = {
            .pclk_hz           = BOARD_LCD_PCLK_HZ,
            .h_res             = MATOUCH_LCD_H_RES,
            .v_res             = MATOUCH_LCD_V_RES,
            .hsync_front_porch = BOARD_LCD_HSYNC_FRONT,
            .hsync_pulse_width = BOARD_LCD_HSYNC_PULSE,
            .hsync_back_porch  = BOARD_LCD_HSYNC_BACK,
            .vsync_front_porch = BOARD_LCD_VSYNC_FRONT,
            .vsync_pulse_width = BOARD_LCD_VSYNC_PULSE,
            .vsync_back_porch  = BOARD_LCD_VSYNC_BACK,
            .flags = {
                .hsync_idle_low = !BOARD_LCD_HSYNC_POL,
                .vsync_idle_low = !BOARD_LCD_VSYNC_POL,
                /* Latch on the RISING edge. 1 here gives a lit panel that is
                 * pure black with no driver error. */
                .pclk_active_neg = 0,
            },
        },
        /* Two PSRAM framebuffers for tear-free double buffering, fed to the
         * LCD peripheral through an internal-RAM bounce buffer so PSRAM
         * latency spikes do not show up as tearing. */
        .num_fbs               = 2,
        .bounce_buffer_size_px = MATOUCH_LCD_H_RES * 10,
        .flags = { .fb_in_psram = 1 },
    };

    /* The vendor's panel-specific init table. esp_lcd_st7701's default table
     * leaves this glass black -- see st7701_matouch_init.h.
     *
     * auto_del_panel_io = 0 keeps the SPI IO alive after init. The sideband
     * pins are not shared with the RGB bus on this board, so there is nothing
     * to release, and disp_on_off below needs the IO. */
    st7701_vendor_config_t vendor_cfg = {
        .init_cmds      = matouch21_round_init,
        .init_cmds_size = MATOUCH21_ROUND_INIT_COUNT,
        .rgb_config     = &rgb_cfg,
        .flags = { .auto_del_panel_io = 0 },
    };
    esp_lcd_panel_dev_config_t dev_cfg = {
        .reset_gpio_num = BOARD_LCD_RST,
        .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config  = &vendor_cfg,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_st7701(s_io, &dev_cfg, &s_panel),
                        TAG, "st7701 panel failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(s_panel), TAG, "reset failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(s_panel),  TAG, "init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(s_panel, true), TAG,
                        "display on failed");

    /*
     * avoid_tearing hands LVGL the panel's own two framebuffers and swaps
     * between them. full_refresh is REQUIRED with it: in PARTIAL mode only
     * the changed region is drawn into whichever buffer is next, the two
     * buffers diverge, and every swap shows a mix of old and new frames.
     */
    lvgl_port_display_rgb_cfg_t rgb_lv = {
        .flags = { .bb_mode = true, .avoid_tearing = true },
    };
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle     = s_io,
        .panel_handle  = s_panel,
        .buffer_size   = MATOUCH_LCD_H_RES * MATOUCH_LCD_V_RES,
        .double_buffer = true,
        .hres          = MATOUCH_LCD_H_RES,
        .vres          = MATOUCH_LCD_V_RES,
        .color_format  = LV_COLOR_FORMAT_RGB565,
        .flags = {
            .buff_dma     = false,
            .buff_spiram  = false,
            .full_refresh = true,
        },
    };
    *out_disp = lvgl_port_add_disp_rgb(&disp_cfg, &rgb_lv);
    ESP_RETURN_ON_FALSE(*out_disp, ESP_FAIL, TAG, "lvgl_port_add_disp_rgb failed");

    ESP_LOGI(TAG, "ST7701S %dx%d RGB up, pclk %d MHz, R/B %sswapped",
             MATOUCH_LCD_H_RES, MATOUCH_LCD_V_RES, BOARD_LCD_PCLK_HZ / 1000000,
             MATOUCH_SWAP_RB ? "" : "not ");
    return ESP_OK;
}
