/*
 * The vault's sound effects, synthesised at boot. See sfx.h.
 *
 * Everything is built from four ingredients:
 *
 *   mode()       one resonance of something struck: a decaying sine. Metal,
 *                glass and bells are a few of these at the right ratios.
 *   burst()      a short burst of noise: the impact itself.
 *   bandnoise()  noise through a resonant filter, swept if asked: rumble,
 *                rustle, a whoosh.
 *   tone()       a held note with harmonics: the crown's fanfare and the
 *                gold's shimmer.
 *
 * Each sound is drawn in LAYERS. A layer is drawn on its own, normalised to
 * its own peak, then added in -- so the balance between, say, the hinge's
 * creak and the door's thud is a number in the recipe, not an accident of
 * how many modes each happens to have. The finished sound is normalised to
 * SFX_LEVEL of full scale at volume 7, like the click (sound.c).
 *
 * The door is timed from app.h: bolts and latch while the haptic ramp runs,
 * the creak while the sprite swings, the thud on its last frame. It ends
 * before UNLOCK_REWARD_MS, when the prize's sound takes over.
 */

#include "sfx.h"

#include <math.h>
#include <string.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app.h"
#include "game.h"
#include "knob_board.h"

static const char *TAG = "sfx";

#define RATE       KNOB_BOARD_AUDIO_RATE
#define TW         (2.0f * (float)M_PI)
#define SFX_LEVEL  0.40f

#define DOOR_S     ((UNLOCK_DOOR_DONE_MS + 280) / 1000.0f)
#define PRIZE_S    2.0f
#define MAX_S      (DOOR_S > PRIZE_S ? DOOR_S : PRIZE_S)

typedef struct {
    const int16_t *pcm;
    size_t n;
    volatile bool ready;
} sfx_t;

static sfx_t s_door;
static sfx_t s_prize[GAME_LEVELS];

/* ---- The drawing surface ------------------------------------------------ */

static float *s_mix;          /* the sound being built */
static float *s_layer;        /* the layer being drawn */
static float *W;              /* where the helpers draw: one of the two */
static int    N;              /* samples in the current sound */
static uint32_t s_rng;

static int at(float t)
{
    const int n = (int)(t * RATE);
    return n < 0 ? 0 : n > N ? N : n;
}

static float rnd(void)        /* 0 .. 1 */
{
    s_rng = s_rng * 1664525u + 1013904223u;
    return (float)(s_rng >> 8) / 16777216.0f;
}

static float rnd2(void)       /* -1 .. 1 */
{
    return rnd() * 2.0f - 1.0f;
}

static void layer_begin(void)
{
    memset(s_layer, 0, N * sizeof(float));
    W = s_layer;
}

/* Add the layer in, its peak scaled to `peak`. */
static void layer_end(float peak)
{
    float m = 0.0f;
    for (int i = 0; i < N; i++) {
        m = fmaxf(m, fabsf(s_layer[i]));
    }
    const float k = m > 0.0f ? peak / m : 0.0f;
    for (int i = 0; i < N; i++) {
        s_mix[i] += s_layer[i] * k;
    }
    /* Let the idle task in: this runs at priority 1, and starving the idle
     * task trips the task watchdog. */
    vTaskDelay(1);
}

/* ---- Ingredients ---------------------------------------------------------- */

/* One resonance, struck at t0: a sine of `hz` dying away with time constant
 * `tau`. A rotating phasor, so no sinf() per sample. */
static void mode(float t0, float hz, float tau, float amp)
{
    if (hz <= 0.0f || hz >= RATE * 0.45f) {
        return;
    }
    const float w = TW * hz / RATE;
    const float c = cosf(w), s = sinf(w), d = expf(-1.0f / (tau * RATE));
    float re = amp, im = 0.0f;
    const int n1 = at(t0 + 7.0f * tau);
    for (int n = at(t0); n < n1; n++) {
        W[n] += im;
        const float r = (re * c - im * s) * d;
        im = (re * s + im * c) * d;
        re = r;
    }
}

/* A burst of noise at t0, dying with `tau`. `hp` (0..1) high-passes it:
 * 0 is raw, 0.9+ is a thin, bright tick. */
static void burst(float t0, float tau, float amp, float hp)
{
    const float d = expf(-1.0f / (tau * RATE));
    float e = amp, prev = 0.0f, y = 0.0f;
    const int n1 = at(t0 + 6.0f * tau);
    for (int n = at(t0); n < n1; n++) {
        const float x = rnd2() * e;
        y = hp > 0.0f ? hp * (y + x - prev) : x;
        prev = x;
        W[n] += y;
        e *= d;
    }
}

/* Noise from t0 to t1 through a resonant band-pass centred on hz0, gliding to
 * hz1, with bandwidth hz/q. Faded in over `att` and out over `rel`. The input
 * is scaled by the filter's noise gain, so `amp` is roughly the output's
 * level whatever the band. */
static void bandnoise(float t0, float t1, float hz0, float hz1, float q,
                      float amp, float att, float rel)
{
    const int n0 = at(t0), n1 = at(t1);
    const float dur = t1 - t0;
    float y1 = 0.0f, y2 = 0.0f, a1 = 0.0f, a2 = 0.0f, g = 0.0f;
    for (int n = n0; n < n1; n++) {
        const float t = (float)(n - n0) / RATE;
        if ((n - n0) % 16 == 0) {
            const float hz = hz0 + (hz1 - hz0) * t / dur;
            const float r = expf(-(float)M_PI * (hz / q) / RATE);
            a1 = 2.0f * r * cosf(TW * hz / RATE);
            a2 = r * r;
            /* Noise power gain of 1 / (1 - a1 z^-1 + a2 z^-2). */
            const float pg = (1.0f + a2) / ((1.0f - a2) * ((1.0f + a2) * (1.0f + a2) - a1 * a1));
            g = 1.0f / sqrtf(pg);
        }
        float env = fminf(1.0f, fminf(t / att, (dur - t) / rel));
        env = env < 0.0f ? 0.0f : env;
        const float y = rnd2() * amp * env * g + a1 * y1 - a2 * y2;
        y2 = y1;
        y1 = y;
        W[n] += y;
    }
}

/* A held note at t0 for `dur`: `nh` harmonics falling off as 1/k^tilt,
 * faded in over `att` and out over `rel`, with `vib` of vibrato (a fraction
 * of the pitch) easing in. */
static void tone(float t0, float dur, float hz, int nh, float tilt,
                 float att, float rel, float vib, float amp)
{
    const int n0 = at(t0), n1 = at(t0 + dur);
    float weight[16];
    nh = nh > 16 ? 16 : nh;
    for (int k = 1; k <= nh; k++) {
        weight[k - 1] = powf((float)k, -tilt);
    }
    float ph = 0.0f;
    for (int n = n0; n < n1; n++) {
        const float t = (float)(n - n0) / RATE;
        const float f = hz * (1.0f + vib * fminf(1.0f, t / 0.2f) * sinf(TW * 5.5f * t));
        ph += TW * f / RATE;
        if (ph > TW) {
            ph -= TW;
        }
        float v = 0.0f;
        for (int k = 1; k <= nh && k * f < RATE * 0.45f; k++) {
            v += sinf(k * ph) * weight[k - 1];
        }
        float env = fminf(1.0f, fminf(t / att, (dur - t) / rel));
        W[n] += v * amp * (env < 0.0f ? 0.0f : env);
    }
}

/* A metal bell or bar: inharmonic partials, the high ones dying first, and
 * a slightly detuned twin of the fundamental for the slow beat a real bell
 * has. */
static void bell(float t0, float hz, float tau, float amp)
{
    mode(t0, hz, tau, amp);
    mode(t0, hz * 1.003f, tau, amp * 0.5f);
    mode(t0, hz * 2.756f, tau * 0.40f, amp * 0.45f);
    mode(t0, hz * 5.404f, tau * 0.18f, amp * 0.25f);
    mode(t0, hz * 8.933f, tau * 0.08f, amp * 0.12f);
    burst(t0, 0.0015f, amp * 0.3f, 0.95f);
}

/* ---- The door --------------------------------------------------------------- */

/* The hinge: stick-slip. The door's weight drags the hinge pin, which grips
 * and lets go many times a second -- each release a tick that rings the
 * door's body (three resonances). The rate wanders, which is what makes it
 * a creak rather than a buzz. */
static void creak(float t0, float t1)
{
    static const float form[3] = { 620.0f, 1250.0f, 2400.0f };
    static const float bw[3]   = { 120.0f, 200.0f, 320.0f };
    static const float gain[3] = { 1.0f, 0.6f, 0.35f };
    float a1[3], a2[3], y1[3] = { 0 }, y2[3] = { 0 };
    for (int k = 0; k < 3; k++) {
        const float r = expf(-(float)M_PI * bw[k] / RATE);
        a1[k] = 2.0f * r * cosf(TW * form[k] / RATE);
        a2[k] = r * r;
    }
    const int n0 = at(t0), n1 = at(t1);
    const float dur = t1 - t0;
    float rate = 100.0f, target = 100.0f, next = 0.0f;
    for (int n = n0; n < n1; n++) {
        const float t = (float)(n - n0) / RATE;
        if ((n - n0) % 1500 == 0) {
            target = 60.0f + 90.0f * rnd();
        }
        rate += (target - rate) * 0.0015f;
        float env = fminf(1.0f, fminf(t / 0.25f, (dur - t) / 0.5f));
        env = (env < 0.0f ? 0.0f : env) *
              (0.6f + 0.4f * sinf(TW * 1.3f * t + 2.0f * sinf(TW * 0.4f * t)));
        float x = 0.0f;
        next -= 1.0f;
        if (next <= 0.0f) {
            x = env * (0.6f + 0.4f * rnd());
            next = RATE / rate * (0.85f + 0.3f * rnd());
        }
        float out = 0.0f;
        for (int k = 0; k < 3; k++) {
            const float y = x + a1[k] * y1[k] - a2[k] * y2[k];
            y2[k] = y1[k];
            y1[k] = y;
            out += y * gain[k];
        }
        W[n] += out;
    }
}

static void make_door(void)
{
    const float swing = UNLOCK_DOOR_START_MS / 1000.0f;
    const float done = UNLOCK_DOOR_DONE_MS / 1000.0f;

    /* Three bolts drawing back, with the haptic click and ramp. */
    layer_begin();
    for (int i = 0; i < 3; i++) {
        const float t = 0.15f * i;
        mode(t, 1150.0f, 0.025f, 1.0f);
        mode(t, 1730.0f, 0.018f, 0.7f);
        mode(t, 2650.0f, 0.012f, 0.5f);
        mode(t, 110.0f, 0.060f, 1.2f);
        burst(t, 0.002f, 1.5f, 0.9f);
    }
    layer_end(0.70f);
    /* ... sliding home ... */
    layer_begin();
    bandnoise(0.32f, 0.55f, 2200.0f, 1800.0f, 3.0f, 1.0f, 0.03f, 0.08f);
    layer_end(0.25f);
    /* ... and the main latch letting go. */
    layer_begin();
    mode(0.55f, 80.0f, 0.15f, 1.0f);
    mode(0.55f, 240.0f, 0.08f, 0.6f);
    mode(0.55f, 520.0f, 0.05f, 0.4f);
    burst(0.55f, 0.003f, 0.8f, 0.6f);
    layer_end(0.90f);
    /* The door swinging: the hinge creaking over the rumble of its weight. */
    layer_begin();
    creak(swing, done);
    layer_end(0.45f);
    layer_begin();
    bandnoise(swing, done, 70.0f, 60.0f, 1.5f, 1.0f, 0.3f, 0.4f);
    layer_end(0.30f);
    /* The thud as it reaches its stop. */
    layer_begin();
    mode(done, 55.0f, 0.30f, 1.0f);
    mode(done, 140.0f, 0.12f, 0.6f);
    mode(done, 900.0f, 0.08f, 0.25f);
    mode(done, 1460.0f, 0.05f, 0.15f);
    burst(done, 0.004f, 0.8f, 0.7f);
    layer_end(1.0f);
}

/* ---- The prizes ------------------------------------------------------------ */

/* 1 -- Coins: a handful pouring onto the pile, thinning out as it settles. */
static void make_coins(void)
{
    layer_begin();
    const int coins = 45;
    for (int i = 0; i < coins; i++) {
        const float t = 1.1f * powf((float)i / coins, 1.5f) + 0.02f * rnd();
        const float f = 2800.0f + 2200.0f * rnd();
        const float a = (0.3f + 0.7f * rnd()) * (1.0f - 0.5f * i / coins);
        mode(t, f, 0.06f + 0.08f * rnd(), a);
        mode(t, f * 1.47f, 0.04f, a * 0.6f);
        mode(t, f * 2.31f, 0.025f, a * 0.4f);
        burst(t, 0.0015f, a * 0.8f, 0.95f);
    }
    layer_end(1.0f);
}

/* 2 -- Silver: three bright bells climbing, then all of them together. */
static void make_silver(void)
{
    static const float notes[3] = { 1318.5f, 1661.2f, 1975.5f };   /* E6 G#6 B6 */
    layer_begin();
    for (int i = 0; i < 3; i++) {
        bell(0.14f * i, notes[i], 0.9f, 1.0f);
    }
    for (int i = 0; i < 3; i++) {
        bell(0.50f, notes[i], 1.0f, 0.7f);
    }
    bell(0.50f, 2637.0f, 1.0f, 0.5f);                              /* E7 */
    layer_end(1.0f);
}

/* 3 -- Gold: two heavy bars set down, then a warm golden shimmer. */
static void make_gold(void)
{
    layer_begin();
    for (int i = 0; i < 2; i++) {
        const float t = 0.2f * i;
        mode(t, 420.0f, 0.12f, 1.0f);
        mode(t, 1130.0f, 0.06f, 0.5f);
        mode(t, 2210.0f, 0.03f, 0.3f);
        mode(t, 90.0f, 0.09f, 1.0f);
        burst(t, 0.002f, 0.8f, 0.7f);
    }
    layer_end(1.0f);
    layer_begin();
    static const float chord[4] = { 523.25f, 659.25f, 783.99f, 1046.5f };  /* C major */
    for (int i = 0; i < 4; i++) {
        tone(0.35f, 1.6f, chord[i], 3, 2.0f, 0.12f, 1.0f, 0.004f, 1.0f);
    }
    for (int i = 0; i < 12; i++) {
        mode(0.40f + rnd(), 3000.0f + 3000.0f * rnd(), 0.05f, 0.4f + 0.6f * rnd());
    }
    layer_end(0.55f);
}

/* 4 -- Cash: a wad of notes riffled, then the till's ka-ching. */
static void make_cash(void)
{
    layer_begin();
    for (int i = 0; i < 26; i++) {
        const float t = 0.035f * i + 0.006f * rnd();
        const float a = fminf(1.0f, (i + 1) / 6.0f) * (1.0f - 0.4f * i / 26.0f);
        burst(t, 0.004f, a, 0.97f);
        mode(t, 1800.0f + 600.0f * rnd(), 0.006f, a * 0.3f);
    }
    layer_end(0.60f);
    layer_begin();                                           /* the drawer */
    mode(1.0f, 180.0f, 0.07f, 1.0f);
    mode(1.0f, 620.0f, 0.04f, 0.6f);
    burst(1.0f, 0.003f, 0.6f, 0.6f);
    layer_end(0.60f);
    layer_begin();                                           /* the bell */
    bell(1.06f, 2093.0f, 0.8f, 1.0f);                        /* C7 */
    bell(1.06f, 2637.0f, 0.6f, 0.6f);                        /* E7 */
    layer_end(1.0f);
}

/* 5 -- Gemstones: a quick climb of crystal notes, then twinkling. */
static void make_gems(void)
{
    static const int steps[11] = { 0, 2, 4, 7, 9, 12, 14, 16, 19, 21, 24 };   /* pentatonic */
    layer_begin();
    for (int i = 0; i < 11; i++) {
        const float t = 0.055f * i;
        const float f = 1046.5f * powf(2.0f, steps[i] / 12.0f);
        mode(t, f, 0.35f, 1.0f);
        mode(t, f * 2.0f, 0.15f, 0.3f);
        mode(t, f * 3.0f, 0.08f, 0.15f);
    }
    layer_end(1.0f);
    layer_begin();
    for (int i = 0; i < 25; i++) {
        mode(0.55f + 0.85f * rnd(), 4000.0f + 3500.0f * rnd(), 0.04f + 0.05f * rnd(),
             0.3f + 0.7f * rnd());
    }
    layer_end(0.50f);
}

/* 6 -- Pearls and jewellery: a string of pearls rattling into the hand,
 * then a little music-box phrase. */
static void make_pearls(void)
{
    layer_begin();
    for (int i = 0; i < 60; i++) {
        const float t = 0.6f * rnd() * rnd();               /* bunched at the start */
        const float a = 0.3f + 0.7f * rnd();
        mode(t, 1800.0f + 1200.0f * rnd(), 0.006f, a);
        burst(t, 0.001f, a * 0.5f, 0.9f);
    }
    layer_end(0.70f);
    static const float notes[4] = { 880.0f, 1108.7f, 1318.5f, 1760.0f };   /* A major */
    layer_begin();
    for (int i = 0; i < 4; i++) {
        const float t = 0.65f + 0.15f * i;
        mode(t, notes[i], 0.7f, 1.0f);
        mode(t, notes[i] * 2.0f, 0.25f, 0.4f);
        mode(t, notes[i] * 3.01f, 0.10f, 0.2f);
        burst(t, 0.001f, 0.3f, 0.9f);
    }
    layer_end(1.0f);
}

/* 7 -- A crown: a royal fanfare, G C E then a held G over the chord, with a
 * drum under the arrival. */
static void make_crown(void)
{
    static const float lead[3] = { 392.0f, 523.25f, 659.25f };   /* G4 C5 E5 */
    layer_begin();
    for (int i = 0; i < 3; i++) {
        tone(0.15f * i, 0.14f, lead[i], 8, 1.0f, 0.025f, 0.03f, 0.0f, 1.0f);
    }
    tone(0.45f, 1.30f, 783.99f, 8, 1.0f, 0.03f, 0.5f, 0.006f, 1.0f);   /* G5 */
    tone(0.45f, 1.30f, 523.25f, 8, 1.0f, 0.05f, 0.5f, 0.004f, 0.6f);   /* C5 */
    tone(0.45f, 1.30f, 659.25f, 8, 1.0f, 0.05f, 0.5f, 0.004f, 0.6f);   /* E5 */
    layer_end(1.0f);
    layer_begin();
    mode(0.45f, 98.0f, 0.35f, 1.0f);
    mode(0.45f, 147.0f, 0.20f, 0.5f);
    burst(0.45f, 0.01f, 0.6f, 0.3f);
    layer_end(0.60f);
}

/* 8 -- A diamond: a rising whoosh, one brilliant glassy ting, sparkles. */
static void make_diamond(void)
{
    layer_begin();
    bandnoise(0.0f, 0.30f, 800.0f, 6000.0f, 4.0f, 1.0f, 0.2f, 0.05f);
    layer_end(0.35f);
    layer_begin();
    const float f = 2637.0f;                                       /* E7 */
    mode(0.30f, f, 1.4f, 1.0f);
    mode(0.30f, f * 1.0015f, 1.4f, 0.5f);
    mode(0.30f, f * 2.32f, 0.6f, 0.5f);
    mode(0.30f, f * 4.25f, 0.3f, 0.3f);
    mode(0.30f, f * 6.63f, 0.15f, 0.15f);
    burst(0.30f, 0.001f, 0.4f, 0.95f);
    layer_end(1.0f);
    layer_begin();
    for (int i = 0; i < 30; i++) {
        const float t = 0.35f + 1.25f * rnd();
        mode(t, 5000.0f + 4000.0f * rnd(), 0.02f + 0.03f * rnd(),
             (0.3f + 0.7f * rnd()) * (1.6f - t));
    }
    layer_end(0.45f);
}

/* ---- Building ------------------------------------------------------------------ */

/* Build one sound with `recipe` into `out`, `secs` long. */
static void build(sfx_t *out, float secs, void (*recipe)(void), uint32_t seed)
{
    N = (int)(secs * RATE);
    s_rng = seed;
    memset(s_mix, 0, N * sizeof(float));
    recipe();

    int16_t *pcm = heap_caps_malloc(N * sizeof(int16_t), MALLOC_CAP_SPIRAM);
    if (!pcm) {
        ESP_LOGE(TAG, "no memory for a %.1f s sound", secs);
        return;
    }
    float m = 0.0f;
    for (int i = 0; i < N; i++) {
        m = fmaxf(m, fabsf(s_mix[i]));
    }
    const float k = m > 0.0f ? SFX_LEVEL * 32767.0f / m : 0.0f;
    for (int i = 0; i < N; i++) {
        const float fade = i > N - 256 ? (N - i) / 256.0f : 1.0f;   /* end on zero */
        pcm[i] = (int16_t)(s_mix[i] * k * fade);
    }
    out->pcm = pcm;
    out->n = N;
    out->ready = true;
}

static void sfx_task(void *arg)
{
    (void)arg;
    const int64_t t0 = esp_timer_get_time();
    const size_t max = (size_t)(MAX_S * RATE) * sizeof(float);
    s_mix = heap_caps_malloc(max, MALLOC_CAP_SPIRAM);
    s_layer = heap_caps_malloc(max, MALLOC_CAP_SPIRAM);
    if (s_mix && s_layer) {
        static void (*const prizes[GAME_LEVELS])(void) = {
            make_coins, make_silver, make_gold, make_cash,
            make_gems, make_pearls, make_crown, make_diamond,
        };
        build(&s_door, DOOR_S, make_door, 0x5AFE0000u);
        for (int i = 0; i < GAME_LEVELS; i++) {
            build(&s_prize[i], PRIZE_S, prizes[i], 0x5AFE0001u + i);
        }
        ESP_LOGI(TAG, "vault sounds ready in %lld ms",
                 (long long)((esp_timer_get_time() - t0) / 1000));
    } else {
        ESP_LOGE(TAG, "no memory to build the vault sounds");
    }
    heap_caps_free(s_mix);
    heap_caps_free(s_layer);
    s_mix = s_layer = NULL;
    vTaskDelete(NULL);
}

void sfx_start(void)
{
    /* Priority 1, on core 0: core 1 is LVGL's (knob_board.c), and the
     * display must not wait on this. */
    xTaskCreatePinnedToCore(sfx_task, "sfx", 4096, NULL, 1, NULL, 0);
}

bool sfx_door(const int16_t **pcm, size_t *samples)
{
    if (!s_door.ready) {
        return false;
    }
    *pcm = s_door.pcm;
    *samples = s_door.n;
    return true;
}

bool sfx_prize(int level, const int16_t **pcm, size_t *samples)
{
    if (level < 1 || level > GAME_LEVELS || !s_prize[level - 1].ready) {
        return false;
    }
    *pcm = s_prize[level - 1].pcm;
    *samples = s_prize[level - 1].n;
    return true;
}
