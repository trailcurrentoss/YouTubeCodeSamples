/*
 * wifi_ui — the LVGL-facing half of the Wi-Fi screen, implemented in
 * actions.c. Split out only so vars.c can drive the repaint from the same
 * pump that advances dash_data, without either file including the other.
 */

#ifndef WIFI_UI_H
#define WIFI_UI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Advances wifi_port and repaints the scan rows if anything changed.
 * Cheap enough to call at the pump rate; does nothing when idle. */
void wifi_ui_tick(uint32_t now_ms);

/* Text for the two expression-bound labels on PageWifi. Never NULL. */
const char *wifi_ui_status_text(void);
const char *wifi_ui_selected_text(void);

/* Status for the Headwaters screen header. Never NULL. */
const char *broker_ui_status_text(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_UI_H */
