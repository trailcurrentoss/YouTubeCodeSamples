#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_PAGE_DASHBOARD = 1,
    SCREEN_ID_PAGE_WIFI = 2,
    SCREEN_ID_PAGE_BROKER = 3,
    _SCREEN_ID_LAST = 3
};

typedef struct _objects_t {
    lv_obj_t *page_dashboard;
    lv_obj_t *page_wifi;
    lv_obj_t *page_broker;
    lv_obj_t *header;
    lv_obj_t *hdr_bolt;
    lv_obj_t *hdr_title;
    lv_obj_t *hdr_kicker;
    lv_obj_t *hdr_spacer;
    lv_obj_t *hdr_wifi_btn;
    lv_obj_t *hdr_wifi_icon;
    lv_obj_t *hdr_brk_btn;
    lv_obj_t *hdr_brk_icon;
    lv_obj_t *hdr_link;
    lv_obj_t *btn_theme;
    lv_obj_t *btn_theme_icon;
    lv_obj_t *body;
    lv_obj_t *card_battery;
    lv_obj_t *batt_title;
    lv_obj_t *batt_gauge;
    lv_obj_t *batt_scale;
    lv_obj_t *batt_arc;
    lv_obj_t *batt_centre;
    lv_obj_t *batt_pct_span;
    lv_obj_t *batt_volts_span;
    lv_obj_t *batt_state;
    lv_obj_t *card_solar;
    lv_obj_t *solar_title;
    lv_obj_t *solar_gauge;
    lv_obj_t *solar_arc;
    lv_obj_t *solar_centre;
    lv_obj_t *solar_sun;
    lv_obj_t *solar_w_span;
    lv_obj_t *solar_strip;
    lv_obj_t *solar_today;
    lv_obj_t *card_tanks;
    lv_obj_t *tanks_title;
    lv_obj_t *tanks_inset;
    lv_obj_t *tank_fresh;
    lv_obj_t *tank_fresh_row;
    lv_obj_t *tank_fresh_icon;
    lv_obj_t *tank_fresh_name;
    lv_obj_t *tank_fresh_sp;
    lv_obj_t *tank_fresh_span;
    lv_obj_t *tank_fresh_bar;
    lv_obj_t *tank_grey;
    lv_obj_t *tank_grey_row;
    lv_obj_t *tank_grey_icon;
    lv_obj_t *tank_grey_name;
    lv_obj_t *tank_grey_sp;
    lv_obj_t *tank_grey_span;
    lv_obj_t *tank_grey_bar;
    lv_obj_t *tanks_note;
    lv_obj_t *footer;
    lv_obj_t *ft_src;
    lv_obj_t *ft_sp;
    lv_obj_t *ft_uptime;
    lv_obj_t *ft_build;
    lv_obj_t *wifi_header;
    lv_obj_t *wifi_back;
    lv_obj_t *wifi_back_lbl;
    lv_obj_t *wifi_title;
    lv_obj_t *wifi_hdr_sp;
    lv_obj_t *wifi_status;
    lv_obj_t *wifi_body;
    lv_obj_t *wifi_list_card;
    lv_obj_t *wifi_list_head;
    lv_obj_t *wifi_list_title;
    lv_obj_t *wifi_list_sp;
    lv_obj_t *wifi_scan_btn;
    lv_obj_t *wifi_scan_lbl;
    lv_obj_t *wifi_rows;
    lv_obj_t *wifi_row_0;
    lv_obj_t *wifi_ssid_0;
    lv_obj_t *wifi_row_sp_0;
    lv_obj_t *wifi_meta_0;
    lv_obj_t *wifi_row_1;
    lv_obj_t *wifi_ssid_1;
    lv_obj_t *wifi_row_sp_1;
    lv_obj_t *wifi_meta_1;
    lv_obj_t *wifi_row_2;
    lv_obj_t *wifi_ssid_2;
    lv_obj_t *wifi_row_sp_2;
    lv_obj_t *wifi_meta_2;
    lv_obj_t *wifi_row_3;
    lv_obj_t *wifi_ssid_3;
    lv_obj_t *wifi_row_sp_3;
    lv_obj_t *wifi_meta_3;
    lv_obj_t *wifi_row_4;
    lv_obj_t *wifi_ssid_4;
    lv_obj_t *wifi_row_sp_4;
    lv_obj_t *wifi_meta_4;
    lv_obj_t *wifi_row_5;
    lv_obj_t *wifi_ssid_5;
    lv_obj_t *wifi_row_sp_5;
    lv_obj_t *wifi_meta_5;
    lv_obj_t *wifi_row_6;
    lv_obj_t *wifi_ssid_6;
    lv_obj_t *wifi_row_sp_6;
    lv_obj_t *wifi_meta_6;
    lv_obj_t *wifi_row_7;
    lv_obj_t *wifi_ssid_7;
    lv_obj_t *wifi_row_sp_7;
    lv_obj_t *wifi_meta_7;
    lv_obj_t *wifi_row_8;
    lv_obj_t *wifi_ssid_8;
    lv_obj_t *wifi_row_sp_8;
    lv_obj_t *wifi_meta_8;
    lv_obj_t *wifi_row_9;
    lv_obj_t *wifi_ssid_9;
    lv_obj_t *wifi_row_sp_9;
    lv_obj_t *wifi_meta_9;
    lv_obj_t *wifi_row_10;
    lv_obj_t *wifi_ssid_10;
    lv_obj_t *wifi_row_sp_10;
    lv_obj_t *wifi_meta_10;
    lv_obj_t *wifi_row_11;
    lv_obj_t *wifi_ssid_11;
    lv_obj_t *wifi_row_sp_11;
    lv_obj_t *wifi_meta_11;
    lv_obj_t *wifi_row_12;
    lv_obj_t *wifi_ssid_12;
    lv_obj_t *wifi_row_sp_12;
    lv_obj_t *wifi_meta_12;
    lv_obj_t *wifi_row_13;
    lv_obj_t *wifi_ssid_13;
    lv_obj_t *wifi_row_sp_13;
    lv_obj_t *wifi_meta_13;
    lv_obj_t *wifi_row_14;
    lv_obj_t *wifi_ssid_14;
    lv_obj_t *wifi_row_sp_14;
    lv_obj_t *wifi_meta_14;
    lv_obj_t *wifi_row_15;
    lv_obj_t *wifi_ssid_15;
    lv_obj_t *wifi_row_sp_15;
    lv_obj_t *wifi_meta_15;
    lv_obj_t *wifi_row_16;
    lv_obj_t *wifi_ssid_16;
    lv_obj_t *wifi_row_sp_16;
    lv_obj_t *wifi_meta_16;
    lv_obj_t *wifi_row_17;
    lv_obj_t *wifi_ssid_17;
    lv_obj_t *wifi_row_sp_17;
    lv_obj_t *wifi_meta_17;
    lv_obj_t *wifi_row_18;
    lv_obj_t *wifi_ssid_18;
    lv_obj_t *wifi_row_sp_18;
    lv_obj_t *wifi_meta_18;
    lv_obj_t *wifi_row_19;
    lv_obj_t *wifi_ssid_19;
    lv_obj_t *wifi_row_sp_19;
    lv_obj_t *wifi_meta_19;
    lv_obj_t *wifi_form_card;
    lv_obj_t *wifi_selected;
    lv_obj_t *wifi_security;
    lv_obj_t *wifi_password;
    lv_obj_t *wifi_kb;
    lv_obj_t *wifi_actions;
    lv_obj_t *wifi_cancel;
    lv_obj_t *wifi_cancel_lbl;
    lv_obj_t *wifi_act_sp;
    lv_obj_t *wifi_connect;
    lv_obj_t *wifi_connect_lbl;
    lv_obj_t *wifi_footer;
    lv_obj_t *wifi_hint;
    lv_obj_t *brk_header;
    lv_obj_t *brk_back;
    lv_obj_t *brk_back_lbl;
    lv_obj_t *brk_title;
    lv_obj_t *brk_hdr_sp;
    lv_obj_t *brk_status;
    lv_obj_t *brk_body;
    lv_obj_t *brk_card;
    lv_obj_t *brk_host;
    lv_obj_t *brk_user;
    lv_obj_t *brk_pass;
    lv_obj_t *brk_hint;
    lv_obj_t *brk_kb;
    lv_obj_t *brk_actions;
    lv_obj_t *brk_cancel;
    lv_obj_t *brk_cancel_lbl;
    lv_obj_t *brk_act_sp;
    lv_obj_t *brk_save;
    lv_obj_t *brk_save_lbl;
    lv_obj_t *brk_footer;
    lv_obj_t *brk_foot;
} objects_t;

extern objects_t objects;

typedef struct {
    lv_scale_section_t *low;
    lv_scale_section_t *warn;
    lv_scale_section_t *good;
    lv_span_t *span_0;
    lv_span_t *span_1;
    lv_span_t *span_01;
    lv_span_t *span_11;
    lv_span_t *span_02;
    lv_span_t *span_12;
    lv_span_t *span_03;
    lv_span_t *span_13;
    lv_span_t *span_04;
    lv_span_t *span_14;
} screen_page_dashboard_state_t;

extern screen_page_dashboard_state_t screen_page_dashboard_state;

void create_screen_page_dashboard();
void tick_screen_page_dashboard();

void create_screen_page_wifi();
void tick_screen_page_wifi();

void create_screen_page_broker();
void tick_screen_page_broker();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

// Color themes

enum Themes {
    THEME_ID_DEFAULT,
    THEME_ID_DARK,
};
enum Colors {
    COLOR_ID_ACCENT_PRIMARY,
    COLOR_ID_ACCENT_SOLAR,
    COLOR_ID_DANGER_COLOR,
    COLOR_ID_WARN_COLOR,
    COLOR_ID_BG_SCREEN,
    COLOR_ID_BG_CARD,
    COLOR_ID_BG_INSET,
    COLOR_ID_BORDER_COLOR,
    COLOR_ID_TEXT_PRIMARY,
    COLOR_ID_TEXT_MUTED,
    COLOR_ID_FOREGROUND_WHITE,
    COLOR_ID_FOREGROUND_BLACK,
    COLOR_ID_TANK_FRESH,
    COLOR_ID_TANK_GREY,
    COLOR_ID_TICK_COLOR,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[2][15];
extern uint32_t active_theme_index;

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/