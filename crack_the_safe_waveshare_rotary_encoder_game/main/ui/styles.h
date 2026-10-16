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

// Style: LabelBody
lv_style_t *get_style_label_body_MAIN_DEFAULT();
void add_style_label_body(lv_obj_t *obj);
void remove_style_label_body(lv_obj_t *obj);

// Style: RingThin
lv_style_t *get_style_ring_thin_MAIN_DEFAULT();
void add_style_ring_thin(lv_obj_t *obj);
void remove_style_ring_thin(lv_obj_t *obj);

// Style: RingWide
lv_style_t *get_style_ring_wide_MAIN_DEFAULT();
void add_style_ring_wide(lv_obj_t *obj);
void remove_style_ring_wide(lv_obj_t *obj);

// Style: RingHair
lv_style_t *get_style_ring_hair_MAIN_DEFAULT();
void add_style_ring_hair(lv_obj_t *obj);
void remove_style_ring_hair(lv_obj_t *obj);

// Style: TitleBolts
lv_style_t *get_style_title_bolts_MAIN_DEFAULT();
lv_style_t *get_style_title_bolts_ITEMS_DEFAULT();
lv_style_t *get_style_title_bolts_INDICATOR_DEFAULT();
void add_style_title_bolts(lv_obj_t *obj);
void remove_style_title_bolts(lv_obj_t *obj);

// Style: TitleBoltSlots
lv_style_t *get_style_title_bolt_slots_MAIN_DEFAULT();
lv_style_t *get_style_title_bolt_slots_ITEMS_DEFAULT();
lv_style_t *get_style_title_bolt_slots_INDICATOR_DEFAULT();
void add_style_title_bolt_slots(lv_obj_t *obj);
void remove_style_title_bolt_slots(lv_obj_t *obj);

// Style: TitleTicks
lv_style_t *get_style_title_ticks_MAIN_DEFAULT();
lv_style_t *get_style_title_ticks_ITEMS_DEFAULT();
lv_style_t *get_style_title_ticks_INDICATOR_DEFAULT();
void add_style_title_ticks(lv_obj_t *obj);
void remove_style_title_ticks(lv_obj_t *obj);

// Style: TitleLock
lv_style_t *get_style_title_lock_MAIN_DEFAULT();
void add_style_title_lock(lv_obj_t *obj);
void remove_style_title_lock(lv_obj_t *obj);

// Style: TitleText
lv_style_t *get_style_title_text_MAIN_DEFAULT();
void add_style_title_text(lv_obj_t *obj);
void remove_style_title_text(lv_obj_t *obj);

// Style: TitleSub
lv_style_t *get_style_title_sub_MAIN_DEFAULT();
void add_style_title_sub(lv_obj_t *obj);
void remove_style_title_sub(lv_obj_t *obj);

// Style: AccentHint
lv_style_t *get_style_accent_hint_MAIN_DEFAULT();
void add_style_accent_hint(lv_obj_t *obj);
void remove_style_accent_hint(lv_obj_t *obj);

// Style: AccentHintIcon
lv_style_t *get_style_accent_hint_icon_MAIN_DEFAULT();
void add_style_accent_hint_icon(lv_obj_t *obj);
void remove_style_accent_hint_icon(lv_obj_t *obj);

// Style: InstructionsBody
lv_style_t *get_style_instructions_body_MAIN_DEFAULT();
void add_style_instructions_body(lv_obj_t *obj);
void remove_style_instructions_body(lv_obj_t *obj);

// Style: StartButton
lv_style_t *get_style_start_button_MAIN_DEFAULT();
lv_style_t *get_style_start_button_MAIN_PRESSED();
void add_style_start_button(lv_obj_t *obj);
void remove_style_start_button(lv_obj_t *obj);

// Style: StartButtonText
lv_style_t *get_style_start_button_text_MAIN_DEFAULT();
void add_style_start_button_text(lv_obj_t *obj);
void remove_style_start_button_text(lv_obj_t *obj);

// Style: ButtonText
lv_style_t *get_style_button_text_MAIN_DEFAULT();
void add_style_button_text(lv_obj_t *obj);
void remove_style_button_text(lv_obj_t *obj);

// Style: ButtonOutline
lv_style_t *get_style_button_outline_MAIN_DEFAULT();
lv_style_t *get_style_button_outline_MAIN_PRESSED();
lv_style_t *get_style_button_outline_MAIN_CHECKED();
void add_style_button_outline(lv_obj_t *obj);
void remove_style_button_outline(lv_obj_t *obj);

// Style: ButtonOutlineText
lv_style_t *get_style_button_outline_text_MAIN_DEFAULT();
lv_style_t *get_style_button_outline_text_MAIN_CHECKED();
void add_style_button_outline_text(lv_obj_t *obj);
void remove_style_button_outline_text(lv_obj_t *obj);

// Style: OutlineIcon
lv_style_t *get_style_outline_icon_MAIN_DEFAULT();
lv_style_t *get_style_outline_icon_MAIN_CHECKED();
void add_style_outline_icon(lv_obj_t *obj);
void remove_style_outline_icon(lv_obj_t *obj);

// Style: DeviceList
lv_style_t *get_style_device_list_MAIN_DEFAULT();
void add_style_device_list(lv_obj_t *obj);
void remove_style_device_list(lv_obj_t *obj);

// Style: DeviceRow
lv_style_t *get_style_device_row_MAIN_DEFAULT();
lv_style_t *get_style_device_row_MAIN_PRESSED();
void add_style_device_row(lv_obj_t *obj);
void remove_style_device_row(lv_obj_t *obj);

// Style: DeviceRowText
lv_style_t *get_style_device_row_text_MAIN_DEFAULT();
lv_style_t *get_style_device_row_text_MAIN_CHECKED();
void add_style_device_row_text(lv_obj_t *obj);
void remove_style_device_row_text(lv_obj_t *obj);

// Style: InfoButton
lv_style_t *get_style_info_button_MAIN_DEFAULT();
lv_style_t *get_style_info_button_MAIN_PRESSED();
void add_style_info_button(lv_obj_t *obj);
void remove_style_info_button(lv_obj_t *obj);

// Style: InfoIcon
lv_style_t *get_style_info_icon_MAIN_DEFAULT();
void add_style_info_icon(lv_obj_t *obj);
void remove_style_info_icon(lv_obj_t *obj);

// Style: Scrim
lv_style_t *get_style_scrim_MAIN_DEFAULT();
void add_style_scrim(lv_obj_t *obj);
void remove_style_scrim(lv_obj_t *obj);

// Style: ButtonDanger
lv_style_t *get_style_button_danger_MAIN_DEFAULT();
lv_style_t *get_style_button_danger_MAIN_PRESSED();
void add_style_button_danger(lv_obj_t *obj);
void remove_style_button_danger(lv_obj_t *obj);

// Style: ButtonDangerText
lv_style_t *get_style_button_danger_text_MAIN_DEFAULT();
void add_style_button_danger_text(lv_obj_t *obj);
void remove_style_button_danger_text(lv_obj_t *obj);

// Style: SettingRow
lv_style_t *get_style_setting_row_MAIN_DEFAULT();
void add_style_setting_row(lv_obj_t *obj);
void remove_style_setting_row(lv_obj_t *obj);

// Style: StepButton
lv_style_t *get_style_step_button_MAIN_DEFAULT();
lv_style_t *get_style_step_button_MAIN_PRESSED();
lv_style_t *get_style_step_button_MAIN_DISABLED();
void add_style_step_button(lv_obj_t *obj);
void remove_style_step_button(lv_obj_t *obj);

// Style: StepIcon
lv_style_t *get_style_step_icon_MAIN_DEFAULT();
lv_style_t *get_style_step_icon_MAIN_DISABLED();
void add_style_step_icon(lv_obj_t *obj);
void remove_style_step_icon(lv_obj_t *obj);

// Style: SettingValue
lv_style_t *get_style_setting_value_MAIN_DEFAULT();
lv_style_t *get_style_setting_value_MAIN_CHECKED();
lv_style_t *get_style_setting_value_MAIN_DISABLED();
void add_style_setting_value(lv_obj_t *obj);
void remove_style_setting_value(lv_obj_t *obj);

// Style: SegArc
lv_style_t *get_style_seg_arc_MAIN_DEFAULT();
lv_style_t *get_style_seg_arc_INDICATOR_DEFAULT();
lv_style_t *get_style_seg_arc_INDICATOR_CHECKED();
lv_style_t *get_style_seg_arc_INDICATOR_DISABLED();
lv_style_t *get_style_seg_arc_KNOB_DEFAULT();
void add_style_seg_arc(lv_obj_t *obj);
void remove_style_seg_arc(lv_obj_t *obj);

// Style: SegArcCurrent
lv_style_t *get_style_seg_arc_current_MAIN_DEFAULT();
lv_style_t *get_style_seg_arc_current_INDICATOR_DEFAULT();
lv_style_t *get_style_seg_arc_current_KNOB_DEFAULT();
void add_style_seg_arc_current(lv_obj_t *obj);
void remove_style_seg_arc_current(lv_obj_t *obj);

// Style: DialTicks
lv_style_t *get_style_dial_ticks_MAIN_DEFAULT();
lv_style_t *get_style_dial_ticks_ITEMS_DEFAULT();
lv_style_t *get_style_dial_ticks_INDICATOR_DEFAULT();
void add_style_dial_ticks(lv_obj_t *obj);
void remove_style_dial_ticks(lv_obj_t *obj);

// Style: DialDisk
lv_style_t *get_style_dial_disk_MAIN_DEFAULT();
void add_style_dial_disk(lv_obj_t *obj);
void remove_style_dial_disk(lv_obj_t *obj);

// Style: DialGrip
lv_style_t *get_style_dial_grip_MAIN_DEFAULT();
lv_style_t *get_style_dial_grip_ITEMS_DEFAULT();
lv_style_t *get_style_dial_grip_INDICATOR_DEFAULT();
void add_style_dial_grip(lv_obj_t *obj);
void remove_style_dial_grip(lv_obj_t *obj);

// Style: DialIndex
lv_style_t *get_style_dial_index_MAIN_DEFAULT();
lv_style_t *get_style_dial_index_MAIN_DISABLED();
void add_style_dial_index(lv_obj_t *obj);
void remove_style_dial_index(lv_obj_t *obj);

// Style: TimerText
lv_style_t *get_style_timer_text_MAIN_DEFAULT();
lv_style_t *get_style_timer_text_MAIN_DISABLED();
void add_style_timer_text(lv_obj_t *obj);
void remove_style_timer_text(lv_obj_t *obj);

// Style: LevelText
lv_style_t *get_style_level_text_MAIN_DEFAULT();
void add_style_level_text(lv_obj_t *obj);
void remove_style_level_text(lv_obj_t *obj);

// Style: BatteryIcon
lv_style_t *get_style_battery_icon_MAIN_DEFAULT();
lv_style_t *get_style_battery_icon_MAIN_CHECKED();
lv_style_t *get_style_battery_icon_MAIN_DISABLED();
void add_style_battery_icon(lv_obj_t *obj);
void remove_style_battery_icon(lv_obj_t *obj);

// Style: HintText
lv_style_t *get_style_hint_text_MAIN_DEFAULT();
lv_style_t *get_style_hint_text_MAIN_DISABLED();
void add_style_hint_text(lv_obj_t *obj);
void remove_style_hint_text(lv_obj_t *obj);

// Style: HintIcon
lv_style_t *get_style_hint_icon_MAIN_DEFAULT();
void add_style_hint_icon(lv_obj_t *obj);
void remove_style_hint_icon(lv_obj_t *obj);

// Style: SlotText
lv_style_t *get_style_slot_text_MAIN_DEFAULT();
lv_style_t *get_style_slot_text_MAIN_PRESSED();
lv_style_t *get_style_slot_text_MAIN_CHECKED();
lv_style_t *get_style_slot_text_MAIN_DISABLED();
void add_style_slot_text(lv_obj_t *obj);
void remove_style_slot_text(lv_obj_t *obj);

// Style: SlotLine
lv_style_t *get_style_slot_line_MAIN_DEFAULT();
lv_style_t *get_style_slot_line_MAIN_CHECKED();
lv_style_t *get_style_slot_line_MAIN_DISABLED();
void add_style_slot_line(lv_obj_t *obj);
void remove_style_slot_line(lv_obj_t *obj);

// Style: SlotLineCurrent
lv_style_t *get_style_slot_line_current_MAIN_DEFAULT();
void add_style_slot_line_current(lv_obj_t *obj);
void remove_style_slot_line_current(lv_obj_t *obj);

// Style: PoliceLight
lv_style_t *get_style_police_light_MAIN_DEFAULT();
void add_style_police_light(lv_obj_t *obj);
void remove_style_police_light(lv_obj_t *obj);

// Style: GoldGlow
lv_style_t *get_style_gold_glow_MAIN_DEFAULT();
lv_style_t *get_style_gold_glow_MAIN_CHECKED();
lv_style_t *get_style_gold_glow_MAIN_DISABLED();
void add_style_gold_glow(lv_obj_t *obj);
void remove_style_gold_glow(lv_obj_t *obj);

// Style: RewardPill
lv_style_t *get_style_reward_pill_MAIN_DEFAULT();
void add_style_reward_pill(lv_obj_t *obj);
void remove_style_reward_pill(lv_obj_t *obj);

// Style: RewardTitle
lv_style_t *get_style_reward_title_MAIN_DEFAULT();
void add_style_reward_title(lv_obj_t *obj);
void remove_style_reward_title(lv_obj_t *obj);

// Style: RewardCode
lv_style_t *get_style_reward_code_MAIN_DEFAULT();
void add_style_reward_code(lv_obj_t *obj);
void remove_style_reward_code(lv_obj_t *obj);

// Style: RewardMeta
lv_style_t *get_style_reward_meta_MAIN_DEFAULT();
void add_style_reward_meta(lv_obj_t *obj);
void remove_style_reward_meta(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/