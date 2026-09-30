/* ui_glue — drives the EEZ Studio screens from app_model. See ui_glue.c. */
#pragma once

/* Call once after ui_init(), with the display lock held. */
void ui_glue_init(void);

/* Text for the idle page's status pill. */
const char *ui_glue_usb_status_text(void);
