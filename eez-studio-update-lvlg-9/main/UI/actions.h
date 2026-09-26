#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_toggle_theme(lv_event_t * e);
extern void action_show_wifi(lv_event_t * e);
extern void action_show_dashboard(lv_event_t * e);
extern void action_wifi_scan(lv_event_t * e);
extern void action_wifi_select(lv_event_t * e);
extern void action_wifi_connect(lv_event_t * e);
extern void action_wifi_cancel(lv_event_t * e);
extern void action_show_broker(lv_event_t * e);
extern void action_broker_field(lv_event_t * e);
extern void action_broker_save(lv_event_t * e);
extern void action_broker_cancel(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/