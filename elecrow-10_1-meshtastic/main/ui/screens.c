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

void create_screen_page_messages() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_messages = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // msg_topbar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.msg_topbar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 48);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_top_bar(obj, 4);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // msg_conv_card
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.msg_conv_card = obj;
            lv_obj_set_pos(obj, 12, 60);
            lv_obj_set_size(obj, 240, 528);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_card(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // msg_conv_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.msg_conv_title = obj;
                    lv_obj_set_pos(obj, 14, 12);
                    lv_obj_set_size(obj, 212, 19);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Conversations");
                }
                {
                    // msg_conv_0
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_conv_0 = obj;
                    lv_obj_set_pos(obj, 8, 44);
                    lv_obj_set_size(obj, 224, 54);
                    lv_obj_add_event_cb(obj, action_select_conversation, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_conv_0_name
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_0_name = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 160, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "LongFast");
                        }
                        {
                            // msg_conv_0_sub
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_0_sub = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 200, 17);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Everyone on the mesh");
                        }
                        {
                            // msg_conv_0_unread
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_0_unread = obj;
                            lv_obj_set_pos(obj, 176, 8);
                            lv_obj_set_size(obj, 36, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // msg_conv_1
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_conv_1 = obj;
                    lv_obj_set_pos(obj, 8, 104);
                    lv_obj_set_size(obj, 224, 54);
                    lv_obj_add_event_cb(obj, action_select_conversation, LV_EVENT_CLICKED, (void *)1);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_conv_1_name
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_1_name = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 160, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_1_sub
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_1_sub = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 200, 17);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_1_unread
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_1_unread = obj;
                            lv_obj_set_pos(obj, 176, 8);
                            lv_obj_set_size(obj, 36, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // msg_conv_2
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_conv_2 = obj;
                    lv_obj_set_pos(obj, 8, 164);
                    lv_obj_set_size(obj, 224, 54);
                    lv_obj_add_event_cb(obj, action_select_conversation, LV_EVENT_CLICKED, (void *)2);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_conv_2_name
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_2_name = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 160, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_2_sub
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_2_sub = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 200, 17);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_2_unread
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_2_unread = obj;
                            lv_obj_set_pos(obj, 176, 8);
                            lv_obj_set_size(obj, 36, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // msg_conv_3
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_conv_3 = obj;
                    lv_obj_set_pos(obj, 8, 224);
                    lv_obj_set_size(obj, 224, 54);
                    lv_obj_add_event_cb(obj, action_select_conversation, LV_EVENT_CLICKED, (void *)3);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_conv_3_name
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_3_name = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 160, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_3_sub
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_3_sub = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 200, 17);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_3_unread
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_3_unread = obj;
                            lv_obj_set_pos(obj, 176, 8);
                            lv_obj_set_size(obj, 36, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // msg_conv_4
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_conv_4 = obj;
                    lv_obj_set_pos(obj, 8, 284);
                    lv_obj_set_size(obj, 224, 54);
                    lv_obj_add_event_cb(obj, action_select_conversation, LV_EVENT_CLICKED, (void *)4);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_conv_4_name
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_4_name = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 160, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_4_sub
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_4_sub = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 200, 17);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_4_unread
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_4_unread = obj;
                            lv_obj_set_pos(obj, 176, 8);
                            lv_obj_set_size(obj, 36, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // msg_conv_5
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_conv_5 = obj;
                    lv_obj_set_pos(obj, 8, 344);
                    lv_obj_set_size(obj, 224, 54);
                    lv_obj_add_event_cb(obj, action_select_conversation, LV_EVENT_CLICKED, (void *)5);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_conv_5_name
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_5_name = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 160, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_5_sub
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_5_sub = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 200, 17);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_5_unread
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_5_unread = obj;
                            lv_obj_set_pos(obj, 176, 8);
                            lv_obj_set_size(obj, 36, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // msg_conv_6
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_conv_6 = obj;
                    lv_obj_set_pos(obj, 8, 404);
                    lv_obj_set_size(obj, 224, 54);
                    lv_obj_add_event_cb(obj, action_select_conversation, LV_EVENT_CLICKED, (void *)6);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_conv_6_name
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_6_name = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 160, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_6_sub
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_6_sub = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 200, 17);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_6_unread
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_6_unread = obj;
                            lv_obj_set_pos(obj, 176, 8);
                            lv_obj_set_size(obj, 36, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // msg_conv_7
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_conv_7 = obj;
                    lv_obj_set_pos(obj, 8, 464);
                    lv_obj_set_size(obj, 224, 54);
                    lv_obj_add_event_cb(obj, action_select_conversation, LV_EVENT_CLICKED, (void *)7);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_conv_7_name
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_7_name = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 160, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_7_sub
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_7_sub = obj;
                            lv_obj_set_pos(obj, 12, 30);
                            lv_obj_set_size(obj, 200, 17);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                        {
                            // msg_conv_7_unread
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_conv_7_unread = obj;
                            lv_obj_set_pos(obj, 176, 8);
                            lv_obj_set_size(obj, 36, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 9, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
            }
        }
        {
            // msg_thread_card
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.msg_thread_card = obj;
            lv_obj_set_pos(obj, 264, 60);
            lv_obj_set_size(obj, 748, 528);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_card(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // msg_thread_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.msg_thread_title = obj;
                    lv_obj_set_pos(obj, 16, 12);
                    lv_obj_set_size(obj, 500, 19);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "LongFast");
                }
                {
                    // msg_thread_sub
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.msg_thread_sub = obj;
                    lv_obj_set_pos(obj, 520, 13);
                    lv_obj_set_size(obj, 212, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Everyone on the mesh");
                }
                {
                    // msg_compose
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.msg_compose = obj;
                    lv_obj_set_pos(obj, 12, 42);
                    lv_obj_set_size(obj, 600, 48);
                    lv_textarea_set_max_length(obj, 200);
                    lv_textarea_set_placeholder_text(obj, "Type a message");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_event_cb(obj, action_show_keyboard, LV_EVENT_CLICKED, (void *)0);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_textarea_default(obj);
                }
                {
                    // msg_send
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.msg_send = obj;
                    lv_obj_set_pos(obj, 624, 42);
                    lv_obj_set_size(obj, 112, 48);
                    lv_obj_add_event_cb(obj, action_send_message, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_primary(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_send_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_send_icon = obj;
                            lv_obj_set_pos(obj, 21, 12);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // msg_send_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.msg_send_text = obj;
                            lv_obj_set_pos(obj, 51, 15);
                            lv_obj_set_size(obj, 40, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Send");
                        }
                    }
                }
                {
                    // msg_list
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.msg_list = obj;
                    lv_obj_set_pos(obj, 12, 100);
                    lv_obj_set_size(obj, 724, 416);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                    add_style_panel_transparent(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // msg_row_0
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_0 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_0_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_0_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_0_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_0_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_0_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_0_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_1
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_1 = obj;
                            lv_obj_set_pos(obj, 0, 104);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_1_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_1_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_1_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_1_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_1_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_1_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_2
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_2 = obj;
                            lv_obj_set_pos(obj, 0, 208);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_2_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_2_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_2_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_2_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_2_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_2_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_3
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_3 = obj;
                            lv_obj_set_pos(obj, 0, 312);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_3_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_3_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_3_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_3_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_3_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_3_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_4
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_4 = obj;
                            lv_obj_set_pos(obj, 0, 416);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_4_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_4_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_4_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_4_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_4_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_4_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_5
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_5 = obj;
                            lv_obj_set_pos(obj, 0, 520);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_5_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_5_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_5_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_5_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_5_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_5_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_6
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_6 = obj;
                            lv_obj_set_pos(obj, 0, 624);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_6_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_6_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_6_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_6_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_6_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_6_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_7
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_7 = obj;
                            lv_obj_set_pos(obj, 0, 728);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_7_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_7_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_7_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_7_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_7_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_7_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_8
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_8 = obj;
                            lv_obj_set_pos(obj, 0, 832);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_8_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_8_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_8_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_8_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_8_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_8_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_9
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_9 = obj;
                            lv_obj_set_pos(obj, 0, 936);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_9_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_9_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_9_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_9_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_9_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_9_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_10
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_10 = obj;
                            lv_obj_set_pos(obj, 0, 1040);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_10_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_10_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_10_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_10_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_10_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_10_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_11
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_11 = obj;
                            lv_obj_set_pos(obj, 0, 1144);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_11_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_11_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_11_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_11_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_11_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_11_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_12
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_12 = obj;
                            lv_obj_set_pos(obj, 0, 1248);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_12_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_12_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_12_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_12_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_12_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_12_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_13
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_13 = obj;
                            lv_obj_set_pos(obj, 0, 1352);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_13_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_13_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_13_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_13_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_13_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_13_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_14
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_14 = obj;
                            lv_obj_set_pos(obj, 0, 1456);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_14_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_14_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_14_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_14_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_14_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_14_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_15
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_15 = obj;
                            lv_obj_set_pos(obj, 0, 1560);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_15_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_15_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_15_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_15_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_15_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_15_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_16
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_16 = obj;
                            lv_obj_set_pos(obj, 0, 1664);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_16_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_16_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_16_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_16_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_16_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_16_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_17
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_17 = obj;
                            lv_obj_set_pos(obj, 0, 1768);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_17_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_17_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_17_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_17_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_17_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_17_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_18
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_18 = obj;
                            lv_obj_set_pos(obj, 0, 1872);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_18_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_18_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_18_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_18_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_18_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_18_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                        {
                            // msg_row_19
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.msg_row_19 = obj;
                            lv_obj_set_pos(obj, 0, 1976);
                            lv_obj_set_size(obj, 708, 96);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // msg_row_19_from
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_19_from = obj;
                                    lv_obj_set_pos(obj, 12, 8);
                                    lv_obj_set_size(obj, 440, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // msg_row_19_meta
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_19_meta = obj;
                                    lv_obj_set_pos(obj, 460, 8);
                                    lv_obj_set_size(obj, 236, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // msg_row_19_text
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.msg_row_19_text = obj;
                                    lv_obj_set_pos(obj, 12, 30);
                                    lv_obj_set_size(obj, 684, 57);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr15, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                            }
                        }
                    }
                }
                {
                    // msg_empty
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.msg_empty = obj;
                    lv_obj_set_pos(obj, 12, 240);
                    lv_obj_set_size(obj, 724, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "No messages yet. Messages heard on the mesh appear here.");
                }
            }
        }
        {
            // msg_kb
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.msg_kb = obj;
            lv_obj_set_pos(obj, 12, 330);
            lv_obj_set_size(obj, 1000, 262);
            lv_obj_add_event_cb(obj, action_keyboard_done, LV_EVENT_READY, (void *)0);
            lv_obj_add_event_cb(obj, action_keyboard_done, LV_EVENT_CANCEL, (void *)0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_keyboard_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_width(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 262, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 262, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_page_messages();
}

void tick_screen_page_messages() {
    tick_user_widget_top_bar(4);
}

void create_screen_page_nodes() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_nodes = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // nodes_topbar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.nodes_topbar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 48);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_top_bar(obj, 143);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // nodes_card
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.nodes_card = obj;
            lv_obj_set_pos(obj, 12, 60);
            lv_obj_set_size(obj, 1000, 528);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_card(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // nodes_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.nodes_title = obj;
                    lv_obj_set_pos(obj, 16, 12);
                    lv_obj_set_size(obj, 300, 19);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Nodes heard");
                }
                {
                    // nodes_count
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.nodes_count = obj;
                    lv_obj_set_pos(obj, 680, 13);
                    lv_obj_set_size(obj, 304, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // nodes_hdr_name
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.nodes_hdr_name = obj;
                    lv_obj_set_pos(obj, 68, 44);
                    lv_obj_set_size(obj, 300, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Name");
                }
                {
                    // nodes_hdr_id
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.nodes_hdr_id = obj;
                    lv_obj_set_pos(obj, 388, 44);
                    lv_obj_set_size(obj, 110, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "ID");
                }
                {
                    // nodes_hdr_heard
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.nodes_hdr_heard = obj;
                    lv_obj_set_pos(obj, 508, 44);
                    lv_obj_set_size(obj, 120, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Last heard");
                }
                {
                    // nodes_hdr_snr
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.nodes_hdr_snr = obj;
                    lv_obj_set_pos(obj, 638, 44);
                    lv_obj_set_size(obj, 90, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "SNR");
                }
                {
                    // nodes_list
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.nodes_list = obj;
                    lv_obj_set_pos(obj, 8, 68);
                    lv_obj_set_size(obj, 984, 452);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    lv_obj_set_scroll_dir(obj, LV_DIR_VER);
                    add_style_panel_transparent(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // nodes_row_0
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_0 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_0_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_0_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_0_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_0_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_0_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_0_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_0_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_0_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_0_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_0_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_0_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_0_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_0_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_0_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)0);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_0_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_0_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_0_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_0_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_1
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_1 = obj;
                            lv_obj_set_pos(obj, 0, 60);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_1_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_1_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_1_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_1_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_1_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_1_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_1_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_1_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_1_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_1_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_1_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_1_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_1_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_1_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)1);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_1_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_1_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_1_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_1_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_2
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_2 = obj;
                            lv_obj_set_pos(obj, 0, 120);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_2_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_2_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_2_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_2_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_2_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_2_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_2_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_2_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_2_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_2_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_2_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_2_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_2_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_2_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)2);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_2_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_2_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_2_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_2_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_3
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_3 = obj;
                            lv_obj_set_pos(obj, 0, 180);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_3_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_3_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_3_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_3_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_3_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_3_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_3_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_3_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_3_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_3_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_3_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_3_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_3_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_3_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)3);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_3_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_3_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_3_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_3_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_4
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_4 = obj;
                            lv_obj_set_pos(obj, 0, 240);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_4_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_4_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_4_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_4_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_4_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_4_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_4_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_4_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_4_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_4_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_4_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_4_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_4_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_4_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)4);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_4_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_4_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_4_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_4_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_5
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_5 = obj;
                            lv_obj_set_pos(obj, 0, 300);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_5_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_5_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_5_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_5_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_5_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_5_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_5_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_5_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_5_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_5_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_5_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_5_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_5_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_5_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)5);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_5_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_5_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_5_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_5_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_6
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_6 = obj;
                            lv_obj_set_pos(obj, 0, 360);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_6_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_6_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_6_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_6_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_6_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_6_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_6_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_6_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_6_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_6_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_6_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_6_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_6_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_6_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)6);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_6_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_6_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_6_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_6_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_7
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_7 = obj;
                            lv_obj_set_pos(obj, 0, 420);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_7_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_7_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_7_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_7_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_7_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_7_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_7_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_7_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_7_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_7_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_7_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_7_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_7_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_7_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)7);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_7_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_7_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_7_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_7_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_8
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_8 = obj;
                            lv_obj_set_pos(obj, 0, 480);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_8_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_8_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_8_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_8_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_8_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_8_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_8_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_8_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_8_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_8_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_8_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_8_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_8_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_8_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)8);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_8_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_8_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_8_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_8_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_9
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_9 = obj;
                            lv_obj_set_pos(obj, 0, 540);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_9_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_9_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_9_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_9_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_9_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_9_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_9_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_9_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_9_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_9_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_9_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_9_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_9_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_9_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)9);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_9_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_9_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_9_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_9_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_10
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_10 = obj;
                            lv_obj_set_pos(obj, 0, 600);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_10_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_10_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_10_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_10_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_10_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_10_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_10_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_10_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_10_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_10_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_10_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_10_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_10_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_10_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)10);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_10_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_10_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_10_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_10_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_11
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_11 = obj;
                            lv_obj_set_pos(obj, 0, 660);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_11_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_11_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_11_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_11_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_11_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_11_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_11_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_11_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_11_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_11_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_11_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_11_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_11_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_11_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)11);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_11_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_11_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_11_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_11_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_12
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_12 = obj;
                            lv_obj_set_pos(obj, 0, 720);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_12_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_12_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_12_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_12_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_12_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_12_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_12_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_12_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_12_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_12_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_12_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_12_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_12_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_12_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)12);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_12_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_12_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_12_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_12_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_13
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_13 = obj;
                            lv_obj_set_pos(obj, 0, 780);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_13_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_13_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_13_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_13_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_13_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_13_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_13_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_13_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_13_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_13_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_13_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_13_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_13_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_13_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)13);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_13_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_13_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_13_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_13_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_14
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_14 = obj;
                            lv_obj_set_pos(obj, 0, 840);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_14_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_14_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_14_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_14_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_14_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_14_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_14_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_14_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_14_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_14_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_14_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_14_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_14_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_14_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)14);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_14_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_14_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_14_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_14_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_15
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_15 = obj;
                            lv_obj_set_pos(obj, 0, 900);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_15_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_15_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_15_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_15_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_15_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_15_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_15_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_15_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_15_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_15_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_15_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_15_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_15_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_15_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)15);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_15_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_15_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_15_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_15_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_16
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_16 = obj;
                            lv_obj_set_pos(obj, 0, 960);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_16_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_16_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_16_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_16_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_16_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_16_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_16_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_16_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_16_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_16_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_16_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_16_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_16_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_16_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)16);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_16_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_16_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_16_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_16_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_17
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_17 = obj;
                            lv_obj_set_pos(obj, 0, 1020);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_17_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_17_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_17_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_17_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_17_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_17_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_17_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_17_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_17_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_17_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_17_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_17_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_17_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_17_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)17);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_17_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_17_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_17_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_17_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_18
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_18 = obj;
                            lv_obj_set_pos(obj, 0, 1080);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_18_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_18_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_18_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_18_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_18_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_18_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_18_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_18_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_18_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_18_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_18_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_18_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_18_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_18_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)18);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_18_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_18_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_18_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_18_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_19
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_19 = obj;
                            lv_obj_set_pos(obj, 0, 1140);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_19_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_19_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_19_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_19_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_19_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_19_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_19_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_19_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_19_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_19_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_19_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_19_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_19_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_19_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)19);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_19_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_19_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_19_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_19_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_20
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_20 = obj;
                            lv_obj_set_pos(obj, 0, 1200);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_20_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_20_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_20_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_20_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_20_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_20_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_20_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_20_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_20_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_20_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_20_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_20_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_20_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_20_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)20);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_20_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_20_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_20_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_20_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_21
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_21 = obj;
                            lv_obj_set_pos(obj, 0, 1260);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_21_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_21_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_21_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_21_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_21_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_21_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_21_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_21_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_21_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_21_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_21_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_21_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_21_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_21_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)21);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_21_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_21_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_21_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_21_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_22
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_22 = obj;
                            lv_obj_set_pos(obj, 0, 1320);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_22_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_22_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_22_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_22_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_22_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_22_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_22_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_22_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_22_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_22_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_22_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_22_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_22_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_22_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)22);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_22_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_22_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_22_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_22_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_23
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_23 = obj;
                            lv_obj_set_pos(obj, 0, 1380);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_23_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_23_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_23_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_23_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_23_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_23_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_23_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_23_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_23_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_23_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_23_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_23_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_23_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_23_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)23);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_23_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_23_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_23_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_23_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_24
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_24 = obj;
                            lv_obj_set_pos(obj, 0, 1440);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_24_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_24_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_24_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_24_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_24_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_24_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_24_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_24_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_24_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_24_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_24_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_24_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_24_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_24_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)24);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_24_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_24_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_24_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_24_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_25
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_25 = obj;
                            lv_obj_set_pos(obj, 0, 1500);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_25_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_25_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_25_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_25_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_25_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_25_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_25_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_25_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_25_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_25_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_25_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_25_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_25_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_25_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)25);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_25_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_25_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_25_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_25_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_26
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_26 = obj;
                            lv_obj_set_pos(obj, 0, 1560);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_26_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_26_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_26_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_26_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_26_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_26_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_26_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_26_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_26_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_26_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_26_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_26_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_26_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_26_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)26);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_26_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_26_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_26_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_26_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_27
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_27 = obj;
                            lv_obj_set_pos(obj, 0, 1620);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_27_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_27_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_27_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_27_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_27_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_27_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_27_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_27_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_27_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_27_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_27_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_27_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_27_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_27_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)27);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_27_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_27_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_27_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_27_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_28
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_28 = obj;
                            lv_obj_set_pos(obj, 0, 1680);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_28_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_28_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_28_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_28_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_28_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_28_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_28_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_28_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_28_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_28_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_28_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_28_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_28_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_28_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)28);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_28_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_28_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_28_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_28_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_29
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_29 = obj;
                            lv_obj_set_pos(obj, 0, 1740);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_29_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_29_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_29_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_29_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_29_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_29_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_29_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_29_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_29_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_29_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_29_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_29_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_29_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_29_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)29);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_29_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_29_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_29_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_29_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_30
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_30 = obj;
                            lv_obj_set_pos(obj, 0, 1800);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_30_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_30_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_30_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_30_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_30_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_30_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_30_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_30_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_30_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_30_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_30_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_30_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_30_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_30_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)30);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_30_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_30_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_30_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_30_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_31
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_31 = obj;
                            lv_obj_set_pos(obj, 0, 1860);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_31_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_31_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_31_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_31_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_31_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_31_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_31_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_31_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_31_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_31_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_31_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_31_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_31_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_31_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)31);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_31_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_31_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_31_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_31_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_32
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_32 = obj;
                            lv_obj_set_pos(obj, 0, 1920);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_32_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_32_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_32_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_32_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_32_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_32_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_32_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_32_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_32_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_32_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_32_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_32_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_32_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_32_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)32);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_32_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_32_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_32_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_32_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_33
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_33 = obj;
                            lv_obj_set_pos(obj, 0, 1980);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_33_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_33_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_33_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_33_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_33_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_33_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_33_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_33_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_33_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_33_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_33_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_33_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_33_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_33_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)33);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_33_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_33_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_33_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_33_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_34
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_34 = obj;
                            lv_obj_set_pos(obj, 0, 2040);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_34_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_34_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_34_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_34_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_34_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_34_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_34_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_34_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_34_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_34_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_34_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_34_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_34_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_34_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)34);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_34_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_34_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_34_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_34_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_35
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_35 = obj;
                            lv_obj_set_pos(obj, 0, 2100);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_35_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_35_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_35_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_35_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_35_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_35_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_35_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_35_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_35_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_35_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_35_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_35_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_35_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_35_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)35);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_35_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_35_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_35_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_35_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_36
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_36 = obj;
                            lv_obj_set_pos(obj, 0, 2160);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_36_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_36_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_36_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_36_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_36_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_36_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_36_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_36_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_36_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_36_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_36_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_36_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_36_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_36_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)36);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_36_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_36_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_36_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_36_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_37
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_37 = obj;
                            lv_obj_set_pos(obj, 0, 2220);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_37_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_37_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_37_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_37_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_37_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_37_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_37_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_37_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_37_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_37_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_37_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_37_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_37_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_37_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)37);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_37_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_37_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_37_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_37_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_38
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_38 = obj;
                            lv_obj_set_pos(obj, 0, 2280);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_38_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_38_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_38_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_38_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_38_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_38_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_38_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_38_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_38_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_38_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_38_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_38_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_38_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_38_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)38);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_38_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_38_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_38_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_38_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // nodes_row_39
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.nodes_row_39 = obj;
                            lv_obj_set_pos(obj, 0, 2340);
                            lv_obj_set_size(obj, 968, 54);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
                            add_style_msg_bubble(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // nodes_row_39_short
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_39_short = obj;
                                    lv_obj_set_pos(obj, 10, 13);
                                    lv_obj_set_size(obj, 48, 28);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_pad_top(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_39_name
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_39_name = obj;
                                    lv_obj_set_pos(obj, 68, 8);
                                    lv_obj_set_size(obj, 300, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_39_sub
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_39_sub = obj;
                                    lv_obj_set_pos(obj, 68, 29);
                                    lv_obj_set_size(obj, 300, 17);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "");
                                }
                                {
                                    // nodes_row_39_id
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_39_id = obj;
                                    lv_obj_set_pos(obj, 380, 18);
                                    lv_obj_set_size(obj, 110, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_39_heard
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_39_heard = obj;
                                    lv_obj_set_pos(obj, 500, 18);
                                    lv_obj_set_size(obj, 120, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_39_snr
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.nodes_row_39_snr = obj;
                                    lv_obj_set_pos(obj, 630, 18);
                                    lv_obj_set_size(obj, 90, 18);
                                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text_static(obj, "");
                                }
                                {
                                    // nodes_row_39_msg
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.nodes_row_39_msg = obj;
                                    lv_obj_set_pos(obj, 840, 9);
                                    lv_obj_set_size(obj, 116, 36);
                                    lv_obj_add_event_cb(obj, action_message_node, LV_EVENT_CLICKED, (void *)39);
                                    add_style_button_tab(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // nodes_row_39_msg_icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_39_msg_icon = obj;
                                            lv_obj_set_pos(obj, 10, 6);
                                            lv_obj_set_size(obj, 24, 24);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "");
                                        }
                                        {
                                            // nodes_row_39_msg_text
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.nodes_row_39_msg_text = obj;
                                            lv_obj_set_pos(obj, 40, 9);
                                            lv_obj_set_size(obj, 65, 18);
                                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text_static(obj, "Message");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                {
                    // nodes_empty
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.nodes_empty = obj;
                    lv_obj_set_pos(obj, 8, 240);
                    lv_obj_set_size(obj, 984, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "No other nodes heard yet. They appear here as their broadcasts arrive.");
                }
            }
        }
    }
    
    tick_screen_page_nodes();
}

void tick_screen_page_nodes() {
    tick_user_widget_top_bar(143);
}

void create_screen_page_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    add_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // set_topbar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.set_topbar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 48);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_top_bar(obj, 567);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
        {
            // set_owner_card
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.set_owner_card = obj;
            lv_obj_set_pos(obj, 12, 60);
            lv_obj_set_size(obj, 494, 528);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_card(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // set_owner_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_owner_title = obj;
                    lv_obj_set_pos(obj, 16, 12);
                    lv_obj_set_size(obj, 300, 19);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "This node");
                }
                {
                    // set_long_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_long_lbl = obj;
                    lv_obj_set_pos(obj, 16, 48);
                    lv_obj_set_size(obj, 300, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Long name");
                }
                {
                    // set_long_name
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.set_long_name = obj;
                    lv_obj_set_pos(obj, 16, 68);
                    lv_obj_set_size(obj, 462, 44);
                    lv_textarea_set_max_length(obj, 39);
                    lv_textarea_set_placeholder_text(obj, "Long name");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_event_cb(obj, action_show_keyboard, LV_EVENT_CLICKED, (void *)1);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_textarea_default(obj);
                }
                {
                    // set_short_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_short_lbl = obj;
                    lv_obj_set_pos(obj, 16, 124);
                    lv_obj_set_size(obj, 400, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Short name (up to 4 characters)");
                }
                {
                    // set_short_name
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.set_short_name = obj;
                    lv_obj_set_pos(obj, 16, 144);
                    lv_obj_set_size(obj, 200, 44);
                    lv_textarea_set_max_length(obj, 4);
                    lv_textarea_set_placeholder_text(obj, "Short");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_event_cb(obj, action_show_keyboard, LV_EVENT_CLICKED, (void *)2);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_textarea_default(obj);
                }
                {
                    // set_save
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_save = obj;
                    lv_obj_set_pos(obj, 232, 144);
                    lv_obj_set_size(obj, 246, 44);
                    lv_obj_add_event_cb(obj, action_save_owner, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_primary(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_save_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_save_icon = obj;
                            lv_obj_set_pos(obj, 69, 10);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // set_save_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_save_text = obj;
                            lv_obj_set_pos(obj, 99, 13);
                            lv_obj_set_size(obj, 77, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Save name");
                        }
                    }
                }
                {
                    // set_save_status
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_save_status = obj;
                    lv_obj_set_pos(obj, 16, 200);
                    lv_obj_set_size(obj, 462, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // set_node_id_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_node_id_lbl = obj;
                    lv_obj_set_pos(obj, 16, 236);
                    lv_obj_set_size(obj, 160, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Node ID");
                }
                {
                    // set_node_id
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_node_id = obj;
                    lv_obj_set_pos(obj, 180, 236);
                    lv_obj_set_size(obj, 298, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_ble_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_ble_lbl = obj;
                    lv_obj_set_pos(obj, 16, 272);
                    lv_obj_set_size(obj, 160, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Bluetooth name");
                }
                {
                    // set_ble
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_ble = obj;
                    lv_obj_set_pos(obj, 180, 272);
                    lv_obj_set_size(obj, 298, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_phone_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_phone_lbl = obj;
                    lv_obj_set_pos(obj, 16, 308);
                    lv_obj_set_size(obj, 160, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Phone app");
                }
                {
                    // set_phone
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_phone = obj;
                    lv_obj_set_pos(obj, 180, 308);
                    lv_obj_set_size(obj, 298, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_radio_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_radio_lbl = obj;
                    lv_obj_set_pos(obj, 16, 344);
                    lv_obj_set_size(obj, 160, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "LoRa module");
                }
                {
                    // set_radio
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_radio = obj;
                    lv_obj_set_pos(obj, 180, 344);
                    lv_obj_set_size(obj, 298, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_note
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_note = obj;
                    lv_obj_set_pos(obj, 16, 384);
                    lv_obj_set_size(obj, 462, 76);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "To reach another Meshtastic node, both need the same region, modem preset, frequency slot, channel name and channel key. The phone app can also connect over Bluetooth and change these.");
                }
            }
        }
        {
            // set_radio_card
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.set_radio_card = obj;
            lv_obj_set_pos(obj, 518, 60);
            lv_obj_set_size(obj, 494, 528);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_card(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // set_radio_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_radio_title = obj;
                    lv_obj_set_pos(obj, 16, 12);
                    lv_obj_set_size(obj, 300, 19);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Radio");
                }
                {
                    // set_ch_name_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_ch_name_lbl = obj;
                    lv_obj_set_pos(obj, 16, 44);
                    lv_obj_set_size(obj, 180, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Channel name");
                }
                {
                    // set_ch_key_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_ch_key_lbl = obj;
                    lv_obj_set_pos(obj, 208, 44);
                    lv_obj_set_size(obj, 270, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Channel key (base64)");
                }
                {
                    // set_ch_name
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.set_ch_name = obj;
                    lv_obj_set_pos(obj, 16, 64);
                    lv_obj_set_size(obj, 180, 44);
                    lv_textarea_set_max_length(obj, 11);
                    lv_textarea_set_placeholder_text(obj, "Default");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_event_cb(obj, action_show_keyboard, LV_EVENT_CLICKED, (void *)3);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_textarea_default(obj);
                }
                {
                    // set_ch_key
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.set_ch_key = obj;
                    lv_obj_set_pos(obj, 208, 64);
                    lv_obj_set_size(obj, 270, 44);
                    lv_textarea_set_accepted_chars(obj, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");
                    lv_textarea_set_max_length(obj, 44);
                    lv_textarea_set_placeholder_text(obj, "No encryption");
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_event_cb(obj, action_show_keyboard, LV_EVENT_CLICKED, (void *)4);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_textarea_default(obj);
                }
                {
                    // set_region_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_region_lbl = obj;
                    lv_obj_set_pos(obj, 16, 131);
                    lv_obj_set_size(obj, 140, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Region");
                }
                {
                    // set_region_dec
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_region_dec = obj;
                    lv_obj_set_pos(obj, 160, 120);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)0);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_region_dec_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_region_dec_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_region
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_region = obj;
                    lv_obj_set_pos(obj, 214, 131);
                    lv_obj_set_size(obj, 210, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_region_inc
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_region_inc = obj;
                    lv_obj_set_pos(obj, 430, 120);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)1);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)1);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_region_inc_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_region_inc_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_preset_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_preset_lbl = obj;
                    lv_obj_set_pos(obj, 16, 177);
                    lv_obj_set_size(obj, 140, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Modem preset");
                }
                {
                    // set_preset_dec
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_preset_dec = obj;
                    lv_obj_set_pos(obj, 160, 166);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)2);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)2);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_preset_dec_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_preset_dec_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_preset
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_preset = obj;
                    lv_obj_set_pos(obj, 214, 177);
                    lv_obj_set_size(obj, 210, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_preset_inc
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_preset_inc = obj;
                    lv_obj_set_pos(obj, 430, 166);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)3);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)3);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_preset_inc_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_preset_inc_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_slot_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_slot_lbl = obj;
                    lv_obj_set_pos(obj, 16, 223);
                    lv_obj_set_size(obj, 140, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Frequency slot");
                }
                {
                    // set_slot_dec
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_slot_dec = obj;
                    lv_obj_set_pos(obj, 160, 212);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)4);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)4);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_slot_dec_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_slot_dec_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_slot
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_slot = obj;
                    lv_obj_set_pos(obj, 214, 223);
                    lv_obj_set_size(obj, 210, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_slot_inc
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_slot_inc = obj;
                    lv_obj_set_pos(obj, 430, 212);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)5);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)5);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_slot_inc_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_slot_inc_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_hops_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_hops_lbl = obj;
                    lv_obj_set_pos(obj, 16, 269);
                    lv_obj_set_size(obj, 140, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Hop limit");
                }
                {
                    // set_hops_dec
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_hops_dec = obj;
                    lv_obj_set_pos(obj, 160, 258);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)6);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)6);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_hops_dec_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_hops_dec_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_hops
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_hops = obj;
                    lv_obj_set_pos(obj, 214, 269);
                    lv_obj_set_size(obj, 210, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_hops_inc
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_hops_inc = obj;
                    lv_obj_set_pos(obj, 430, 258);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)7);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)7);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_hops_inc_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_hops_inc_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_power_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_power_lbl = obj;
                    lv_obj_set_pos(obj, 16, 315);
                    lv_obj_set_size(obj, 140, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "TX power");
                }
                {
                    // set_power_dec
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_power_dec = obj;
                    lv_obj_set_pos(obj, 160, 304);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)8);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)8);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_power_dec_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_power_dec_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_power
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_power = obj;
                    lv_obj_set_pos(obj, 214, 315);
                    lv_obj_set_size(obj, 210, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // set_power_inc
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_power_inc = obj;
                    lv_obj_set_pos(obj, 430, 304);
                    lv_obj_set_size(obj, 48, 40);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_PRESSED, (void *)9);
                    lv_obj_add_event_cb(obj, action_step_setting, LV_EVENT_LONG_PRESSED_REPEAT, (void *)9);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_power_inc_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_power_inc_icon = obj;
                            lv_obj_set_pos(obj, 12, 8);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                    }
                }
                {
                    // set_freq_lbl
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_freq_lbl = obj;
                    lv_obj_set_pos(obj, 16, 356);
                    lv_obj_set_size(obj, 140, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Frequency");
                }
                {
                    // set_freq
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_freq = obj;
                    lv_obj_set_pos(obj, 214, 356);
                    lv_obj_set_size(obj, 210, 18);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // set_apply
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.set_apply = obj;
                    lv_obj_set_pos(obj, 16, 386);
                    lv_obj_set_size(obj, 462, 44);
                    lv_obj_add_event_cb(obj, action_apply_radio, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_primary(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // set_apply_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_apply_icon = obj;
                            lv_obj_set_pos(obj, 150, 10);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // set_apply_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.set_apply_text = obj;
                            lv_obj_set_pos(obj, 180, 13);
                            lv_obj_set_size(obj, 132, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Apply radio settings");
                        }
                    }
                }
                {
                    // set_apply_status
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.set_apply_status = obj;
                    lv_obj_set_pos(obj, 16, 438);
                    lv_obj_set_size(obj, 462, 40);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
        {
            // set_kb
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.set_kb = obj;
            lv_obj_set_pos(obj, 12, 330);
            lv_obj_set_size(obj, 1000, 262);
            lv_obj_add_event_cb(obj, action_keyboard_done, LV_EVENT_READY, (void *)1);
            lv_obj_add_event_cb(obj, action_keyboard_done, LV_EVENT_CANCEL, (void *)1);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            add_style_keyboard_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_width(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_width(obj, 1000, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_min_height(obj, 262, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_max_height(obj, 262, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_page_settings();
}

void tick_screen_page_settings() {
    tick_user_widget_top_bar(567);
}

void create_user_widget_top_bar(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 48);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
            add_style_panel_top_bar48(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // tb_icon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
                    lv_obj_set_pos(obj, 16, 12);
                    lv_obj_set_size(obj, 28, 24);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // tb_title
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
                    lv_obj_set_pos(obj, 50, 14);
                    lv_obj_set_size(obj, 83, 19);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rm15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Meshtastic");
                }
                {
                    // tb_node
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
                    lv_obj_set_pos(obj, 168, 15);
                    lv_obj_set_size(obj, 300, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // tb_nav_messages
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 3] = obj;
                    lv_obj_set_pos(obj, 480, 6);
                    lv_obj_set_size(obj, 150, 36);
                    lv_obj_add_event_cb(obj, action_navigate, LV_EVENT_CLICKED, (void *)0);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // tb_nav_messages_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            ((lv_obj_t **)&objects)[startWidgetIndex + 4] = obj;
                            lv_obj_set_pos(obj, 24, 6);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // tb_nav_messages_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            ((lv_obj_t **)&objects)[startWidgetIndex + 5] = obj;
                            lv_obj_set_pos(obj, 54, 9);
                            lv_obj_set_size(obj, 72, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Messages");
                        }
                    }
                }
                {
                    // tb_nav_nodes
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 6] = obj;
                    lv_obj_set_pos(obj, 638, 6);
                    lv_obj_set_size(obj, 130, 36);
                    lv_obj_add_event_cb(obj, action_navigate, LV_EVENT_CLICKED, (void *)1);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // tb_nav_nodes_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            ((lv_obj_t **)&objects)[startWidgetIndex + 7] = obj;
                            lv_obj_set_pos(obj, 25, 6);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // tb_nav_nodes_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            ((lv_obj_t **)&objects)[startWidgetIndex + 8] = obj;
                            lv_obj_set_pos(obj, 55, 9);
                            lv_obj_set_size(obj, 49, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Nodes");
                        }
                    }
                }
                {
                    // tb_nav_settings
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 9] = obj;
                    lv_obj_set_pos(obj, 776, 6);
                    lv_obj_set_size(obj, 130, 36);
                    lv_obj_add_event_cb(obj, action_navigate, LV_EVENT_CLICKED, (void *)2);
                    add_style_button_tab(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // tb_nav_settings_icon
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            ((lv_obj_t **)&objects)[startWidgetIndex + 10] = obj;
                            lv_obj_set_pos(obj, 20, 6);
                            lv_obj_set_size(obj, 24, 24);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "");
                        }
                        {
                            // tb_nav_settings_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            ((lv_obj_t **)&objects)[startWidgetIndex + 11] = obj;
                            lv_obj_set_pos(obj, 50, 9);
                            lv_obj_set_size(obj, 60, 18);
                            lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &ui_font_rm14, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text_static(obj, "Settings");
                        }
                    }
                }
                {
                    // tb_phone
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 12] = obj;
                    lv_obj_set_pos(obj, 914, 12);
                    lv_obj_set_size(obj, 24, 24);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_HIDDEN);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_fa20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
                {
                    // tb_count
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 13] = obj;
                    lv_obj_set_pos(obj, 942, 15);
                    lv_obj_set_size(obj, 70, 17);
                    lv_label_set_long_mode(obj, LV_LABEL_LONG_CLIP);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_rr13, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "");
                }
            }
        }
    }
}

void tick_user_widget_top_bar(int startWidgetIndex) {
    (void)startWidgetIndex;
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_page_messages,
    tick_screen_page_nodes,
    tick_screen_page_settings,
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
    { "rr13", &ui_font_rr13 },
    { "rr14", &ui_font_rr14 },
    { "rr15", &ui_font_rr15 },
    { "rm13", &ui_font_rm13 },
    { "rm14", &ui_font_rm14 },
    { "rm15", &ui_font_rm15 },
    { "fa20", &ui_font_fa20 },
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
        lv_obj_set_style_text_color(objects.msg_conv_0_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_0_unread, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.msg_conv_0_unread, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_1_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_1_unread, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.msg_conv_1_unread, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_2_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_2_unread, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.msg_conv_2_unread, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_3_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_3_unread, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.msg_conv_3_unread, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_4_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_4_unread, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.msg_conv_4_unread, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_5_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_5_unread, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.msg_conv_5_unread, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_6_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_6_unread, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.msg_conv_6_unread, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_7_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_conv_7_unread, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.msg_conv_7_unread, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_thread_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_send_icon, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_send_text, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_0_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_0_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_1_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_1_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_2_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_2_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_3_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_3_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_4_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_4_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_5_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_5_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_6_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_6_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_7_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_7_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_8_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_8_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_9_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_9_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_10_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_10_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_11_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_11_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_12_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_12_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_13_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_13_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_14_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_14_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_15_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_15_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_16_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_16_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_17_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_17_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_18_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_18_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_19_from, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_row_19_meta, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.msg_empty, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        {
            int startWidgetIndex;
            (void)startWidgetIndex;
            {
                startWidgetIndex = 4;
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 0], lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 2], lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 12], lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 13], lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }
    }
    {
        lv_obj_set_style_text_color(objects.nodes_count, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_hdr_name, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_hdr_id, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_hdr_heard, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_hdr_snr, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_0_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_0_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_0_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_0_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_1_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_1_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_1_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_1_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_2_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_2_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_2_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_2_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_3_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_3_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_3_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_3_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_4_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_4_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_4_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_4_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_5_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_5_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_5_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_5_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_6_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_6_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_6_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_6_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_7_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_7_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_7_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_7_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_8_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_8_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_8_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_8_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_9_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_9_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_9_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_9_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_10_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_10_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_10_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_10_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_11_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_11_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_11_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_11_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_12_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_12_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_12_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_12_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_13_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_13_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_13_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_13_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_14_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_14_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_14_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_14_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_15_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_15_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_15_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_15_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_16_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_16_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_16_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_16_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_17_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_17_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_17_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_17_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_18_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_18_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_18_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_18_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_19_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_19_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_19_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_19_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_20_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_20_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_20_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_20_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_21_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_21_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_21_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_21_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_22_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_22_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_22_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_22_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_23_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_23_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_23_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_23_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_24_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_24_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_24_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_24_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_25_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_25_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_25_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_25_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_26_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_26_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_26_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_26_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_27_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_27_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_27_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_27_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_28_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_28_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_28_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_28_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_29_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_29_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_29_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_29_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_30_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_30_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_30_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_30_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_31_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_31_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_31_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_31_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_32_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_32_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_32_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_32_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_33_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_33_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_33_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_33_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_34_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_34_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_34_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_34_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_35_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_35_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_35_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_35_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_36_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_36_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_36_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_36_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_37_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_37_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_37_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_37_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_38_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_38_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_38_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_38_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_39_short, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.nodes_row_39_short, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_39_sub, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_row_39_id, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.nodes_empty, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        {
            int startWidgetIndex;
            (void)startWidgetIndex;
            {
                startWidgetIndex = 143;
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 0], lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 2], lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 12], lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 13], lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }
    }
    {
        lv_obj_set_style_text_color(objects.set_long_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_short_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_save_icon, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_save_text, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_save_status, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_node_id_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_ble_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_phone_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_radio_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_note, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_ch_name_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_ch_key_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_region_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_preset_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_slot_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_hops_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_power_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_freq_lbl, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_apply_icon, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_apply_text, lv_color_hex(theme_colors[theme_index][26]), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(objects.set_apply_status, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
        {
            int startWidgetIndex;
            (void)startWidgetIndex;
            {
                startWidgetIndex = 567;
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 0], lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 2], lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 12], lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(((lv_obj_t **)&objects)[startWidgetIndex + 13], lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
        }
    }
    lv_style_set_bg_color(get_style_screen_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][1]));
    lv_style_set_text_color(get_style_screen_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_card_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_button_primary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_border_color(get_style_button_primary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_button_primary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][26]));
    lv_style_set_text_color(get_style_label_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_textarea_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_textarea_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_textarea_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_keyboard_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_keyboard_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_keyboard_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_keyboard_default_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_keyboard_default_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_keyboard_default_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_keyboard_default_ITEMS_PRESSED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_text_color(get_style_keyboard_default_ITEMS_PRESSED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_keyboard_default_ITEMS_CHECKED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_keyboard_default_ITEMS_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_text_color(get_style_keyboard_default_ITEMS_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_panel_top_bar48_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    lv_style_set_border_color(get_style_panel_top_bar48_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_bg_color(get_style_button_tab_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_button_tab_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_button_tab_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_button_tab_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][3]));
    lv_style_set_bg_color(get_style_button_tab_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_button_tab_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_style_set_bg_color(get_style_msg_bubble_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    lv_style_set_border_color(get_style_msg_bubble_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    lv_style_set_text_color(get_style_msg_bubble_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][6]));
    lv_style_set_bg_color(get_style_msg_bubble_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][10]));
    lv_style_set_border_color(get_style_msg_bubble_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][9]));
    lv_obj_invalidate(objects.page_messages);
    lv_obj_invalidate(objects.page_nodes);
    lv_obj_invalidate(objects.page_settings);
}
uint32_t theme_colors[2][28] = {
    { 0xffe8e8e8, 0xffe4e4e4, 0xffffffff, 0xffededed, 0xffffffff, 0xffc8c8c8, 0xff1a1a1a, 0xff4a4a4a, 0xff888888, 0xff52a441, 0xffcbe3c6, 0xffffc107, 0xfffff4d1, 0xff48e6fe, 0xffff5453, 0xff74fe00, 0xff505050, 0xff0088cc, 0xff48e6fe, 0xff777777, 0xffb5b5b5, 0xff333333, 0xff666666, 0xff9e9e9e, 0xff424242, 0xffe5e5e5, 0xffffffff, 0xff000000 },
    { 0xff0a0a0a, 0xff000000, 0xff1a1a1a, 0xff252525, 0xff0a0a0a, 0xff333333, 0xffffffff, 0xffaaaaaa, 0xff666666, 0xff7bc96a, 0xff2e4a2a, 0xffffc107, 0xff3a2f00, 0xff48e6fe, 0xffff5453, 0xff74fe00, 0xffbdbdbd, 0xff0088cc, 0xff48e6fe, 0xff777777, 0xffb5b5b5, 0xff333333, 0xff666666, 0xff9e9e9e, 0xff424242, 0xff1e1e1e, 0xffffffff, 0xff000000 },
};

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_page_messages();
    create_screen_page_nodes();
    create_screen_page_settings();
}