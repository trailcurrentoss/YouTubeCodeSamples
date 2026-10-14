/*
 * The game's sounds and the volume setting. See sound.h.
 */

#include "sound.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "nvs.h"

#include "bt_link.h"
#include "game.h"
#include "knob_board.h"
#include "sfx.h"

static const char *TAG = "sound";

/* ---- The lock-pin click ---------------------------------------------------
 *
 * Played when a number is hit: a lock pin dropping into place. A sharp
 * metallic strike (three decaying resonances over a burst of noise) with a
 * short low thud under it, then a smaller second strike PIN_SETTLE_MS later
 * as the pin settles. Peaks at CLICK_LEVEL of full scale at volume 7 --
 * noticeable, not loud; the knob's buzz is still the main
 * feedback. Generated once at boot. */
#define CLICK_MS       60
#define PIN_SETTLE_MS  18
#define CLICK_LEVEL    0.32f
#define CLICK_SAMPLES  (KNOB_BOARD_AUDIO_RATE * CLICK_MS / 1000)

static int16_t s_click[CLICK_SAMPLES];

/* The click's number in the Bluetooth link's sound table. */
#define SOUND_PIN 0

/* One strike starting at t0 (s), scaled by `amp`. */
static float strike(float t, float t0, float amp, uint32_t *rng)
{
    if (t < t0) {
        return 0.0f;
    }
    const float u = t - t0;
    const float tw = 2.0f * (float)M_PI;
    /* Metal: three partials, the higher ones dying faster. */
    const float ring = 0.50f * expf(-u / 0.0060f) * sinf(tw * 3200.0f * u)
                     + 0.30f * expf(-u / 0.0040f) * sinf(tw * 5100.0f * u)
                     + 0.20f * expf(-u / 0.0025f) * sinf(tw * 7400.0f * u);
    /* The impact: a 1-2 ms burst of noise. */
    *rng = *rng * 1664525u + 1013904223u;
    const float noise = (((float)(*rng >> 16) / 32768.0f) - 1.0f) * expf(-u / 0.0008f);
    /* Body: a low thud. */
    const float thud = 0.45f * expf(-u / 0.0050f) * sinf(tw * 420.0f * u);
    return amp * (ring + 0.8f * noise + thud);
}

static void make_click(void)
{
    uint32_t rng = 0x2545F491u;
    float peak = 0.0f;
    static float tmp[CLICK_SAMPLES];
    for (int i = 0; i < CLICK_SAMPLES; i++) {
        const float t = (float)i / KNOB_BOARD_AUDIO_RATE;
        tmp[i] = strike(t, 0.0f, 1.0f, &rng) + strike(t, PIN_SETTLE_MS / 1000.0f, 0.35f, &rng);
        if (fabsf(tmp[i]) > peak) {
            peak = fabsf(tmp[i]);
        }
    }
    /* Normalise to CLICK_LEVEL, and fade the tail so it ends at zero. */
    const float k = peak > 0.0f ? CLICK_LEVEL * 32767.0f / peak : 0.0f;
    for (int i = 0; i < CLICK_SAMPLES; i++) {
        const float fade = i > CLICK_SAMPLES - 64 ? (CLICK_SAMPLES - i) / 64.0f : 1.0f;
        s_click[i] = (int16_t)(tmp[i] * k * fade);
    }
}

/* ---- The siren --------------------------------------------------------------
 *
 * A police wail in step with the police lights on the dial: the pitch rises
 * from SIREN_LOW_HZ to SIREN_HIGH_HZ while one colour is up and falls back
 * while the other is (ui_game.c swaps them every 350 ms, so one sweep up and
 * down is SIREN_CYCLE_MS). A horn-like tone -- the 1st, 3rd and 5th
 * harmonics -- at SIREN_LEVEL of full scale at volume 7, faded in
 * and out so it starts and stops without a click. It lasts the whole loss
 * hold.
 *
 * The ESP32 co-processor synthesises the same siren for the headphones
 * (coprocessor/main/main.c); keep the two sets of numbers the same. */
#define SIREN_LOW_HZ     650.0f
#define SIREN_HIGH_HZ    1250.0f
#define SIREN_CYCLE_MS   700
#define SIREN_ATTACK_MS  20
#define SIREN_RELEASE_MS 150
#define SIREN_LEVEL      0.18f
#define SIREN_SAMPLES    (KNOB_BOARD_AUDIO_RATE / 1000 * GAME_WRONG_HOLD_MS)

/* 88 KB: in PSRAM. knob_board_audio_play() streams it from there. */
static int16_t *s_siren;

static float siren_wave(float ph)
{
    return sinf(ph) + sinf(3.0f * ph) / 3.0f + sinf(5.0f * ph) / 5.0f;
}

static void make_siren(void)
{
    s_siren = heap_caps_malloc(SIREN_SAMPLES * sizeof(int16_t), MALLOC_CAP_SPIRAM);
    if (!s_siren) {
        s_siren = malloc(SIREN_SAMPLES * sizeof(int16_t));
    }
    if (!s_siren) {
        ESP_LOGE(TAG, "no memory for the siren");
        return;
    }
    const float tw = 2.0f * (float)M_PI;
    const float mid = (SIREN_HIGH_HZ + SIREN_LOW_HZ) / 2.0f;
    const float depth = (SIREN_HIGH_HZ - SIREN_LOW_HZ) / 2.0f;
    /* The waveform's peak, so SIREN_LEVEL is the real peak. */
    float peak = 0.0f;
    for (int i = 0; i < 1024; i++) {
        peak = fmaxf(peak, fabsf(siren_wave(tw * i / 1024.0f)));
    }
    const float k = SIREN_LEVEL * 32767.0f / peak;
    const int attack = KNOB_BOARD_AUDIO_RATE / 1000 * SIREN_ATTACK_MS;
    const int release = KNOB_BOARD_AUDIO_RATE / 1000 * SIREN_RELEASE_MS;
    float ph = 0.0f;
    for (int i = 0; i < SIREN_SAMPLES; i++) {
        const float t = (float)i / KNOB_BOARD_AUDIO_RATE;
        /* Lowest at the start, so it rises while the first colour shows. */
        const float f = mid - depth * cosf(tw * t * 1000.0f / SIREN_CYCLE_MS);
        ph += tw * f / KNOB_BOARD_AUDIO_RATE;
        if (ph > tw) {
            ph -= tw;
        }
        float env = 1.0f;
        if (i < attack) {
            env = (float)i / attack;
        } else if (i > SIREN_SAMPLES - release) {
            env = (float)(SIREN_SAMPLES - i) / release;
        }
        s_siren[i] = (int16_t)(siren_wave(ph) * k * env);
    }
}

/* ---- Volume -----------------------------------------------------------------
 *
 * Eleven levels on a square-law curve, which sounds roughly even step to
 * step. UNITY_LEVEL plays the sounds as generated (gain 256); the default,
 * SOUND_VOLUME_DEFAULT, is quieter than that. Saved to NVS on every change,
 * so a power cut straight after one cannot lose it -- a byte, a few times a
 * minute at most, which NVS's wear levelling shrugs off. */
#define NVS_NS       "sound"
#define NVS_KEY      "volume"
#define UNITY_LEVEL  7

static int s_volume = SOUND_VOLUME_DEFAULT;

static uint16_t gain_q8(int level)
{
    const float r = (float)level / UNITY_LEVEL;
    return (uint16_t)lroundf(256.0f * r * r);
}

static void apply_volume(void)
{
    const uint16_t g = gain_q8(s_volume);
    knob_board_audio_set_gain(g);
    bt_link_set_gain(g);
}

static void save_volume(void)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (err == ESP_OK) {
        err = nvs_set_u8(h, NVS_KEY, (uint8_t)s_volume);
        if (err == ESP_OK) {
            err = nvs_commit(h);
        }
        nvs_close(h);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "volume %d not saved: %s", s_volume, esp_err_to_name(err));
    }
}

static void load_volume(void)
{
    nvs_handle_t h;
    uint8_t v;
    if (nvs_open(NVS_NS, NVS_READONLY, &h) == ESP_OK) {
        if (nvs_get_u8(h, NVS_KEY, &v) == ESP_OK && v <= SOUND_VOLUME_MAX) {
            s_volume = v;
        }
        nvs_close(h);
    }
}

/* ---- Playing ---------------------------------------------------------------- */

void sound_init(void)
{
    make_click();
    make_siren();
    sfx_start();
    bt_link_set_clip(SOUND_PIN, s_click, CLICK_SAMPLES, KNOB_BOARD_AUDIO_RATE);
    load_volume();
    apply_volume();
    ESP_LOGI(TAG, "volume %d", s_volume);
}

void sound_pin(void)
{
    if (bt_link_connected()) {
        bt_link_play(SOUND_PIN);
    } else {
        knob_board_audio_play(s_click, CLICK_SAMPLES);
    }
}

void sound_siren(void)
{
    if (bt_link_connected()) {
        bt_link_siren(GAME_WRONG_HOLD_MS);
    } else if (s_siren) {
        knob_board_audio_play(s_siren, SIREN_SAMPLES);
    }
}

/* The vault's sounds are too long to upload to the headphones; they play on
 * the board, and while headphones are connected bt_link streams them there
 * as they play (the wired output is muted meanwhile). */
void sound_vault_door(void)
{
    const int16_t *pcm;
    size_t n;
    if (sfx_door(&pcm, &n)) {
        knob_board_audio_play(pcm, n);
    }
}

void sound_prize(int level)
{
    const int16_t *pcm;
    size_t n;
    if (sfx_prize(level, &pcm, &n)) {
        knob_board_audio_play(pcm, n);
    }
}

void sound_stop(void)
{
    knob_board_audio_stop();
    if (bt_link_connected()) {
        bt_link_siren(0);
    }
}

int sound_volume(void)
{
    return s_volume;
}

void sound_set_volume(int level)
{
    level = level < 0 ? 0 : level > SOUND_VOLUME_MAX ? SOUND_VOLUME_MAX : level;
    if (level == s_volume) {
        return;
    }
    s_volume = level;
    apply_volume();
    save_volume();
    sound_pin();
}
