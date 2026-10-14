/*
 * Change-only setters for the widgets the 250 ms refresh keeps rewriting.
 *
 * LVGL redraws on lv_label_set_text() and on a HIDDEN flag change even when
 * nothing differs, and a remove-then-add of a state is two restyles. With
 * every screen rewriting every label four times a second, the panels were
 * redrawing continuously -- 60-130 ms a frame, 5 frames a second -- and
 * touch is only read between frames, so a quick tap on Back could begin and
 * end inside one frame and never be seen. These make an unchanged value a
 * no-op, so a screen only redraws when something on it actually changes.
 *
 * Use them for anything written on a timer. A one-off write on a key press
 * does not need them.
 */
#pragma once

#include <stdbool.h>
#include <string.h>

#include "lvgl.h"

/** Set a label's text if it differs. NULL label is a no-op. */
static inline void ui_lv_set_text(lv_obj_t *label, const char *text)
{
    if (!label || !text) {
        return;
    }
    const char *cur = lv_label_get_text(label);
    if (cur && strcmp(cur, text) == 0) {
        return;
    }
    lv_label_set_text(label, text);
}

/** Show or hide if that is a change. */
static inline void ui_lv_set_hidden(lv_obj_t *obj, bool hidden)
{
    if (!obj || lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN) == hidden) {
        return;
    }
    if (hidden) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

/**
 * Make exactly `st` (possibly 0) the set bits of `mask` in the object's
 * state -- the "colour as a state" pattern the styles use -- touching the
 * object only if that changes anything.
 */
static inline void ui_lv_set_state_in(lv_obj_t *obj, lv_state_t mask,
                                      lv_state_t st)
{
    if (!obj || (lv_obj_get_state(obj) & mask) == st) {
        return;
    }
    lv_obj_remove_state(obj, mask & ~st);
    if (st) {
        lv_obj_add_state(obj, st);
    }
}

/** Set the MAIN translation if it differs. */
static inline void ui_lv_set_translate(lv_obj_t *obj, int32_t x, int32_t y)
{
    if (!obj) {
        return;
    }
    if (lv_obj_get_style_translate_x(obj, LV_PART_MAIN) != x) {
        lv_obj_set_style_translate_x(obj, x, LV_PART_MAIN);
    }
    if (lv_obj_get_style_translate_y(obj, LV_PART_MAIN) != y) {
        lv_obj_set_style_translate_y(obj, y, LV_PART_MAIN);
    }
}
