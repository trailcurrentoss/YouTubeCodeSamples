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

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

//
// Screens
//

void create_screen_page_idle() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_idle = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // idle_clock
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.idle_clock = obj;
            lv_obj_set_pos(obj, 3, 3);
            lv_obj_set_size(obj, 354, 354);
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_scale_set_range(obj, 0, 3600);
            lv_scale_set_angle_range(obj, 360);
            lv_scale_set_rotation(obj, 270);
            lv_scale_set_total_tick_count(obj, 61);
            lv_scale_set_major_tick_every(obj, 5);
            lv_scale_set_label_show(obj, true);
            static const char *label_texts[14] = {
                "12",
                "1",
                "2",
                "3",
                "4",
                "5",
                "6",
                "7",
                "8",
                "9",
                "10",
                "11",
                "12",
                NULL
            };
            lv_scale_set_text_src(obj, label_texts);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_clock_dial(obj);
            lv_obj_set_style_min_width(obj, 354, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 354, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 354, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 354, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 2, LV_PART_MAIN);
            lv_obj_set_style_length(obj, 6, LV_PART_ITEMS);
            lv_obj_set_style_length(obj, 13, LV_PART_INDICATOR);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // idle_hand_hour
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.idle_hand_hour = obj;
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_hand_hour(obj);
                }
                {
                    // idle_hand_minute
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.idle_hand_minute = obj;
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_hand_minute(obj);
                }
                {
                    // idle_hand_second
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    objects.idle_hand_second = obj;
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_hand_second(obj);
                }
            }
        }
        {
            // idle_clock_cap
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.idle_clock_cap = obj;
            lv_obj_set_pos(obj, 174, 174);
            lv_obj_set_size(obj, 12, 12);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_clock_cap(obj);
        }
        {
            // idle_info
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.idle_info = obj;
            lv_obj_set_pos(obj, 45, 200);
            lv_obj_set_size(obj, 270, 76);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_info_column(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // idle_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.idle_title = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_idle_title(obj);
                    lv_label_set_text_static(obj, "No supported app");
                }
                {
                    // idle_hint
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.idle_hint = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_idle_hint(obj);
                    lv_label_set_text_static(obj, "Focus a supported app");
                }
                {
                    // idle_pill
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.idle_pill = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_pill(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // idle_dot
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.idle_dot = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 8, 8);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_status_dot(obj);
                        }
                        {
                            // idle_status
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.idle_status = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_pill_text(obj);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_page_idle();
}

void tick_screen_page_idle() {
    {
        int32_t new_val = get_var_clock_hour();
        lv_scale_set_line_needle_value(lv_obj_get_parent(objects.idle_hand_hour), objects.idle_hand_hour, 82, new_val);
    }
    {
        int32_t new_val = get_var_clock_minute();
        lv_scale_set_line_needle_value(lv_obj_get_parent(objects.idle_hand_minute), objects.idle_hand_minute, 128, new_val);
    }
    {
        int32_t new_val = get_var_clock_second();
        lv_scale_set_line_needle_value(lv_obj_get_parent(objects.idle_hand_second), objects.idle_hand_second, 135, new_val);
    }
    {
        const char *new_val = get_var_usb_status_text();
        const char *cur_val = lv_label_get_text(objects.idle_status);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.idle_status;
            lv_label_set_text(objects.idle_status, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_freecad() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_freecad = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // fc_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.fc_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_center_panel(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.fc_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_freecad);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // fc_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text(obj, "");
                }
                {
                    // fc_angle
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_angle = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_angle_label(obj);
                    lv_label_set_text(obj, "");
                }
                {
                    // fc_axes
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.fc_axes = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_axis_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // fc_axis_x
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.fc_axis_x = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_axis_chip_x(obj);
                            lv_label_set_text_static(obj, "X");
                        }
                        {
                            // fc_axis_y
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.fc_axis_y = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_axis_chip_y(obj);
                            lv_label_set_text_static(obj, "Y");
                        }
                        {
                            // fc_axis_z
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.fc_axis_z = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_axis_chip_z(obj);
                            lv_label_set_text_static(obj, "Z");
                        }
                    }
                }
            }
        }
        {
            // fc_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.fc_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // fc_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Fit All");
                }
                {
                    // fc_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "V F");
                }
            }
        }
        {
            // fc_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.fc_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // fc_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Iso");
                }
                {
                    // fc_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "0");
                }
            }
        }
        {
            // fc_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.fc_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // fc_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Front");
                }
                {
                    // fc_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "1");
                }
            }
        }
        {
            // fc_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.fc_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // fc_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Top");
                }
                {
                    // fc_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "2");
                }
            }
        }
        {
            // fc_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.fc_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // fc_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Right");
                }
                {
                    // fc_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "3");
                }
            }
        }
        {
            // fc_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.fc_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // fc_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Hide/Show");
                }
                {
                    // fc_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Space");
                }
            }
        }
        {
            // fc_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.fc_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // fc_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Undo");
                }
                {
                    // fc_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Z");
                }
            }
        }
        {
            // fc_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.fc_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // fc_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // fc_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Redo");
                }
                {
                    // fc_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.fc_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Y");
                }
            }
        }
    }
    
    tick_screen_page_freecad();
}

void tick_screen_page_freecad() {
    {
        const char *new_val = get_var_axis_mode_text();
        const char *cur_val = lv_label_get_text(objects.fc_mode);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.fc_mode;
            lv_label_set_text(objects.fc_mode, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_angle_text();
        const char *cur_val = lv_label_get_text(objects.fc_angle);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.fc_angle;
            lv_label_set_text(objects.fc_angle, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_blender() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_blender = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // bl_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.bl_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_center_panel(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.bl_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_blender);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // bl_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text(obj, "");
                }
                {
                    // bl_angle
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_angle = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_angle_label(obj);
                    lv_label_set_text(obj, "");
                }
                {
                    // bl_axes
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.bl_axes = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_axis_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // bl_axis_x
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.bl_axis_x = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_axis_chip_x(obj);
                            lv_label_set_text_static(obj, "X");
                        }
                        {
                            // bl_axis_y
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.bl_axis_y = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_axis_chip_y(obj);
                            lv_label_set_text_static(obj, "Y");
                        }
                        {
                            // bl_axis_z
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.bl_axis_z = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_axis_chip_z(obj);
                            lv_label_set_text_static(obj, "Z");
                        }
                    }
                }
            }
        }
        {
            // bl_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.bl_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // bl_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Grab");
                }
                {
                    // bl_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "G");
                }
            }
        }
        {
            // bl_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.bl_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // bl_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Rotate");
                }
                {
                    // bl_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "R");
                }
            }
        }
        {
            // bl_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.bl_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // bl_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Scale");
                }
                {
                    // bl_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "S");
                }
            }
        }
        {
            // bl_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.bl_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // bl_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Edit Mode");
                }
                {
                    // bl_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Tab");
                }
            }
        }
        {
            // bl_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.bl_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // bl_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Frame Sel");
                }
                {
                    // bl_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Num .");
                }
            }
        }
        {
            // bl_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.bl_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // bl_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Camera");
                }
                {
                    // bl_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Num 0");
                }
            }
        }
        {
            // bl_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.bl_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // bl_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Undo");
                }
                {
                    // bl_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Z");
                }
            }
        }
        {
            // bl_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.bl_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // bl_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // bl_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Render");
                }
                {
                    // bl_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.bl_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "F12");
                }
            }
        }
    }
    
    tick_screen_page_blender();
}

void tick_screen_page_blender() {
    {
        const char *new_val = get_var_axis_mode_text();
        const char *cur_val = lv_label_get_text(objects.bl_mode);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.bl_mode;
            lv_label_set_text(objects.bl_mode, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_angle_text();
        const char *cur_val = lv_label_get_text(objects.bl_angle);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.bl_angle;
            lv_label_set_text(objects.bl_angle, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_kdenlive() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_kdenlive = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // kd_center
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_event_cb(obj, action_toggle_step, LV_EVENT_CLICKED, (void *)0);
            add_style_center_tap(obj);
            lv_obj_set_style_min_width(obj, 172, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 172, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 172, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 172, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.kd_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_kdenlive);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // kd_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text_static(obj, "Push to Cut");
                }
                {
                    // kd_timecode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_timecode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_timecode(obj);
                    lv_label_set_text(obj, "");
                }
                {
                    // kd_strip
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.kd_strip = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 126, 16);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_scrub_strip(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // kd_playhead
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.kd_playhead = obj;
                            lv_obj_set_pos(obj, 61, 0);
                            lv_obj_set_size(obj, 2, 14);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_playhead(obj);
                        }
                    }
                }
                {
                    // kd_step_pill
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.kd_step_pill = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_pill(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // kd_step
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.kd_step = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_pill_text(obj);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
            }
        }
        {
            // kd_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // kd_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Play");
                }
                {
                    // kd_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Space");
                }
            }
        }
        {
            // kd_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // kd_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Set In");
                }
                {
                    // kd_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "I");
                }
            }
        }
        {
            // kd_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // kd_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Set Out");
                }
                {
                    // kd_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "O");
                }
            }
        }
        {
            // kd_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // kd_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Select");
                }
                {
                    // kd_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "S");
                }
            }
        }
        {
            // kd_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // kd_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Razor");
                }
                {
                    // kd_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "X");
                }
            }
        }
        {
            // kd_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // kd_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Spacer");
                }
                {
                    // kd_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "M");
                }
            }
        }
        {
            // kd_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // kd_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Undo");
                }
                {
                    // kd_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Z");
                }
            }
        }
        {
            // kd_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.kd_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // kd_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // kd_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Render");
                }
                {
                    // kd_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.kd_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Ret");
                }
            }
        }
    }
    
    tick_screen_page_kdenlive();
}

void tick_screen_page_kdenlive() {
    {
        const char *new_val = get_var_timecode_text();
        const char *cur_val = lv_label_get_text(objects.kd_timecode);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.kd_timecode;
            lv_label_set_text(objects.kd_timecode, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = get_var_step_text();
        const char *cur_val = lv_label_get_text(objects.kd_step);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.kd_step;
            lv_label_set_text(objects.kd_step, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_vscodium() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_vscodium = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // vs_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.vs_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_center_panel_scroll(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.vs_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_vscodium);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // vs_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text_static(obj, "Push for Page");
                }
                {
                    // vs_scroll_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_scroll_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_idle_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_step_pill
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.vs_step_pill = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_pill(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // vs_step
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.vs_step = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_pill_text(obj);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
            }
        }
        {
            // vs_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.vs_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Palette");
                }
                {
                    // vs_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "F1");
                }
            }
        }
        {
            // vs_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.vs_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Open");
                }
                {
                    // vs_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+P");
                }
            }
        }
        {
            // vs_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.vs_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Find");
                }
                {
                    // vs_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+F");
                }
            }
        }
        {
            // vs_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.vs_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Go to Def");
                }
                {
                    // vs_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "F12");
                }
            }
        }
        {
            // vs_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.vs_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Rename");
                }
                {
                    // vs_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "F2");
                }
            }
        }
        {
            // vs_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.vs_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Comment");
                }
                {
                    // vs_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+/");
                }
            }
        }
        {
            // vs_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.vs_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Terminal");
                }
                {
                    // vs_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+`");
                }
            }
        }
        {
            // vs_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.vs_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // vs_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // vs_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Save");
                }
                {
                    // vs_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.vs_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+S");
                }
            }
        }
    }
    
    tick_screen_page_vscodium();
}

void tick_screen_page_vscodium() {
    {
        const char *new_val = get_var_scroll_step_text();
        const char *cur_val = lv_label_get_text(objects.vs_step);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.vs_step;
            lv_label_set_text(objects.vs_step, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_gimp() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_gimp = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // gp_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.gp_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_center_panel_zoom(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.gp_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_gimp);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // gp_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text_static(obj, "Push to Fit");
                }
                {
                    // gp_zoom_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_zoom_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_idle_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // gp_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.gp_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // gp_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Move");
                }
                {
                    // gp_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "M");
                }
            }
        }
        {
            // gp_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.gp_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // gp_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Rect Sel");
                }
                {
                    // gp_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "R");
                }
            }
        }
        {
            // gp_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.gp_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // gp_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Crop");
                }
                {
                    // gp_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Shift+C");
                }
            }
        }
        {
            // gp_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.gp_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // gp_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Brush");
                }
                {
                    // gp_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "P");
                }
            }
        }
        {
            // gp_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.gp_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // gp_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Eraser");
                }
                {
                    // gp_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Shift+E");
                }
            }
        }
        {
            // gp_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.gp_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // gp_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Text");
                }
                {
                    // gp_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "T");
                }
            }
        }
        {
            // gp_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.gp_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // gp_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Undo");
                }
                {
                    // gp_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Z");
                }
            }
        }
        {
            // gp_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.gp_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // gp_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // gp_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Redo");
                }
                {
                    // gp_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.gp_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Y");
                }
            }
        }
    }
    
    tick_screen_page_gimp();
}

void tick_screen_page_gimp() {
}

void create_screen_page_inkscape() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_inkscape = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ik_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ik_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_center_panel_zoom(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.ik_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_inkscape);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // ik_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text_static(obj, "Push to Fit");
                }
                {
                    // ik_zoom_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_zoom_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_idle_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // ik_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ik_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ik_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Selector");
                }
                {
                    // ik_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "S");
                }
            }
        }
        {
            // ik_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ik_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ik_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Node");
                }
                {
                    // ik_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "N");
                }
            }
        }
        {
            // ik_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ik_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ik_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Rect");
                }
                {
                    // ik_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "R");
                }
            }
        }
        {
            // ik_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ik_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ik_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Ellipse");
                }
                {
                    // ik_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "E");
                }
            }
        }
        {
            // ik_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ik_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ik_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Pen");
                }
                {
                    // ik_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "B");
                }
            }
        }
        {
            // ik_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ik_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ik_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Text");
                }
                {
                    // ik_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "T");
                }
            }
        }
        {
            // ik_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ik_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ik_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Undo");
                }
                {
                    // ik_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Z");
                }
            }
        }
        {
            // ik_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ik_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ik_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ik_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Redo");
                }
                {
                    // ik_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ik_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Y");
                }
            }
        }
    }
    
    tick_screen_page_inkscape();
}

void tick_screen_page_inkscape() {
}

void create_screen_page_libreoffice() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_libreoffice = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lo_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.lo_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_center_panel_scroll(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.lo_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_libreoffice);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // lo_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text_static(obj, "Push for Page");
                }
                {
                    // lo_scroll_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_scroll_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_idle_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_step_pill
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.lo_step_pill = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_pill(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lo_step
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lo_step = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_pill_text(obj);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
            }
        }
        {
            // lo_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.lo_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Bold");
                }
                {
                    // lo_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+B");
                }
            }
        }
        {
            // lo_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.lo_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Italic");
                }
                {
                    // lo_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+I");
                }
            }
        }
        {
            // lo_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.lo_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Underline");
                }
                {
                    // lo_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+U");
                }
            }
        }
        {
            // lo_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.lo_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Find");
                }
                {
                    // lo_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+H");
                }
            }
        }
        {
            // lo_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.lo_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Save");
                }
                {
                    // lo_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+S");
                }
            }
        }
        {
            // lo_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.lo_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Print");
                }
                {
                    // lo_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+P");
                }
            }
        }
        {
            // lo_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.lo_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Undo");
                }
                {
                    // lo_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Z");
                }
            }
        }
        {
            // lo_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.lo_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lo_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // lo_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Redo");
                }
                {
                    // lo_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lo_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+Y");
                }
            }
        }
    }
    
    tick_screen_page_libreoffice();
}

void tick_screen_page_libreoffice() {
    {
        const char *new_val = get_var_scroll_step_text();
        const char *cur_val = lv_label_get_text(objects.lo_step);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.lo_step;
            lv_label_set_text(objects.lo_step, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_firefox() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_firefox = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // ff_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.ff_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_center_panel_scroll(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.ff_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_firefox);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // ff_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text_static(obj, "Push for Page");
                }
                {
                    // ff_scroll_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_scroll_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_idle_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_step_pill
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.ff_step_pill = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_pill(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // ff_step
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.ff_step = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_pill_text(obj);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
            }
        }
        {
            // ff_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ff_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Back");
                }
                {
                    // ff_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Alt+");
                }
            }
        }
        {
            // ff_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ff_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Forward");
                }
                {
                    // ff_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Alt+");
                }
            }
        }
        {
            // ff_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ff_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Reload");
                }
                {
                    // ff_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "F5");
                }
            }
        }
        {
            // ff_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ff_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "New Tab");
                }
                {
                    // ff_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+T");
                }
            }
        }
        {
            // ff_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ff_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Close Tab");
                }
                {
                    // ff_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+W");
                }
            }
        }
        {
            // ff_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ff_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Find");
                }
                {
                    // ff_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+F");
                }
            }
        }
        {
            // ff_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ff_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Sidebar");
                }
                {
                    // ff_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+B");
                }
            }
        }
        {
            // ff_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.ff_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // ff_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // ff_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "History");
                }
                {
                    // ff_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.ff_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+H");
                }
            }
        }
    }
    
    tick_screen_page_firefox();
}

void tick_screen_page_firefox() {
    {
        const char *new_val = get_var_scroll_step_text();
        const char *cur_val = lv_label_get_text(objects.ff_step);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.ff_step;
            lv_label_set_text(objects.ff_step, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_page_chromium() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_chromium = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // cr_center
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.cr_center = obj;
            lv_obj_set_pos(obj, 94, 94);
            lv_obj_set_size(obj, 172, 172);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_center_panel_scroll(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_logo
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.cr_logo = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 40, 40);
                    lv_image_set_src(obj, &img_logo_chromium);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_SCROLLABLE);
                }
                {
                    // cr_mode
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_mode = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_mode_label(obj);
                    lv_label_set_text_static(obj, "Push for Page");
                }
                {
                    // cr_scroll_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_scroll_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_idle_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_step_pill
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.cr_step_pill = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                    add_style_pill(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // cr_step
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.cr_step = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_pill_text(obj);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
            }
        }
        {
            // cr_key0
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.cr_key0 = obj;
            lv_obj_set_pos(obj, 136, 4);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)0);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_key0_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key0_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_key0_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key0_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Back");
                }
                {
                    // cr_key0_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key0_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Alt+");
                }
            }
        }
        {
            // cr_key1
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.cr_key1 = obj;
            lv_obj_set_pos(obj, 229, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)1);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_key1_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key1_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_key1_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key1_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Forward");
                }
                {
                    // cr_key1_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key1_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Alt+");
                }
            }
        }
        {
            // cr_key2
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.cr_key2 = obj;
            lv_obj_set_pos(obj, 268, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)2);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_key2_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key2_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_key2_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key2_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Reload");
                }
                {
                    // cr_key2_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key2_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "F5");
                }
            }
        }
        {
            // cr_key3
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.cr_key3 = obj;
            lv_obj_set_pos(obj, 229, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)3);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_key3_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key3_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_key3_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key3_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "New Tab");
                }
                {
                    // cr_key3_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key3_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+T");
                }
            }
        }
        {
            // cr_key4
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.cr_key4 = obj;
            lv_obj_set_pos(obj, 136, 268);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)4);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_key4_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key4_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_key4_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key4_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Close Tab");
                }
                {
                    // cr_key4_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key4_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+W");
                }
            }
        }
        {
            // cr_key5
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.cr_key5 = obj;
            lv_obj_set_pos(obj, 43, 229);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)5);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_key5_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key5_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_key5_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key5_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Bookmark");
                }
                {
                    // cr_key5_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key5_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+D");
                }
            }
        }
        {
            // cr_key6
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.cr_key6 = obj;
            lv_obj_set_pos(obj, 4, 136);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)6);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_key6_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key6_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_key6_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key6_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "History");
                }
                {
                    // cr_key6_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key6_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+H");
                }
            }
        }
        {
            // cr_key7
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.cr_key7 = obj;
            lv_obj_set_pos(obj, 43, 43);
            lv_obj_set_size(obj, 88, 88);
            lv_obj_add_event_cb(obj, action_key_tap, LV_EVENT_CLICKED, (void *)7);
            add_style_key_button(obj);
            lv_obj_set_style_min_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 88, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // cr_key7_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key7_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_icon(obj);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // cr_key7_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key7_name = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_name(obj);
                    lv_label_set_text_static(obj, "Download");
                }
                {
                    // cr_key7_sc
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.cr_key7_sc = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_key_shortcut(obj);
                    lv_label_set_text_static(obj, "Ctrl+J");
                }
            }
        }
    }
    
    tick_screen_page_chromium();
}

void tick_screen_page_chromium() {
    {
        const char *new_val = get_var_scroll_step_text();
        const char *cur_val = lv_label_get_text(objects.cr_step);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.cr_step;
            lv_label_set_text(objects.cr_step, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_page_idle,
    tick_screen_page_freecad,
    tick_screen_page_blender,
    tick_screen_page_kdenlive,
    tick_screen_page_vscodium,
    tick_screen_page_gimp,
    tick_screen_page_inkscape,
    tick_screen_page_libreoffice,
    tick_screen_page_firefox,
    tick_screen_page_chromium,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 10) {
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
    { "fa24", &ui_font_fa24 },
    { "fa36", &ui_font_fa36 },
    { "mono10", &ui_font_mono10 },
    { "mono20", &ui_font_mono20 },
    { "num32", &ui_font_num32 },
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
    
    lv_style_set_bg_color(get_style_screen_root_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_text_color(get_style_screen_root_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_label_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_key_button_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_key_button_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_key_button_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_key_button_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_key_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_key_name_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_key_shortcut_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_center_tap_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_mode_label_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_angle_label_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_border_color(get_style_axis_chip_x_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_axis_chip_x_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_axis_chip_x_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_border_color(get_style_axis_chip_x_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_text_color(get_style_axis_chip_x_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][27]));
    lv_style_set_border_color(get_style_axis_chip_y_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_axis_chip_y_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_axis_chip_y_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_border_color(get_style_axis_chip_y_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_text_color(get_style_axis_chip_y_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][27]));
    lv_style_set_border_color(get_style_axis_chip_z_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_axis_chip_z_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_axis_chip_z_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_border_color(get_style_axis_chip_z_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_text_color(get_style_axis_chip_z_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][27]));
    lv_style_set_text_color(get_style_timecode_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_scrub_strip_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_scrub_strip_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_playhead_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_cut_mark_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_border_color(get_style_pill_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_pill_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_bg_color(get_style_status_dot_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_bg_color(get_style_status_dot_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_arc_color(get_style_clock_dial_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_line_color(get_style_clock_dial_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_clock_dial_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_line_color(get_style_clock_dial_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_line_color(get_style_clock_dial_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_clock_dial_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_line_color(get_style_hand_hour_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_line_color(get_style_hand_minute_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_line_color(get_style_hand_second_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_bg_color(get_style_clock_cap_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_border_color(get_style_clock_cap_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_text_color(get_style_idle_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_idle_title_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_idle_hint_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_obj_invalidate(objects.page_idle);
    lv_obj_invalidate(objects.page_freecad);
    lv_obj_invalidate(objects.page_blender);
    lv_obj_invalidate(objects.page_kdenlive);
    lv_obj_invalidate(objects.page_vscodium);
    lv_obj_invalidate(objects.page_gimp);
    lv_obj_invalidate(objects.page_inkscape);
    lv_obj_invalidate(objects.page_libreoffice);
    lv_obj_invalidate(objects.page_firefox);
    lv_obj_invalidate(objects.page_chromium);
}
uint32_t theme_colors[2][32] = {
    { 0xffe8e8e8, 0xffe4e4e4, 0xffffffff, 0xffededed, 0xffffffff, 0xffc8c8c8, 0xff1a1a1a, 0xff4a4a4a, 0xff696969, 0xff52a441, 0xffcbe3c6, 0xffffc107, 0xfffff4d1, 0xff48e6fe, 0xffff5453, 0xff74fe00, 0xff505050, 0xff0088cc, 0xff48e6fe, 0xff777777, 0xffb5b5b5, 0xff333333, 0xff666666, 0xff9e9e9e, 0xff424242, 0xffe5e5e5, 0xffffffff, 0xff000000, 0xff3d7b31, 0xff7d5800, 0xff00697d, 0xffb3261e },
    { 0xff0a0a0a, 0xff000000, 0xff1a1a1a, 0xff252525, 0xff0a0a0a, 0xff333333, 0xffffffff, 0xffaaaaaa, 0xff6f6f6f, 0xff7bc96a, 0xff2e4a2a, 0xffffc107, 0xff3a2f00, 0xff48e6fe, 0xffff5453, 0xff74fe00, 0xffbdbdbd, 0xff0088cc, 0xff48e6fe, 0xff777777, 0xffb5b5b5, 0xff333333, 0xff666666, 0xff9e9e9e, 0xff424242, 0xff1e1e1e, 0xffffffff, 0xff000000, 0xff7bc96a, 0xffffc107, 0xff48e6fe, 0xffff5453 },
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
    create_screen_page_idle();
    create_screen_page_freecad();
    create_screen_page_blender();
    create_screen_page_kdenlive();
    create_screen_page_vscodium();
    create_screen_page_gimp();
    create_screen_page_inkscape();
    create_screen_page_libreoffice();
    create_screen_page_firefox();
    create_screen_page_chromium();
}