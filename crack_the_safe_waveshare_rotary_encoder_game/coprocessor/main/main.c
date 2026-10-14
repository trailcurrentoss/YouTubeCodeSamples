/*
 * Crack the Safe -- ESP32-U4WDH co-processor: Bluetooth headphones.
 *
 * The ESP32-S3 runs the game and owns the board's DAC. The S3 has no Classic
 * Bluetooth, and headphones need it (A2DP), so this chip is the game's
 * Bluetooth radio: it finds headphones, pairs with the one the player picks,
 * remembers it, and plays the game's sounds to it.
 *
 * It talks to the S3 over a UART, one text line per message:
 *
 *   S3 -> here                         here -> S3
 *   HELLO                              READY <version>
 *   SCAN      search for headphones    SCAN start | SCAN done
 *   STOP      stop searching           DEV <addr> <rssi> <name>
 *   CONNECT <addr>                     STATE idle|connecting|connected <addr|-> <name>
 *   FORGET    disconnect, unremember   SAVED <addr|-> <name>   the remembered one
 *                                      FAIL      a CONNECT did not succeed
 *   CLIP <id> <rate> <base64 pcm>      (sounds, signed 16-bit mono, sent once)
 *   PLAY <id>                          (play a sound to the headphones)
 *   VOL <gain>                         (volume: 256 = as uploaded, 0 = silent)
 *   SIREN <ms>                         (the police siren, synthesised here; 0 stops)
 *   PCM <crc> <base64 pcm>             (live audio: 16-bit mono at 22050 Hz, as the
 *                                       board plays it -- the vault's sounds; <crc>
 *                                       is 4 hex digits, esp_rom_crc16_le of the pcm)
 *                                      LOG <text>  a line of this chip's own log
 *
 * Every line starts with an upper-case keyword; both ends skip anything
 * before the first capital, which absorbs the junk byte a UART pin can
 * produce while it is being set up at boot.
 *
 * A search drops connected headphones first. With a connection up -- even
 * with the stream paused -- this controller's inquiry hears nothing at all,
 * so the board searches alone and reconnects afterwards (RECONNECT_S) if the
 * player picks nothing new. While searching, and whenever no headphones are
 * remembered, the board is not connectable: headphones that still remember
 * it cannot reconnect themselves into the middle of a search, or after a
 * FORGET.
 *
 * The board's PCM5100A DAC belongs to the S3, but its soft-mute line (XSMT)
 * is wired to this chip. It is held HIGH (playing) except while headphones
 * are connected, when it goes LOW: sound goes to the headphones only.
 *
 * Pins, from the board schematic (page 3, "ESP32-CHIP"):
 *   IO32  XSMT            to the DAC's soft mute, high = play
 *   IO23  ESP32S3_RX net  this chip's TX -> S3 GPIO48
 *   IO18  ESP32S3_TX net  this chip's RX <- S3 GPIO38
 * IO25/26/27 (this chip's own I2S path to the DAC) and IO19/22 (the second
 * knob contact pair) are left alone.
 */

#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_log.h"
#include "esp_rom_crc.h"
#include "esp_timer.h"
#include "mbedtls/base64.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "cts_bt";

#define FW_VERSION      "4"
#define DEVICE_NAME     "Crack the Safe"

#define PIN_XSMT        GPIO_NUM_32
#define PIN_LINK_TX     GPIO_NUM_23
#define PIN_LINK_RX     GPIO_NUM_18
#define LINK_UART       UART_NUM_1
#define LINK_BAUD       921600        /* fast enough for PCM lines */
#define LINK_LINE_MAX   4096          /* a CLIP line: ~3.6 KB of base64 */

#define BT_NAME_MAX        48            /* names as sent to the S3 */
#define SEEN_MAX        16            /* devices remembered per search */
#define INQUIRY_LEN     10            /* x 1.28 s = 12.8 s search */
#define RECONNECT_S     15            /* retry the saved headphones this often, */
#define RECONNECT_SLOW_S 60           /* ... and this often after a few misses */
#define RECONNECT_FAST  3

/* A2DP source streams 44.1 kHz, 16-bit stereo (SBC's default). */
#define OUT_RATE        44100
#define CLIPS           4
#define VOICES          4

/* ---- Events into the control task ------------------------------------ */

typedef enum {
    EV_LINE,           /* a command from the S3 */
    EV_FOUND,          /* a search result */
    EV_SEARCH_DONE,
    EV_CONN,           /* A2DP connection state changed */
    EV_MEDIA_ACK,      /* A2DP media control acknowledged */
    EV_TICK,           /* once a second */
    EV_NAME,           /* a remote name request came back */
    EV_AVRC,           /* the AVRCP (remote control) link came up */
} ev_type_t;

typedef struct {
    ev_type_t type;
    union {
        char line[80];
        struct {
            esp_bd_addr_t bda;
            int8_t rssi;
            char name[BT_NAME_MAX];
        } found;
        struct {
            esp_bd_addr_t bda;
            esp_a2d_connection_state_t state;
        } conn;
        struct {
            esp_a2d_media_ctrl_t cmd;
            esp_a2d_media_ctrl_ack_t status;
        } ack;
    };
} ev_t;

static QueueHandle_t s_ev;

/* ---- Link to the S3 -------------------------------------------------- */

static SemaphoreHandle_t s_tx_lock;

static void link_send(const char *fmt, ...)
{
    char buf[128];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
    va_end(ap);
    if (n < 0) {
        return;
    }
    if (n > (int)sizeof(buf) - 2) {
        n = sizeof(buf) - 2;
    }
    buf[n++] = '\n';
    xSemaphoreTake(s_tx_lock, portMAX_DELAY);
    uart_write_bytes(LINK_UART, buf, n);
    xSemaphoreGive(s_tx_lock);
    ESP_LOGI(TAG, "-> %.*s", n - 1, buf);
}

/* Every line of this chip's log also goes to the S3 as "LOG <line>", which
 * prints it on its own USB port. This chip's USB is on the other side of the
 * plug, and turning the plug over power-cycles the board -- so without this
 * there is no way to watch it while something is going wrong. The command
 * echoes ("-> ", "<- ") are left out: the S3 already sees those lines. */
static vprintf_like_t s_prev_vprintf;

static int log_forward(const char *fmt, va_list ap)
{
    va_list copy;
    va_copy(copy, ap);
    const int r = s_prev_vprintf(fmt, ap);
    char line[160] = "LOG ";
    int n = vsnprintf(line + 4, sizeof(line) - 5, fmt, copy);
    va_end(copy);
    if (n <= 0 || !s_tx_lock || xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return r;
    }
    n = strlen(line);
    /* One line, printable: drop colour codes and the trailing newline. */
    int w = 4;
    for (int i = 4; i < n; i++) {
        if (line[i] == '\033') {
            while (i < n && line[i] != 'm') {
                i++;
            }
        } else if (line[i] >= ' ') {
            line[w++] = line[i];
        }
    }
    if (strstr(line, "cts_bt: -> ") || strstr(line, "cts_bt: <- ")) {
        return r;
    }
    line[w++] = '\n';
    xSemaphoreTake(s_tx_lock, portMAX_DELAY);
    uart_write_bytes(LINK_UART, line, w);
    xSemaphoreGive(s_tx_lock);
    return r;
}

static void bda_str(const esp_bd_addr_t bda, char out[18])
{
    snprintf(out, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
             bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
}

static bool parse_bda(const char *s, esp_bd_addr_t out)
{
    unsigned v[6];
    if (sscanf(s, "%2x:%2x:%2x:%2x:%2x:%2x", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) != 6) {
        return false;
    }
    for (int i = 0; i < 6; i++) {
        out[i] = (uint8_t)v[i];
    }
    return true;
}

/* ---- The mixer: sounds the S3 uploaded, played into the A2DP stream ---- */

typedef struct {
    int16_t *pcm;
    uint32_t n;
    uint32_t step_q16;     /* source samples per output sample, 16.16 */
} clip_t;

typedef struct {
    bool on;
    uint8_t clip;
    uint32_t pos_q16;
} voice_t;

static clip_t s_clips[CLIPS];
static voice_t s_voices[VOICES];
static portMUX_TYPE s_mix_lock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint16_t s_gain_q8 = 256;      /* VOL */

/* Headphones get the mix HEADPHONE_BOOST times louder than the wired jack.
 * The jack is line level into an amplifier that adds its own gain;
 * headphones play the stream as it is, where the game's quiet effects (the
 * siren peaks at -25 dBFS at volume 4) are faint. Peaks above LIMIT_KNEE of
 * full scale are rounded off rather than clipped, so the top volume levels
 * stay clean. */
#define HEADPHONE_BOOST  3.0f
/* The headphones' own volume (AVRCP absolute volume, 0..127), set when they
 * connect -- see avrc_ct_cb(). */
#define HEADPHONE_VOLUME 100
#define AVRC_TL_VOLUME   1
#define LIMIT_KNEE       0.6f

/* ---- Live audio from the S3 ("PCM" lines) -------------------------------
 *
 * A ring of 22050 Hz samples, filled by the UART task and drained by the A2DP
 * callback at twice the rate (each sample becomes two output frames, the
 * first halfway from the last). Playback waits until STREAM_PREBUF samples
 * have arrived -- or the line has gone quiet, for a sound shorter than that
 * -- so the lines' arrival jitter does not break the sound up. */
#define STREAM_RATE    22050
#define STREAM_CAP     8192          /* 370 ms */
#define STREAM_PREBUF  2048          /* 93 ms */
#define STREAM_IDLE_US (60 * 1000)

static int16_t s_stream[STREAM_CAP];
static volatile uint32_t s_stream_head, s_stream_tail;  /* free-running */
static volatile int64_t s_stream_rx_us;

/* Per sound, logged once the stream goes quiet (stream_report()): lines
 * received, lines dropped as damaged, and times playback ran dry early. */
static volatile uint32_t s_stream_lines, s_stream_bad, s_stream_dry;

/* "PCM <crc> <base64>". Runs on the UART task. A line that lost or garbled
 * bytes on the wire is dropped: played, a byte lost mid-line misaligns every
 * sample after it into full-scale noise. */
static void stream_push(const char *args)
{
    static int16_t pcm[1024];
    s_stream_lines++;
    char *b64;
    const unsigned long crc = strtoul(args, &b64, 16);
    size_t bytes = 0;
    if (b64 != args + 4 || *b64 != ' ' ||
        mbedtls_base64_decode((unsigned char *)pcm, sizeof(pcm), &bytes,
                              (const unsigned char *)b64 + 1, strlen(b64 + 1)) != 0 ||
        (bytes & 1) || esp_rom_crc16_le(0, (const uint8_t *)pcm, bytes) != crc) {
        s_stream_bad++;
        return;
    }
    const uint32_t n = bytes / 2;
    uint32_t head = s_stream_head;
    const uint32_t tail = s_stream_tail;
    for (uint32_t i = 0; i < n && head - tail < STREAM_CAP; i++) {
        s_stream[head % STREAM_CAP] = pcm[i];
        head++;
    }
    s_stream_head = head;
    s_stream_rx_us = esp_timer_get_time();
}

/* Add the live audio to `frames` of 44.1 kHz stereo. Bluetooth task. */
static void stream_mix(int16_t *out, int frames)
{
    static bool s_on;
    static int16_t s_last;
    uint32_t tail = s_stream_tail;
    const uint32_t fill = s_stream_head - tail;
    if (!s_on) {
        if (fill >= STREAM_PREBUF ||
            (fill > 0 && esp_timer_get_time() - s_stream_rx_us > STREAM_IDLE_US)) {
            s_on = true;
        } else {
            return;
        }
    }
    for (int f = 0; f + 1 < frames; f += 2) {
        if (s_stream_head == tail) {
            s_on = false;                    /* drained: wait to refill */
            s_stream_dry++;
            break;
        }
        const int16_t v = s_stream[tail % STREAM_CAP];
        tail++;
        const int16_t mid = (int16_t)((s_last + v) / 2);
        s_last = v;
        int32_t a = out[2 * f] + mid;
        int32_t b = out[2 * f + 2] + v;
        a = a > 32767 ? 32767 : a < -32768 ? -32768 : a;
        b = b > 32767 ? 32767 : b < -32768 ? -32768 : b;
        out[2 * f] = out[2 * f + 1] = (int16_t)a;
        out[2 * f + 2] = out[2 * f + 3] = (int16_t)b;
    }
    s_stream_tail = tail;
    if (!s_on) {
        s_last = 0;
    }
}

/* Log the last sound's stream health once it has gone quiet. Control task. */
static void stream_report(void)
{
    if (!s_stream_lines || esp_timer_get_time() - s_stream_rx_us < 500 * 1000) {
        return;
    }
    /* The end of every sound drains the ring once; more is a dropout. */
    const uint32_t dry = s_stream_dry;
    ESP_LOGI(TAG, "stream: %" PRIu32 " lines, %" PRIu32 " damaged, %" PRIu32 " dropouts",
             s_stream_lines, s_stream_bad, dry ? dry - 1 : 0);
    s_stream_lines = s_stream_bad = s_stream_dry = 0;
}

static int16_t limit(float x)          /* x in full-scale units */
{
    const float a = fabsf(x);
    if (a > LIMIT_KNEE) {
        const float over = (a - LIMIT_KNEE) / (1.0f - LIMIT_KNEE);
        const float y = LIMIT_KNEE + (1.0f - LIMIT_KNEE) * tanhf(over);
        x = x < 0.0f ? -y : y;
    }
    return (int16_t)(x * 32767.0f);
}

/* ---- The siren ----------------------------------------------------------
 *
 * Too long to upload as a CLIP (a line holds ~70 ms of sound), so it is
 * synthesised here: a police wail sweeping SIREN_LOW_HZ..SIREN_HIGH_HZ and
 * back every SIREN_CYCLE_MS, in step with the lights on the S3's screen, as
 * the 1st, 3rd and 5th harmonics. The S3 makes the same sound for its own
 * output (main/sound.c); keep the two sets of numbers the same.
 *
 * A wavetable and a phase accumulator keep it cheap enough for the A2DP
 * callback; the pitch is updated every SIREN_STEP frames. */
#define SIREN_LOW_HZ     650.0f
#define SIREN_HIGH_HZ    1250.0f
#define SIREN_CYCLE_MS   700
#define SIREN_ATTACK_MS  20
#define SIREN_RELEASE_MS 150
#define SIREN_LEVEL      0.18f
#define SIREN_TABLE      256
#define SIREN_STEP       32

static int16_t s_siren_table[SIREN_TABLE];
static volatile int32_t s_siren_req = -1;  /* SIREN <ms>, for the callback to take */

static float siren_wave(float ph)
{
    return sinf(ph) + sinf(3.0f * ph) / 3.0f + sinf(5.0f * ph) / 5.0f;
}

static void siren_init(void)
{
    const float tw = 2.0f * (float)M_PI;
    float peak = 0.0f;
    for (int i = 0; i < SIREN_TABLE; i++) {
        peak = fmaxf(peak, fabsf(siren_wave(tw * i / SIREN_TABLE)));
    }
    for (int i = 0; i < SIREN_TABLE; i++) {
        s_siren_table[i] = (int16_t)(siren_wave(tw * i / SIREN_TABLE) * SIREN_LEVEL * 32767.0f / peak);
    }
}

/* Add the siren, if one is sounding, to `frames` of stereo. Bluetooth task. */
static void siren_mix(int16_t *out, int frames, int32_t gain)
{
    static uint32_t s_pos, s_len, s_phase, s_inc;
    const int32_t req = s_siren_req;
    if (req >= 0) {
        s_siren_req = -1;
        s_pos = 0;
        s_len = (uint32_t)((uint64_t)req * OUT_RATE / 1000);
    }
    const uint32_t attack = OUT_RATE / 1000 * SIREN_ATTACK_MS;
    const uint32_t release = OUT_RATE / 1000 * SIREN_RELEASE_MS;
    const float mid = (SIREN_HIGH_HZ + SIREN_LOW_HZ) / 2.0f;
    const float depth = (SIREN_HIGH_HZ - SIREN_LOW_HZ) / 2.0f;
    for (int f = 0; f < frames && s_pos < s_len; f++, s_pos++) {
        if (s_pos % SIREN_STEP == 0) {
            const float t = (float)s_pos / OUT_RATE;
            const float hz = mid - depth * cosf(2.0f * (float)M_PI * t * 1000.0f / SIREN_CYCLE_MS);
            s_inc = (uint32_t)(hz / OUT_RATE * 4294967296.0f);
        }
        int32_t v = s_siren_table[s_phase >> 24];
        s_phase += s_inc;
        if (s_pos < attack) {
            v = v * (int32_t)s_pos / (int32_t)attack;
        } else if (s_len - s_pos < release) {
            v = v * (int32_t)(s_len - s_pos) / (int32_t)release;
        }
        int32_t l = out[2 * f] + ((v * gain) >> 8);
        l = l > 32767 ? 32767 : l < -32768 ? -32768 : l;
        out[2 * f] = out[2 * f + 1] = (int16_t)l;
    }
}

/* "CLIP <id> <rate> <base64>": replaces sound <id>. Runs on the UART task. */
static void clip_load(char *args)
{
    char *save = NULL;
    const char *id_s = strtok_r(args, " ", &save);
    const char *rate_s = strtok_r(NULL, " ", &save);
    const char *b64 = strtok_r(NULL, " ", &save);
    if (!id_s || !rate_s || !b64) {
        ESP_LOGW(TAG, "bad CLIP");
        return;
    }
    const int id = atoi(id_s);
    const uint32_t rate = (uint32_t)strtoul(rate_s, NULL, 10);
    if (id < 0 || id >= CLIPS || rate < 4000 || rate > 48000) {
        ESP_LOGW(TAG, "CLIP %d at %" PRIu32 " Hz rejected", id, rate);
        return;
    }
    const size_t b64_len = strlen(b64);
    size_t bytes = 0;
    mbedtls_base64_decode(NULL, 0, &bytes, (const unsigned char *)b64, b64_len);
    int16_t *pcm = malloc(bytes + 2);
    if (!pcm || mbedtls_base64_decode((unsigned char *)pcm, bytes + 2, &bytes,
                                      (const unsigned char *)b64, b64_len) != 0) {
        free(pcm);
        ESP_LOGW(TAG, "CLIP %d: bad base64", id);
        return;
    }
    clip_t fresh = {
        .pcm = pcm,
        .n = (uint32_t)(bytes / 2),
        .step_q16 = (uint32_t)(((uint64_t)rate << 16) / OUT_RATE),
    };
    portENTER_CRITICAL(&s_mix_lock);
    int16_t *old = s_clips[id].pcm;
    s_clips[id] = fresh;
    for (int v = 0; v < VOICES; v++) {
        if (s_voices[v].clip == id) {
            s_voices[v].on = false;
        }
    }
    portEXIT_CRITICAL(&s_mix_lock);
    free(old);
    ESP_LOGI(TAG, "clip %d: %" PRIu32 " samples at %" PRIu32 " Hz", id, fresh.n, rate);
}

static void clip_play(int id)
{
    if (id < 0 || id >= CLIPS) {
        return;
    }
    portENTER_CRITICAL(&s_mix_lock);
    if (s_clips[id].pcm) {
        /* A free voice, or else the one furthest through its sound. */
        int pick = 0;
        for (int v = 0; v < VOICES; v++) {
            if (!s_voices[v].on) {
                pick = v;
                break;
            }
            if (s_voices[v].pos_q16 > s_voices[pick].pos_q16) {
                pick = v;
            }
        }
        s_voices[pick] = (voice_t){ .on = true, .clip = (uint8_t)id, .pos_q16 = 0 };
    }
    portEXIT_CRITICAL(&s_mix_lock);
}

/* The A2DP source asks for `len` bytes of 44.1 kHz 16-bit stereo. Silence,
 * with whatever sounds are playing mixed in. Runs on the Bluetooth task. */
static int32_t a2d_data_cb(uint8_t *data, int32_t len)
{
    if (!data || len < 4) {
        return 0;
    }
    int16_t *out = (int16_t *)data;
    const int frames = len / 4;
    memset(data, 0, len);
    const int32_t gain = s_gain_q8;
    portENTER_CRITICAL(&s_mix_lock);
    for (int v = 0; v < VOICES; v++) {
        voice_t *vo = &s_voices[v];
        if (!vo->on) {
            continue;
        }
        const clip_t *c = &s_clips[vo->clip];
        for (int f = 0; f < frames; f++) {
            const uint32_t i = vo->pos_q16 >> 16;
            if (!c->pcm || i >= c->n) {
                vo->on = false;
                break;
            }
            int32_t l = out[2 * f] + ((c->pcm[i] * gain) >> 8);
            l = l > 32767 ? 32767 : l < -32768 ? -32768 : l;
            out[2 * f] = out[2 * f + 1] = (int16_t)l;
            vo->pos_q16 += c->step_q16;
        }
    }
    portEXIT_CRITICAL(&s_mix_lock);
    siren_mix(out, frames, gain);
    stream_mix(out, frames);             /* volume already applied by the S3 */
    for (int f = 0; f < frames; f++) {
        if (out[2 * f]) {
            out[2 * f] = out[2 * f + 1] = limit(out[2 * f] * (HEADPHONE_BOOST / 32768.0f));
        }
    }
    return frames * 4;
}

/* ---- Saved headphones (NVS) ------------------------------------------ */

static bool saved_load(esp_bd_addr_t bda, char name[BT_NAME_MAX])
{
    nvs_handle_t h;
    if (nvs_open("bt", NVS_READONLY, &h) != ESP_OK) {
        return false;
    }
    size_t len = ESP_BD_ADDR_LEN;
    bool ok = nvs_get_blob(h, "peer", bda, &len) == ESP_OK && len == ESP_BD_ADDR_LEN;
    len = BT_NAME_MAX;
    if (ok && nvs_get_str(h, "name", name, &len) != ESP_OK) {
        name[0] = '\0';
    }
    nvs_close(h);
    return ok;
}

static void saved_store(const esp_bd_addr_t bda, const char *name)
{
    nvs_handle_t h;
    if (nvs_open("bt", NVS_READWRITE, &h) != ESP_OK) {
        return;
    }
    nvs_set_blob(h, "peer", bda, ESP_BD_ADDR_LEN);
    nvs_set_str(h, "name", name);
    nvs_commit(h);
    nvs_close(h);
}

static void saved_erase(void)
{
    nvs_handle_t h;
    if (nvs_open("bt", NVS_READWRITE, &h) == ESP_OK) {
        nvs_erase_all(h);
        nvs_commit(h);
        nvs_close(h);
    }
}

/* ---- Control state: touched only by the control task ------------------ */

typedef enum { CONN_IDLE, CONN_CONNECTING, CONN_CONNECTED } conn_t;

static conn_t s_conn;
static esp_bd_addr_t s_peer;           /* connected / connecting to */
static char s_peer_name[BT_NAME_MAX];
static bool s_user_connect;            /* the S3 asked: report a failure */

static bool s_have_saved;
static esp_bd_addr_t s_saved;
static char s_saved_name[BT_NAME_MAX];
static int s_reconnect_s;
static int s_reconnect_misses;

static bool s_searching;
static bool s_pending;                 /* connect to s_pending_bda when free */
static esp_bd_addr_t s_pending_bda;
static char s_pending_name[BT_NAME_MAX];

static struct {
    esp_bd_addr_t bda;
    char name[BT_NAME_MAX];
} s_seen[SEEN_MAX];
static int s_nseen;

static bool s_media_started;
static bool s_search_after_drop;       /* SCAN arrived while connected */

static void send_state(void)
{
    static const char *const names[] = { "idle", "connecting", "connected" };
    char a[18] = "-";
    if (s_conn != CONN_IDLE) {
        bda_str(s_peer, a);
    }
    link_send("STATE %s %s %s", names[s_conn], a, s_conn != CONN_IDLE ? s_peer_name : "");
}

static void send_saved(void)
{
    char a[18] = "-";
    if (s_have_saved) {
        bda_str(s_saved, a);
    }
    link_send("SAVED %s %s", a, s_have_saved ? s_saved_name : "");
}

/* Connectable only when there are remembered headphones to come back, and
 * never in the middle of a search. */
static void update_connectable(void)
{
    esp_bt_gap_set_scan_mode(s_have_saved && !s_searching ? ESP_BT_CONNECTABLE
                                                          : ESP_BT_NON_CONNECTABLE,
                             ESP_BT_NON_DISCOVERABLE);
}

static void start_search(void)
{
    s_search_after_drop = false;
    update_connectable();
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, INQUIRY_LEN, 0);
}

/* Speakers on the DAC play only while no headphones are connected. */
static void set_wired_mute(bool mute)
{
    gpio_set_level(PIN_XSMT, mute ? 0 : 1);
}

static void connect_now(const esp_bd_addr_t bda, const char *name, bool user)
{
    memcpy(s_peer, bda, ESP_BD_ADDR_LEN);
    strlcpy(s_peer_name, name, sizeof(s_peer_name));
    s_user_connect = user;
    s_conn = CONN_CONNECTING;
    s_media_started = false;
    esp_a2d_source_connect(s_peer);
    send_state();
}

/* Connect as soon as nothing is in the way: a search is cancelled first, an
 * existing connection dropped first. */
static void connect_when_free(const esp_bd_addr_t bda, const char *name)
{
    if (s_conn == CONN_CONNECTED && memcmp(bda, s_peer, ESP_BD_ADDR_LEN) == 0) {
        send_state();                  /* already there */
        return;
    }
    memcpy(s_pending_bda, bda, ESP_BD_ADDR_LEN);
    strlcpy(s_pending_name, name, sizeof(s_pending_name));
    s_pending = true;
    if (s_searching) {
        esp_bt_gap_cancel_discovery();          /* continues at EV_SEARCH_DONE */
    } else if (s_conn != CONN_IDLE) {
        esp_a2d_source_disconnect(s_peer);      /* continues at EV_CONN */
    } else {
        s_pending = false;
        connect_now(s_pending_bda, s_pending_name, true);
    }
}

static const char *seen_name(const esp_bd_addr_t bda)
{
    for (int i = 0; i < s_nseen; i++) {
        if (memcmp(s_seen[i].bda, bda, ESP_BD_ADDR_LEN) == 0) {
            return s_seen[i].name;
        }
    }
    if (s_have_saved && memcmp(s_saved, bda, ESP_BD_ADDR_LEN) == 0) {
        return s_saved_name;
    }
    return "";
}

static void on_line(char *line)
{
    ESP_LOGI(TAG, "<- %s", line);
    if (strcmp(line, "HELLO") == 0) {
        link_send("READY " FW_VERSION);
        send_saved();
        send_state();
    } else if (strcmp(line, "SCAN") == 0) {
        if (!s_searching) {
            s_nseen = 0;
            s_searching = true;
            link_send("SCAN start");
            if (s_conn == CONN_CONNECTED) {
                s_search_after_drop = true;       /* continues at EV_CONN */
                esp_a2d_source_disconnect(s_peer);
            } else if (s_conn == CONN_CONNECTING) {
                s_search_after_drop = true;       /* when the attempt ends */
            } else {
                start_search();
            }
        }
    } else if (strcmp(line, "STOP") == 0) {
        if (s_searching) {
            if (s_search_after_drop) {
                s_search_after_drop = false;      /* never started: just end */
                s_searching = false;
                update_connectable();
                link_send("SCAN done");
            } else {
                esp_bt_gap_cancel_discovery();
            }
        }
    } else if (strncmp(line, "CONNECT ", 8) == 0) {
        esp_bd_addr_t bda;
        if (parse_bda(line + 8, bda)) {
            connect_when_free(bda, seen_name(bda));
        }
    } else if (strcmp(line, "FORGET") == 0) {
        s_have_saved = false;
        s_pending = false;
        saved_erase();
        update_connectable();
        send_saved();
        if (s_conn != CONN_IDLE) {
            esp_a2d_source_disconnect(s_peer);
        }
    } else if (strncmp(line, "PLAY ", 5) == 0) {
        clip_play(atoi(line + 5));
    } else if (strncmp(line, "VOL ", 4) == 0) {
        const long g = strtol(line + 4, NULL, 10);
        s_gain_q8 = (uint16_t)(g < 0 ? 0 : g > 1024 ? 1024 : g);
    } else if (strncmp(line, "SIREN ", 6) == 0) {
        const long ms = strtol(line + 6, NULL, 10);
        s_siren_req = ms < 0 ? 0 : ms > 10000 ? 10000 : ms;
    }
}

static void on_found(const esp_bd_addr_t bda, int8_t rssi, const char *name)
{
    int i;
    for (i = 0; i < s_nseen; i++) {
        if (memcmp(s_seen[i].bda, bda, ESP_BD_ADDR_LEN) == 0) {
            break;
        }
    }
    if (i == s_nseen) {
        if (s_nseen == SEEN_MAX) {
            return;
        }
        memcpy(s_seen[i].bda, bda, ESP_BD_ADDR_LEN);
        s_seen[i].name[0] = '\0';
        s_nseen++;
    } else if (!name[0] || strcmp(s_seen[i].name, name) == 0) {
        return;                         /* nothing new about it */
    }
    if (name[0]) {
        strlcpy(s_seen[i].name, name, BT_NAME_MAX);
    }
    char a[18];
    bda_str(bda, a);
    link_send("DEV %s %d %s", a, rssi, s_seen[i].name);
}

static void on_conn(const esp_bd_addr_t bda, esp_a2d_connection_state_t st)
{
    if (st == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        memcpy(s_peer, bda, ESP_BD_ADDR_LEN);
        if (!s_peer_name[0] || s_conn == CONN_IDLE) {
            /* Headphones that reconnected by themselves. */
            strlcpy(s_peer_name, seen_name(bda), sizeof(s_peer_name));
        }
        if (!s_peer_name[0]) {
            esp_bt_gap_read_remote_name((uint8_t *)bda);   /* EV_NAME */
        }
        s_conn = CONN_CONNECTED;
        s_media_started = false;
        s_user_connect = false;
        memcpy(s_saved, bda, ESP_BD_ADDR_LEN);
        strlcpy(s_saved_name, s_peer_name, sizeof(s_saved_name));
        s_have_saved = true;
        s_reconnect_misses = 0;
        saved_store(bda, s_peer_name);
        update_connectable();
        send_saved();
        set_wired_mute(true);
        send_state();
        if (s_search_after_drop) {
            esp_a2d_source_disconnect(s_peer);   /* a search is waiting */
        } else {
            esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
        }
    } else if (st == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        const bool failed = s_conn == CONN_CONNECTING && s_user_connect;
        if (s_conn == CONN_CONNECTING && !s_user_connect) {
            s_reconnect_misses++;         /* switched off, or out of range */
        }
        s_conn = CONN_IDLE;
        s_user_connect = false;
        s_media_started = false;
        s_reconnect_s = 0;
        set_wired_mute(false);
        if (failed) {
            link_send("FAIL");
        }
        if (s_search_after_drop) {
            send_state();
            start_search();
        } else if (s_pending && !s_searching) {
            s_pending = false;
            connect_now(s_pending_bda, s_pending_name, true);
        } else {
            send_state();
        }
    }
}

static void on_media_ack(esp_a2d_media_ctrl_t cmd, esp_a2d_media_ctrl_ack_t status)
{
    ESP_LOGI(TAG, "media ctrl %d ack %d", cmd, status);
    if (s_conn != CONN_CONNECTED) {
        return;
    }
    if (cmd == ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY && status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS) {
        esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_START);
    } else if (cmd == ESP_A2D_MEDIA_CTRL_START) {
        /* Started: stream (silence between sounds) for as long as we are
         * connected, so a click plays with no start-up delay. */
        s_media_started = status == ESP_A2D_MEDIA_CTRL_ACK_SUCCESS;
    }
}

static void on_tick(void)
{
    stream_report();
    if (s_conn == CONN_CONNECTED && !s_media_started && !s_searching) {
        esp_a2d_media_ctrl(ESP_A2D_MEDIA_CTRL_CHECK_SRC_RDY);
    }
    /* Headphones switched on later reconnect without the player doing
     * anything -- but never in the middle of a search, which the radio
     * cannot do at the same time. */
    if (s_conn == CONN_IDLE && s_have_saved && !s_searching && !s_pending &&
        ++s_reconnect_s >= (s_reconnect_misses < RECONNECT_FAST ? RECONNECT_S : RECONNECT_SLOW_S)) {
        s_reconnect_s = 0;
        connect_now(s_saved, s_saved_name, false);
    }
}

static void control_task(void *arg)
{
    (void)arg;
    ev_t ev;
    for (;;) {
        if (xQueueReceive(s_ev, &ev, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        switch (ev.type) {
        case EV_LINE:
            on_line(ev.line);
            break;
        case EV_FOUND:
            on_found(ev.found.bda, ev.found.rssi, ev.found.name);
            break;
        case EV_SEARCH_DONE:
            if (s_searching && !s_search_after_drop) {
                s_searching = false;
                update_connectable();
                link_send("SCAN done");
            }
            if (s_pending) {
                s_pending = false;
                connect_when_free(s_pending_bda, s_pending_name);
            }
            break;
        case EV_CONN:
            on_conn(ev.conn.bda, ev.conn.state);
            break;
        case EV_MEDIA_ACK:
            on_media_ack(ev.ack.cmd, ev.ack.status);
            break;
        case EV_TICK:
            on_tick();
            break;
        case EV_AVRC:
            /* Headphones play the stream at their own volume, which only
             * AVRCP can set. Put it near the top, so the game's Volume
             * setting is what decides how loud it is. */
            esp_avrc_ct_send_set_absolute_volume_cmd(AVRC_TL_VOLUME, HEADPHONE_VOLUME);
            break;
        case EV_NAME:
            if (ev.found.name[0] && s_conn != CONN_IDLE &&
                memcmp(ev.found.bda, s_peer, ESP_BD_ADDR_LEN) == 0) {
                strlcpy(s_peer_name, ev.found.name, sizeof(s_peer_name));
                if (s_have_saved && memcmp(s_saved, s_peer, ESP_BD_ADDR_LEN) == 0) {
                    strlcpy(s_saved_name, s_peer_name, sizeof(s_saved_name));
                    saved_store(s_saved, s_saved_name);
                    send_saved();
                }
                send_state();
            }
            break;
        }
    }
}

static void post(const ev_t *ev)
{
    /* Never block the Bluetooth task for long; a dropped search result is
     * harmless, and state changes are rare enough not to overflow. */
    if (xQueueSend(s_ev, ev, pdMS_TO_TICKS(20)) != pdTRUE) {
        ESP_LOGW(TAG, "event %d dropped", ev->type);
    }
}

/* ---- Bluetooth callbacks (Bluetooth task) ---------------------------- */

/* AVRCP, the remote-control profile. Some headphones -- AirPods among them
 * -- accept an A2DP stream without it but play nothing: the connection and
 * the stream look healthy and the earbuds stay silent. With it up, the
 * board can also set their volume (0..127). */
static void avrc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *p)
{
    switch (event) {
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
        ESP_LOGI(TAG, "AVRCP %s", p->conn_stat.connected ? "connected" : "disconnected");
        if (p->conn_stat.connected) {
            post(&(ev_t){ .type = EV_AVRC });
        }
        break;
    case ESP_AVRC_CT_SET_ABSOLUTE_VOLUME_RSP_EVT:
        ESP_LOGI(TAG, "headphone volume set to %d/127", p->set_volume_rsp.volume);
        break;
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT:
        ESP_LOGI(TAG, "AVRCP remote features 0x%" PRIx32 ", target features 0x%x",
                 p->rmt_feats.feat_mask, p->rmt_feats.tg_feat_flag);
        break;
    default:
        break;
    }
}

/* Copy a name out of a found device's properties: its own name property,
 * else the complete or short name in its extended inquiry response. */
static void found_name(const esp_bt_gap_cb_param_t *p, char out[BT_NAME_MAX])
{
    out[0] = '\0';
    for (int i = 0; i < p->disc_res.num_prop; i++) {
        const esp_bt_gap_dev_prop_t *pr = &p->disc_res.prop[i];
        const uint8_t *s = NULL;
        uint8_t len = 0;
        if (pr->type == ESP_BT_GAP_DEV_PROP_BDNAME) {
            s = pr->val;
            len = (uint8_t)pr->len;
        } else if (pr->type == ESP_BT_GAP_DEV_PROP_EIR) {
            s = esp_bt_gap_resolve_eir_data(pr->val, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &len);
            if (!s) {
                s = esp_bt_gap_resolve_eir_data(pr->val, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &len);
            }
        }
        if (s && len) {
            size_t n = len < BT_NAME_MAX - 1 ? len : BT_NAME_MAX - 1;
            memcpy(out, s, n);
            out[n] = '\0';
            /* One line per device on the link: no newlines in a name. */
            for (size_t k = 0; k < n; k++) {
                if (out[k] == '\n' || out[k] == '\r') {
                    out[k] = ' ';
                }
            }
            return;
        }
    }
}

static void gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *p)
{
    switch (event) {
    case ESP_BT_GAP_DISC_RES_EVT: {
        uint32_t cod = 0;
        int8_t rssi = -127;
        for (int i = 0; i < p->disc_res.num_prop; i++) {
            const esp_bt_gap_dev_prop_t *pr = &p->disc_res.prop[i];
            if (pr->type == ESP_BT_GAP_DEV_PROP_COD) {
                cod = *(uint32_t *)pr->val;
            } else if (pr->type == ESP_BT_GAP_DEV_PROP_RSSI) {
                rssi = *(int8_t *)pr->val;
            }
        }
        {
            char a[18], nm[BT_NAME_MAX];
            bda_str(p->disc_res.bda, a);
            found_name(p, nm);
            ESP_LOGI(TAG, "heard %s cod 0x%06" PRIx32 " (major %" PRIu32 ", srvc 0x%03" PRIx32
                     ") rssi %d \"%s\"", a, cod, esp_bt_gap_get_cod_major_dev(cod),
                     esp_bt_gap_get_cod_srvc(cod), rssi, nm);
        }
        /* Headphones and speakers: the audio/video major class, or anything
         * offering the "rendering" service. Phones and laptops are left out. */
        if (!esp_bt_gap_is_valid_cod(cod) ||
            !(esp_bt_gap_get_cod_major_dev(cod) == ESP_BT_COD_MAJOR_DEV_AV ||
              (esp_bt_gap_get_cod_srvc(cod) & ESP_BT_COD_SRVC_RENDERING))) {
            break;
        }
        ev_t ev = { .type = EV_FOUND };
        memcpy(ev.found.bda, p->disc_res.bda, ESP_BD_ADDR_LEN);
        ev.found.rssi = rssi;
        found_name(p, ev.found.name);
        post(&ev);
        break;
    }
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
        if (p->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
            post(&(ev_t){ .type = EV_SEARCH_DONE });
        }
        break;
    case ESP_BT_GAP_READ_REMOTE_NAME_EVT:
        if (p->read_rmt_name.stat == ESP_BT_STATUS_SUCCESS) {
            ev_t ev = { .type = EV_NAME };
            memcpy(ev.found.bda, p->read_rmt_name.bda, ESP_BD_ADDR_LEN);
            strlcpy(ev.found.name, (const char *)p->read_rmt_name.rmt_name, BT_NAME_MAX);
            post(&ev);
        }
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT:
        ESP_LOGI(TAG, "pairing %s: %s", p->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS ? "ok" : "FAILED",
                 p->auth_cmpl.device_name);
        break;
    case ESP_BT_GAP_PIN_REQ_EVT: {
        /* Legacy pairing: headphones almost always use 0000. */
        esp_bt_pin_code_t pin = { '0', '0', '0', '0' };
        esp_bt_gap_pin_reply(p->pin_req.bda, true, 4, pin);
        break;
    }
    case ESP_BT_GAP_CFM_REQ_EVT:
        /* Secure Simple Pairing: headphones have no screen, "just works". */
        esp_bt_gap_ssp_confirm_reply(p->cfm_req.bda, true);
        break;
    default:
        break;
    }
}

static void a2d_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *p)
{
    if (event == ESP_A2D_CONNECTION_STATE_EVT) {
        ev_t ev = { .type = EV_CONN };
        memcpy(ev.conn.bda, p->conn_stat.remote_bda, ESP_BD_ADDR_LEN);
        ev.conn.state = p->conn_stat.state;
        post(&ev);
    } else if (event == ESP_A2D_AUDIO_STATE_EVT) {
        ESP_LOGI(TAG, "audio state %d", p->audio_stat.state);
    } else if (event == ESP_A2D_MEDIA_CTRL_ACK_EVT) {
        ev_t ev = { .type = EV_MEDIA_ACK };
        ev.ack.cmd = p->media_ctrl_stat.cmd;
        ev.ack.status = p->media_ctrl_stat.status;
        post(&ev);
    }
}

/* ---- UART reader ----------------------------------------------------- */

static void link_rx_task(void *arg)
{
    (void)arg;
    static char line[LINK_LINE_MAX];
    size_t n = 0;
    bool overflow = false;
    uint8_t buf[128];
    for (;;) {
        const int got = uart_read_bytes(LINK_UART, buf, sizeof(buf), pdMS_TO_TICKS(100));
        for (int i = 0; i < got; i++) {
            const char c = (char)buf[i];
            if (c == '\r') {
                continue;
            }
            if (c != '\n') {
                if (n == 0 && !(c >= 'A' && c <= 'Z')) {
                    continue;             /* junk before the keyword */
                }
                if (n < sizeof(line) - 1) {
                    line[n++] = c;
                } else {
                    overflow = true;
                }
                continue;
            }
            line[n] = '\0';
            if (!overflow && n) {
                if (strncmp(line, "PCM ", 4) == 0) {
                    stream_push(line + 4);       /* every ~12 ms: straight in */
                } else if (strncmp(line, "CLIP ", 5) == 0) {
                    clip_load(line + 5);         /* too big for the queue */
                } else if (n < sizeof(((ev_t *)0)->line)) {
                    ev_t ev = { .type = EV_LINE };
                    memcpy(ev.line, line, n + 1);
                    post(&ev);
                }
            }
            n = 0;
            overflow = false;
        }
    }
}

static void tick_task(void *arg)
{
    (void)arg;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        post(&(ev_t){ .type = EV_TICK });
    }
}

/* ---- Start-up -------------------------------------------------------- */

void app_main(void)
{
    /* First thing: un-mute the DAC, so the S3's wired sound works whatever
     * happens to the radio below. */
    gpio_config_t xsmt = { .pin_bit_mask = 1ULL << PIN_XSMT, .mode = GPIO_MODE_OUTPUT };
    gpio_config(&xsmt);
    set_wired_mute(false);

    siren_init();

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    s_tx_lock = xSemaphoreCreateMutex();
    s_ev = xQueueCreate(24, sizeof(ev_t));
    const uart_config_t uc = {
        .baud_rate = LINK_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(LINK_UART, 8192, 1024, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(LINK_UART, &uc));
    /* Empty the 128-byte RX FIFO from a quarter full, not the driver's 120
     * bytes: at 921600 baud that leaves ~90 us before bytes are lost, less
     * than the Bluetooth controller's interrupts can hold the UART's off.
     * 32 leaves ~1 ms. */
    ESP_ERROR_CHECK(uart_set_rx_full_threshold(LINK_UART, 32));
    ESP_ERROR_CHECK(uart_set_pin(LINK_UART, PIN_LINK_TX, PIN_LINK_RX,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    s_prev_vprintf = esp_log_set_vprintf(log_forward);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT));
    /* Full transmit power. The board's chip antenna sits inside the knob,
     * and at the default level a search reaches little beyond arm's length;
     * headphones that never hear the inquiry never answer it. The board is
     * USB powered, so the extra current does not matter. */
    esp_err_t pwr = esp_bredr_tx_power_set(ESP_PWR_LVL_P6, ESP_PWR_LVL_P9);
    if (pwr != ESP_OK) {
        ESP_LOGW(TAG, "BR/EDR TX power: %s", esp_err_to_name(pwr));
    }
    esp_bluedroid_config_t bd_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bluedroid_init_with_cfg(&bd_cfg));
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_NONE;
    esp_bt_gap_set_security_param(ESP_BT_SP_IOCAP_MODE, &iocap, sizeof(iocap));
    esp_bt_pin_code_t pin = { 0 };
    esp_bt_gap_set_pin(ESP_BT_PIN_TYPE_VARIABLE, 0, pin);
    esp_bt_gap_set_device_name(DEVICE_NAME);
    esp_bt_gap_register_callback(gap_cb);
    /* AVRCP (controller only) before A2DP, as ESP-IDF's a2dp_source example
     * does. */
    ESP_ERROR_CHECK(esp_avrc_ct_init());
    esp_avrc_ct_register_callback(avrc_ct_cb);
    ESP_ERROR_CHECK(esp_a2d_source_init());
    esp_a2d_register_callback(a2d_cb);
    esp_a2d_source_register_data_callback(a2d_data_cb);
    s_have_saved = saved_load(s_saved, s_saved_name);
    /* Connectable when there are remembered headphones, so they can
     * reconnect by themselves when switched on; never discoverable -- this
     * is not a speaker. */
    update_connectable();
    if (s_have_saved) {
        char a[18];
        bda_str(s_saved, a);
        ESP_LOGI(TAG, "remembered headphones %s (%s)", a, s_saved_name);
        s_reconnect_s = RECONNECT_S - 2;      /* try within a couple of seconds */
    }

    xTaskCreate(control_task, "cts_ctl", 4096, NULL, 5, NULL);
    xTaskCreate(link_rx_task, "cts_rx", 4096, NULL, 6, NULL);
    xTaskCreate(tick_task, "cts_tick", 2048, NULL, 4, NULL);

    /* A bare newline first: whatever the TX pin glitched out while it was
     * being configured becomes an empty line the S3 ignores, not the front
     * of READY. */
    uart_write_bytes(LINK_UART, "\n", 1);
    link_send("READY " FW_VERSION);
    send_saved();
    send_state();
}
