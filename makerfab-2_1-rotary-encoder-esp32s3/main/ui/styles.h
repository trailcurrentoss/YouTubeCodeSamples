#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: ScreenRoot
lv_style_t *get_style_screen_root_MAIN_DEFAULT();
void add_style_screen_root(lv_obj_t *obj);
void remove_style_screen_root(lv_obj_t *obj);

// Style: LabelDefault
lv_style_t *get_style_label_default_MAIN_DEFAULT();
void add_style_label_default(lv_obj_t *obj);
void remove_style_label_default(lv_obj_t *obj);

// Style: PanelClear
lv_style_t *get_style_panel_clear_MAIN_DEFAULT();
void add_style_panel_clear(lv_obj_t *obj);
void remove_style_panel_clear(lv_obj_t *obj);

// Style: KeyButton
lv_style_t *get_style_key_button_MAIN_DEFAULT();
lv_style_t *get_style_key_button_MAIN_PRESSED();
void add_style_key_button(lv_obj_t *obj);
void remove_style_key_button(lv_obj_t *obj);

// Style: KeyIcon
lv_style_t *get_style_key_icon_MAIN_DEFAULT();
void add_style_key_icon(lv_obj_t *obj);
void remove_style_key_icon(lv_obj_t *obj);

// Style: KeyName
lv_style_t *get_style_key_name_MAIN_DEFAULT();
void add_style_key_name(lv_obj_t *obj);
void remove_style_key_name(lv_obj_t *obj);

// Style: KeyShortcut
lv_style_t *get_style_key_shortcut_MAIN_DEFAULT();
void add_style_key_shortcut(lv_obj_t *obj);
void remove_style_key_shortcut(lv_obj_t *obj);

// Style: CenterPanel
lv_style_t *get_style_center_panel_MAIN_DEFAULT();
void add_style_center_panel(lv_obj_t *obj);
void remove_style_center_panel(lv_obj_t *obj);

// Style: CenterTap
lv_style_t *get_style_center_tap_MAIN_DEFAULT();
lv_style_t *get_style_center_tap_MAIN_PRESSED();
void add_style_center_tap(lv_obj_t *obj);
void remove_style_center_tap(lv_obj_t *obj);

// Style: ModeLabel
lv_style_t *get_style_mode_label_MAIN_DEFAULT();
void add_style_mode_label(lv_obj_t *obj);
void remove_style_mode_label(lv_obj_t *obj);

// Style: AngleLabel
lv_style_t *get_style_angle_label_MAIN_DEFAULT();
void add_style_angle_label(lv_obj_t *obj);
void remove_style_angle_label(lv_obj_t *obj);

// Style: AxisRow
lv_style_t *get_style_axis_row_MAIN_DEFAULT();
void add_style_axis_row(lv_obj_t *obj);
void remove_style_axis_row(lv_obj_t *obj);

// Style: AxisChipX
lv_style_t *get_style_axis_chip_x_MAIN_DEFAULT();
lv_style_t *get_style_axis_chip_x_MAIN_CHECKED();
void add_style_axis_chip_x(lv_obj_t *obj);
void remove_style_axis_chip_x(lv_obj_t *obj);

// Style: AxisChipY
lv_style_t *get_style_axis_chip_y_MAIN_DEFAULT();
lv_style_t *get_style_axis_chip_y_MAIN_CHECKED();
void add_style_axis_chip_y(lv_obj_t *obj);
void remove_style_axis_chip_y(lv_obj_t *obj);

// Style: AxisChipZ
lv_style_t *get_style_axis_chip_z_MAIN_DEFAULT();
lv_style_t *get_style_axis_chip_z_MAIN_CHECKED();
void add_style_axis_chip_z(lv_obj_t *obj);
void remove_style_axis_chip_z(lv_obj_t *obj);

// Style: Timecode
lv_style_t *get_style_timecode_MAIN_DEFAULT();
void add_style_timecode(lv_obj_t *obj);
void remove_style_timecode(lv_obj_t *obj);

// Style: ScrubStrip
lv_style_t *get_style_scrub_strip_MAIN_DEFAULT();
void add_style_scrub_strip(lv_obj_t *obj);
void remove_style_scrub_strip(lv_obj_t *obj);

// Style: Playhead
lv_style_t *get_style_playhead_MAIN_DEFAULT();
void add_style_playhead(lv_obj_t *obj);
void remove_style_playhead(lv_obj_t *obj);

// Style: CutMark
lv_style_t *get_style_cut_mark_MAIN_DEFAULT();
void add_style_cut_mark(lv_obj_t *obj);
void remove_style_cut_mark(lv_obj_t *obj);

// Style: Pill
lv_style_t *get_style_pill_MAIN_DEFAULT();
void add_style_pill(lv_obj_t *obj);
void remove_style_pill(lv_obj_t *obj);

// Style: PillText
lv_style_t *get_style_pill_text_MAIN_DEFAULT();
void add_style_pill_text(lv_obj_t *obj);
void remove_style_pill_text(lv_obj_t *obj);

// Style: StatusDot
lv_style_t *get_style_status_dot_MAIN_DEFAULT();
lv_style_t *get_style_status_dot_MAIN_CHECKED();
void add_style_status_dot(lv_obj_t *obj);
void remove_style_status_dot(lv_obj_t *obj);

// Style: ClockDial
lv_style_t *get_style_clock_dial_MAIN_DEFAULT();
lv_style_t *get_style_clock_dial_ITEMS_DEFAULT();
lv_style_t *get_style_clock_dial_INDICATOR_DEFAULT();
void add_style_clock_dial(lv_obj_t *obj);
void remove_style_clock_dial(lv_obj_t *obj);

// Style: HandHour
lv_style_t *get_style_hand_hour_MAIN_DEFAULT();
void add_style_hand_hour(lv_obj_t *obj);
void remove_style_hand_hour(lv_obj_t *obj);

// Style: HandMinute
lv_style_t *get_style_hand_minute_MAIN_DEFAULT();
void add_style_hand_minute(lv_obj_t *obj);
void remove_style_hand_minute(lv_obj_t *obj);

// Style: HandSecond
lv_style_t *get_style_hand_second_MAIN_DEFAULT();
void add_style_hand_second(lv_obj_t *obj);
void remove_style_hand_second(lv_obj_t *obj);

// Style: ClockCap
lv_style_t *get_style_clock_cap_MAIN_DEFAULT();
void add_style_clock_cap(lv_obj_t *obj);
void remove_style_clock_cap(lv_obj_t *obj);

// Style: InfoColumn
lv_style_t *get_style_info_column_MAIN_DEFAULT();
void add_style_info_column(lv_obj_t *obj);
void remove_style_info_column(lv_obj_t *obj);

// Style: IdleIcon
lv_style_t *get_style_idle_icon_MAIN_DEFAULT();
void add_style_idle_icon(lv_obj_t *obj);
void remove_style_idle_icon(lv_obj_t *obj);

// Style: IdleTitle
lv_style_t *get_style_idle_title_MAIN_DEFAULT();
void add_style_idle_title(lv_obj_t *obj);
void remove_style_idle_title(lv_obj_t *obj);

// Style: IdleHint
lv_style_t *get_style_idle_hint_MAIN_DEFAULT();
void add_style_idle_hint(lv_obj_t *obj);
void remove_style_idle_hint(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/