#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_PAGE_TITLE = 1,
    SCREEN_ID_PAGE_INSTRUCTIONS = 2,
    SCREEN_ID_PAGE_MENU = 3,
    SCREEN_ID_PAGE_SETTINGS = 4,
    SCREEN_ID_PAGE_HEADPHONES = 5,
    SCREEN_ID_PAGE_PLAY = 6,
    SCREEN_ID_PAGE_UNLOCKED = 7,
    _SCREEN_ID_LAST = 7
};

typedef struct _objects_t {
    lv_obj_t *page_title;
    lv_obj_t *page_instructions;
    lv_obj_t *page_menu;
    lv_obj_t *page_settings;
    lv_obj_t *page_headphones;
    lv_obj_t *page_play;
    lv_obj_t *page_unlocked;
    lv_obj_t *title_ring;
    lv_obj_t *title_bolts;
    lv_obj_t *title_bolt_slots;
    lv_obj_t *title_ring_wide;
    lv_obj_t *title_ticks;
    lv_obj_t *title_lock;
    lv_obj_t *title_text;
    lv_obj_t *title_sub;
    lv_obj_t *title_hint_l;
    lv_obj_t *title_hint;
    lv_obj_t *title_hint_r;
    lv_obj_t *instr_title;
    lv_obj_t *instr_body;
    lv_obj_t *start_button;
    lv_obj_t *start_text;
    lv_obj_t *hp_button;
    lv_obj_t *hp_button_icon;
    lv_obj_t *hp_button_text;
    lv_obj_t *menu_title;
    lv_obj_t *menu_new;
    lv_obj_t *menu_new_text;
    lv_obj_t *menu_settings;
    lv_obj_t *menu_settings_text;
    lv_obj_t *menu_back;
    lv_obj_t *menu_back_text;
    lv_obj_t *settings_title;
    lv_obj_t *settings_hp;
    lv_obj_t *settings_hp_icon;
    lv_obj_t *settings_hp_text;
    lv_obj_t *settings_hp_value;
    lv_obj_t *settings_vol;
    lv_obj_t *settings_vol_icon;
    lv_obj_t *settings_vol_text;
    lv_obj_t *settings_vol_down;
    lv_obj_t *settings_vol_down_icon;
    lv_obj_t *settings_vol_value;
    lv_obj_t *settings_vol_up;
    lv_obj_t *settings_vol_up_icon;
    lv_obj_t *settings_back;
    lv_obj_t *settings_back_text;
    lv_obj_t *hp_title;
    lv_obj_t *hp_status;
    lv_obj_t *hp_list;
    lv_obj_t *hp_row0;
    lv_obj_t *hp_row0_text;
    lv_obj_t *hp_row0_info;
    lv_obj_t *hp_row0_info_icon;
    lv_obj_t *hp_row1;
    lv_obj_t *hp_row1_text;
    lv_obj_t *hp_row2;
    lv_obj_t *hp_row2_text;
    lv_obj_t *hp_row3;
    lv_obj_t *hp_row3_text;
    lv_obj_t *hp_row4;
    lv_obj_t *hp_row4_text;
    lv_obj_t *hp_row5;
    lv_obj_t *hp_row5_text;
    lv_obj_t *hp_row6;
    lv_obj_t *hp_row6_text;
    lv_obj_t *hp_row7;
    lv_obj_t *hp_row7_text;
    lv_obj_t *hp_search;
    lv_obj_t *hp_search_text;
    lv_obj_t *hp_done;
    lv_obj_t *hp_done_text;
    lv_obj_t *hp_info;
    lv_obj_t *hp_info_name;
    lv_obj_t *hp_forget;
    lv_obj_t *hp_forget_text;
    lv_obj_t *hp_info_cancel;
    lv_obj_t *hp_info_cancel_text;
    lv_obj_t *seg1;
    lv_obj_t *seg1_cur;
    lv_obj_t *seg2;
    lv_obj_t *seg2_cur;
    lv_obj_t *seg3;
    lv_obj_t *seg3_cur;
    lv_obj_t *dial_ticks;
    lv_obj_t *dial_disk;
    lv_obj_t *dial_grip;
    lv_obj_t *dial_inner;
    lv_obj_t *dial_index;
    lv_obj_t *hint_arrow_cw;
    lv_obj_t *hint_arrow_ccw;
    lv_obj_t *hint_text;
    lv_obj_t *level_text;
    lv_obj_t *timer_text;
    lv_obj_t *slot1;
    lv_obj_t *slot1_line;
    lv_obj_t *slot1_cur;
    lv_obj_t *slot2;
    lv_obj_t *slot2_line;
    lv_obj_t *slot2_cur;
    lv_obj_t *slot3;
    lv_obj_t *slot3_line;
    lv_obj_t *slot3_cur;
    lv_obj_t *police_red;
    lv_obj_t *police_blue;
    lv_obj_t *prize_1;
    lv_obj_t *prize_2;
    lv_obj_t *prize_3;
    lv_obj_t *prize_4;
    lv_obj_t *prize_5;
    lv_obj_t *prize_6;
    lv_obj_t *prize_7;
    lv_obj_t *prize_8;
    lv_obj_t *gold_glow;
    lv_obj_t *door_00;
    lv_obj_t *door_01;
    lv_obj_t *door_02;
    lv_obj_t *door_03;
    lv_obj_t *door_04;
    lv_obj_t *door_05;
    lv_obj_t *door_06;
    lv_obj_t *door_07;
    lv_obj_t *door_08;
    lv_obj_t *door_09;
    lv_obj_t *door_10;
    lv_obj_t *door_11;
    lv_obj_t *door_12;
    lv_obj_t *door_13;
    lv_obj_t *door_14;
    lv_obj_t *door_15;
    lv_obj_t *reward_pill;
    lv_obj_t *reward_title;
    lv_obj_t *reward_code;
    lv_obj_t *reward_meta;
    lv_obj_t *reward_again;
} objects_t;

extern objects_t objects;

void create_screen_page_title();
void tick_screen_page_title();

void create_screen_page_instructions();
void tick_screen_page_instructions();

void create_screen_page_menu();
void tick_screen_page_menu();

void create_screen_page_settings();
void tick_screen_page_settings();

void create_screen_page_headphones();
void tick_screen_page_headphones();

void create_screen_page_play();
void tick_screen_page_play();

void create_screen_page_unlocked();
void tick_screen_page_unlocked();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

// Color themes

enum Themes {
    THEME_ID_DEFAULT,
    THEME_ID_DARK,
};
enum Colors {
    COLOR_ID_BACKGROUND,
    COLOR_ID_RING_OUTER,
    COLOR_ID_BOLT_FILL,
    COLOR_ID_STEEL,
    COLOR_ID_RING_WELL,
    COLOR_ID_DISK_BOTTOM,
    COLOR_ID_SEG_PENDING,
    COLOR_ID_MUTED,
    COLOR_ID_TICK_MAJOR,
    COLOR_ID_TEXT_SOFT,
    COLOR_ID_TEXT_WHITE,
    COLOR_ID_GREEN,
    COLOR_ID_GREEN_LIGHT,
    COLOR_ID_GREEN_DARK,
    COLOR_ID_LIME,
    COLOR_ID_RED,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[2][16];
extern uint32_t active_theme_index;

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/