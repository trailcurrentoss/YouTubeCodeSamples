/*
 * What the Play and Unlocked screens show, from the game state.
 *
 * C here sets text, states and the HIDDEN flag. It never moves, resizes or
 * restyles an EEZ-authored widget -- every look a widget can take is a state
 * in its style (CHECKED = done, DISABLED = wrong), authored in
 * GUI/CrackTheSafe.eez-project.
 */
#pragma once

#include <stdbool.h>

/** Bring every widget in line with g_game. Change-only, so cheap to call
 *  every tick. LVGL task, lock held. */
void ui_game_refresh(void);

/** Scroll the Headphones list by knob detents. LVGL task, lock held. */
void ui_game_scroll_headphones(int detents);

/** Show or hide the Headphones page's (i) panel (Forget Device / Cancel).
 *  Takes effect at the next ui_game_refresh(). */
void ui_game_headphones_info(bool open);
