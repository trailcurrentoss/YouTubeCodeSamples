#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_navigate(lv_event_t * e);
extern void action_select_conversation(lv_event_t * e);
extern void action_send_message(lv_event_t * e);
extern void action_show_keyboard(lv_event_t * e);
extern void action_keyboard_done(lv_event_t * e);
extern void action_message_node(lv_event_t * e);
extern void action_save_owner(lv_event_t * e);
extern void action_step_setting(lv_event_t * e);
extern void action_apply_radio(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/