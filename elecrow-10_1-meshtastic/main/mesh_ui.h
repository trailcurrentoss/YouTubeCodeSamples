#pragma once

/*
 * Screen side of the messenger: keeps conversations and paints the EEZ Studio
 * widgets from an LVGL timer. Mesh callbacks only update the store, so no
 * LVGL call ever happens outside the LVGL task.
 */

#include <stdbool.h>
#include <stdint.h>

/* Call once under the LVGL lock, after ui_init() and before the node starts. */
void mesh_ui_init(void);

/* Called by the action handlers (LVGL task). */
void mesh_ui_navigate(int page);
void mesh_ui_select_conversation(int row);
void mesh_ui_send(void);
void mesh_ui_show_keyboard(int field);
void mesh_ui_keyboard_done(int page, bool ready);
void mesh_ui_message_node(int row);
void mesh_ui_save_owner(void);
void mesh_ui_step_setting(int step);    /* row * 2, + 1 for the > button */
void mesh_ui_apply_radio(void);
