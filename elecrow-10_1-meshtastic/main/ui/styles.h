#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: ScreenDefault
lv_style_t *get_style_screen_default_MAIN_DEFAULT();
void add_style_screen_default(lv_obj_t *obj);
void remove_style_screen_default(lv_obj_t *obj);

// Style: Card
lv_style_t *get_style_card_MAIN_DEFAULT();
void add_style_card(lv_obj_t *obj);
void remove_style_card(lv_obj_t *obj);

// Style: PanelTransparent
lv_style_t *get_style_panel_transparent_MAIN_DEFAULT();
void add_style_panel_transparent(lv_obj_t *obj);
void remove_style_panel_transparent(lv_obj_t *obj);

// Style: ButtonPrimary
lv_style_t *get_style_button_primary_MAIN_DEFAULT();
lv_style_t *get_style_button_primary_MAIN_PRESSED();
void add_style_button_primary(lv_obj_t *obj);
void remove_style_button_primary(lv_obj_t *obj);

// Style: LabelDefault
lv_style_t *get_style_label_default_MAIN_DEFAULT();
void add_style_label_default(lv_obj_t *obj);
void remove_style_label_default(lv_obj_t *obj);

// Style: TextareaDefault
lv_style_t *get_style_textarea_default_MAIN_DEFAULT();
void add_style_textarea_default(lv_obj_t *obj);
void remove_style_textarea_default(lv_obj_t *obj);

// Style: KeyboardDefault
lv_style_t *get_style_keyboard_default_MAIN_DEFAULT();
lv_style_t *get_style_keyboard_default_ITEMS_DEFAULT();
lv_style_t *get_style_keyboard_default_ITEMS_PRESSED();
lv_style_t *get_style_keyboard_default_ITEMS_CHECKED();
void add_style_keyboard_default(lv_obj_t *obj);
void remove_style_keyboard_default(lv_obj_t *obj);

// Style: PanelTopBar48
lv_style_t *get_style_panel_top_bar48_MAIN_DEFAULT();
void add_style_panel_top_bar48(lv_obj_t *obj);
void remove_style_panel_top_bar48(lv_obj_t *obj);

// Style: ButtonTab
lv_style_t *get_style_button_tab_MAIN_DEFAULT();
lv_style_t *get_style_button_tab_MAIN_PRESSED();
lv_style_t *get_style_button_tab_MAIN_CHECKED();
void add_style_button_tab(lv_obj_t *obj);
void remove_style_button_tab(lv_obj_t *obj);

// Style: MsgBubble
lv_style_t *get_style_msg_bubble_MAIN_DEFAULT();
lv_style_t *get_style_msg_bubble_MAIN_CHECKED();
void add_style_msg_bubble(lv_obj_t *obj);
void remove_style_msg_bubble(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/