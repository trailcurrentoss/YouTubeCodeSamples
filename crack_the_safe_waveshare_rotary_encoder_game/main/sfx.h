/*
 * The vault's sound effects: the door opening, and one sound per prize.
 *
 * Synthesised once, in PSRAM, by a low-priority task started at boot (a
 * second or two of work), so boot is not held up. Until a sound is ready its
 * getter returns false and the moment simply plays silently -- in practice
 * they are ready long before anyone opens a safe.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Start building the sounds in the background. Call once. */
void sfx_start(void);

/**
 * The door: the bolts drawing back, the latch, the hinge creaking as the
 * door swings, and the thud as it reaches its stop -- timed to the unlock
 * sequence in app.h, so play it the moment the safe opens.
 */
bool sfx_door(const int16_t **pcm, size_t *samples);

/** The prize behind the door at `level` (1..GAME_LEVELS). */
bool sfx_prize(int level, const int16_t **pcm, size_t *samples);
