/*
 * JD9855 — 360x360 round IPS controller on the Elecrow CrowPanel 1.46".
 *
 * There is no esp_lcd driver for this part anywhere: no esp_lcd_jd9855 in
 * esp-iot-solution, and no public repository implements it. This is the only
 * hand-written panel driver in the project.
 *
 * The register sequence is transcribed from LovyanGFX's Panel_ST77961
 * (src/lgfx/v1/panel/Panel_ST77961.hpp), which is what Elecrow's own Arduino
 * firmware runs on this board -- they vendor LovyanGFX and instantiate
 * lgfx::Panel_ST77961. The JD9855 and ST77961 are register-compatible for
 * initialisation purposes.
 *
 * Do not "tidy" the magic numbers. They are an undocumented gamma and power
 * sequence; the vendor panel datasheet (P146B001-IPS-CTP-V2) contains no
 * initialisation section at all, so there is nothing to check them against
 * except a working panel.
 */
#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create a JD9855 panel handle on an already-created SPI panel IO.
 *
 * Mirrors the signature of the espressif esp_lcd_new_panel_* drivers so this
 * can be swapped for a first-party component if one ever appears.
 */
esp_err_t esp_lcd_new_panel_jd9855(esp_lcd_panel_io_handle_t io,
                                   const esp_lcd_panel_dev_config_t *dev_cfg,
                                   esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif
