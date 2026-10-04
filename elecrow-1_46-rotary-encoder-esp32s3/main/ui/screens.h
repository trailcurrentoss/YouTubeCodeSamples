#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_PAGE_IDLE = 1,
    SCREEN_ID_PAGE_FREECAD = 2,
    SCREEN_ID_PAGE_BLENDER = 3,
    SCREEN_ID_PAGE_KDENLIVE = 4,
    SCREEN_ID_PAGE_VSCODIUM = 5,
    SCREEN_ID_PAGE_GIMP = 6,
    SCREEN_ID_PAGE_INKSCAPE = 7,
    SCREEN_ID_PAGE_LIBREOFFICE = 8,
    SCREEN_ID_PAGE_FIREFOX = 9,
    SCREEN_ID_PAGE_CHROMIUM = 10,
    _SCREEN_ID_LAST = 10
};

typedef struct _objects_t {
    lv_obj_t *page_idle;
    lv_obj_t *page_freecad;
    lv_obj_t *page_blender;
    lv_obj_t *page_kdenlive;
    lv_obj_t *page_vscodium;
    lv_obj_t *page_gimp;
    lv_obj_t *page_inkscape;
    lv_obj_t *page_libreoffice;
    lv_obj_t *page_firefox;
    lv_obj_t *page_chromium;
    lv_obj_t *idle_clock;
    lv_obj_t *idle_hand_hour;
    lv_obj_t *idle_hand_minute;
    lv_obj_t *idle_hand_second;
    lv_obj_t *idle_clock_cap;
    lv_obj_t *idle_info;
    lv_obj_t *idle_title;
    lv_obj_t *idle_hint;
    lv_obj_t *idle_pill;
    lv_obj_t *idle_dot;
    lv_obj_t *idle_status;
    lv_obj_t *fc_center;
    lv_obj_t *fc_logo;
    lv_obj_t *fc_mode;
    lv_obj_t *fc_angle;
    lv_obj_t *fc_axes;
    lv_obj_t *fc_axis_x;
    lv_obj_t *fc_axis_y;
    lv_obj_t *fc_axis_z;
    lv_obj_t *fc_key0;
    lv_obj_t *fc_key0_icon;
    lv_obj_t *fc_key0_name;
    lv_obj_t *fc_key0_sc;
    lv_obj_t *fc_key1;
    lv_obj_t *fc_key1_icon;
    lv_obj_t *fc_key1_name;
    lv_obj_t *fc_key1_sc;
    lv_obj_t *fc_key2;
    lv_obj_t *fc_key2_icon;
    lv_obj_t *fc_key2_name;
    lv_obj_t *fc_key2_sc;
    lv_obj_t *fc_key3;
    lv_obj_t *fc_key3_icon;
    lv_obj_t *fc_key3_name;
    lv_obj_t *fc_key3_sc;
    lv_obj_t *fc_key4;
    lv_obj_t *fc_key4_icon;
    lv_obj_t *fc_key4_name;
    lv_obj_t *fc_key4_sc;
    lv_obj_t *fc_key5;
    lv_obj_t *fc_key5_icon;
    lv_obj_t *fc_key5_name;
    lv_obj_t *fc_key5_sc;
    lv_obj_t *fc_key6;
    lv_obj_t *fc_key6_icon;
    lv_obj_t *fc_key6_name;
    lv_obj_t *fc_key6_sc;
    lv_obj_t *fc_key7;
    lv_obj_t *fc_key7_icon;
    lv_obj_t *fc_key7_name;
    lv_obj_t *fc_key7_sc;
    lv_obj_t *bl_center;
    lv_obj_t *bl_logo;
    lv_obj_t *bl_mode;
    lv_obj_t *bl_angle;
    lv_obj_t *bl_axes;
    lv_obj_t *bl_axis_x;
    lv_obj_t *bl_axis_y;
    lv_obj_t *bl_axis_z;
    lv_obj_t *bl_key0;
    lv_obj_t *bl_key0_icon;
    lv_obj_t *bl_key0_name;
    lv_obj_t *bl_key0_sc;
    lv_obj_t *bl_key1;
    lv_obj_t *bl_key1_icon;
    lv_obj_t *bl_key1_name;
    lv_obj_t *bl_key1_sc;
    lv_obj_t *bl_key2;
    lv_obj_t *bl_key2_icon;
    lv_obj_t *bl_key2_name;
    lv_obj_t *bl_key2_sc;
    lv_obj_t *bl_key3;
    lv_obj_t *bl_key3_icon;
    lv_obj_t *bl_key3_name;
    lv_obj_t *bl_key3_sc;
    lv_obj_t *bl_key4;
    lv_obj_t *bl_key4_icon;
    lv_obj_t *bl_key4_name;
    lv_obj_t *bl_key4_sc;
    lv_obj_t *bl_key5;
    lv_obj_t *bl_key5_icon;
    lv_obj_t *bl_key5_name;
    lv_obj_t *bl_key5_sc;
    lv_obj_t *bl_key6;
    lv_obj_t *bl_key6_icon;
    lv_obj_t *bl_key6_name;
    lv_obj_t *bl_key6_sc;
    lv_obj_t *bl_key7;
    lv_obj_t *bl_key7_icon;
    lv_obj_t *bl_key7_name;
    lv_obj_t *bl_key7_sc;
    lv_obj_t *kd_center;
    lv_obj_t *kd_logo;
    lv_obj_t *kd_mode;
    lv_obj_t *kd_timecode;
    lv_obj_t *kd_strip;
    lv_obj_t *kd_playhead;
    lv_obj_t *kd_step_pill;
    lv_obj_t *kd_step;
    lv_obj_t *kd_key0;
    lv_obj_t *kd_key0_icon;
    lv_obj_t *kd_key0_name;
    lv_obj_t *kd_key0_sc;
    lv_obj_t *kd_key1;
    lv_obj_t *kd_key1_icon;
    lv_obj_t *kd_key1_name;
    lv_obj_t *kd_key1_sc;
    lv_obj_t *kd_key2;
    lv_obj_t *kd_key2_icon;
    lv_obj_t *kd_key2_name;
    lv_obj_t *kd_key2_sc;
    lv_obj_t *kd_key3;
    lv_obj_t *kd_key3_icon;
    lv_obj_t *kd_key3_name;
    lv_obj_t *kd_key3_sc;
    lv_obj_t *kd_key4;
    lv_obj_t *kd_key4_icon;
    lv_obj_t *kd_key4_name;
    lv_obj_t *kd_key4_sc;
    lv_obj_t *kd_key5;
    lv_obj_t *kd_key5_icon;
    lv_obj_t *kd_key5_name;
    lv_obj_t *kd_key5_sc;
    lv_obj_t *kd_key6;
    lv_obj_t *kd_key6_icon;
    lv_obj_t *kd_key6_name;
    lv_obj_t *kd_key6_sc;
    lv_obj_t *kd_key7;
    lv_obj_t *kd_key7_icon;
    lv_obj_t *kd_key7_name;
    lv_obj_t *kd_key7_sc;
    lv_obj_t *vs_center;
    lv_obj_t *vs_logo;
    lv_obj_t *vs_mode;
    lv_obj_t *vs_scroll_icon;
    lv_obj_t *vs_step_pill;
    lv_obj_t *vs_step;
    lv_obj_t *vs_key0;
    lv_obj_t *vs_key0_icon;
    lv_obj_t *vs_key0_name;
    lv_obj_t *vs_key0_sc;
    lv_obj_t *vs_key1;
    lv_obj_t *vs_key1_icon;
    lv_obj_t *vs_key1_name;
    lv_obj_t *vs_key1_sc;
    lv_obj_t *vs_key2;
    lv_obj_t *vs_key2_icon;
    lv_obj_t *vs_key2_name;
    lv_obj_t *vs_key2_sc;
    lv_obj_t *vs_key3;
    lv_obj_t *vs_key3_icon;
    lv_obj_t *vs_key3_name;
    lv_obj_t *vs_key3_sc;
    lv_obj_t *vs_key4;
    lv_obj_t *vs_key4_icon;
    lv_obj_t *vs_key4_name;
    lv_obj_t *vs_key4_sc;
    lv_obj_t *vs_key5;
    lv_obj_t *vs_key5_icon;
    lv_obj_t *vs_key5_name;
    lv_obj_t *vs_key5_sc;
    lv_obj_t *vs_key6;
    lv_obj_t *vs_key6_icon;
    lv_obj_t *vs_key6_name;
    lv_obj_t *vs_key6_sc;
    lv_obj_t *vs_key7;
    lv_obj_t *vs_key7_icon;
    lv_obj_t *vs_key7_name;
    lv_obj_t *vs_key7_sc;
    lv_obj_t *gp_center;
    lv_obj_t *gp_logo;
    lv_obj_t *gp_mode;
    lv_obj_t *gp_zoom_icon;
    lv_obj_t *gp_key0;
    lv_obj_t *gp_key0_icon;
    lv_obj_t *gp_key0_name;
    lv_obj_t *gp_key0_sc;
    lv_obj_t *gp_key1;
    lv_obj_t *gp_key1_icon;
    lv_obj_t *gp_key1_name;
    lv_obj_t *gp_key1_sc;
    lv_obj_t *gp_key2;
    lv_obj_t *gp_key2_icon;
    lv_obj_t *gp_key2_name;
    lv_obj_t *gp_key2_sc;
    lv_obj_t *gp_key3;
    lv_obj_t *gp_key3_icon;
    lv_obj_t *gp_key3_name;
    lv_obj_t *gp_key3_sc;
    lv_obj_t *gp_key4;
    lv_obj_t *gp_key4_icon;
    lv_obj_t *gp_key4_name;
    lv_obj_t *gp_key4_sc;
    lv_obj_t *gp_key5;
    lv_obj_t *gp_key5_icon;
    lv_obj_t *gp_key5_name;
    lv_obj_t *gp_key5_sc;
    lv_obj_t *gp_key6;
    lv_obj_t *gp_key6_icon;
    lv_obj_t *gp_key6_name;
    lv_obj_t *gp_key6_sc;
    lv_obj_t *gp_key7;
    lv_obj_t *gp_key7_icon;
    lv_obj_t *gp_key7_name;
    lv_obj_t *gp_key7_sc;
    lv_obj_t *ik_center;
    lv_obj_t *ik_logo;
    lv_obj_t *ik_mode;
    lv_obj_t *ik_zoom_icon;
    lv_obj_t *ik_key0;
    lv_obj_t *ik_key0_icon;
    lv_obj_t *ik_key0_name;
    lv_obj_t *ik_key0_sc;
    lv_obj_t *ik_key1;
    lv_obj_t *ik_key1_icon;
    lv_obj_t *ik_key1_name;
    lv_obj_t *ik_key1_sc;
    lv_obj_t *ik_key2;
    lv_obj_t *ik_key2_icon;
    lv_obj_t *ik_key2_name;
    lv_obj_t *ik_key2_sc;
    lv_obj_t *ik_key3;
    lv_obj_t *ik_key3_icon;
    lv_obj_t *ik_key3_name;
    lv_obj_t *ik_key3_sc;
    lv_obj_t *ik_key4;
    lv_obj_t *ik_key4_icon;
    lv_obj_t *ik_key4_name;
    lv_obj_t *ik_key4_sc;
    lv_obj_t *ik_key5;
    lv_obj_t *ik_key5_icon;
    lv_obj_t *ik_key5_name;
    lv_obj_t *ik_key5_sc;
    lv_obj_t *ik_key6;
    lv_obj_t *ik_key6_icon;
    lv_obj_t *ik_key6_name;
    lv_obj_t *ik_key6_sc;
    lv_obj_t *ik_key7;
    lv_obj_t *ik_key7_icon;
    lv_obj_t *ik_key7_name;
    lv_obj_t *ik_key7_sc;
    lv_obj_t *lo_center;
    lv_obj_t *lo_logo;
    lv_obj_t *lo_mode;
    lv_obj_t *lo_scroll_icon;
    lv_obj_t *lo_step_pill;
    lv_obj_t *lo_step;
    lv_obj_t *lo_key0;
    lv_obj_t *lo_key0_icon;
    lv_obj_t *lo_key0_name;
    lv_obj_t *lo_key0_sc;
    lv_obj_t *lo_key1;
    lv_obj_t *lo_key1_icon;
    lv_obj_t *lo_key1_name;
    lv_obj_t *lo_key1_sc;
    lv_obj_t *lo_key2;
    lv_obj_t *lo_key2_icon;
    lv_obj_t *lo_key2_name;
    lv_obj_t *lo_key2_sc;
    lv_obj_t *lo_key3;
    lv_obj_t *lo_key3_icon;
    lv_obj_t *lo_key3_name;
    lv_obj_t *lo_key3_sc;
    lv_obj_t *lo_key4;
    lv_obj_t *lo_key4_icon;
    lv_obj_t *lo_key4_name;
    lv_obj_t *lo_key4_sc;
    lv_obj_t *lo_key5;
    lv_obj_t *lo_key5_icon;
    lv_obj_t *lo_key5_name;
    lv_obj_t *lo_key5_sc;
    lv_obj_t *lo_key6;
    lv_obj_t *lo_key6_icon;
    lv_obj_t *lo_key6_name;
    lv_obj_t *lo_key6_sc;
    lv_obj_t *lo_key7;
    lv_obj_t *lo_key7_icon;
    lv_obj_t *lo_key7_name;
    lv_obj_t *lo_key7_sc;
    lv_obj_t *ff_center;
    lv_obj_t *ff_logo;
    lv_obj_t *ff_mode;
    lv_obj_t *ff_scroll_icon;
    lv_obj_t *ff_step_pill;
    lv_obj_t *ff_step;
    lv_obj_t *ff_key0;
    lv_obj_t *ff_key0_icon;
    lv_obj_t *ff_key0_name;
    lv_obj_t *ff_key0_sc;
    lv_obj_t *ff_key1;
    lv_obj_t *ff_key1_icon;
    lv_obj_t *ff_key1_name;
    lv_obj_t *ff_key1_sc;
    lv_obj_t *ff_key2;
    lv_obj_t *ff_key2_icon;
    lv_obj_t *ff_key2_name;
    lv_obj_t *ff_key2_sc;
    lv_obj_t *ff_key3;
    lv_obj_t *ff_key3_icon;
    lv_obj_t *ff_key3_name;
    lv_obj_t *ff_key3_sc;
    lv_obj_t *ff_key4;
    lv_obj_t *ff_key4_icon;
    lv_obj_t *ff_key4_name;
    lv_obj_t *ff_key4_sc;
    lv_obj_t *ff_key5;
    lv_obj_t *ff_key5_icon;
    lv_obj_t *ff_key5_name;
    lv_obj_t *ff_key5_sc;
    lv_obj_t *ff_key6;
    lv_obj_t *ff_key6_icon;
    lv_obj_t *ff_key6_name;
    lv_obj_t *ff_key6_sc;
    lv_obj_t *ff_key7;
    lv_obj_t *ff_key7_icon;
    lv_obj_t *ff_key7_name;
    lv_obj_t *ff_key7_sc;
    lv_obj_t *cr_center;
    lv_obj_t *cr_logo;
    lv_obj_t *cr_mode;
    lv_obj_t *cr_scroll_icon;
    lv_obj_t *cr_step_pill;
    lv_obj_t *cr_step;
    lv_obj_t *cr_key0;
    lv_obj_t *cr_key0_icon;
    lv_obj_t *cr_key0_name;
    lv_obj_t *cr_key0_sc;
    lv_obj_t *cr_key1;
    lv_obj_t *cr_key1_icon;
    lv_obj_t *cr_key1_name;
    lv_obj_t *cr_key1_sc;
    lv_obj_t *cr_key2;
    lv_obj_t *cr_key2_icon;
    lv_obj_t *cr_key2_name;
    lv_obj_t *cr_key2_sc;
    lv_obj_t *cr_key3;
    lv_obj_t *cr_key3_icon;
    lv_obj_t *cr_key3_name;
    lv_obj_t *cr_key3_sc;
    lv_obj_t *cr_key4;
    lv_obj_t *cr_key4_icon;
    lv_obj_t *cr_key4_name;
    lv_obj_t *cr_key4_sc;
    lv_obj_t *cr_key5;
    lv_obj_t *cr_key5_icon;
    lv_obj_t *cr_key5_name;
    lv_obj_t *cr_key5_sc;
    lv_obj_t *cr_key6;
    lv_obj_t *cr_key6_icon;
    lv_obj_t *cr_key6_name;
    lv_obj_t *cr_key6_sc;
    lv_obj_t *cr_key7;
    lv_obj_t *cr_key7_icon;
    lv_obj_t *cr_key7_name;
    lv_obj_t *cr_key7_sc;
} objects_t;

extern objects_t objects;

void create_screen_page_idle();
void tick_screen_page_idle();

void create_screen_page_freecad();
void tick_screen_page_freecad();

void create_screen_page_blender();
void tick_screen_page_blender();

void create_screen_page_kdenlive();
void tick_screen_page_kdenlive();

void create_screen_page_vscodium();
void tick_screen_page_vscodium();

void create_screen_page_gimp();
void tick_screen_page_gimp();

void create_screen_page_inkscape();
void tick_screen_page_inkscape();

void create_screen_page_libreoffice();
void tick_screen_page_libreoffice();

void create_screen_page_firefox();
void tick_screen_page_firefox();

void create_screen_page_chromium();
void tick_screen_page_chromium();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

// Color themes

enum Themes {
    THEME_ID_DEFAULT,
    THEME_ID_DARK,
};
enum Colors {
    COLOR_ID_BG_DESK,
    COLOR_ID_BG_BODY,
    COLOR_ID_BG_CARD,
    COLOR_ID_BG_CARD_HOVER,
    COLOR_ID_BG_BAR,
    COLOR_ID_BORDER_COLOR,
    COLOR_ID_TEXT_PRIMARY,
    COLOR_ID_TEXT_SECONDARY,
    COLOR_ID_TEXT_MUTED,
    COLOR_ID_ACCENT_PRIMARY,
    COLOR_ID_ACCENT_SOFT,
    COLOR_ID_SOLAR,
    COLOR_ID_SOLAR_SOFT,
    COLOR_ID_INFO,
    COLOR_ID_DANGER,
    COLOR_ID_SUCCESS,
    COLOR_ID_SLATE_NEUTRAL,
    COLOR_ID_TANK_FRESH_DARK,
    COLOR_ID_TANK_FRESH_LIGHT,
    COLOR_ID_TANK_GREY_DARK,
    COLOR_ID_TANK_GREY_LIGHT,
    COLOR_ID_TANK_BLACK_DARK,
    COLOR_ID_TANK_BLACK_LIGHT,
    COLOR_ID_GREY_WATER,
    COLOR_ID_BLACK_WATER,
    COLOR_ID_GRID_LINE,
    COLOR_ID_FOREGROUND_WHITE,
    COLOR_ID_FOREGROUND_BLACK,
    COLOR_ID_ACCENT_TEXT,
    COLOR_ID_SOLAR_TEXT,
    COLOR_ID_INFO_TEXT,
    COLOR_ID_DANGER_TEXT,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[2][32];
extern uint32_t active_theme_index;

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/