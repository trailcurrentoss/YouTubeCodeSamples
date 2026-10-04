#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: ScreenRoot
//

void init_style_screen_root_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_screen_root_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_root_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_screen_root(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_root_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_screen_root(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_root_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelDefault
//

void init_style_label_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
};

lv_style_t *get_style_label_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
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
// Style: PanelClear
//

void init_style_panel_clear_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_panel_clear_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_clear_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_clear(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_clear_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_clear(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_clear_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: KeyButton
//

void init_style_key_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 44);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_pad_row(style, 1);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
};

lv_style_t *get_style_key_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_key_button_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_key_button_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_key_button_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_key_button_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_key_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_key_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_key_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_key_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_key_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_key_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: KeyIcon
//

void init_style_key_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_fa24);
};

lv_style_t *get_style_key_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_key_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_key_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_key_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_key_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_key_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: KeyName
//

void init_style_key_name_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
};

lv_style_t *get_style_key_name_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_key_name_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_key_name(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_key_name_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_key_name(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_key_name_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: KeyShortcut
//

void init_style_key_shortcut_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_mono10);
};

lv_style_t *get_style_key_shortcut_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_key_shortcut_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_key_shortcut(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_key_shortcut_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_key_shortcut(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_key_shortcut_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: CenterPanel
//

void init_style_center_panel_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 86);
    lv_style_set_pad_row(style, 6);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
};

lv_style_t *get_style_center_panel_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_center_panel_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_center_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_center_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_center_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_center_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: CenterPanelScroll
//

void init_style_center_panel_scroll_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 86);
    lv_style_set_pad_row(style, 4);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
};

lv_style_t *get_style_center_panel_scroll_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_center_panel_scroll_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_center_panel_scroll(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_center_panel_scroll_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_center_panel_scroll(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_center_panel_scroll_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: CenterPanelZoom
//

void init_style_center_panel_zoom_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 86);
    lv_style_set_pad_row(style, 8);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
};

lv_style_t *get_style_center_panel_zoom_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_center_panel_zoom_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_center_panel_zoom(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_center_panel_zoom_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_center_panel_zoom(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_center_panel_zoom_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: CenterTap
//

void init_style_center_tap_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 86);
    lv_style_set_pad_row(style, 3);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
};

lv_style_t *get_style_center_tap_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_center_tap_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_center_tap_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_center_tap_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_center_tap_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_center_tap(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_center_tap_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_center_tap_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_center_tap(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_center_tap_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_center_tap_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: ModeLabel
//

void init_style_mode_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
};

lv_style_t *get_style_mode_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_mode_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_mode_label(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_mode_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_mode_label(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_mode_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: AngleLabel
//

void init_style_angle_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_num32);
};

lv_style_t *get_style_angle_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_angle_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_angle_label(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_angle_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_angle_label(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_angle_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: AxisRow
//

void init_style_axis_row_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_column(style, 5);
};

lv_style_t *get_style_axis_row_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_axis_row_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_axis_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_axis_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_axis_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_axis_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: AxisChipX
//

void init_style_axis_chip_x_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 3);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_axis_chip_x_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_axis_chip_x_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_axis_chip_x_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][27]));
};

lv_style_t *get_style_axis_chip_x_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_axis_chip_x_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_axis_chip_x(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_axis_chip_x_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_axis_chip_x_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_axis_chip_x(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_axis_chip_x_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_axis_chip_x_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: AxisChipY
//

void init_style_axis_chip_y_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 3);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_axis_chip_y_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_axis_chip_y_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_axis_chip_y_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][27]));
};

lv_style_t *get_style_axis_chip_y_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_axis_chip_y_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_axis_chip_y(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_axis_chip_y_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_axis_chip_y_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_axis_chip_y(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_axis_chip_y_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_axis_chip_y_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: AxisChipZ
//

void init_style_axis_chip_z_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 3);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_axis_chip_z_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_axis_chip_z_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_axis_chip_z_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][27]));
};

lv_style_t *get_style_axis_chip_z_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_axis_chip_z_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_axis_chip_z(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_axis_chip_z_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_axis_chip_z_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_axis_chip_z(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_axis_chip_z_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_axis_chip_z_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: Timecode
//

void init_style_timecode_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_mono20);
};

lv_style_t *get_style_timecode_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_timecode_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_timecode(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_timecode_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_timecode(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_timecode_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ScrubStrip
//

void init_style_scrub_strip_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 4);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_clip_corner(style, true);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_scrub_strip_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scrub_strip_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_scrub_strip(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scrub_strip_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_scrub_strip(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scrub_strip_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Playhead
//

void init_style_playhead_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_playhead_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_playhead_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_playhead(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_playhead_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_playhead(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_playhead_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: CutMark
//

void init_style_cut_mark_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_cut_mark_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_cut_mark_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_cut_mark(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_cut_mark_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_cut_mark(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_cut_mark_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Pill
//

void init_style_pill_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_pad_top(style, 3);
    lv_style_set_pad_bottom(style, 3);
    lv_style_set_pad_left(style, 9);
    lv_style_set_pad_right(style, 9);
    lv_style_set_pad_column(style, 6);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
};

lv_style_t *get_style_pill_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_pill_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_pill(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_pill_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_pill(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_pill_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PillText
//

void init_style_pill_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
};

lv_style_t *get_style_pill_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_pill_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_pill_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_pill_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_pill_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_pill_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StatusDot
//

void init_style_status_dot_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 4);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_status_dot_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_status_dot_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_status_dot_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_status_dot_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_status_dot_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_status_dot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_status_dot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_status_dot_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_status_dot(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_status_dot_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_status_dot_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ClockDial
//

void init_style_clock_dial_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_arc_width(style, 2);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &lv_font_montserrat_16);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_clock_dial_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_clock_dial_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_clock_dial_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_line_width(style, 1);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_clock_dial_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_clock_dial_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_clock_dial_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &lv_font_montserrat_16);
};

lv_style_t *get_style_clock_dial_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_clock_dial_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_clock_dial(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_clock_dial_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_clock_dial_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_clock_dial_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_clock_dial(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_clock_dial_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_clock_dial_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_clock_dial_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: HandHour
//

void init_style_hand_hour_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_line_width(style, 6);
    lv_style_set_line_rounded(style, true);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_hand_hour_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hand_hour_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hand_hour(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hand_hour_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hand_hour(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hand_hour_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HandMinute
//

void init_style_hand_minute_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_rounded(style, true);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_hand_minute_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hand_minute_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hand_minute(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hand_minute_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hand_minute(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hand_minute_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HandSecond
//

void init_style_hand_second_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_rounded(style, true);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_hand_second_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hand_second_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hand_second(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hand_second_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hand_second(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hand_second_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ClockCap
//

void init_style_clock_cap_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 1);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 6);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_border_opa(style, 255);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_clock_cap_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_clock_cap_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_clock_cap(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_clock_cap_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_clock_cap(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_clock_cap_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: InfoColumn
//

void init_style_info_column_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_row(style, 5);
};

lv_style_t *get_style_info_column_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_info_column_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_info_column(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_info_column_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_info_column(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_info_column_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: IdleIcon
//

void init_style_idle_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_fa36);
};

lv_style_t *get_style_idle_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_idle_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_idle_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_idle_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_idle_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_idle_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: IdleTitle
//

void init_style_idle_title_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &lv_font_montserrat_18);
};

lv_style_t *get_style_idle_title_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_idle_title_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_idle_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_idle_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_idle_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_idle_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: IdleHint
//

void init_style_idle_hint_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_idle_hint_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_idle_hint_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_idle_hint(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_idle_hint_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_idle_hint(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_idle_hint_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_screen_root,
        add_style_label_default,
        add_style_panel_clear,
        add_style_key_button,
        add_style_key_icon,
        add_style_key_name,
        add_style_key_shortcut,
        add_style_center_panel,
        add_style_center_panel_scroll,
        add_style_center_panel_zoom,
        add_style_center_tap,
        add_style_mode_label,
        add_style_angle_label,
        add_style_axis_row,
        add_style_axis_chip_x,
        add_style_axis_chip_y,
        add_style_axis_chip_z,
        add_style_timecode,
        add_style_scrub_strip,
        add_style_playhead,
        add_style_cut_mark,
        add_style_pill,
        add_style_pill_text,
        add_style_status_dot,
        add_style_clock_dial,
        add_style_hand_hour,
        add_style_hand_minute,
        add_style_hand_second,
        add_style_clock_cap,
        add_style_info_column,
        add_style_idle_icon,
        add_style_idle_title,
        add_style_idle_hint,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_screen_root,
        remove_style_label_default,
        remove_style_panel_clear,
        remove_style_key_button,
        remove_style_key_icon,
        remove_style_key_name,
        remove_style_key_shortcut,
        remove_style_center_panel,
        remove_style_center_panel_scroll,
        remove_style_center_panel_zoom,
        remove_style_center_tap,
        remove_style_mode_label,
        remove_style_angle_label,
        remove_style_axis_row,
        remove_style_axis_chip_x,
        remove_style_axis_chip_y,
        remove_style_axis_chip_z,
        remove_style_timecode,
        remove_style_scrub_strip,
        remove_style_playhead,
        remove_style_cut_mark,
        remove_style_pill,
        remove_style_pill_text,
        remove_style_status_dot,
        remove_style_clock_dial,
        remove_style_hand_hour,
        remove_style_hand_minute,
        remove_style_hand_second,
        remove_style_clock_cap,
        remove_style_info_column,
        remove_style_idle_icon,
        remove_style_idle_title,
        remove_style_idle_hint,
    };
    remove_style_funcs[styleIndex](obj);
}