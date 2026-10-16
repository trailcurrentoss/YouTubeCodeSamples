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

void create_screen_page_title() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_title = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    lv_obj_add_event_cb(obj, action_tap, LV_EVENT_SHORT_CLICKED, (void *)0);
    lv_obj_add_event_cb(obj, action_menu, LV_EVENT_LONG_PRESSED, (void *)0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // title_ring
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.title_ring = obj;
            lv_obj_set_pos(obj, 3, 3);
            lv_obj_set_size(obj, 354, 354);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_ring_thin(obj);
        }
        {
            // title_bolts
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.title_bolts = obj;
            lv_obj_set_pos(obj, 10, 10);
            lv_obj_set_size(obj, 340, 340);
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_scale_set_range(obj, 0, 11);
            lv_scale_set_angle_range(obj, 330);
            lv_scale_set_rotation(obj, 270);
            lv_scale_set_total_tick_count(obj, 12);
            lv_scale_set_major_tick_every(obj, 1);
            lv_scale_set_label_show(obj, false);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_title_bolts(obj);
        }
        {
            // title_bolt_slots
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.title_bolt_slots = obj;
            lv_obj_set_pos(obj, 16, 16);
            lv_obj_set_size(obj, 328, 328);
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_scale_set_range(obj, 0, 11);
            lv_scale_set_angle_range(obj, 330);
            lv_scale_set_rotation(obj, 270);
            lv_scale_set_total_tick_count(obj, 12);
            lv_scale_set_major_tick_every(obj, 1);
            lv_scale_set_label_show(obj, false);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_title_bolt_slots(obj);
        }
        {
            // title_ring_wide
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.title_ring_wide = obj;
            lv_obj_set_pos(obj, 40, 40);
            lv_obj_set_size(obj, 280, 280);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_ring_wide(obj);
        }
        {
            // title_ticks
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.title_ticks = obj;
            lv_obj_set_pos(obj, 62, 62);
            lv_obj_set_size(obj, 236, 236);
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_scale_set_range(obj, 0, 59);
            lv_scale_set_angle_range(obj, 354);
            lv_scale_set_rotation(obj, 270);
            lv_scale_set_total_tick_count(obj, 60);
            lv_scale_set_major_tick_every(obj, 5);
            lv_scale_set_label_show(obj, false);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_title_ticks(obj);
        }
        {
            // title_lock
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.title_lock = obj;
            lv_obj_set_pos(obj, 140, 108);
            lv_obj_set_size(obj, 80, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_title_lock(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // title_text
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.title_text = obj;
            lv_obj_set_pos(obj, 50, 163);
            lv_obj_set_size(obj, 260, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_title_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Crack the Safe");
        }
        {
            // title_sub
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.title_sub = obj;
            lv_obj_set_pos(obj, 70, 199);
            lv_obj_set_size(obj, 220, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_title_sub(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "3-Number Combination");
        }
        {
            // title_hint_l
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.title_hint_l = obj;
            lv_obj_set_pos(obj, 108, 235);
            lv_obj_set_size(obj, 20, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_accent_hint_icon(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // title_hint
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.title_hint = obj;
            lv_obj_set_pos(obj, 130, 235);
            lv_obj_set_size(obj, 100, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_accent_hint(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Turn to Start");
        }
        {
            // title_hint_r
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.title_hint_r = obj;
            lv_obj_set_pos(obj, 232, 235);
            lv_obj_set_size(obj, 20, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_accent_hint_icon(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
    }
    
    tick_screen_page_title();
}

void tick_screen_page_title() {
}

void create_screen_page_instructions() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_instructions = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    lv_obj_add_event_cb(obj, action_menu, LV_EVENT_LONG_PRESSED, (void *)0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // instr_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.instr_title = obj;
            lv_obj_set_pos(obj, 80, 50);
            lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_title_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "How to Play");
        }
        {
            // instr_body
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.instr_body = obj;
            lv_obj_set_pos(obj, 70, 94);
            lv_obj_set_size(obj, 220, LV_SIZE_CONTENT);
            add_style_instructions_body(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Use the dial to crack the safe before time runs out.\n\nHeadphones let you hear the pins click, but real safecrackers work by feel.");
        }
        {
            // start_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.start_button = obj;
            lv_obj_set_pos(obj, 110, 212);
            lv_obj_set_size(obj, 140, 48);
            lv_obj_add_event_cb(obj, action_start, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_start_button(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // start_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.start_text = obj;
                    lv_obj_set_pos(obj, 0, 9);
                    lv_obj_set_size(obj, 140, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_start_button_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Start");
                }
            }
        }
        {
            // hp_button
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.hp_button = obj;
            lv_obj_set_pos(obj, 105, 270);
            lv_obj_set_size(obj, 150, 40);
            lv_obj_add_event_cb(obj, action_headphones, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_button_outline(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // hp_button_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hp_button_icon = obj;
                    lv_obj_set_pos(obj, 16, 11);
                    lv_obj_set_size(obj, 18, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_outline_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // hp_button_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hp_button_text = obj;
                    lv_obj_set_pos(obj, 40, 11);
                    lv_obj_set_size(obj, 93, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_outline_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Headphones");
                }
            }
        }
    }
    
    tick_screen_page_instructions();
}

void tick_screen_page_instructions() {
}

void create_screen_page_menu() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_menu = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // menu_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.menu_title = obj;
            lv_obj_set_pos(obj, 80, 78);
            lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_title_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Menu");
        }
        {
            // menu_new
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.menu_new = obj;
            lv_obj_set_pos(obj, 90, 126);
            lv_obj_set_size(obj, 180, 48);
            lv_obj_add_event_cb(obj, action_menu_new_game, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_start_button(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_new_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_new_text = obj;
                    lv_obj_set_pos(obj, 0, 9);
                    lv_obj_set_size(obj, 180, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_start_button_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "New Game");
                }
            }
        }
        {
            // menu_settings
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.menu_settings = obj;
            lv_obj_set_pos(obj, 90, 184);
            lv_obj_set_size(obj, 180, 44);
            lv_obj_add_event_cb(obj, action_menu_settings, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_button_outline(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_settings_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_settings_text = obj;
                    lv_obj_set_pos(obj, 0, 13);
                    lv_obj_set_size(obj, 180, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_outline_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Settings");
                }
            }
        }
        {
            // menu_back
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.menu_back = obj;
            lv_obj_set_pos(obj, 90, 238);
            lv_obj_set_size(obj, 180, 44);
            lv_obj_add_event_cb(obj, action_menu_back, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_button_outline(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // menu_back_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.menu_back_text = obj;
                    lv_obj_set_pos(obj, 0, 13);
                    lv_obj_set_size(obj, 180, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_outline_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
    }
    
    tick_screen_page_menu();
}

void tick_screen_page_menu() {
}

void create_screen_page_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // settings_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.settings_title = obj;
            lv_obj_set_pos(obj, 80, 73);
            lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_title_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Settings");
        }
        {
            // settings_hp
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.settings_hp = obj;
            lv_obj_set_pos(obj, 50, 121);
            lv_obj_set_size(obj, 260, 44);
            lv_obj_add_event_cb(obj, action_settings_headphones, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_device_row(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // settings_hp_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_hp_icon = obj;
                    lv_obj_set_pos(obj, 14, 13);
                    lv_obj_set_size(obj, 18, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_outline_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // settings_hp_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_hp_text = obj;
                    lv_obj_set_pos(obj, 40, 13);
                    lv_obj_set_size(obj, 93, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Headphones");
                }
                {
                    // settings_hp_value
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_hp_value = obj;
                    lv_obj_set_pos(obj, 140, 13);
                    lv_obj_set_size(obj, 106, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_setting_value(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Not connected");
                }
            }
        }
        {
            // settings_vol
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.settings_vol = obj;
            lv_obj_set_pos(obj, 50, 175);
            lv_obj_set_size(obj, 260, 44);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_setting_row(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // settings_vol_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_vol_icon = obj;
                    lv_obj_set_pos(obj, 14, 13);
                    lv_obj_set_size(obj, 20, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_outline_icon(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // settings_vol_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_vol_text = obj;
                    lv_obj_set_pos(obj, 40, 13);
                    lv_obj_set_size(obj, 60, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Volume");
                }
                {
                    // settings_vol_down
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.settings_vol_down = obj;
                    lv_obj_set_pos(obj, 128, 2);
                    lv_obj_set_size(obj, 40, 40);
                    lv_obj_add_event_cb(obj, action_settings_volume_down, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_step_button(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // settings_vol_down_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_vol_down_icon = obj;
                            lv_obj_set_pos(obj, 0, 11);
                            lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_step_icon(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // settings_vol_value
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_vol_value = obj;
                    lv_obj_set_pos(obj, 170, 13);
                    lv_obj_set_size(obj, 36, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_setting_value(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // settings_vol_up
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.settings_vol_up = obj;
                    lv_obj_set_pos(obj, 208, 2);
                    lv_obj_set_size(obj, 40, 40);
                    lv_obj_add_event_cb(obj, action_settings_volume_up, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_step_button(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // settings_vol_up_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.settings_vol_up_icon = obj;
                            lv_obj_set_pos(obj, 0, 11);
                            lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            add_style_step_icon(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
            }
        }
        {
            // settings_back
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.settings_back = obj;
            lv_obj_set_pos(obj, 130, 243);
            lv_obj_set_size(obj, 100, 44);
            lv_obj_add_event_cb(obj, action_settings_back, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_button_outline(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // settings_back_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.settings_back_text = obj;
                    lv_obj_set_pos(obj, 0, 13);
                    lv_obj_set_size(obj, 100, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_outline_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
    }
    
    tick_screen_page_settings();
}

void tick_screen_page_settings() {
}

void create_screen_page_headphones() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_headphones = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // hp_title
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.hp_title = obj;
            lv_obj_set_pos(obj, 80, 44);
            lv_obj_set_size(obj, 200, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_title_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Headphones");
        }
        {
            // hp_status
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.hp_status = obj;
            lv_obj_set_pos(obj, 60, 80);
            lv_obj_set_size(obj, 240, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
            add_style_hint_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Put headphones in pairing mode");
        }
        {
            // hp_list
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.hp_list = obj;
            lv_obj_set_pos(obj, 50, 107);
            lv_obj_set_size(obj, 260, 152);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_device_list(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // hp_row0
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_row0 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 260, 34);
                    lv_obj_add_event_cb(obj, action_headphones_select, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_device_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_row0_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_row0_text = obj;
                            lv_obj_set_pos(obj, 14, 8);
                            lv_obj_set_size(obj, 192, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_device_row_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // hp_row0_info
                            lv_obj_t *obj = lv_button_create(parent_obj);
                            objects.hp_row0_info = obj;
                            lv_obj_set_pos(obj, 220, 0);
                            lv_obj_set_size(obj, 40, 34);
                            lv_obj_add_event_cb(obj, action_headphones_info, LV_EVENT_CLICKED, (void *)0);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                            add_style_info_button(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // hp_row0_info_icon
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.hp_row0_info_icon = obj;
                                    lv_obj_set_pos(obj, 10, 8);
                                    lv_obj_set_size(obj, 20, LV_SIZE_CONTENT);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    add_style_info_icon(obj);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                    }
                }
                {
                    // hp_row1
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_row1 = obj;
                    lv_obj_set_pos(obj, 0, 38);
                    lv_obj_set_size(obj, 260, 34);
                    lv_obj_add_event_cb(obj, action_headphones_select, LV_EVENT_CLICKED, (void *)1);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_device_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_row1_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_row1_text = obj;
                            lv_obj_set_pos(obj, 14, 8);
                            lv_obj_set_size(obj, 232, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_device_row_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // hp_row2
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_row2 = obj;
                    lv_obj_set_pos(obj, 0, 76);
                    lv_obj_set_size(obj, 260, 34);
                    lv_obj_add_event_cb(obj, action_headphones_select, LV_EVENT_CLICKED, (void *)2);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_device_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_row2_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_row2_text = obj;
                            lv_obj_set_pos(obj, 14, 8);
                            lv_obj_set_size(obj, 232, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_device_row_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // hp_row3
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_row3 = obj;
                    lv_obj_set_pos(obj, 0, 114);
                    lv_obj_set_size(obj, 260, 34);
                    lv_obj_add_event_cb(obj, action_headphones_select, LV_EVENT_CLICKED, (void *)3);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_device_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_row3_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_row3_text = obj;
                            lv_obj_set_pos(obj, 14, 8);
                            lv_obj_set_size(obj, 232, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_device_row_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // hp_row4
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_row4 = obj;
                    lv_obj_set_pos(obj, 0, 152);
                    lv_obj_set_size(obj, 260, 34);
                    lv_obj_add_event_cb(obj, action_headphones_select, LV_EVENT_CLICKED, (void *)4);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_device_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_row4_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_row4_text = obj;
                            lv_obj_set_pos(obj, 14, 8);
                            lv_obj_set_size(obj, 232, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_device_row_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // hp_row5
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_row5 = obj;
                    lv_obj_set_pos(obj, 0, 190);
                    lv_obj_set_size(obj, 260, 34);
                    lv_obj_add_event_cb(obj, action_headphones_select, LV_EVENT_CLICKED, (void *)5);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_device_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_row5_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_row5_text = obj;
                            lv_obj_set_pos(obj, 14, 8);
                            lv_obj_set_size(obj, 232, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_device_row_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // hp_row6
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_row6 = obj;
                    lv_obj_set_pos(obj, 0, 228);
                    lv_obj_set_size(obj, 260, 34);
                    lv_obj_add_event_cb(obj, action_headphones_select, LV_EVENT_CLICKED, (void *)6);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_device_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_row6_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_row6_text = obj;
                            lv_obj_set_pos(obj, 14, 8);
                            lv_obj_set_size(obj, 232, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_device_row_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // hp_row7
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_row7 = obj;
                    lv_obj_set_pos(obj, 0, 266);
                    lv_obj_set_size(obj, 260, 34);
                    lv_obj_add_event_cb(obj, action_headphones_select, LV_EVENT_CLICKED, (void *)7);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_device_row(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_row7_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_row7_text = obj;
                            lv_obj_set_pos(obj, 14, 8);
                            lv_obj_set_size(obj, 232, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_device_row_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
            }
        }
        {
            // hp_search
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.hp_search = obj;
            lv_obj_set_pos(obj, 75, 271);
            lv_obj_set_size(obj, 100, 44);
            lv_obj_add_event_cb(obj, action_headphones_search, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_button_outline(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // hp_search_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hp_search_text = obj;
                    lv_obj_set_pos(obj, 0, 13);
                    lv_obj_set_size(obj, 100, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_outline_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Search");
                }
            }
        }
        {
            // hp_done
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.hp_done = obj;
            lv_obj_set_pos(obj, 185, 271);
            lv_obj_set_size(obj, 100, 44);
            lv_obj_add_event_cb(obj, action_headphones_done, LV_EVENT_CLICKED, (void *)0);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            add_style_start_button(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // hp_done_text
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hp_done_text = obj;
                    lv_obj_set_pos(obj, 0, 13);
                    lv_obj_set_size(obj, 100, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Done");
                }
            }
        }
        {
            // hp_info
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.hp_info = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            add_style_scrim(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // hp_info_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.hp_info_name = obj;
                    lv_obj_set_pos(obj, 60, 112);
                    lv_obj_set_size(obj, 240, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_button_text(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // hp_forget
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_forget = obj;
                    lv_obj_set_pos(obj, 100, 149);
                    lv_obj_set_size(obj, 160, 44);
                    lv_obj_add_event_cb(obj, action_headphones_forget, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_button_danger(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_forget_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_forget_text = obj;
                            lv_obj_set_pos(obj, 0, 13);
                            lv_obj_set_size(obj, 160, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_button_danger_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Forget Device");
                        }
                    }
                }
                {
                    // hp_info_cancel
                    lv_obj_t *obj = lv_button_create(parent_obj);
                    objects.hp_info_cancel = obj;
                    lv_obj_set_pos(obj, 100, 203);
                    lv_obj_set_size(obj, 160, 44);
                    lv_obj_add_event_cb(obj, action_headphones_info_close, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    add_style_button_outline(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // hp_info_cancel_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.hp_info_cancel_text = obj;
                            lv_obj_set_pos(obj, 0, 13);
                            lv_obj_set_size(obj, 160, LV_SIZE_CONTENT);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            add_style_button_outline_text(obj);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Cancel");
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_page_headphones();
}

void tick_screen_page_headphones() {
}

void create_screen_page_play() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_play = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    lv_obj_add_event_cb(obj, action_menu, LV_EVENT_LONG_PRESSED, (void *)0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // seg1
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.seg1 = obj;
            lv_obj_set_pos(obj, 6, 6);
            lv_obj_set_size(obj, 348, 348);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 4);
            lv_arc_set_bg_end_angle(obj, 116);
            lv_arc_set_rotation(obj, 270);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_seg_arc(obj);
        }
        {
            // seg1_cur
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.seg1_cur = obj;
            lv_obj_set_pos(obj, 6, 6);
            lv_obj_set_size(obj, 348, 348);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 4);
            lv_arc_set_bg_end_angle(obj, 116);
            lv_arc_set_rotation(obj, 270);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_seg_arc_current(obj);
        }
        {
            // seg2
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.seg2 = obj;
            lv_obj_set_pos(obj, 6, 6);
            lv_obj_set_size(obj, 348, 348);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 124);
            lv_arc_set_bg_end_angle(obj, 236);
            lv_arc_set_rotation(obj, 270);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_seg_arc(obj);
        }
        {
            // seg2_cur
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.seg2_cur = obj;
            lv_obj_set_pos(obj, 6, 6);
            lv_obj_set_size(obj, 348, 348);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 124);
            lv_arc_set_bg_end_angle(obj, 236);
            lv_arc_set_rotation(obj, 270);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_seg_arc_current(obj);
        }
        {
            // seg3
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.seg3 = obj;
            lv_obj_set_pos(obj, 6, 6);
            lv_obj_set_size(obj, 348, 348);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 244);
            lv_arc_set_bg_end_angle(obj, 356);
            lv_arc_set_rotation(obj, 270);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_seg_arc(obj);
        }
        {
            // seg3_cur
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.seg3_cur = obj;
            lv_obj_set_pos(obj, 6, 6);
            lv_obj_set_size(obj, 348, 348);
            lv_arc_set_value(obj, 100);
            lv_arc_set_bg_start_angle(obj, 244);
            lv_arc_set_bg_end_angle(obj, 356);
            lv_arc_set_rotation(obj, 270);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            add_style_seg_arc_current(obj);
        }
        {
            // dial_ticks
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.dial_ticks = obj;
            lv_obj_set_pos(obj, 15, 15);
            lv_obj_set_size(obj, 330, 330);
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_scale_set_range(obj, 0, 39);
            lv_scale_set_angle_range(obj, 351);
            lv_scale_set_rotation(obj, 270);
            lv_scale_set_total_tick_count(obj, 40);
            lv_scale_set_major_tick_every(obj, 5);
            lv_scale_set_label_show(obj, true);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_dial_ticks(obj);
        }
        {
            // dial_disk
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.dial_disk = obj;
            lv_obj_set_pos(obj, 74, 74);
            lv_obj_set_size(obj, 212, 212);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_dial_disk(obj);
        }
        {
            // dial_grip
            lv_obj_t *obj = lv_scale_create(parent_obj);
            objects.dial_grip = obj;
            lv_obj_set_pos(obj, 78, 78);
            lv_obj_set_size(obj, 204, 204);
            lv_scale_set_mode(obj, LV_SCALE_MODE_ROUND_INNER);
            lv_scale_set_range(obj, 0, 35);
            lv_scale_set_angle_range(obj, 350);
            lv_scale_set_rotation(obj, 270);
            lv_scale_set_total_tick_count(obj, 36);
            lv_scale_set_major_tick_every(obj, 1);
            lv_scale_set_label_show(obj, false);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_dial_grip(obj);
        }
        {
            // dial_inner
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.dial_inner = obj;
            lv_obj_set_pos(obj, 90, 90);
            lv_obj_set_size(obj, 180, 180);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_ring_hair(obj);
        }
        {
            // dial_index
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.dial_index = obj;
            lv_obj_set_pos(obj, 165, 0);
            lv_obj_set_size(obj, 30, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_dial_index(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // hint_arrow_cw
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.hint_arrow_cw = obj;
            lv_obj_set_pos(obj, 122, 138);
            lv_obj_set_size(obj, 20, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_hint_icon(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // hint_arrow_ccw
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.hint_arrow_ccw = obj;
            lv_obj_set_pos(obj, 122, 138);
            lv_obj_set_size(obj, 20, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_hint_icon(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // hint_text
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.hint_text = obj;
            lv_obj_set_pos(obj, 110, 138);
            lv_obj_set_size(obj, 140, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_hint_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Turn Right");
        }
        {
            // level_text
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.level_text = obj;
            lv_obj_set_pos(obj, 120, 115);
            lv_obj_set_size(obj, 120, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_level_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Level 1");
        }
        {
            // battery_icon
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.battery_icon = obj;
            lv_obj_set_pos(obj, 160, 96);
            lv_obj_set_size(obj, 40, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_battery_icon(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // timer_text
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.timer_text = obj;
            lv_obj_set_pos(obj, 100, 163);
            lv_obj_set_size(obj, 160, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_timer_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "5:00");
        }
        {
            // slot1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.slot1 = obj;
            lv_obj_set_pos(obj, 125, 223);
            lv_obj_set_size(obj, 30, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_slot_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "--");
        }
        {
            // slot1_line
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.slot1_line = obj;
            lv_obj_set_pos(obj, 125, 243);
            lv_obj_set_size(obj, 30, 2);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_slot_line(obj);
        }
        {
            // slot1_cur
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.slot1_cur = obj;
            lv_obj_set_pos(obj, 125, 243);
            lv_obj_set_size(obj, 30, 2);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_slot_line_current(obj);
        }
        {
            // slot2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.slot2 = obj;
            lv_obj_set_pos(obj, 165, 223);
            lv_obj_set_size(obj, 30, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_slot_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "--");
        }
        {
            // slot2_line
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.slot2_line = obj;
            lv_obj_set_pos(obj, 165, 243);
            lv_obj_set_size(obj, 30, 2);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_slot_line(obj);
        }
        {
            // slot2_cur
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.slot2_cur = obj;
            lv_obj_set_pos(obj, 165, 243);
            lv_obj_set_size(obj, 30, 2);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_slot_line_current(obj);
        }
        {
            // slot3
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.slot3 = obj;
            lv_obj_set_pos(obj, 205, 223);
            lv_obj_set_size(obj, 30, LV_SIZE_CONTENT);
            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
            add_style_slot_text(obj);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "--");
        }
        {
            // slot3_line
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.slot3_line = obj;
            lv_obj_set_pos(obj, 205, 243);
            lv_obj_set_size(obj, 30, 2);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_slot_line(obj);
        }
        {
            // slot3_cur
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.slot3_cur = obj;
            lv_obj_set_pos(obj, 205, 243);
            lv_obj_set_size(obj, 30, 2);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_slot_line_current(obj);
        }
        {
            // police_red
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.police_red = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_police_red);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_police_light(obj);
        }
        {
            // police_blue
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.police_blue = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_police_blue);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_police_light(obj);
        }
    }
    
    tick_screen_page_play();
}

void tick_screen_page_play() {
}

void create_screen_page_unlocked() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_unlocked = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 360, 360);
    lv_obj_add_event_cb(obj, action_tap, LV_EVENT_SHORT_CLICKED, (void *)0);
    lv_obj_add_event_cb(obj, action_menu, LV_EVENT_LONG_PRESSED, (void *)0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_root(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // prize_1
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.prize_1 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_prize_1);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // prize_2
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.prize_2 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_prize_2);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // prize_3
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.prize_3 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_prize_3);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // prize_4
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.prize_4 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_prize_4);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // prize_5
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.prize_5 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_prize_5);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // prize_6
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.prize_6 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_prize_6);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // prize_7
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.prize_7 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_prize_7);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // prize_8
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.prize_8 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 360, 360);
            lv_image_set_src(obj, &img_prize_8);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // gold_glow
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.gold_glow = obj;
            lv_obj_set_pos(obj, 30, 50);
            lv_obj_set_size(obj, 300, 240);
            lv_image_set_src(obj, &img_gold_glow);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_gold_glow(obj);
        }
        {
            // door_00
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_00 = obj;
            lv_obj_set_pos(obj, 18, 18);
            lv_obj_set_size(obj, 324, 324);
            lv_image_set_src(obj, &img_door_00);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_01
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_01 = obj;
            lv_obj_set_pos(obj, 14, 16);
            lv_obj_set_size(obj, 328, 327);
            lv_image_set_src(obj, &img_door_01);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_02
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_02 = obj;
            lv_obj_set_pos(obj, 12, 16);
            lv_obj_set_size(obj, 330, 328);
            lv_image_set_src(obj, &img_door_02);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_03
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_03 = obj;
            lv_obj_set_pos(obj, 8, 14);
            lv_obj_set_size(obj, 334, 332);
            lv_image_set_src(obj, &img_door_03);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_04
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_04 = obj;
            lv_obj_set_pos(obj, 8, 10);
            lv_obj_set_size(obj, 334, 339);
            lv_image_set_src(obj, &img_door_04);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_05
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_05 = obj;
            lv_obj_set_pos(obj, 10, 6);
            lv_obj_set_size(obj, 332, 347);
            lv_image_set_src(obj, &img_door_05);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_06
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_06 = obj;
            lv_obj_set_pos(obj, 20, 4);
            lv_obj_set_size(obj, 322, 352);
            lv_image_set_src(obj, &img_door_06);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_07
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_07 = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, 304, 360);
            lv_image_set_src(obj, &img_door_07);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_08
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_08 = obj;
            lv_obj_set_pos(obj, 68, 0);
            lv_obj_set_size(obj, 274, 360);
            lv_image_set_src(obj, &img_door_08);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_09
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_09 = obj;
            lv_obj_set_pos(obj, 104, 0);
            lv_obj_set_size(obj, 238, 360);
            lv_image_set_src(obj, &img_door_09);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_10
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_10 = obj;
            lv_obj_set_pos(obj, 146, 0);
            lv_obj_set_size(obj, 196, 360);
            lv_image_set_src(obj, &img_door_10);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_11
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_11 = obj;
            lv_obj_set_pos(obj, 190, 0);
            lv_obj_set_size(obj, 152, 360);
            lv_image_set_src(obj, &img_door_11);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_12
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_12 = obj;
            lv_obj_set_pos(obj, 230, 0);
            lv_obj_set_size(obj, 112, 360);
            lv_image_set_src(obj, &img_door_12);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_13
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_13 = obj;
            lv_obj_set_pos(obj, 260, 0);
            lv_obj_set_size(obj, 82, 360);
            lv_image_set_src(obj, &img_door_13);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_14
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_14 = obj;
            lv_obj_set_pos(obj, 280, 0);
            lv_obj_set_size(obj, 62, 360);
            lv_image_set_src(obj, &img_door_14);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // door_15
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.door_15 = obj;
            lv_obj_set_pos(obj, 286, 0);
            lv_obj_set_size(obj, 56, 360);
            lv_image_set_src(obj, &img_door_15);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
        {
            // reward_pill
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.reward_pill = obj;
            lv_obj_set_pos(obj, 62, 206);
            lv_obj_set_size(obj, 236, 104);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_reward_pill(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // reward_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.reward_title = obj;
                    lv_obj_set_pos(obj, 0, 6);
                    lv_obj_set_size(obj, 234, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_reward_title(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Unlocked");
                }
                {
                    // reward_code
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.reward_code = obj;
                    lv_obj_set_pos(obj, 0, 40);
                    lv_obj_set_size(obj, 234, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_reward_code(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "12 · 31 · 07");
                }
                {
                    // reward_meta
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.reward_meta = obj;
                    lv_obj_set_pos(obj, 0, 61);
                    lv_obj_set_size(obj, 234, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    add_style_reward_meta(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Level 1 · 3:12 left");
                }
                {
                    // reward_again
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.reward_again = obj;
                    lv_obj_set_pos(obj, 0, 81);
                    lv_obj_set_size(obj, 234, LV_SIZE_CONTENT);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    add_style_accent_hint(obj);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Turn for Level 2");
                }
            }
        }
    }
    
    tick_screen_page_unlocked();
}

void tick_screen_page_unlocked() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_page_title,
    tick_screen_page_instructions,
    tick_screen_page_menu,
    tick_screen_page_settings,
    tick_screen_page_headphones,
    tick_screen_page_play,
    tick_screen_page_unlocked,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 7) {
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
    { "rn64", &ui_font_rn64 },
    { "rm28", &ui_font_rm28 },
    { "rm15", &ui_font_rm15 },
    { "rm14", &ui_font_rm14 },
    { "rr13", &ui_font_rr13 },
    { "fa40", &ui_font_fa40 },
    { "fa26", &ui_font_fa26 },
    { "fa14", &ui_font_fa14 },
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
    
    lv_style_set_bg_color(get_style_screen_root_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_text_color(get_style_screen_root_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_label_body_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_ring_thin_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_border_color(get_style_ring_wide_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    lv_style_set_border_color(get_style_ring_hair_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_line_color(get_style_title_bolts_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_line_color(get_style_title_bolt_slots_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_line_color(get_style_title_ticks_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_line_color(get_style_title_ticks_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_text_color(get_style_title_lock_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_text_color(get_style_title_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_title_sub_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_accent_hint_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_accent_hint_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_instructions_body_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_start_button_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_bg_color(get_style_start_button_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_text_color(get_style_start_button_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_button_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_button_outline_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_bg_color(get_style_button_outline_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_border_color(get_style_button_outline_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_text_color(get_style_button_outline_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_button_outline_text_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_outline_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_outline_icon_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_bg_color(get_style_device_row_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_bg_color(get_style_device_row_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_text_color(get_style_device_row_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_device_row_text_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_bg_color(get_style_info_button_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_text_color(get_style_info_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_scrim_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_border_color(get_style_button_danger_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_bg_color(get_style_button_danger_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_text_color(get_style_button_danger_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_bg_color(get_style_setting_row_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_bg_color(get_style_step_button_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_bg_color(get_style_step_button_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_bg_color(get_style_step_button_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_text_color(get_style_step_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_step_icon_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_setting_value_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_setting_value_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_setting_value_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_arc_color(get_style_seg_arc_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_arc_color(get_style_seg_arc_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_arc_color(get_style_seg_arc_INDICATOR_DISABLED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_arc_color(get_style_seg_arc_current_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_line_color(get_style_dial_ticks_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_line_color(get_style_dial_ticks_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][8]));
    lv_style_set_text_color(get_style_dial_ticks_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_dial_disk_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_bg_grad_color(get_style_dial_disk_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_border_color(get_style_dial_disk_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_line_color(get_style_dial_grip_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_line_color(get_style_dial_grip_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_text_color(get_style_dial_index_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_dial_index_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_text_color(get_style_timer_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_timer_text_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_text_color(get_style_level_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_battery_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_battery_icon_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_battery_icon_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_text_color(get_style_hint_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_hint_text_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_text_color(get_style_hint_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_slot_text_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    lv_style_set_text_color(get_style_slot_text_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_slot_text_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_slot_text_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_bg_color(get_style_slot_line_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_bg_color(get_style_slot_line_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][13]));
    lv_style_set_bg_color(get_style_slot_line_MAIN_DISABLED(), lv_color_hex(theme_colors[theme_index][15]));
    lv_style_set_bg_color(get_style_slot_line_current_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][11]));
    lv_style_set_bg_color(get_style_reward_pill_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    lv_style_set_border_color(get_style_reward_pill_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_text_color(get_style_reward_title_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][14]));
    lv_style_set_text_color(get_style_reward_code_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][12]));
    lv_style_set_text_color(get_style_reward_meta_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_obj_invalidate(objects.page_title);
    lv_obj_invalidate(objects.page_instructions);
    lv_obj_invalidate(objects.page_menu);
    lv_obj_invalidate(objects.page_settings);
    lv_obj_invalidate(objects.page_headphones);
    lv_obj_invalidate(objects.page_play);
    lv_obj_invalidate(objects.page_unlocked);
}
uint32_t theme_colors[2][16] = {
    { 0xff000000, 0xff252525, 0xff1a1a1a, 0xff333333, 0xff141414, 0xff0e0e0e, 0xff1f1f1f, 0xff505050, 0xffcacaca, 0xffbdbdbd, 0xffffffff, 0xff52a441, 0xff7bc96a, 0xff3d7b31, 0xff74fe00, 0xffff5453 },
    { 0xff000000, 0xff252525, 0xff1a1a1a, 0xff333333, 0xff141414, 0xff0e0e0e, 0xff1f1f1f, 0xff505050, 0xffcacaca, 0xffbdbdbd, 0xffffffff, 0xff52a441, 0xff7bc96a, 0xff3d7b31, 0xff74fe00, 0xffff5453 },
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
    create_screen_page_title();
    create_screen_page_instructions();
    create_screen_page_menu();
    create_screen_page_settings();
    create_screen_page_headphones();
    create_screen_page_play();
    create_screen_page_unlocked();
}