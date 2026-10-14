#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_tap(lv_event_t * e);
extern void action_menu(lv_event_t * e);
extern void action_start(lv_event_t * e);
extern void action_menu_new_game(lv_event_t * e);
extern void action_menu_settings(lv_event_t * e);
extern void action_menu_back(lv_event_t * e);
extern void action_settings_headphones(lv_event_t * e);
extern void action_settings_back(lv_event_t * e);
extern void action_settings_volume_down(lv_event_t * e);
extern void action_settings_volume_up(lv_event_t * e);
extern void action_headphones(lv_event_t * e);
extern void action_headphones_search(lv_event_t * e);
extern void action_headphones_select(lv_event_t * e);
extern void action_headphones_done(lv_event_t * e);
extern void action_headphones_info(lv_event_t * e);
extern void action_headphones_info_close(lv_event_t * e);
extern void action_headphones_forget(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/