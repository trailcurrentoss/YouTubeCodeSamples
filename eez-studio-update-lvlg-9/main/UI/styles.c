#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: ScreenRoot
//

void init_style_screen_root_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_14);
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
// Style: HeaderBar
//

void init_style_header_bar_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_side(style, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 20);
    lv_style_set_pad_right(style, 20);
};

lv_style_t *get_style_header_bar_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_header_bar_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_header_bar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_header_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_header_bar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_header_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FooterBar
//

void init_style_footer_bar_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_side(style, LV_BORDER_SIDE_TOP);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 20);
    lv_style_set_pad_right(style, 20);
};

lv_style_t *get_style_footer_bar_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_footer_bar_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_footer_bar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_footer_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_footer_bar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_footer_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: BodyArea
//

void init_style_body_area_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_body_area_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_body_area_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_body_area(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_body_area_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_body_area(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_body_area_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Card
//

void init_style_card_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 14);
    lv_style_set_shadow_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_shadow_opa(style, 60);
    lv_style_set_shadow_width(style, 10);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_pad_top(style, 16);
    lv_style_set_pad_bottom(style, 16);
    lv_style_set_pad_left(style, 16);
    lv_style_set_pad_right(style, 16);
};

lv_style_t *get_style_card_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
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
// Style: PanelPlain
//

void init_style_panel_plain_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_panel_plain_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_plain_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_plain(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_plain_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_plain(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_plain_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PanelInset
//

void init_style_panel_inset_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 10);
    lv_style_set_pad_top(style, 10);
    lv_style_set_pad_bottom(style, 10);
    lv_style_set_pad_left(style, 12);
    lv_style_set_pad_right(style, 12);
};

lv_style_t *get_style_panel_inset_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_inset_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_inset(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_inset_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_inset(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_inset_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelDefault
//

void init_style_label_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
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
// Style: CardTitle
//

void init_style_card_title_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_16);
    lv_style_set_text_letter_space(style, 2);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_card_title_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_card_title_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_card_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_card_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_card_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_card_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HeaderTitle
//

void init_style_header_title_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_28);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_header_title_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_header_title_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_header_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_header_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_header_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_header_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HeaderKicker
//

void init_style_header_kicker_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_text_letter_space(style, 2);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_header_kicker_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_header_kicker_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_header_kicker(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_header_kicker_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_header_kicker(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_header_kicker_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: IconAccent
//

void init_style_icon_accent_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_text_font(style, &ui_font_dashicons22);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_icon_accent_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_icon_accent_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_icon_accent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_icon_accent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_icon_accent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_icon_accent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: IconSolar
//

void init_style_icon_solar_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_text_font(style, &ui_font_dashicons22);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_icon_solar_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_icon_solar_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_icon_solar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_icon_solar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_icon_solar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_icon_solar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: IconFresh
//

void init_style_icon_fresh_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
    lv_style_set_text_font(style, &ui_font_dashicons16);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_icon_fresh_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_icon_fresh_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_icon_fresh(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_icon_fresh_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_icon_fresh(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_icon_fresh_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: IconGrey
//

void init_style_icon_grey_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_text_font(style, &ui_font_dashicons16);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_icon_grey_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_icon_grey_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_icon_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_icon_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_icon_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_icon_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StatusLine
//

void init_style_status_line_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_status_line_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_status_line_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_status_line(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_status_line_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_status_line(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_status_line_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: FooterText
//

void init_style_footer_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_footer_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_footer_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_footer_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_footer_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_footer_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_footer_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TankRowLabel
//

void init_style_tank_row_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    lv_style_set_text_letter_space(style, 1);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_tank_row_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_tank_row_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_tank_row_label(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_tank_row_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_tank_row_label(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_tank_row_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SpanHero
//

void init_style_span_hero_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_mono44);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_span_hero_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_span_hero_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_span_hero(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_span_hero_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_span_hero(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_span_hero_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SpanSecondary
//

void init_style_span_secondary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_mono26);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_span_secondary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_span_secondary_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_span_secondary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_span_secondary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_span_secondary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_span_secondary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SpanTank
//

void init_style_span_tank_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_mono26);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_span_tank_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_span_tank_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_span_tank(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_span_tank_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_span_tank(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_span_tank_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ScaleRing
//

void init_style_scale_ring_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_arc_width(style, 6);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_scale_ring_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_ring_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_scale_ring_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_scale_ring_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_ring_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_scale_ring_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
};

lv_style_t *get_style_scale_ring_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_ring_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_scale_ring(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scale_ring_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scale_ring_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scale_ring_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_scale_ring(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scale_ring_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scale_ring_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scale_ring_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: ScaleStrip
//

void init_style_scale_strip_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_scale_strip_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_strip_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_scale_strip_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
};

lv_style_t *get_style_scale_strip_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_strip_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_scale_strip_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
};

lv_style_t *get_style_scale_strip_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_strip_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_scale_strip(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scale_strip_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scale_strip_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scale_strip_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_scale_strip(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scale_strip_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scale_strip_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scale_strip_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: ScaleSectionDanger
//

void init_style_scale_section_danger_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_arc_width(style, 6);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
};

lv_style_t *get_style_scale_section_danger_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_section_danger_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_scale_section_danger(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scale_section_danger_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_scale_section_danger(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scale_section_danger_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ScaleSectionWarn
//

void init_style_scale_section_warn_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_arc_width(style, 6);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_scale_section_warn_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_section_warn_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_scale_section_warn(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scale_section_warn_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_scale_section_warn(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scale_section_warn_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ScaleSectionGood
//

void init_style_scale_section_good_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_line_width(style, 4);
    lv_style_set_line_opa(style, 255);
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_arc_width(style, 6);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_scale_section_good_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scale_section_good_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_scale_section_good(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scale_section_good_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_scale_section_good(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scale_section_good_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ArcBattery
//

void init_style_arc_battery_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_arc_battery_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_battery_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_arc_battery_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_arc_battery_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_battery_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_arc_battery_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
};

lv_style_t *get_style_arc_battery_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_battery_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_arc_battery(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arc_battery_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_battery_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_battery_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_arc_battery(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arc_battery_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_battery_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_battery_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: ArcSolar
//

void init_style_arc_solar_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_arc_solar_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_solar_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_arc_solar_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_arc_width(style, 14);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_arc_solar_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_solar_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_arc_solar_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
};

lv_style_t *get_style_arc_solar_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_solar_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_arc_solar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arc_solar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_solar_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_solar_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_arc_solar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arc_solar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_solar_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_solar_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: BarFresh
//

void init_style_bar_fresh_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 9);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_bar_fresh_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_fresh_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_bar_fresh_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 9);
};

lv_style_t *get_style_bar_fresh_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_fresh_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_bar_fresh(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_bar_fresh_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_bar_fresh_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_bar_fresh(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bar_fresh_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_bar_fresh_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: BarGrey
//

void init_style_bar_grey_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 9);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_bar_grey_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_grey_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_bar_grey_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_radius(style, 9);
};

lv_style_t *get_style_bar_grey_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_grey_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_bar_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_bar_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_bar_grey_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_bar_grey(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bar_grey_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_bar_grey_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: ButtonGhost
//

void init_style_button_ghost_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &ui_font_dashicons22);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_button_ghost_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_ghost_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_ghost_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
};

lv_style_t *get_style_button_ghost_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_ghost_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_ghost(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_ghost_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_ghost_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_ghost(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_ghost_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_ghost_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: ScanRow
//

void init_style_scan_row_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 6);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
};

lv_style_t *get_style_scan_row_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scan_row_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_scan_row_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_bg_opa(style, 60);
};

lv_style_t *get_style_scan_row_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scan_row_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_scan_row_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
};

lv_style_t *get_style_scan_row_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scan_row_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_scan_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scan_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scan_row_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_scan_row_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_scan_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scan_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scan_row_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_scan_row_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ScanRowSsid
//

void init_style_scan_row_ssid_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_16);
};

lv_style_t *get_style_scan_row_ssid_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scan_row_ssid_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_scan_row_ssid_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
};

lv_style_t *get_style_scan_row_ssid_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scan_row_ssid_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_scan_row_ssid(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scan_row_ssid_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scan_row_ssid_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_scan_row_ssid(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scan_row_ssid_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scan_row_ssid_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ScanRowMeta
//

void init_style_scan_row_meta_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_dashicons16);
};

lv_style_t *get_style_scan_row_meta_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scan_row_meta_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_scan_row_meta_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
};

lv_style_t *get_style_scan_row_meta_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scan_row_meta_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_scan_row_meta(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scan_row_meta_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_scan_row_meta_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_scan_row_meta(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scan_row_meta_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_scan_row_meta_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: TextareaDefault
//

void init_style_textarea_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 6);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_20);
    lv_style_set_pad_top(style, 8);
    lv_style_set_pad_bottom(style, 8);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
};

lv_style_t *get_style_textarea_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_textarea_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_textarea_default_MAIN_FOCUSED(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_border_width(style, 2);
};

lv_style_t *get_style_textarea_default_MAIN_FOCUSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_textarea_default_MAIN_FOCUSED(style);
    }
    return style;
};

void add_style_textarea_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_textarea_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_textarea_default_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
};

void remove_style_textarea_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_textarea_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_textarea_default_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
};

//
// Style: KeyboardDefault
//

void init_style_keyboard_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 6);
    lv_style_set_text_font(style, &lv_font_montserrat_20);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_keyboard_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_keyboard_default_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_radius(style, 4);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
};

lv_style_t *get_style_keyboard_default_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_ITEMS_DEFAULT(style);
    }
    return style;
};

void add_style_keyboard_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_keyboard_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_keyboard_default_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

void remove_style_keyboard_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_keyboard_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_keyboard_default_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

//
// Style: DropdownDefault
//

void init_style_dropdown_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 6);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_text_font(style, &lv_font_montserrat_16);
    lv_style_set_pad_top(style, 6);
    lv_style_set_pad_bottom(style, 6);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
};

lv_style_t *get_style_dropdown_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dropdown_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_dropdown_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_dropdown_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_dropdown_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_dropdown_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonPrimary
//

void init_style_button_primary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 6);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_text_font(style, &lv_font_montserrat_16);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_button_primary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_primary_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_opa(style, 200);
};

lv_style_t *get_style_button_primary_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_primary_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_button_primary_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_primary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_primary_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_screen_root,
        add_style_header_bar,
        add_style_footer_bar,
        add_style_body_area,
        add_style_card,
        add_style_panel_plain,
        add_style_panel_inset,
        add_style_label_default,
        add_style_card_title,
        add_style_header_title,
        add_style_header_kicker,
        add_style_icon_accent,
        add_style_icon_solar,
        add_style_icon_fresh,
        add_style_icon_grey,
        add_style_status_line,
        add_style_footer_text,
        add_style_tank_row_label,
        add_style_span_hero,
        add_style_span_secondary,
        add_style_span_tank,
        add_style_scale_ring,
        add_style_scale_strip,
        add_style_scale_section_danger,
        add_style_scale_section_warn,
        add_style_scale_section_good,
        add_style_arc_battery,
        add_style_arc_solar,
        add_style_bar_fresh,
        add_style_bar_grey,
        add_style_button_ghost,
        add_style_scan_row,
        add_style_scan_row_ssid,
        add_style_scan_row_meta,
        add_style_textarea_default,
        add_style_keyboard_default,
        add_style_dropdown_default,
        add_style_button_primary,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_screen_root,
        remove_style_header_bar,
        remove_style_footer_bar,
        remove_style_body_area,
        remove_style_card,
        remove_style_panel_plain,
        remove_style_panel_inset,
        remove_style_label_default,
        remove_style_card_title,
        remove_style_header_title,
        remove_style_header_kicker,
        remove_style_icon_accent,
        remove_style_icon_solar,
        remove_style_icon_fresh,
        remove_style_icon_grey,
        remove_style_status_line,
        remove_style_footer_text,
        remove_style_tank_row_label,
        remove_style_span_hero,
        remove_style_span_secondary,
        remove_style_span_tank,
        remove_style_scale_ring,
        remove_style_scale_strip,
        remove_style_scale_section_danger,
        remove_style_scale_section_warn,
        remove_style_scale_section_good,
        remove_style_arc_battery,
        remove_style_arc_solar,
        remove_style_bar_fresh,
        remove_style_bar_grey,
        remove_style_button_ghost,
        remove_style_scan_row,
        remove_style_scan_row_ssid,
        remove_style_scan_row_meta,
        remove_style_textarea_default,
        remove_style_keyboard_default,
        remove_style_dropdown_default,
        remove_style_button_primary,
    };
    remove_style_funcs[styleIndex](obj);
}