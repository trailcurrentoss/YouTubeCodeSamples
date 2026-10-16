/* Shared between the board's own files only -- not part of the public API. */
#pragma once

#include <stdbool.h>

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "lvgl.h"

esp_err_t board_display_init(lv_display_t **out_disp);
esp_err_t board_touch_init(i2c_master_bus_handle_t bus, lv_display_t *disp,
                           lv_indev_t **out_indev);
esp_err_t board_knob_init(void);
esp_err_t board_haptic_init(i2c_master_bus_handle_t bus, bool *present);
esp_err_t board_audio_init(void);
esp_err_t board_battery_init(void);

/** Any touch or detent. Kept so the touch and knob files match their
 *  verified originals; the game does not use an idle timeout. */
void board_note_input(void);
