#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: ScreenRoot
//

void init_style_screen_root_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_text_font(style, &ui_font_rr13);
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
// Style: LabelBody
//

void init_style_label_body_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_text_font(style, &ui_font_rr13);
};

lv_style_t *get_style_label_body_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_body_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_body(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_body_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_body(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_body_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: RingThin
//

void init_style_ring_thin_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_ring_thin_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_ring_thin_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_ring_thin(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_ring_thin_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_ring_thin(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_ring_thin_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: RingWide
//

void init_style_ring_wide_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_border_width(style, 12);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_ring_wide_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_ring_wide_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_ring_wide(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_ring_wide_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_ring_wide(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_ring_wide_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: RingHair
//

void init_style_ring_hair_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_ring_hair_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_ring_hair_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_ring_hair(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_ring_hair_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_ring_hair(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_ring_hair_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TitleBolts
//

void init_style_title_bolts_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_width(style, 0);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_title_bolts_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_bolts_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_title_bolts_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_length(style, 0);
};

lv_style_t *get_style_title_bolts_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_bolts_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_title_bolts_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_line_width(style, 20);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 30);
};

lv_style_t *get_style_title_bolts_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_bolts_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_title_bolts(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_title_bolts_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_title_bolts_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_title_bolts_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_title_bolts(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_title_bolts_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_title_bolts_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_title_bolts_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: TitleBoltSlots
//

void init_style_title_bolt_slots_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_width(style, 0);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_title_bolt_slots_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_bolt_slots_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_title_bolt_slots_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_length(style, 0);
};

lv_style_t *get_style_title_bolt_slots_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_bolt_slots_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_title_bolt_slots_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_line_width(style, 6);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 18);
};

lv_style_t *get_style_title_bolt_slots_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_bolt_slots_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_title_bolt_slots(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_title_bolt_slots_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_title_bolt_slots_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_title_bolt_slots_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_title_bolt_slots(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_title_bolt_slots_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_title_bolt_slots_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_title_bolt_slots_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: TitleTicks
//

void init_style_title_ticks_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_width(style, 0);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_title_ticks_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_ticks_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_title_ticks_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 5);
};

lv_style_t *get_style_title_ticks_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_ticks_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_title_ticks_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 10);
};

lv_style_t *get_style_title_ticks_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_ticks_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_title_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_title_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_title_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_title_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_title_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_title_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_title_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_title_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: TitleLock
//

void init_style_title_lock_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_text_font(style, &ui_font_fa40);
};

lv_style_t *get_style_title_lock_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_lock_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_title_lock(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_title_lock_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_title_lock(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_title_lock_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TitleText
//

void init_style_title_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_text_font(style, &ui_font_rm28);
};

lv_style_t *get_style_title_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_title_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_title_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_title_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_title_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TitleSub
//

void init_style_title_sub_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_rr13);
};

lv_style_t *get_style_title_sub_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_title_sub_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_title_sub(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_title_sub_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_title_sub(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_title_sub_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: AccentHint
//

void init_style_accent_hint_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
    lv_style_set_text_font(style, &ui_font_rm14);
};

lv_style_t *get_style_accent_hint_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_accent_hint_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_accent_hint(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_accent_hint_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_accent_hint(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_accent_hint_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: AccentHintIcon
//

void init_style_accent_hint_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
    lv_style_set_text_font(style, &ui_font_fa14);
};

lv_style_t *get_style_accent_hint_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_accent_hint_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_accent_hint_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_accent_hint_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_accent_hint_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_accent_hint_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: InstructionsBody
//

void init_style_instructions_body_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_rm15);
};

lv_style_t *get_style_instructions_body_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_instructions_body_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_instructions_body(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_instructions_body_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_instructions_body(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_instructions_body_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StartButton
//

void init_style_start_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 24);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_start_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_button_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_start_button_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_start_button_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_button_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_start_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_start_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_start_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_start_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_start_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_start_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: StartButtonText
//

void init_style_start_button_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_text_font(style, &ui_font_rm28);
};

lv_style_t *get_style_start_button_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_button_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_button_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_start_button_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_button_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_start_button_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonText
//

void init_style_button_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_text_font(style, &ui_font_rm15);
};

lv_style_t *get_style_button_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonOutline
//

void init_style_button_outline_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 22);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_button_outline_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_outline_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_outline_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_button_outline_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_outline_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_outline_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
};

lv_style_t *get_style_button_outline_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_outline_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_outline(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_outline_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_outline_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_outline_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_outline(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_outline_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_outline_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_outline_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ButtonOutlineText
//

void init_style_button_outline_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_rm15);
};

lv_style_t *get_style_button_outline_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_outline_text_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_outline_text_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
};

lv_style_t *get_style_button_outline_text_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_outline_text_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_outline_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_outline_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_outline_text_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_outline_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_outline_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_outline_text_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: OutlineIcon
//

void init_style_outline_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_fa14);
};

lv_style_t *get_style_outline_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_outline_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_outline_icon_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
};

lv_style_t *get_style_outline_icon_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_outline_icon_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_outline_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_outline_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_outline_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_outline_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_outline_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_outline_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: DeviceList
//

void init_style_device_list_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_device_list_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_list_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_device_list(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_device_list_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_device_list(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_device_list_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: DeviceRow
//

void init_style_device_row_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 10);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_device_row_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_row_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_device_row_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_device_row_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_row_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_device_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_device_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_device_row_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_device_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_device_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_device_row_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: DeviceRowText
//

void init_style_device_row_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_text_font(style, &ui_font_rm15);
};

lv_style_t *get_style_device_row_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_row_text_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_device_row_text_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
};

lv_style_t *get_style_device_row_text_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_device_row_text_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_device_row_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_device_row_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_device_row_text_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_device_row_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_device_row_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_device_row_text_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: InfoButton
//

void init_style_info_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 10);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_info_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_info_button_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_info_button_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_info_button_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_info_button_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_info_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_info_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_info_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_info_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_info_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_info_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: InfoIcon
//

void init_style_info_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_fa14);
};

lv_style_t *get_style_info_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_info_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_info_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_info_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_info_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_info_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Scrim
//

void init_style_scrim_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_bg_opa(style, 235);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_scrim_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_scrim_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_scrim(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_scrim_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_scrim(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_scrim_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonDanger
//

void init_style_button_danger_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 22);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_button_danger_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_danger_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_danger_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_button_danger_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_danger_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_danger(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_danger_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_danger_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_danger(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_danger_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_danger_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: ButtonDangerText
//

void init_style_button_danger_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
    lv_style_set_text_font(style, &ui_font_rm15);
};

lv_style_t *get_style_button_danger_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_danger_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_danger_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_danger_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_danger_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_danger_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SettingRow
//

void init_style_setting_row_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 10);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_setting_row_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_setting_row_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_setting_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_setting_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_setting_row(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_setting_row_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StepButton
//

void init_style_step_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1000);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_step_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_step_button_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_step_button_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_step_button_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_step_button_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_step_button_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_step_button_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_step_button_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_step_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_step_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_step_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_step_button_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_step_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_step_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_step_button_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_step_button_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: StepIcon
//

void init_style_step_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_text_font(style, &ui_font_fa14);
};

lv_style_t *get_style_step_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_step_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_step_icon_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
};

lv_style_t *get_style_step_icon_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_step_icon_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_step_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_step_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_step_icon_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_step_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_step_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_step_icon_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: SettingValue
//

void init_style_setting_value_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_rm14);
};

lv_style_t *get_style_setting_value_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_setting_value_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_setting_value_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
};

lv_style_t *get_style_setting_value_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_setting_value_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_setting_value_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
};

lv_style_t *get_style_setting_value_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_setting_value_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_setting_value(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_setting_value_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_setting_value_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_setting_value_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_setting_value(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_setting_value_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_setting_value_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_setting_value_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: SegArc
//

void init_style_seg_arc_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_opa(style, 0);
    lv_style_set_arc_width(style, 5);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_seg_arc_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seg_arc_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_seg_arc_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_arc_width(style, 5);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_seg_arc_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seg_arc_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_seg_arc_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_arc_width(style, 5);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_seg_arc_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seg_arc_INDICATOR_CHECKED(style);
    }
    return style;
};

void init_style_seg_arc_INDICATOR_DISABLED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
    lv_style_set_arc_width(style, 5);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_seg_arc_INDICATOR_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seg_arc_INDICATOR_DISABLED(style);
    }
    return style;
};

void init_style_seg_arc_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_seg_arc_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seg_arc_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_seg_arc(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_seg_arc_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_seg_arc_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_seg_arc_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_seg_arc_INDICATOR_DISABLED(), LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_add_style(obj, get_style_seg_arc_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_seg_arc(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_seg_arc_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_seg_arc_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_seg_arc_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_seg_arc_INDICATOR_DISABLED(), LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_remove_style(obj, get_style_seg_arc_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: SegArcCurrent
//

void init_style_seg_arc_current_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_opa(style, 0);
    lv_style_set_arc_width(style, 5);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_seg_arc_current_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seg_arc_current_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_seg_arc_current_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_arc_width(style, 5);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_arc_rounded(style, true);
};

lv_style_t *get_style_seg_arc_current_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seg_arc_current_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_seg_arc_current_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_seg_arc_current_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_seg_arc_current_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_seg_arc_current(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_seg_arc_current_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_seg_arc_current_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_seg_arc_current_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_seg_arc_current(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_seg_arc_current_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_seg_arc_current_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_seg_arc_current_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: DialTicks
//

void init_style_dial_ticks_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_width(style, 0);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_dial_ticks_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_ticks_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_dial_ticks_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 10);
};

lv_style_t *get_style_dial_ticks_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_ticks_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_dial_ticks_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_line_width(style, 3);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 21);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_rm15);
};

lv_style_t *get_style_dial_ticks_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_ticks_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_dial_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_dial_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_dial_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_dial_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_dial_ticks(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_dial_ticks_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_dial_ticks_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_dial_ticks_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: DialDisk
//

void init_style_dial_disk_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_grad_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_bg_grad_dir(style, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 1000);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_dial_disk_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_disk_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_dial_disk(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_dial_disk_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_dial_disk(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_dial_disk_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: DialGrip
//

void init_style_dial_grip_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_width(style, 0);
    lv_style_set_arc_opa(style, 0);
    lv_style_set_line_width(style, 0);
    lv_style_set_line_opa(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_dial_grip_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_grip_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_dial_grip_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 7);
};

lv_style_t *get_style_dial_grip_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_grip_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_dial_grip_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_line_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_line_width(style, 2);
    lv_style_set_line_opa(style, 255);
    lv_style_set_line_rounded(style, true);
    lv_style_set_length(style, 7);
};

lv_style_t *get_style_dial_grip_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_grip_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_dial_grip(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_dial_grip_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_dial_grip_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_dial_grip_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_dial_grip(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_dial_grip_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_dial_grip_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_dial_grip_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: DialIndex
//

void init_style_dial_index_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
    lv_style_set_text_font(style, &ui_font_fa26);
};

lv_style_t *get_style_dial_index_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_index_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_dial_index_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
};

lv_style_t *get_style_dial_index_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dial_index_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_dial_index(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_dial_index_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_dial_index_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_dial_index(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_dial_index_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_dial_index_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: TimerText
//

void init_style_timer_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_text_font(style, &ui_font_rn64);
};

lv_style_t *get_style_timer_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_timer_text_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_timer_text_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
};

lv_style_t *get_style_timer_text_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_timer_text_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_timer_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_timer_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_timer_text_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_timer_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_timer_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_timer_text_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: LevelText
//

void init_style_level_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
    lv_style_set_text_font(style, &ui_font_rm14);
    lv_style_set_text_letter_space(style, 1);
};

lv_style_t *get_style_level_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_level_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_level_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_level_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_level_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_level_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: HintText
//

void init_style_hint_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_rm14);
};

lv_style_t *get_style_hint_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hint_text_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_hint_text_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
};

lv_style_t *get_style_hint_text_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hint_text_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_hint_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hint_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_hint_text_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_hint_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hint_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_hint_text_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: HintIcon
//

void init_style_hint_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_fa14);
};

lv_style_t *get_style_hint_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_hint_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_hint_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_hint_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_hint_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_hint_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SlotText
//

void init_style_slot_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rm15);
};

lv_style_t *get_style_slot_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slot_text_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_slot_text_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
};

lv_style_t *get_style_slot_text_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slot_text_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_slot_text_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
};

lv_style_t *get_style_slot_text_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slot_text_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_slot_text_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
};

lv_style_t *get_style_slot_text_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slot_text_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_slot_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slot_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slot_text_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_slot_text_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_slot_text_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_slot_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slot_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slot_text_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_slot_text_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_slot_text_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: SlotLine
//

void init_style_slot_line_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_slot_line_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slot_line_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_slot_line_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][13]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_slot_line_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slot_line_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_slot_line_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][15]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_slot_line_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slot_line_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_slot_line(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slot_line_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slot_line_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_slot_line_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_slot_line(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slot_line_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slot_line_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_slot_line_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: SlotLineCurrent
//

void init_style_slot_line_current_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][11]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 1);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_slot_line_current_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slot_line_current_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_slot_line_current(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slot_line_current_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_slot_line_current(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slot_line_current_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PoliceLight
//

void init_style_police_light_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_blend_mode(style, LV_BLEND_MODE_ADDITIVE);
    lv_style_set_opa(style, 255);
};

lv_style_t *get_style_police_light_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_police_light_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_police_light(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_police_light_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_police_light(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_police_light_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: GoldGlow
//

void init_style_gold_glow_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_blend_mode(style, LV_BLEND_MODE_ADDITIVE);
    lv_style_set_opa(style, 255);
};

lv_style_t *get_style_gold_glow_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_gold_glow_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_gold_glow_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_opa(style, 185);
};

lv_style_t *get_style_gold_glow_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_gold_glow_MAIN_CHECKED(style);
    }
    return style;
};

void init_style_gold_glow_MAIN_DISABLED(lv_style_t *style) {
    lv_style_set_opa(style, 110);
};

lv_style_t *get_style_gold_glow_MAIN_DISABLED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_gold_glow_MAIN_DISABLED(style);
    }
    return style;
};

void add_style_gold_glow(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_gold_glow_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_gold_glow_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_gold_glow_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

void remove_style_gold_glow(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_gold_glow_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_gold_glow_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_gold_glow_MAIN_DISABLED(), LV_PART_MAIN | LV_STATE_DISABLED);
};

//
// Style: RewardPill
//

void init_style_reward_pill_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_bg_opa(style, 200);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 28);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_reward_pill_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_reward_pill_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_reward_pill(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_reward_pill_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_reward_pill(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_reward_pill_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: RewardTitle
//

void init_style_reward_title_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_text_font(style, &ui_font_rm28);
};

lv_style_t *get_style_reward_title_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_reward_title_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_reward_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_reward_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_reward_title(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_reward_title_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: RewardCode
//

void init_style_reward_code_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][12]));
    lv_style_set_text_font(style, &ui_font_rm15);
    lv_style_set_text_letter_space(style, 1);
};

lv_style_t *get_style_reward_code_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_reward_code_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_reward_code(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_reward_code_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_reward_code(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_reward_code_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: RewardMeta
//

void init_style_reward_meta_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &ui_font_rr13);
};

lv_style_t *get_style_reward_meta_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_reward_meta_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_reward_meta(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_reward_meta_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_reward_meta(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_reward_meta_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_screen_root,
        add_style_label_body,
        add_style_ring_thin,
        add_style_ring_wide,
        add_style_ring_hair,
        add_style_title_bolts,
        add_style_title_bolt_slots,
        add_style_title_ticks,
        add_style_title_lock,
        add_style_title_text,
        add_style_title_sub,
        add_style_accent_hint,
        add_style_accent_hint_icon,
        add_style_instructions_body,
        add_style_start_button,
        add_style_start_button_text,
        add_style_button_text,
        add_style_button_outline,
        add_style_button_outline_text,
        add_style_outline_icon,
        add_style_device_list,
        add_style_device_row,
        add_style_device_row_text,
        add_style_info_button,
        add_style_info_icon,
        add_style_scrim,
        add_style_button_danger,
        add_style_button_danger_text,
        add_style_setting_row,
        add_style_step_button,
        add_style_step_icon,
        add_style_setting_value,
        add_style_seg_arc,
        add_style_seg_arc_current,
        add_style_dial_ticks,
        add_style_dial_disk,
        add_style_dial_grip,
        add_style_dial_index,
        add_style_timer_text,
        add_style_level_text,
        add_style_hint_text,
        add_style_hint_icon,
        add_style_slot_text,
        add_style_slot_line,
        add_style_slot_line_current,
        add_style_police_light,
        add_style_gold_glow,
        add_style_reward_pill,
        add_style_reward_title,
        add_style_reward_code,
        add_style_reward_meta,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_screen_root,
        remove_style_label_body,
        remove_style_ring_thin,
        remove_style_ring_wide,
        remove_style_ring_hair,
        remove_style_title_bolts,
        remove_style_title_bolt_slots,
        remove_style_title_ticks,
        remove_style_title_lock,
        remove_style_title_text,
        remove_style_title_sub,
        remove_style_accent_hint,
        remove_style_accent_hint_icon,
        remove_style_instructions_body,
        remove_style_start_button,
        remove_style_start_button_text,
        remove_style_button_text,
        remove_style_button_outline,
        remove_style_button_outline_text,
        remove_style_outline_icon,
        remove_style_device_list,
        remove_style_device_row,
        remove_style_device_row_text,
        remove_style_info_button,
        remove_style_info_icon,
        remove_style_scrim,
        remove_style_button_danger,
        remove_style_button_danger_text,
        remove_style_setting_row,
        remove_style_step_button,
        remove_style_step_icon,
        remove_style_setting_value,
        remove_style_seg_arc,
        remove_style_seg_arc_current,
        remove_style_dial_ticks,
        remove_style_dial_disk,
        remove_style_dial_grip,
        remove_style_dial_index,
        remove_style_timer_text,
        remove_style_level_text,
        remove_style_hint_text,
        remove_style_hint_icon,
        remove_style_slot_text,
        remove_style_slot_line,
        remove_style_slot_line_current,
        remove_style_police_light,
        remove_style_gold_glow,
        remove_style_reward_pill,
        remove_style_reward_title,
        remove_style_reward_code,
        remove_style_reward_meta,
    };
    remove_style_funcs[styleIndex](obj);
}