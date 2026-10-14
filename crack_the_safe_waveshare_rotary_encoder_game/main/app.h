/*
 * The glue between the game (game.c), the board (knob_board) and the EEZ
 * export (main/ui/). Everything here runs in the LVGL task with the display
 * lock held.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "game.h"

/** The one game. ui_game.c reads it to fill the screens. */
extern game_t g_game;

/** Milliseconds since boot, on the same clock the game uses. */
uint32_t app_now_ms(void);

/** Call once, after ui_init(), with the lock held. */
void app_init(uint32_t seed);

/** Knob detents, from the board's rotate callback. */
void app_rotate(int detents);

/** A tap on the glass (the Tap action). */
void app_tap(void);

/** The Start button on the instructions (the Start action). */
void app_start(void);

/* Touch and hold on a game screen (the Menu action): New Game, Settings,
 * Back. The game is paused while the menu or anything opened from it is up. */
void app_menu(void);
void app_menu_new_game(void);
void app_menu_settings(void);
/** Back out of the menu, Settings or Headphones, one screen at a time. */
void app_back(void);
/** Settings' Volume -/+ (and the knob, while Settings is up). */
void app_volume_step(int steps);

/* The Headphones (Bluetooth pairing) screen and its buttons. */
void app_headphones_open(void);
void app_headphones_search(void);
void app_headphones_select(int row);
/** The (i) panel on the remembered headphones: Forget Device, Cancel. */
void app_headphones_info(bool open);
void app_headphones_forget(void);

/* ---- The unlock sequence, from the moment the last number is reached ----
 *
 *   0 ms                  door closed; the haptic ramp ("bolts drawing back")
 *   UNLOCK_DOOR_START_MS  the door starts to swing, one sprite frame every
 *                         UNLOCK_FRAME_MS
 *   frame UNLOCK_GLOW_FRAME   far enough open to see in: the gold glows
 *   door done + UNLOCK_REWARD_DELAY_MS   the result panel appears
 *   GAME_REPLAY_LOCKOUT_MS               "Turn to Play Again", input counts
 */
#define UNLOCK_DOOR_FRAMES      16
#define UNLOCK_DOOR_START_MS    650
#define UNLOCK_FRAME_MS         150
#define UNLOCK_GLOW_FRAME       6
#define UNLOCK_REWARD_DELAY_MS  300

/** The last door frame shows (the door is fully open), and the result panel
 *  appears -- when the prize's own sound plays. */
#define UNLOCK_DOOR_DONE_MS  (UNLOCK_DOOR_START_MS + (UNLOCK_DOOR_FRAMES - 1) * UNLOCK_FRAME_MS)
#define UNLOCK_REWARD_MS     (UNLOCK_DOOR_DONE_MS + UNLOCK_REWARD_DELAY_MS)

/** Milliseconds since the safe opened (meaningful in GAME_UNLOCKED only). */
uint32_t app_unlock_ms(void);

/** The door sprite frame to show at `t` ms into the sequence. */
int app_door_frame_at(uint32_t t);
