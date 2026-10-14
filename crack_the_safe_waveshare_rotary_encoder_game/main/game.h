/*
 * Crack the Safe -- the game itself, with no LVGL and no hardware in it.
 *
 * Any turn or tap on the title shows how to play; touching Start there deals
 * the first code. The knob does nothing on the rules, so the turn that left
 * the title cannot skip them.
 *
 * A random combination of GAME_CODE_LEN numbers on a GAME_POSITIONS dial.
 * Number 1 is found turning right, number 2 turning left, number 3 turning
 * right again. Landing on the number in the required direction gives a haptic
 * click; reversing right there locks it in. Turning past the number, or
 * reversing anywhere else, is game over: the screen holds red, then a new
 * game starts -- fresh numbers, the dial back at 0.
 * The last number is found the same way, and turning past it loses just the
 * same; the safe opens when the dial is held on it for GAME_OPEN_DWELL_MS, or
 * reversed on it.
 *
 * Against the clock, in eight levels: 5:00, 2:30, 1:00, 0:30, then 0:20 for
 * levels 5-8 (halving would leave no time to turn the dial at all; from
 * level 5 the haptic "feel" range narrows instead -- app.c). Opening the safe
 * moves to the next level (after level 8, back to level 1). Running out of time is a loss like any other; a loss
 * replays the same level with fresh numbers.
 *
 * Speed matters, like a real dial: turned slowly the hit on the number is
 * always felt, but the faster the knob is turning the more likely it comes
 * with no feedback at all -- and carrying on past the number is a loss
 * whether it was felt or not.
 *
 * The caller feeds in detents and the clock; the game reports what happened
 * as a set of events (haptics to play, screens to show) and exposes its
 * state for the UI to read.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#define GAME_POSITIONS   40
#define GAME_CODE_LEN    3
#define GAME_DEG_PER_POS (360 / GAME_POSITIONS)
#define GAME_LEVELS      8

typedef enum {
    GAME_TITLE = 0,  /* attract screen, any turn or tap shows the rules */
    GAME_INSTRUCTIONS, /* how to play; only game_start() leaves it */
    GAME_PLAY,       /* searching for the current number */
    GAME_WRONG,      /* went past / reversed off target -- held red, then a
                      * new game */
    GAME_UNLOCKED,   /* door opening, then the reward */
} game_mode_t;

/* Things that happened during one call, OR-ed together. */
typedef enum {
    GAME_EV_NONE     = 0,
    GAME_EV_STARTED  = 1 << 0,  /* a new code was dealt */
    GAME_EV_HIT      = 1 << 1,  /* reached the number in the right direction */
    GAME_EV_LOCKED   = 1 << 2,  /* reversed on the number: it is locked in */
    GAME_EV_WRONG    = 1 << 3,  /* game over for this code */
    GAME_EV_UNLOCKED = 1 << 4,  /* last number reached: the safe opens */
    GAME_EV_RESUMED  = 1 << 5,  /* the red hold ended: a new game (with STARTED) */
    GAME_EV_TIMEOUT  = 1 << 6,  /* the clock ran out (with WRONG) */
    GAME_EV_RESET    = 1 << 7,  /* back to the title */
    GAME_EV_INSTRUCTIONS = 1 << 8,  /* the title gave way to the rules */
} game_event_t;

typedef struct {
    game_mode_t mode;
    int32_t  ticks;                   /* detents since boot, + = clockwise */
    uint8_t  code[GAME_CODE_LEN];
    uint8_t  stage;                   /* index of the number being searched */
    uint8_t  locked;                  /* numbers locked in so far */
    uint8_t  travel;                  /* detents moved in the stage direction */
    bool     on_target;               /* resting on code[stage], stage direction */
    bool     hit_felt;                /* ... and the player was told (HIT) */
    uint16_t attempts;                /* codes tried, including the current one */
    uint8_t  level;                   /* 1..GAME_LEVELS */
    bool     timed_out;               /* the current loss was the clock, not a turn */
    uint32_t started_ms;              /* first code dealt in this round */
    uint32_t finished_ms;             /* safe opened */
    uint32_t mode_since_ms;           /* when the current mode began */
    uint32_t last_turn_ms;            /* when the previous detents arrived */
    uint32_t speed_dps;               /* smoothed turning speed, detents/s */
    bool     paused;                  /* the clock is stopped (a menu is up) */
    uint32_t paused_ms;               /* ... since this moment */
} game_t;

/* Turning at or below FEEL_SLOW_DPS the number always clicks; at or above
 * FEEL_FAST_DPS it never does; in between the chance falls off linearly. */
#define GAME_FEEL_SLOW_DPS  4
#define GAME_FEEL_FAST_DPS  16

/* Resting on the last number this long opens the safe. */
#define GAME_OPEN_DWELL_MS     400

/* How long the red "wrong" state -- and the police lights -- are held
 * before play resumes. */
#define GAME_WRONG_HOLD_MS     2000
/* Inputs are ignored this long after opening, so the turn that cracked the
 * safe does not immediately start the next game. */
#define GAME_REPLAY_LOCKOUT_MS 3500

void game_init(game_t *g, uint32_t seed);

/** One or more detents (+ clockwise). Returns game_event_t bits. */
unsigned game_rotate(game_t *g, int detents, uint32_t now_ms);

/** A tap on the glass. Shows the rules from the title; starts the next game
 *  from the reward screen. */
unsigned game_tap(game_t *g, uint32_t now_ms);

/** The Start button on the rules: the first game begins. */
unsigned game_start(game_t *g, uint32_t now_ms);

/** "New Game" from the menu: level 1, a fresh code, the dial at 0. */
unsigned game_new(game_t *g, uint32_t now_ms);

/**
 * Stop and restart the clock, for a menu shown over the game. While paused
 * the caller must not feed in turns, taps or ticks; on resume every timer --
 * the countdown, the red hold, the unlock animation -- carries on from where
 * it stopped.
 */
void game_pause(game_t *g, uint32_t now_ms);
void game_resume(game_t *g, uint32_t now_ms);

/** Back to the start: the title screen, level 1 next, the dial at 0. */
unsigned game_reset(game_t *g, uint32_t now_ms);

/** Advance timers (the red hold). Call regularly. */
unsigned game_tick(game_t *g, uint32_t now_ms);

/** The number under the index, 0..GAME_POSITIONS-1. */
int game_dial_value(const game_t *g);

/** Direction the current stage must be turned: +1 right, -1 left. */
int game_stage_dir(const game_t *g);

/** Detents still to turn, in the stage direction, to reach the number being
 *  searched: 0 on it. -1 outside play. */
int game_distance(const game_t *g);

/** Seconds from the first code to now, or to the opening once unlocked. */
uint32_t game_elapsed_s(const game_t *g, uint32_t now_ms);

/** The time limit for a level, in milliseconds. */
uint32_t game_level_ms(int level);

/** Milliseconds left on the clock: counts down in play, frozen at the
 *  moment of opening once unlocked, 0 once it has run out. */
uint32_t game_time_left_ms(const game_t *g, uint32_t now_ms);

/** The level the next game will be (after a win, the one after). */
int game_next_level(const game_t *g);
