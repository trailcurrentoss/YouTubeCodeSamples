/* Internal interface between the matouch_board translation units. */
#pragma once

#include <stdbool.h>

#include "esp_err.h"
#include "lvgl.h"

esp_err_t board_display_init(lv_display_t **out_disp);
esp_err_t board_touch_init(lv_display_t *disp, lv_indev_t **out_indev);
esp_err_t board_encoder_init(lv_indev_t **out_indev);

/* Called by the touch and encoder drivers on any user activity. */
void board_note_input(void);

/* Touch-vs-push bookkeeping, shared between board_touch.c and
 * board_encoder.c. See matouch_board_touch_contact_pushed(). */
bool board_button_is_down(void);
void board_touch_note_push(void);
