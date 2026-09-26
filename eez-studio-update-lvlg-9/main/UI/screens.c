#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

screen_page_dashboard_state_t screen_page_dashboard_state;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_page_dashboard_batt_arc(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target_obj(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_arc_get_value(ta);
            set_var_batt_decivolts(value);
        }
    }
}

//
// Screens
//

void create_screen_page_dashboard() {
    screen_page_dashboard_state_t *state = &screen_page_dashboard_state;
    (void)state;
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_dashboard = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_screen_root(obj);
    lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    {
        static lv_coord_t dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // header
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.header = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 64);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_header_bar(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // hdr_bolt
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hdr_bolt = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_icon_accent(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // hdr_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hdr_title = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_header_title(obj);
                    lv_label_set_text_static(obj, "POWER");
                }
                {
                    // hdr_kicker
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hdr_kicker = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_header_kicker(obj);
                    lv_label_set_text_static(obj, "OFF-GRID");
                }
                {
                    // hdr_spacer
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.hdr_spacer = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 10, 10);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_panel_plain(obj);
                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // hdr_wifi_btn
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hdr_wifi_btn = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 52, 40);
                    lv_obj_add_event_cb(obj, action_show_wifi, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_ghost(obj);
                    lv_obj_set_style_min_width(obj, 52, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_width(obj, 52, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hdr_wifi_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hdr_wifi_icon = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_icon_accent(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_dashicons22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // hdr_brk_btn
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hdr_brk_btn = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 52, 40);
                    lv_obj_add_event_cb(obj, action_show_broker, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_ghost(obj);
                    lv_obj_set_style_min_width(obj, 52, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_width(obj, 52, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hdr_brk_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hdr_brk_icon = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_icon_accent(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_dashicons22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // hdr_link
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hdr_link = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_icon_accent(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_dashicons16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // btn_theme
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.btn_theme = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 44, 36);
                    lv_obj_add_event_cb(obj, action_toggle_theme, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_ghost(obj);
                    lv_obj_set_style_min_width(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_width(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_min_height(obj, 36, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_height(obj, 36, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // btn_theme_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.btn_theme_icon = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_dashicons22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
            }
        }
        {
            // body
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.body = obj;
            lv_obj_set_pos(obj, 0, 64);
            lv_obj_set_size(obj, 1024, 496);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_body_area(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
                lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            {
                static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
                lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            lv_obj_set_style_pad_column(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // card_battery
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.card_battery = obj;
                    lv_obj_set_pos(obj, 12, 0);
                    lv_obj_set_size(obj, 325, 472);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_card(obj);
                    lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_EVENLY, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // batt_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.batt_title = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_card_title(obj);
                            lv_label_set_text_static(obj, "BATTERY");
                        }
                        {
                            // batt_gauge
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.batt_gauge = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 236, 236);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_panel_plain(obj);
                            lv_obj_set_style_min_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // batt_scale
                                    lv_obj_t *obj = lv_scale_create(parent_obj);
                                    objects.batt_scale = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 236, 236);
                                    lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
                                    lv_scale_set_range(obj, 100, 140);
                                    lv_scale_set_angle_range(obj, 270);
                                    lv_scale_set_rotation(obj, 135);
                                    lv_scale_set_total_tick_count(obj, 21);
                                    lv_scale_set_major_tick_every(obj, 5);
                                    lv_scale_set_label_show(obj, true);
                                    static const char *label_texts[6] = {
                                        "10",
                                        "11",
                                        "12",
                                        "13",
                                        "14",
                                        NULL
                                    };
                                    lv_scale_set_text_src(obj, label_texts);
                                    {
                                        state->low = lv_scale_add_section(obj);
                                        lv_scale_section_set_range(state->low, 100, 118);
                                        lv_scale_section_set_style(state->low, LV_PART_MAIN, get_style_scale_section_danger_MAIN_DEFAULT());
                                    }
                                    {
                                        state->warn = lv_scale_add_section(obj);
                                        lv_scale_section_set_range(state->warn, 118, 124);
                                        lv_scale_section_set_style(state->warn, LV_PART_MAIN, get_style_scale_section_warn_MAIN_DEFAULT());
                                    }
                                    {
                                        state->good = lv_scale_add_section(obj);
                                        lv_scale_section_set_range(state->good, 124, 140);
                                        lv_scale_section_set_style(state->good, LV_PART_MAIN, get_style_scale_section_good_MAIN_DEFAULT());
                                    }
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_scale_ring(obj);
                                    lv_obj_set_style_min_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_arc_width(obj, 6, LV_PART_MAIN);
                                    lv_obj_set_style_length(obj, 7, LV_PART_ITEMS);
                                    lv_obj_set_style_length(obj, 14, LV_PART_INDICATOR);
                                }
                                {
                                    // batt_arc
                                    lv_obj_t *obj = lv_arc_create(parent_obj);
                                    objects.batt_arc = obj;
                                    lv_obj_set_pos(obj, 26, 26);
                                    lv_obj_set_size(obj, 184, 184);
                                    lv_arc_set_range(obj, 100, 140);
                                    lv_obj_add_event_cb(obj, event_handler_cb_page_dashboard_batt_arc, LV_EVENT_ALL, 0);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                                    add_style_arc_battery(obj);
                                    lv_obj_set_style_min_width(obj, 184, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 184, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 184, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 184, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                                {
                                    // batt_centre
                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                    objects.batt_centre = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 236, 236);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_panel_plain(obj);
                                    lv_obj_set_style_min_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // batt_pct_span
                                            lv_obj_t *obj = lv_spangroup_create(parent_obj);
                                            objects.batt_pct_span = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            {
                                                state->span_0 = lv_spangroup_new_span(obj);
                                            }
                                            {
                                                state->span_1 = lv_spangroup_new_span(obj);
                                                lv_span_set_text_static(state->span_1, " %");
                                                lv_style_t *span_1_style = lv_span_get_style(state->span_1);
                                                lv_style_set_text_font(span_1_style, &lv_font_montserrat_20);
                                            }
                                            lv_spangroup_refr_mode(obj);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_span_hero(obj);
                                        }
                                        {
                                            // batt_volts_span
                                            lv_obj_t *obj = lv_spangroup_create(parent_obj);
                                            objects.batt_volts_span = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            {
                                                state->span_01 = lv_spangroup_new_span(obj);
                                            }
                                            {
                                                state->span_11 = lv_spangroup_new_span(obj);
                                                lv_span_set_text_static(state->span_11, " V");
                                                lv_style_t *span_1_style = lv_span_get_style(state->span_11);
                                                lv_style_set_text_font(span_1_style, &lv_font_montserrat_16);
                                            }
                                            lv_spangroup_refr_mode(obj);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_span_secondary(obj);
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // batt_state
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.batt_state = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_status_line(obj);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // card_solar
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.card_solar = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 325, 472);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_card(obj);
                    lv_obj_set_style_grid_cell_column_pos(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_EVENLY, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // solar_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.solar_title = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_card_title(obj);
                            lv_label_set_text_static(obj, "SOLAR INPUT");
                        }
                        {
                            // solar_gauge
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.solar_gauge = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 236, 236);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_panel_plain(obj);
                            lv_obj_set_style_min_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // solar_arc
                                    lv_obj_t *obj = lv_arc_create(parent_obj);
                                    objects.solar_arc = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 236, 236);
                                    lv_arc_set_start_angle(obj, 135);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                                    add_style_arc_solar(obj);
                                    lv_obj_set_style_min_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                                {
                                    // solar_centre
                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                    objects.solar_centre = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 236, 236);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_panel_plain(obj);
                                    lv_obj_set_style_min_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 236, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // solar_sun
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.solar_sun = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_icon_solar(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // solar_w_span
                                            lv_obj_t *obj = lv_spangroup_create(parent_obj);
                                            objects.solar_w_span = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            {
                                                state->span_02 = lv_spangroup_new_span(obj);
                                            }
                                            {
                                                state->span_12 = lv_spangroup_new_span(obj);
                                                lv_span_set_text_static(state->span_12, " W");
                                                lv_style_t *span_1_style = lv_span_get_style(state->span_12);
                                                lv_style_set_text_font(span_1_style, &lv_font_montserrat_20);
                                            }
                                            lv_spangroup_refr_mode(obj);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_span_hero(obj);
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // solar_strip
                            lv_obj_t *obj = lv_scale_create(parent_obj);
                            objects.solar_strip = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 265, 40);
                            lv_scale_set_mode(obj, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
                            lv_scale_set_range(obj, 0, 1200);
                            lv_scale_set_angle_range(obj, 270);
                            lv_scale_set_rotation(obj, 135);
                            lv_scale_set_total_tick_count(obj, 13);
                            lv_scale_set_major_tick_every(obj, 3);
                            lv_scale_set_label_show(obj, true);
                            static const char *label_texts[6] = {
                                "0",
                                "300",
                                "600",
                                "900",
                                "1200",
                                NULL
                            };
                            lv_scale_set_text_src(obj, label_texts);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_scale_strip(obj);
                            lv_obj_set_style_min_width(obj, 265, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 265, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_length(obj, 5, LV_PART_ITEMS);
                            lv_obj_set_style_length(obj, 10, LV_PART_INDICATOR);
                        }
                        {
                            // solar_today
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.solar_today = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_status_line(obj);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // card_tanks
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.card_tanks = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 325, 472);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_card(obj);
                    lv_obj_set_style_grid_cell_column_pos(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_SPACE_EVENLY, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // tanks_title
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.tanks_title = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_card_title(obj);
                            lv_label_set_text_static(obj, "TANKS");
                        }
                        {
                            // tanks_inset
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.tanks_inset = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 293, 168);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_panel_inset(obj);
                            lv_obj_set_style_min_width(obj, 293, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 293, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 168, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 168, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 18, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // tank_fresh
                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                    objects.tank_fresh = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 269, 56);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_panel_plain(obj);
                                    lv_obj_set_style_min_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 56, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 56, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // tank_fresh_row
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.tank_fresh_row = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 269, 28);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_min_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_min_height(obj, 28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_height(obj, 28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // tank_fresh_icon
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.tank_fresh_icon = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                    add_style_icon_fresh(obj);
                                                    lv_label_set_text_static(obj, "");
                                                }
                                                {
                                                    // tank_fresh_name
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.tank_fresh_name = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                    add_style_tank_row_label(obj);
                                                    lv_label_set_text_static(obj, "FRESH");
                                                }
                                                {
                                                    // tank_fresh_sp
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.tank_fresh_sp = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, 10, 10);
                                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                                    add_style_panel_plain(obj);
                                                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    // tank_fresh_span
                                                    lv_obj_t *obj = lv_spangroup_create(parent_obj);
                                                    objects.tank_fresh_span = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    {
                                                        state->span_03 = lv_spangroup_new_span(obj);
                                                    }
                                                    {
                                                        state->span_13 = lv_spangroup_new_span(obj);
                                                        lv_span_set_text_static(state->span_13, " %");
                                                        lv_style_t *span_1_style = lv_span_get_style(state->span_13);
                                                        lv_style_set_text_font(span_1_style, &lv_font_montserrat_14);
                                                    }
                                                    lv_spangroup_refr_mode(obj);
                                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                                    add_style_span_tank(obj);
                                                }
                                            }
                                        }
                                        {
                                            // tank_fresh_bar
                                            lv_obj_t *obj = lv_bar_create(parent_obj);
                                            objects.tank_fresh_bar = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 269, 18);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                                            add_style_bar_fresh(obj);
                                            lv_obj_set_style_min_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_min_height(obj, 18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_height(obj, 18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                                {
                                    // tank_grey
                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                    objects.tank_grey = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 269, 56);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_panel_plain(obj);
                                    lv_obj_set_style_min_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 56, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 56, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // tank_grey_row
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.tank_grey_row = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 269, 28);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_min_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_min_height(obj, 28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_height(obj, 28, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            {
                                                lv_obj_t *parent_obj = obj;
                                                {
                                                    // tank_grey_icon
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.tank_grey_icon = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                    add_style_icon_grey(obj);
                                                    lv_label_set_text_static(obj, "");
                                                }
                                                {
                                                    // tank_grey_name
                                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                                    objects.tank_grey_name = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                                    add_style_tank_row_label(obj);
                                                    lv_label_set_text_static(obj, "GREY");
                                                }
                                                {
                                                    // tank_grey_sp
                                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                                    objects.tank_grey_sp = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, 10, 10);
                                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                                    add_style_panel_plain(obj);
                                                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                                }
                                                {
                                                    // tank_grey_span
                                                    lv_obj_t *obj = lv_spangroup_create(parent_obj);
                                                    objects.tank_grey_span = obj;
                                                    lv_obj_set_pos(obj, 0, 0);
                                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                                    {
                                                        state->span_04 = lv_spangroup_new_span(obj);
                                                    }
                                                    {
                                                        state->span_14 = lv_spangroup_new_span(obj);
                                                        lv_span_set_text_static(state->span_14, " %");
                                                        lv_style_t *span_1_style = lv_span_get_style(state->span_14);
                                                        lv_style_set_text_font(span_1_style, &lv_font_montserrat_14);
                                                    }
                                                    lv_spangroup_refr_mode(obj);
                                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                                    add_style_span_tank(obj);
                                                }
                                            }
                                        }
                                        {
                                            // tank_grey_bar
                                            lv_obj_t *obj = lv_bar_create(parent_obj);
                                            objects.tank_grey_bar = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 269, 18);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                                            add_style_bar_grey(obj);
                                            lv_obj_set_style_min_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_width(obj, 269, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_min_height(obj, 18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_max_height(obj, 18, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // tanks_note
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.tanks_note = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_status_line(obj);
                            lv_label_set_text_static(obj, "Readings are indicative only");
                        }
                    }
                }
            }
        }
        {
            // footer
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.footer = obj;
            lv_obj_set_pos(obj, 0, 560);
            lv_obj_set_size(obj, 1024, 40);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_footer_bar(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ft_src
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ft_src = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_footer_text(obj);
                    lv_label_set_text_static(obj, "MQTT  headwaters.local");
                }
                {
                    // ft_sp
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ft_sp = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 10, 10);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_panel_plain(obj);
                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // ft_uptime
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ft_uptime = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_footer_text(obj);
                    lv_label_set_text(obj, "");
                }
                {
                    // ft_build
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ft_build = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_footer_text(obj);
                    lv_label_set_text_static(obj, "LVGL 9.2.2  EEZ 0.29");
                }
            }
        }
    }
    
    tick_screen_page_dashboard();
}

void tick_screen_page_dashboard() {
    screen_page_dashboard_state_t *state = &screen_page_dashboard_state;
    (void)state;
    {
        const char *new_val = get_var_link_text();
        const char *cur_val = lv_label_get_text(objects.hdr_link);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.hdr_link;
            lv_label_set_text(objects.hdr_link, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_batt_decivolts();
        int32_t cur_val = lv_arc_get_value(objects.batt_arc);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.batt_arc;
            lv_arc_set_value(objects.batt_arc, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_batt_percent_text();
        lv_span_set_text(state->span_0, new_val);
        lv_spangroup_refr_mode(objects.batt_pct_span);
    }
    {
        const char *new_val = get_var_batt_volts_text();
        lv_span_set_text(state->span_01, new_val);
        lv_spangroup_refr_mode(objects.batt_volts_span);
    }
    {
        const char *new_val = get_var_batt_state_text();
        const char *cur_val = lv_label_get_text(objects.batt_state);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.batt_state;
            lv_label_set_text(objects.batt_state, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = get_var_solar_sweep_end();
        int32_t cur_val = lv_arc_get_angle_end(objects.solar_arc);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.solar_arc;
            lv_arc_set_end_angle(objects.solar_arc, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_solar_watts_text();
        lv_span_set_text(state->span_02, new_val);
        lv_spangroup_refr_mode(objects.solar_w_span);
    }
    {
        const char *new_val = get_var_solar_today_text();
        const char *cur_val = lv_label_get_text(objects.solar_today);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.solar_today;
            lv_label_set_text(objects.solar_today, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_fresh_percent_text();
        lv_span_set_text(state->span_03, new_val);
        lv_spangroup_refr_mode(objects.tank_fresh_span);
    }
    {
        int32_t new_val = get_var_fresh_percent();
        int32_t cur_val = lv_bar_get_value(objects.tank_fresh_bar);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.tank_fresh_bar;
            lv_bar_set_value(objects.tank_fresh_bar, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_grey_percent_text();
        lv_span_set_text(state->span_04, new_val);
        lv_spangroup_refr_mode(objects.tank_grey_span);
    }
    {
        int32_t new_val = get_var_grey_percent();
        int32_t cur_val = lv_bar_get_value(objects.tank_grey_bar);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.tank_grey_bar;
            lv_bar_set_value(objects.tank_grey_bar, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_uptime_text();
        const char *cur_val = lv_label_get_text(objects.ft_uptime);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.ft_uptime;
            lv_label_set_text(objects.ft_uptime, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_wifi() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_wifi = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_screen_root(obj);
    lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    {
        static lv_coord_t dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // wifi_header
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.wifi_header = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 64);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_header_bar(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // wifi_back
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.wifi_back = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 120, 40);
                    lv_obj_add_event_cb(obj, action_show_dashboard, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_ghost(obj);
                    lv_obj_set_style_min_width(obj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_width(obj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // wifi_back_lbl
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.wifi_back_lbl = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_label_set_text_static(obj, "BACK");
                        }
                    }
                }
                {
                    // wifi_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.wifi_title = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_header_title(obj);
                    lv_label_set_text_static(obj, "WI-FI SETUP");
                }
                {
                    // wifi_hdr_sp
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.wifi_hdr_sp = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 10, 10);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_panel_plain(obj);
                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // wifi_status
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.wifi_status = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_icon_accent(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_dashicons16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // wifi_body
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.wifi_body = obj;
            lv_obj_set_pos(obj, 0, 64);
            lv_obj_set_size(obj, 1024, 496);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_body_area(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                static lv_coord_t dsc[] = {LV_GRID_FR(2), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
                lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            {
                static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
                lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            lv_obj_set_style_pad_column(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // wifi_list_card
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.wifi_list_card = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 395, 472);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_card(obj);
                    lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // wifi_list_head
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.wifi_list_head = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 363, 34);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_panel_plain(obj);
                            lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 34, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 34, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // wifi_list_title
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.wifi_list_title = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_card_title(obj);
                                    lv_label_set_text_static(obj, "NETWORKS");
                                }
                                {
                                    // wifi_list_sp
                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                    objects.wifi_list_sp = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 10, 10);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_panel_plain(obj);
                                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                                {
                                    // wifi_scan_btn
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_scan_btn = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 110, 34);
                                    lv_obj_add_event_cb(obj, action_wifi_scan, LV_EVENT_CLICKED, (void *)0);
                                    add_style_button_primary(obj);
                                    lv_obj_set_style_min_width(obj, 110, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 110, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 34, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 34, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_scan_lbl
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_scan_lbl = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_label_set_text_static(obj, "SCAN");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // wifi_rows
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.wifi_rows = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 363, 400);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_AUTO);
                            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                            add_style_panel_plain(obj);
                            lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 400, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 400, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_COLUMN, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // wifi_row_0
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_0 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)0);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_0
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_0 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_0
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_0 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_0
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_0 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_1
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_1 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)1);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_1
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_1 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_1
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_1 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_1
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_1 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_2
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_2 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)2);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_2
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_2 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_2
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_2 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_2
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_2 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_3
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_3 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)3);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_3
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_3 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_3
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_3 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_3
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_3 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_4
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_4 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)4);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_4
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_4 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_4
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_4 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_4
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_4 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_5
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_5 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)5);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_5
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_5 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_5
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_5 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_5
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_5 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_6
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_6 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)6);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_6
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_6 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_6
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_6 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_6
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_6 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_7
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_7 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)7);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_7
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_7 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_7
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_7 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_7
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_7 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_8
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_8 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)8);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_8
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_8 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_8
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_8 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_8
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_8 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_9
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_9 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)9);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_9
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_9 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_9
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_9 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_9
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_9 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_10
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_10 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)10);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_10
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_10 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_10
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_10 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_10
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_10 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_11
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_11 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)11);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_11
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_11 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_11
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_11 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_11
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_11 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_12
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_12 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)12);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_12
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_12 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_12
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_12 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_12
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_12 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_13
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_13 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)13);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_13
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_13 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_13
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_13 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_13
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_13 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_14
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_14 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)14);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_14
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_14 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_14
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_14 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_14
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_14 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_15
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_15 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)15);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_15
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_15 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_15
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_15 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_15
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_15 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_16
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_16 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)16);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_16
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_16 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_16
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_16 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_16
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_16 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_17
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_17 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)17);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_17
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_17 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_17
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_17 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_17
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_17 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_18
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_18 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)18);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_18
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_18 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_18
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_18 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_18
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_18 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                                {
                                    // wifi_row_19
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_row_19 = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 363, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_select, LV_EVENT_CLICKED, (void *)19);
                                    add_style_scan_row(obj);
                                    lv_obj_set_style_min_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 363, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_ssid_19
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_ssid_19 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 220, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_ssid(obj);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // wifi_row_sp_19
                                            lv_obj_t *obj = lv_obj_create(parent_obj);
                                            objects.wifi_row_sp_19 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 10, 10);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_panel_plain(obj);
                                            lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                        }
                                        {
                                            // wifi_meta_19
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_meta_19 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 90, 22);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_scan_row_meta(obj);
                                            lv_label_set_text_static(obj, "");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                {
                    // wifi_form_card
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.wifi_form_card = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 593, 472);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_card(obj);
                    lv_obj_set_style_grid_cell_column_pos(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
                        lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    {
                        static lv_coord_t dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
                        lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // wifi_selected
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.wifi_selected = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 561, 28);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_card_title(obj);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 28, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // wifi_security
                            lv_obj_t *obj = lv_dropdown_create(parent_obj);
                            objects.wifi_security = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 561, 40);
                            lv_dropdown_set_options_static(obj, "WPA2 / WPA3\nWPA\nWEP\nOPEN");
                            lv_dropdown_set_selected(obj, 0);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_dropdown_default(obj);
                            lv_obj_set_style_min_width(obj, 561, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 561, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // wifi_password
                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                            objects.wifi_password = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 561, 48);
                            lv_textarea_set_max_length(obj, 64);
                            lv_textarea_set_placeholder_text(obj, "password");
                            lv_textarea_set_one_line(obj, true);
                            lv_textarea_set_password_mode(obj, true);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_textarea_default(obj);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // wifi_kb
                            lv_obj_t *obj = lv_keyboard_create(parent_obj);
                            objects.wifi_kb = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 561, 248);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_keyboard_default(obj);
                            lv_obj_set_style_min_width(obj, 561, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 561, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 248, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 248, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // wifi_actions
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.wifi_actions = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 561, 44);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_panel_plain(obj);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // wifi_cancel
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_cancel = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 150, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_cancel, LV_EVENT_CLICKED, (void *)0);
                                    add_style_button_ghost(obj);
                                    lv_obj_set_style_min_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_cancel_lbl
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_cancel_lbl = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_label_set_text_static(obj, "CANCEL");
                                        }
                                    }
                                }
                                {
                                    // wifi_act_sp
                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                    objects.wifi_act_sp = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 10, 10);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_panel_plain(obj);
                                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                                {
                                    // wifi_connect
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.wifi_connect = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 200, 44);
                                    lv_obj_add_event_cb(obj, action_wifi_connect, LV_EVENT_CLICKED, (void *)0);
                                    add_style_button_primary(obj);
                                    lv_obj_set_style_min_width(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // wifi_connect_lbl
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.wifi_connect_lbl = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_label_set_text_static(obj, "CONNECT");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            // wifi_footer
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.wifi_footer = obj;
            lv_obj_set_pos(obj, 0, 560);
            lv_obj_set_size(obj, 1024, 40);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_footer_bar(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // wifi_hint
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.wifi_hint = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_footer_text(obj);
                    lv_label_set_text_static(obj, "CREDENTIALS ARE STORED ON THE PANEL IN NVS");
                }
            }
        }
    }
    
    tick_screen_page_wifi();
}

void tick_screen_page_wifi() {
    {
        const char *new_val = get_var_wifi_status_text();
        const char *cur_val = lv_label_get_text(objects.wifi_status);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.wifi_status;
            lv_label_set_text(objects.wifi_status, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_wifi_selected_text();
        const char *cur_val = lv_label_get_text(objects.wifi_selected);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.wifi_selected;
            lv_label_set_text(objects.wifi_selected, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_broker() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_broker = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_screen_root(obj);
    lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    {
        static lv_coord_t dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // brk_header
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.brk_header = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 64);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_header_bar(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 64, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // brk_back
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.brk_back = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 120, 40);
                    lv_obj_add_event_cb(obj, action_show_dashboard, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_ghost(obj);
                    lv_obj_set_style_min_width(obj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_width(obj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // brk_back_lbl
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.brk_back_lbl = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_label_set_text_static(obj, "BACK");
                        }
                    }
                }
                {
                    // brk_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.brk_title = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_header_title(obj);
                    lv_label_set_text_static(obj, "HEADWATERS");
                }
                {
                    // brk_hdr_sp
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.brk_hdr_sp = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 10, 10);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_panel_plain(obj);
                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // brk_status
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.brk_status = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_icon_accent(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_dashicons16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
            }
        }
        {
            // brk_body
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.brk_body = obj;
            lv_obj_set_pos(obj, 0, 64);
            lv_obj_set_size(obj, 1024, 496);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_body_area(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
                lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            {
                static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
                lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // brk_card
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.brk_card = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 1000, 472);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_card(obj);
                    lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        static lv_coord_t dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
                        lv_obj_set_style_grid_column_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    {
                        static lv_coord_t dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
                        lv_obj_set_style_grid_row_dsc_array(obj, dsc, LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_row(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // brk_host
                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                            objects.brk_host = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 968, 52);
                            lv_textarea_set_max_length(obj, 64);
                            lv_textarea_set_placeholder_text(obj, "broker host, e.g. headwaters.local");
                            lv_textarea_set_one_line(obj, true);
                            lv_textarea_set_password_mode(obj, false);
                            lv_obj_add_event_cb(obj, action_broker_field, LV_EVENT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_textarea_default(obj);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // brk_user
                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                            objects.brk_user = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 968, 52);
                            lv_textarea_set_max_length(obj, 64);
                            lv_textarea_set_placeholder_text(obj, "username");
                            lv_textarea_set_one_line(obj, true);
                            lv_textarea_set_password_mode(obj, false);
                            lv_obj_add_event_cb(obj, action_broker_field, LV_EVENT_CLICKED, (void *)1);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_textarea_default(obj);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // brk_pass
                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                            objects.brk_pass = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 968, 52);
                            lv_textarea_set_max_length(obj, 64);
                            lv_textarea_set_placeholder_text(obj, "password");
                            lv_textarea_set_one_line(obj, true);
                            lv_textarea_set_password_mode(obj, true);
                            lv_obj_add_event_cb(obj, action_broker_field, LV_EVENT_CLICKED, (void *)2);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_textarea_default(obj);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // brk_hint
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.brk_hint = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 968, 22);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_footer_text(obj);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 22, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "TLS on port 8883. Saved to this panel only.");
                        }
                        {
                            // brk_kb
                            lv_obj_t *obj = lv_keyboard_create(parent_obj);
                            objects.brk_kb = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 968, 178);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_keyboard_default(obj);
                            lv_obj_set_style_min_width(obj, 968, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_width(obj, 968, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 178, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 178, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // brk_actions
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.brk_actions = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 968, 44);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_panel_plain(obj);
                            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_pos(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_column(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // brk_cancel
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.brk_cancel = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 150, 44);
                                    lv_obj_add_event_cb(obj, action_broker_cancel, LV_EVENT_CLICKED, (void *)0);
                                    add_style_button_ghost(obj);
                                    lv_obj_set_style_min_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // brk_cancel_lbl
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.brk_cancel_lbl = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_label_set_text_static(obj, "CANCEL");
                                        }
                                    }
                                }
                                {
                                    // brk_act_sp
                                    lv_obj_t *obj = lv_obj_create(parent_obj);
                                    objects.brk_act_sp = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 10, 10);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_panel_plain(obj);
                                    lv_obj_set_style_flex_grow(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                                {
                                    // brk_save
                                    lv_obj_t *obj = lv_button_create(parent_obj);
                                    objects.brk_save = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, 220, 44);
                                    lv_obj_add_event_cb(obj, action_broker_save, LV_EVENT_CLICKED, (void *)0);
                                    add_style_button_primary(obj);
                                    lv_obj_set_style_min_width(obj, 220, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_width(obj, 220, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_min_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_max_height(obj, 44, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // brk_save_lbl
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.brk_save_lbl = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_label_set_text_static(obj, "SAVE & CONNECT");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            // brk_footer
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.brk_footer = obj;
            lv_obj_set_pos(obj, 0, 560);
            lv_obj_set_size(obj, 1024, 40);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_footer_bar(obj);
            lv_obj_set_style_grid_cell_column_pos(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_column_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_pos(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_row_span(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_x_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_grid_cell_y_align(obj, LV_GRID_ALIGN_STRETCH, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_flow(obj, LV_FLEX_FLOW_ROW, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_main_place(obj, LV_FLEX_ALIGN_START, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_cross_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(obj, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // brk_foot
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.brk_foot = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_footer_text(obj);
                    lv_label_set_text_static(obj, "CREDENTIALS ARE STORED ON THE PANEL IN NVS");
                }
            }
        }
    }
    
    tick_screen_page_broker();
}

void tick_screen_page_broker() {
    {
        const char *new_val = get_var_broker_status_text();
        const char *cur_val = lv_label_get_text(objects.brk_status);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.brk_status;
            lv_label_set_text(objects.brk_status, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_page_dashboard,
    tick_screen_page_wifi,
    tick_screen_page_broker,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 3) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
    { "dashicons16", &ui_font_dashicons16 },
    { "dashicons22", &ui_font_dashicons22 },
    { "mono26", &ui_font_mono26 },
    { "mono44", &ui_font_mono44 },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;
void change_color_theme(uint32_t theme_index) {
    active_theme_index = theme_index;
    
    {
        screen_page_dashboard_state_t *state = &screen_page_dashboard_state;
        (void)state;
        lv_obj_set_style_text_color(objects.btn_theme_icon, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_style_set_bg_color(get_style_screen_root_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    lv_style_set_text_color(get_style_screen_root_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_header_bar_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_border_color(get_style_header_bar_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_header_bar_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_footer_bar_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_border_color(get_style_footer_bar_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_footer_bar_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_border_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_shadow_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_panel_inset_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_label_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_card_title_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_header_title_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_header_kicker_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_icon_accent_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_text_color(get_style_icon_solar_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_text_color(get_style_icon_fresh_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_icon_grey_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_text_color(get_style_status_line_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_footer_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_tank_row_label_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_span_hero_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_span_secondary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_span_tank_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_scale_ring_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_line_color(get_style_scale_ring_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_arc_color(get_style_scale_ring_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_line_color(get_style_scale_ring_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_line_color(get_style_scale_ring_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_scale_ring_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_scale_strip_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_line_color(get_style_scale_strip_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_line_color(get_style_scale_strip_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_line_color(get_style_scale_strip_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_scale_strip_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_line_color(get_style_scale_section_danger_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_arc_color(get_style_scale_section_danger_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_text_color(get_style_scale_section_danger_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_line_color(get_style_scale_section_warn_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_arc_color(get_style_scale_section_warn_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_text_color(get_style_scale_section_warn_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_line_color(get_style_scale_section_good_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_arc_color(get_style_scale_section_good_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_text_color(get_style_scale_section_good_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_arc_color(get_style_arc_battery_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_arc_color(get_style_arc_battery_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_arc_color(get_style_arc_solar_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_arc_color(get_style_arc_solar_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_bg_color(get_style_bar_fresh_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_bar_fresh_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_bg_color(get_style_bar_grey_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_bar_grey_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_bg_color(get_style_button_ghost_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_border_color(get_style_button_ghost_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_button_ghost_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_button_ghost_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_text_color(get_style_button_ghost_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_bg_color(get_style_scan_row_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_border_color(get_style_scan_row_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_bg_color(get_style_scan_row_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_bg_color(get_style_scan_row_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_border_color(get_style_scan_row_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_text_color(get_style_scan_row_ssid_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_scan_row_ssid_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_text_color(get_style_scan_row_meta_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_scan_row_meta_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_bg_color(get_style_textarea_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_border_color(get_style_textarea_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_textarea_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_border_color(get_style_textarea_default_MAIN_FOCUSED(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_bg_color(get_style_keyboard_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_keyboard_default_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_keyboard_default_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_border_color(get_style_keyboard_default_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_bg_color(get_style_dropdown_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_border_color(get_style_dropdown_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_dropdown_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_button_primary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_text_color(get_style_button_primary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_bg_color(get_style_button_primary_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_button_primary_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_obj_invalidate(objects.page_dashboard);
    lv_obj_invalidate(objects.page_wifi);
    lv_obj_invalidate(objects.page_broker);
}
uint32_t theme_colors[2][15] = {
    { 0xff52a441, 0xffd99000, 0xffc4321f, 0xffcf8100, 0xffeceff2, 0xffffffff, 0xffdfe4e9, 0xffccd3da, 0xff141a1f, 0xff5b6672, 0xffffffff, 0xff0f140e, 0xff2a7fbf, 0xff82653f, 0xff95a0aa },
    { 0xff52a441, 0xfff0b81e, 0xffe04a36, 0xfff0a020, 0xff13161a, 0xff1e2429, 0xff0e1115, 0xff2b323a, 0xfff1f5f8, 0xff8e9ba8, 0xffffffff, 0xff0f140e, 0xff3f9ede, 0xffa5825d, 0xff57616b },
};

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_page_dashboard();
    create_screen_page_wifi();
    create_screen_page_broker();
}