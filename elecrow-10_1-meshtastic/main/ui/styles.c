#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: ScreenDefault
//

void init_style_screen_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_screen_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_screen_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_screen_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Card
//

void init_style_card_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
};

lv_style_t *get_style_card_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_card_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_card(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_card_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_card(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_card_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PanelTransparent
//

void init_style_panel_transparent_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_panel_transparent_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_transparent_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_transparent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_transparent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_transparent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_transparent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonPrimary
//

void init_style_button_primary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 10);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][26]));
    lv_style_set_text_font(style, &ui_font_rm14);
};

lv_style_t *get_style_button_primary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_primary_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_opa(style, 220);
};

lv_style_t *get_style_button_primary_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_primary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: LabelDefault
//

void init_style_label_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_label_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TextareaDefault
//

void init_style_textarea_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 8);
    lv_style_set_pad_bottom(style, 8);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
};

lv_style_t *get_style_textarea_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_textarea_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_textarea_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_textarea_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_textarea_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_textarea_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: KeyboardDefault
//

void init_style_keyboard_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    lv_style_set_align(style, LV_ALIGN_TOP_LEFT);
};

lv_style_t *get_style_keyboard_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_keyboard_default_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_radius(style, 6);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_outline_width(style, 0);
};

lv_style_t *get_style_keyboard_default_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_keyboard_default_ITEMS_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_keyboard_default_ITEMS_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_ITEMS_PRESSED(style);
    }
    return style;
};

void init_style_keyboard_default_ITEMS_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 1);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_keyboard_default_ITEMS_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_ITEMS_CHECKED(style);
    }
    return style;
};

void add_style_keyboard_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_keyboard_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_keyboard_default_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_keyboard_default_ITEMS_PRESSED(), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_keyboard_default_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

void remove_style_keyboard_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_keyboard_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_keyboard_default_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_keyboard_default_ITEMS_PRESSED(), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_keyboard_default_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

//
// Style: PanelTopBar48
//

void init_style_panel_top_bar48_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_side(style, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 0);
    lv_style_set_min_width(style, 1024);
    lv_style_set_max_width(style, 1024);
    lv_style_set_min_height(style, 48);
    lv_style_set_max_height(style, 48);
    lv_style_set_align(style, LV_ALIGN_TOP_LEFT);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_panel_top_bar48_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_top_bar48_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_top_bar48(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_top_bar48_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_top_bar48(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_top_bar48_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonTab
//

void init_style_button_tab_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 10);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rm14);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_button_tab_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_tab_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_tab_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_button_tab_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_tab_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_tab_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_button_tab_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_tab_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_tab(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_tab_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_tab_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_tab_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_tab(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_tab_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_tab_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_tab_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: MsgBubble
//

void init_style_msg_bubble_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 10);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_msg_bubble_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_msg_bubble_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_msg_bubble_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_msg_bubble_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_msg_bubble_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_msg_bubble(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_msg_bubble_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_msg_bubble_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_msg_bubble(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_msg_bubble_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_msg_bubble_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_screen_default,
        add_style_card,
        add_style_panel_transparent,
        add_style_button_primary,
        add_style_label_default,
        add_style_textarea_default,
        add_style_keyboard_default,
        add_style_panel_top_bar48,
        add_style_button_tab,
        add_style_msg_bubble,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_screen_default,
        remove_style_card,
        remove_style_panel_transparent,
        remove_style_button_primary,
        remove_style_label_default,
        remove_style_textarea_default,
        remove_style_keyboard_default,
        remove_style_panel_top_bar48,
        remove_style_button_tab,
        remove_style_msg_bubble,
    };
    remove_style_funcs[styleIndex](obj);
}