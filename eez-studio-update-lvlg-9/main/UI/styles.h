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

// Style: HeaderBar
lv_style_t *get_style_header_bar_MAIN_DEFAULT();
void add_style_header_bar(lv_obj_t *obj);
void remove_style_header_bar(lv_obj_t *obj);

// Style: FooterBar
lv_style_t *get_style_footer_bar_MAIN_DEFAULT();
void add_style_footer_bar(lv_obj_t *obj);
void remove_style_footer_bar(lv_obj_t *obj);

// Style: BodyArea
lv_style_t *get_style_body_area_MAIN_DEFAULT();
void add_style_body_area(lv_obj_t *obj);
void remove_style_body_area(lv_obj_t *obj);

// Style: Card
lv_style_t *get_style_card_MAIN_DEFAULT();
void add_style_card(lv_obj_t *obj);
void remove_style_card(lv_obj_t *obj);

// Style: PanelPlain
lv_style_t *get_style_panel_plain_MAIN_DEFAULT();
void add_style_panel_plain(lv_obj_t *obj);
void remove_style_panel_plain(lv_obj_t *obj);

// Style: PanelInset
lv_style_t *get_style_panel_inset_MAIN_DEFAULT();
void add_style_panel_inset(lv_obj_t *obj);
void remove_style_panel_inset(lv_obj_t *obj);

// Style: LabelDefault
lv_style_t *get_style_label_default_MAIN_DEFAULT();
void add_style_label_default(lv_obj_t *obj);
void remove_style_label_default(lv_obj_t *obj);

// Style: CardTitle
lv_style_t *get_style_card_title_MAIN_DEFAULT();
void add_style_card_title(lv_obj_t *obj);
void remove_style_card_title(lv_obj_t *obj);

// Style: HeaderTitle
lv_style_t *get_style_header_title_MAIN_DEFAULT();
void add_style_header_title(lv_obj_t *obj);
void remove_style_header_title(lv_obj_t *obj);

// Style: HeaderKicker
lv_style_t *get_style_header_kicker_MAIN_DEFAULT();
void add_style_header_kicker(lv_obj_t *obj);
void remove_style_header_kicker(lv_obj_t *obj);

// Style: IconAccent
lv_style_t *get_style_icon_accent_MAIN_DEFAULT();
void add_style_icon_accent(lv_obj_t *obj);
void remove_style_icon_accent(lv_obj_t *obj);

// Style: IconSolar
lv_style_t *get_style_icon_solar_MAIN_DEFAULT();
void add_style_icon_solar(lv_obj_t *obj);
void remove_style_icon_solar(lv_obj_t *obj);

// Style: IconFresh
lv_style_t *get_style_icon_fresh_MAIN_DEFAULT();
void add_style_icon_fresh(lv_obj_t *obj);
void remove_style_icon_fresh(lv_obj_t *obj);

// Style: IconGrey
lv_style_t *get_style_icon_grey_MAIN_DEFAULT();
void add_style_icon_grey(lv_obj_t *obj);
void remove_style_icon_grey(lv_obj_t *obj);

// Style: StatusLine
lv_style_t *get_style_status_line_MAIN_DEFAULT();
void add_style_status_line(lv_obj_t *obj);
void remove_style_status_line(lv_obj_t *obj);

// Style: FooterText
lv_style_t *get_style_footer_text_MAIN_DEFAULT();
void add_style_footer_text(lv_obj_t *obj);
void remove_style_footer_text(lv_obj_t *obj);

// Style: TankRowLabel
lv_style_t *get_style_tank_row_label_MAIN_DEFAULT();
void add_style_tank_row_label(lv_obj_t *obj);
void remove_style_tank_row_label(lv_obj_t *obj);

// Style: SpanHero
lv_style_t *get_style_span_hero_MAIN_DEFAULT();
void add_style_span_hero(lv_obj_t *obj);
void remove_style_span_hero(lv_obj_t *obj);

// Style: SpanSecondary
lv_style_t *get_style_span_secondary_MAIN_DEFAULT();
void add_style_span_secondary(lv_obj_t *obj);
void remove_style_span_secondary(lv_obj_t *obj);

// Style: SpanTank
lv_style_t *get_style_span_tank_MAIN_DEFAULT();
void add_style_span_tank(lv_obj_t *obj);
void remove_style_span_tank(lv_obj_t *obj);

// Style: ScaleRing
lv_style_t *get_style_scale_ring_MAIN_DEFAULT();
lv_style_t *get_style_scale_ring_ITEMS_DEFAULT();
lv_style_t *get_style_scale_ring_INDICATOR_DEFAULT();
void add_style_scale_ring(lv_obj_t *obj);
void remove_style_scale_ring(lv_obj_t *obj);

// Style: ScaleStrip
lv_style_t *get_style_scale_strip_MAIN_DEFAULT();
lv_style_t *get_style_scale_strip_ITEMS_DEFAULT();
lv_style_t *get_style_scale_strip_INDICATOR_DEFAULT();
void add_style_scale_strip(lv_obj_t *obj);
void remove_style_scale_strip(lv_obj_t *obj);

// Style: ScaleSectionDanger
lv_style_t *get_style_scale_section_danger_MAIN_DEFAULT();
void add_style_scale_section_danger(lv_obj_t *obj);
void remove_style_scale_section_danger(lv_obj_t *obj);

// Style: ScaleSectionWarn
lv_style_t *get_style_scale_section_warn_MAIN_DEFAULT();
void add_style_scale_section_warn(lv_obj_t *obj);
void remove_style_scale_section_warn(lv_obj_t *obj);

// Style: ScaleSectionGood
lv_style_t *get_style_scale_section_good_MAIN_DEFAULT();
void add_style_scale_section_good(lv_obj_t *obj);
void remove_style_scale_section_good(lv_obj_t *obj);

// Style: ArcBattery
lv_style_t *get_style_arc_battery_MAIN_DEFAULT();
lv_style_t *get_style_arc_battery_INDICATOR_DEFAULT();
lv_style_t *get_style_arc_battery_KNOB_DEFAULT();
void add_style_arc_battery(lv_obj_t *obj);
void remove_style_arc_battery(lv_obj_t *obj);

// Style: ArcSolar
lv_style_t *get_style_arc_solar_MAIN_DEFAULT();
lv_style_t *get_style_arc_solar_INDICATOR_DEFAULT();
lv_style_t *get_style_arc_solar_KNOB_DEFAULT();
void add_style_arc_solar(lv_obj_t *obj);
void remove_style_arc_solar(lv_obj_t *obj);

// Style: BarFresh
lv_style_t *get_style_bar_fresh_MAIN_DEFAULT();
lv_style_t *get_style_bar_fresh_INDICATOR_DEFAULT();
void add_style_bar_fresh(lv_obj_t *obj);
void remove_style_bar_fresh(lv_obj_t *obj);

// Style: BarGrey
lv_style_t *get_style_bar_grey_MAIN_DEFAULT();
lv_style_t *get_style_bar_grey_INDICATOR_DEFAULT();
void add_style_bar_grey(lv_obj_t *obj);
void remove_style_bar_grey(lv_obj_t *obj);

// Style: ButtonGhost
lv_style_t *get_style_button_ghost_MAIN_DEFAULT();
lv_style_t *get_style_button_ghost_MAIN_PRESSED();
void add_style_button_ghost(lv_obj_t *obj);
void remove_style_button_ghost(lv_obj_t *obj);

// Style: ScanRow
lv_style_t *get_style_scan_row_MAIN_DEFAULT();
lv_style_t *get_style_scan_row_MAIN_PRESSED();
lv_style_t *get_style_scan_row_MAIN_CHECKED();
void add_style_scan_row(lv_obj_t *obj);
void remove_style_scan_row(lv_obj_t *obj);

// Style: ScanRowSsid
lv_style_t *get_style_scan_row_ssid_MAIN_DEFAULT();
lv_style_t *get_style_scan_row_ssid_MAIN_CHECKED();
void add_style_scan_row_ssid(lv_obj_t *obj);
void remove_style_scan_row_ssid(lv_obj_t *obj);

// Style: ScanRowMeta
lv_style_t *get_style_scan_row_meta_MAIN_DEFAULT();
lv_style_t *get_style_scan_row_meta_MAIN_CHECKED();
void add_style_scan_row_meta(lv_obj_t *obj);
void remove_style_scan_row_meta(lv_obj_t *obj);

// Style: TextareaDefault
lv_style_t *get_style_textarea_default_MAIN_DEFAULT();
lv_style_t *get_style_textarea_default_MAIN_FOCUSED();
void add_style_textarea_default(lv_obj_t *obj);
void remove_style_textarea_default(lv_obj_t *obj);

// Style: KeyboardDefault
lv_style_t *get_style_keyboard_default_MAIN_DEFAULT();
lv_style_t *get_style_keyboard_default_ITEMS_DEFAULT();
void add_style_keyboard_default(lv_obj_t *obj);
void remove_style_keyboard_default(lv_obj_t *obj);

// Style: DropdownDefault
lv_style_t *get_style_dropdown_default_MAIN_DEFAULT();
void add_style_dropdown_default(lv_obj_t *obj);
void remove_style_dropdown_default(lv_obj_t *obj);

// Style: ButtonPrimary
lv_style_t *get_style_button_primary_MAIN_DEFAULT();
lv_style_t *get_style_button_primary_MAIN_PRESSED();
lv_style_t *get_style_button_primary_MAIN_DISABLED();
void add_style_button_primary(lv_obj_t *obj);
void remove_style_button_primary(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/