/*
 * app_model — input to keystrokes, per focused app.
 *
 * The key tables here are the other half of the key labels on the
 * PageFreecad / PageBlender / PageKdenlive / PageVscodium / PageGimp /
 * PageInkscape / PageLibreoffice / PageFirefox / PageChromium screens in
 * GUI/RotaryHid.eez-project: the project draws key N (fc_key0..fc_key7,
 * clockwise from 12 o'clock), this file sends key N's chord. Keep the two in
 * the same order. docs/apps.md lists both side by side.
 */

#include <stdio.h>
#include <string.h>

#include "class/hid/hid.h"
#include "esp_log.h"

#include "app_model.h"

static const char *TAG = "app";

#define C(m, k) { .mods = (m), .key = (k) }
#define CTRL  ROTARY_MOD_CTRL
#define SHIFT ROTARY_MOD_SHIFT

/* One key on the ring: up to two chords (FreeCAD's "V F" is a sequence). */
typedef struct {
    const char    *name;
    rotary_chord_t chords[2];
    uint8_t        count;
} app_key_t;

#define KEY1(n, m, k)          { n, { C(m, k) }, 1 }
#define KEY2(n, m1, k1, m2, k2) { n, { C(m1, k1), C(m2, k2) }, 2 }

/* Default FreeCAD shortcuts. */
static const app_key_t KEYS_FREECAD[8] = {
    KEY2("Fit All",   0, HID_KEY_V, 0, HID_KEY_F),
    KEY1("Iso",       0, HID_KEY_0),
    KEY1("Front",     0, HID_KEY_1),
    KEY1("Top",       0, HID_KEY_2),
    KEY1("Right",     0, HID_KEY_3),
    KEY1("Hide/Show", 0, HID_KEY_SPACE),
    KEY1("Undo",      CTRL, HID_KEY_Z),
    KEY1("Redo",      CTRL, HID_KEY_Y),
};

/* Default Blender keymap. */
static const app_key_t KEYS_BLENDER[8] = {
    KEY1("Grab",      0, HID_KEY_G),
    KEY1("Rotate",    0, HID_KEY_R),
    KEY1("Scale",     0, HID_KEY_S),
    KEY1("Edit Mode", 0, HID_KEY_TAB),
    KEY1("Frame Sel", 0, HID_KEY_KEYPAD_DECIMAL),
    KEY1("Camera",    0, HID_KEY_KEYPAD_0),
    KEY1("Undo",      CTRL, HID_KEY_Z),
    KEY1("Render",    0, HID_KEY_F12),
};

/* Default Kdenlive shortcuts. */
static const app_key_t KEYS_KDENLIVE[8] = {
    KEY1("Play",      0, HID_KEY_SPACE),
    KEY1("Set In",    0, HID_KEY_I),
    KEY1("Set Out",   0, HID_KEY_O),
    KEY1("Select",    0, HID_KEY_S),
    KEY1("Razor",     0, HID_KEY_X),
    KEY1("Spacer",    0, HID_KEY_M),
    KEY1("Undo",      CTRL, HID_KEY_Z),
    KEY1("Render",    CTRL, HID_KEY_ENTER),
};

/*
 * An option added to main/Kconfig.projbuild only reaches sdkconfig.h when
 * ESP-IDF reconfigures, and editing the Kconfig file does not always trigger
 * that. Say so, rather than failing on an "undeclared" symbol.
 */
#if !defined(CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT) || !defined(CONFIG_ROTARY_KDENLIVE_FPS) || \
    !defined(CONFIG_ROTARY_STEP_DEG) || !defined(CONFIG_ROTARY_VSCODIUM_LINES_PER_DETENT) || \
    !defined(CONFIG_ROTARY_LIBREOFFICE_LINES_PER_DETENT) || \
    !defined(CONFIG_ROTARY_BROWSER_LINES_PER_DETENT)
#  error "Rotary Macro Pad options missing from sdkconfig -- run: idf.py reconfigure"
#endif

/* Upper bound of CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT (Kconfig range). */
#define KD_MAX_FRAMES_PER_DETENT 30
_Static_assert(CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT <= KD_MAX_FRAMES_PER_DETENT,
               "raise KD_MAX_FRAMES_PER_DETENT to match the Kconfig range");

/* VSCodium / VS Code default keybindings (Linux). */
static const app_key_t KEYS_VSCODIUM[8] = {
    KEY1("Palette",   0, HID_KEY_F1),
    KEY1("Open",      CTRL, HID_KEY_P),
    KEY1("Find",      CTRL, HID_KEY_F),
    KEY1("Go to Def", 0, HID_KEY_F12),
    KEY1("Rename",    0, HID_KEY_F2),
    KEY1("Comment",   CTRL, HID_KEY_SLASH),
    KEY1("Terminal",  CTRL, HID_KEY_GRAVE),
    KEY1("Save",      CTRL, HID_KEY_S),
};

/* GIMP 3 default shortcuts. */
static const app_key_t KEYS_GIMP[8] = {
    KEY1("Move",      0, HID_KEY_M),
    KEY1("Rect Sel",  0, HID_KEY_R),
    KEY1("Crop",      SHIFT, HID_KEY_C),
    KEY1("Brush",     0, HID_KEY_P),
    KEY1("Eraser",    SHIFT, HID_KEY_E),
    KEY1("Text",      0, HID_KEY_T),
    KEY1("Undo",      CTRL, HID_KEY_Z),
    KEY1("Redo",      CTRL, HID_KEY_Y),
};

/* Inkscape 1.4 default shortcuts (share/inkscape/keys/inkscape.xml). */
static const app_key_t KEYS_INKSCAPE[8] = {
    KEY1("Selector",  0, HID_KEY_S),
    KEY1("Node",      0, HID_KEY_N),
    KEY1("Rect",      0, HID_KEY_R),
    KEY1("Ellipse",   0, HID_KEY_E),
    KEY1("Pen",       0, HID_KEY_B),
    KEY1("Text",      0, HID_KEY_T),
    KEY1("Undo",      CTRL, HID_KEY_Z),
    KEY1("Redo",      CTRL, HID_KEY_Y),
};

/* LibreOffice 24 defaults; the same keys in Writer, Calc and Impress. */
static const app_key_t KEYS_LIBREOFFICE[8] = {
    KEY1("Bold",      CTRL, HID_KEY_B),
    KEY1("Italic",    CTRL, HID_KEY_I),
    KEY1("Underline", CTRL, HID_KEY_U),
    KEY1("Find",      CTRL, HID_KEY_H),   /* Find & Replace */
    KEY1("Save",      CTRL, HID_KEY_S),
    KEY1("Print",     CTRL, HID_KEY_P),
    KEY1("Undo",      CTRL, HID_KEY_Z),
    KEY1("Redo",      CTRL, HID_KEY_Y),
};

/* Firefox default shortcuts. Ctrl+B / Ctrl+H toggle its sidebar. */
static const app_key_t KEYS_FIREFOX[8] = {
    KEY1("Back",      ROTARY_MOD_ALT, HID_KEY_ARROW_LEFT),
    KEY1("Forward",   ROTARY_MOD_ALT, HID_KEY_ARROW_RIGHT),
    KEY1("Reload",    0, HID_KEY_F5),
    KEY1("New Tab",   CTRL, HID_KEY_T),
    KEY1("Close Tab", CTRL, HID_KEY_W),
    KEY1("Find",      CTRL, HID_KEY_F),
    KEY1("Sidebar",   CTRL, HID_KEY_B),   /* bookmarks sidebar */
    KEY1("History",   CTRL, HID_KEY_H),   /* history sidebar */
};

/* Chromium (and Google Chrome) default shortcuts. */
static const app_key_t KEYS_CHROMIUM[8] = {
    KEY1("Back",      ROTARY_MOD_ALT, HID_KEY_ARROW_LEFT),
    KEY1("Forward",   ROTARY_MOD_ALT, HID_KEY_ARROW_RIGHT),
    KEY1("Reload",    0, HID_KEY_F5),
    KEY1("New Tab",   CTRL, HID_KEY_T),
    KEY1("Close Tab", CTRL, HID_KEY_W),
    KEY1("Bookmark",  CTRL, HID_KEY_D),   /* bookmark this page */
    KEY1("History",   CTRL, HID_KEY_H),   /* history page */
    KEY1("Download",  CTRL, HID_KEY_J),   /* downloads page */
};

/* Upper bound of the *_LINES_PER_DETENT options (their Kconfig range). */
#define MAX_LINES_PER_DETENT 20
_Static_assert(CONFIG_ROTARY_VSCODIUM_LINES_PER_DETENT <= MAX_LINES_PER_DETENT &&
               CONFIG_ROTARY_LIBREOFFICE_LINES_PER_DETENT <= MAX_LINES_PER_DETENT &&
               CONFIG_ROTARY_BROWSER_LINES_PER_DETENT <= MAX_LINES_PER_DETENT,
               "raise MAX_LINES_PER_DETENT to match the Kconfig range");

static const char AXIS_LETTER[AXIS_COUNT] = { 'X', 'Y', 'Z' };
static const uint8_t AXIS_KEY[AXIS_COUNT] = { HID_KEY_X, HID_KEY_Y, HID_KEY_Z };

/*
 * FreeCAD has no built-in "rotate the view N degrees about an axis", so the
 * ring sends F14 / F15 / F16 (X / Y / Z), with Shift for the negative
 * direction, and the RotaryHid add-on in freecad/ does the rotating (it
 * registers these keys with Qt directly -- see docs/apps.md). F13 is avoided:
 * X11 maps it to XF86Tools, which GNOME grabs to open Settings. F14-F16
 * arrive as XF86Launch5-7, which nothing on a stock desktop binds.
 */
static const uint8_t FREECAD_AXIS_KEY[AXIS_COUNT] = { HID_KEY_F14, HID_KEY_F15, HID_KEY_F16 };

/* ------------------------------------------------------------------------ */

static rotary_app_t s_app = ROTARY_APP_OTHER;
static app_axis_t   s_axis[2]  = { AXIS_Z, AXIS_Z };   /* [0] FreeCAD, [1] Blender */
static int32_t      s_angle[2];                        /* degrees, display tally */
static int32_t      s_frame;                           /* Kdenlive playhead estimate */
static bool         s_step_second;
/*
 * Scrolling apps: how a ring click is sent, and each app's own line/page
 * mode (switching one does not switch the other).
 *
 *   VSCodium     Ctrl+Up/Down scrolls the view without moving the cursor;
 *                Alt+PgUp/PgDn scrolls a page.
 *   LibreOffice  has no scroll-the-view-only key, so plain arrows and
 *                PgUp/PgDn -- they move the cursor (Writer) or active cell
 *                (Calc), and the view follows.
 *   Browsers     plain arrows and PgUp/PgDn scroll the page (unless a text
 *                field has focus).
 */
typedef struct {
    int     lines;
    uint8_t line_mods;
    uint8_t page_mods;
    bool    page;          /* true: page steps */
} scroll_app_t;

static scroll_app_t s_scroll_vs = { CONFIG_ROTARY_VSCODIUM_LINES_PER_DETENT,
                                    CTRL, ROTARY_MOD_ALT, false };
static scroll_app_t s_scroll_lo = { CONFIG_ROTARY_LIBREOFFICE_LINES_PER_DETENT,
                                    0, 0, false };
static scroll_app_t s_scroll_ff = { CONFIG_ROTARY_BROWSER_LINES_PER_DETENT,
                                    0, 0, false };
static scroll_app_t s_scroll_cr = { CONFIG_ROTARY_BROWSER_LINES_PER_DETENT,
                                    0, 0, false };

static scroll_app_t *scroll_app(void)
{
    switch (s_app) {
    case ROTARY_APP_VSCODIUM:    return &s_scroll_vs;
    case ROTARY_APP_LIBREOFFICE: return &s_scroll_lo;
    case ROTARY_APP_FIREFOX:     return &s_scroll_ff;
    case ROTARY_APP_CHROMIUM:    return &s_scroll_cr;
    default:                     return NULL;
    }
}
static int32_t      s_cuts[APP_MAX_CUTS];
static int          s_cut_count;

static char s_mode_text[32];
static char s_angle_text[16];
static char s_tc_text[40];   /* sized for any int32 frame count */
static char s_step_text[24];
static char s_scroll_text[24];

static app_model_changed_cb_t s_changed_cb;

static int rot_slot(void)
{
    return (s_app == ROTARY_APP_BLENDER) ? 1 : 0;
}

static void reformat(void)
{
    const int slot = rot_slot();
    snprintf(s_mode_text, sizeof(s_mode_text), "%s - %c",
             s_app == ROTARY_APP_BLENDER ? "Rotate Object" : "Rotate View",
             AXIS_LETTER[s_axis[slot]]);

    /* As in the design: "+5°", "0°", "-15°". \xC2\xB0 is the degree sign,
     * which is in the num32 font's range. */
    snprintf(s_angle_text, sizeof(s_angle_text), "%s%ld\xC2\xB0",
             s_angle[slot] > 0 ? "+" : "", (long)s_angle[slot]);

    const int fps = CONFIG_ROTARY_KDENLIVE_FPS;
    const int32_t secs = s_frame / fps;
    snprintf(s_tc_text, sizeof(s_tc_text), "%02ld:%02ld:%02ld:%02ld",
             (long)(secs / 3600), (long)((secs / 60) % 60), (long)(secs % 60),
             (long)(s_frame % fps));

    /* The step pills read "3 Lines" / "Page" and "5 Frames" / "1 Second",
     * without the 2.1in's "Step - " prefix: the pill sits at the bottom of
     * the centre panel, between the lower keys, where the short form reads
     * best. */
    const scroll_app_t *sa = scroll_app();
    if (sa && sa->page) {
        snprintf(s_scroll_text, sizeof(s_scroll_text), "Page");
    } else if (sa) {
        snprintf(s_scroll_text, sizeof(s_scroll_text), "%d Line%s",
                 sa->lines, sa->lines == 1 ? "" : "s");
    }

    if (s_step_second) {
        snprintf(s_step_text, sizeof(s_step_text), "1 Second");
    } else {
        snprintf(s_step_text, sizeof(s_step_text), "%d Frame%s",
                 CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT,
                 CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT == 1 ? "" : "s");
    }
}

static void changed(void)
{
    reformat();
    if (s_changed_cb) {
        s_changed_cb();
    }
}

static bool send(const rotary_chord_t *chords, size_t n)
{
    esp_err_t err = rotary_usb_type(chords, n);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "HID %s -- input dropped",
                 err == ESP_ERR_INVALID_STATE ? "not mounted" : "queue full");
        return false;
    }
    return true;
}

/* ------------------------------------------------------------------------ */

void app_model_set_app(rotary_app_t app)
{
    if (app != s_app) {
        ESP_LOGI(TAG, "focus: %s", rotary_app_name(app));
        s_app = app;
    }
    changed();
}

rotary_app_t app_model_app(void) { return s_app; }

void app_model_rotate(int detents)
{
    if (detents == 0) {
        return;
    }
    const int dir = detents > 0 ? 1 : -1;
    const int n   = detents > 0 ? detents : -detents;

    for (int i = 0; i < n; i++) {
        switch (s_app) {
        case ROTARY_APP_FREECAD: {
            const rotary_chord_t c = C(dir < 0 ? SHIFT : 0,
                                       FREECAD_AXIS_KEY[s_axis[0]]);
            if (send(&c, 1)) {
                s_angle[0] += dir * CONFIG_ROTARY_STEP_DEG;
            }
            break;
        }
        case ROTARY_APP_BLENDER: {
            /* R <axis> [-] <step> Enter: a typed, exact rotation. */
            rotary_chord_t seq[8];
            size_t k = 0;
            seq[k++] = (rotary_chord_t)C(0, HID_KEY_R);
            seq[k++] = (rotary_chord_t)C(0, AXIS_KEY[s_axis[1]]);
            if (dir < 0) {
                seq[k++] = (rotary_chord_t)C(0, HID_KEY_MINUS);
            }
            char digits[4];
            snprintf(digits, sizeof(digits), "%d", CONFIG_ROTARY_STEP_DEG);
            for (const char *d = digits; *d && k < 7; d++) {
                seq[k++] = (rotary_chord_t)C(0, *d == '0' ? HID_KEY_0
                                                          : HID_KEY_1 + (*d - '1'));
            }
            seq[k++] = (rotary_chord_t)C(0, HID_KEY_ENTER);
            if (send(seq, k)) {
                s_angle[1] += dir * CONFIG_ROTARY_STEP_DEG;
            }
            break;
        }
        case ROTARY_APP_KDENLIVE: {
            /*
             * Kdenlive's arrow keys move exactly one frame and it has no
             * "N frames" shortcut, so frame mode presses the arrow several
             * times per detent. Second mode is one Shift+arrow.
             */
            const uint8_t arrow = dir > 0 ? HID_KEY_ARROW_RIGHT : HID_KEY_ARROW_LEFT;
            rotary_chord_t seq[KD_MAX_FRAMES_PER_DETENT];
            size_t k = 0;
            int32_t moved;
            if (s_step_second) {
                seq[k++] = (rotary_chord_t)C(SHIFT, arrow);
                moved = CONFIG_ROTARY_KDENLIVE_FPS;
            } else {
                while (k < CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT) {
                    seq[k++] = (rotary_chord_t)C(0, arrow);
                }
                moved = CONFIG_ROTARY_KDENLIVE_FRAMES_PER_DETENT;
            }
            if (send(seq, k)) {
                /* Clamp at zero: the playhead cannot go before the start,
                 * and the display must not store overshoot below it. */
                s_frame += dir * moved;
                if (s_frame < 0) {
                    s_frame = 0;
                }
            }
            break;
        }
        case ROTARY_APP_VSCODIUM:
        case ROTARY_APP_LIBREOFFICE:
        case ROTARY_APP_FIREFOX:
        case ROTARY_APP_CHROMIUM: {
            /* Several lines per click, all-or-nothing; see scroll_app_t. */
            const scroll_app_t *sa = scroll_app();
            rotary_chord_t seq[MAX_LINES_PER_DETENT];
            size_t k = 0;
            if (sa->page) {
                seq[k++] = (rotary_chord_t)C(sa->page_mods,
                                             dir > 0 ? HID_KEY_PAGE_DOWN : HID_KEY_PAGE_UP);
            } else {
                while (k < (size_t)sa->lines) {
                    seq[k++] = (rotary_chord_t)C(sa->line_mods,
                                                 dir > 0 ? HID_KEY_ARROW_DOWN : HID_KEY_ARROW_UP);
                }
            }
            send(seq, k);
            break;
        }
        case ROTARY_APP_GIMP:
        case ROTARY_APP_INKSCAPE: {
            /* Keypad +/- is zoom in/out in both apps, and unlike the main
             * '+' needs no Shift on a US layout. */
            const rotary_chord_t c = C(0, dir > 0 ? HID_KEY_KEYPAD_ADD
                                                  : HID_KEY_KEYPAD_SUBTRACT);
            send(&c, 1);
            break;
        }
        default:
            return;   /* no app: the ring does nothing */
        }
    }
    changed();
}

void app_model_push(void)
{
    switch (s_app) {
    case ROTARY_APP_FREECAD:
    case ROTARY_APP_BLENDER: {
        /* Handled on the device: X -> Y -> Z. */
        const int slot = rot_slot();
        s_axis[slot] = (app_axis_t)((s_axis[slot] + 1) % AXIS_COUNT);
        break;
    }
    case ROTARY_APP_GIMP: {
        const rotary_chord_t c = C(CTRL | SHIFT, HID_KEY_J);  /* Fit Image in Window */
        send(&c, 1);
        return;   /* nothing on the pad changes */
    }
    case ROTARY_APP_INKSCAPE: {
        const rotary_chord_t c = C(0, HID_KEY_5);             /* Zoom Page */
        send(&c, 1);
        return;
    }
    case ROTARY_APP_VSCODIUM:
    case ROTARY_APP_LIBREOFFICE:
    case ROTARY_APP_FIREFOX:
    case ROTARY_APP_CHROMIUM:
        /* Handled on the device: line <-> page scrolling, per app. */
        scroll_app()->page = !scroll_app()->page;
        break;
    case ROTARY_APP_KDENLIVE: {
        const rotary_chord_t c = C(SHIFT, HID_KEY_R);   /* Cut clip at playhead */
        if (send(&c, 1)) {
            bool have = false;
            for (int i = 0; i < s_cut_count; i++) {
                have |= (s_cuts[i] == s_frame);
            }
            if (!have) {
                if (s_cut_count == APP_MAX_CUTS) {
                    /* Oldest mark goes; the cut itself was still sent. */
                    memmove(s_cuts, s_cuts + 1, sizeof(s_cuts[0]) * (APP_MAX_CUTS - 1));
                    s_cut_count--;
                }
                s_cuts[s_cut_count++] = s_frame;
            }
        }
        break;
    }
    default:
        return;
    }
    changed();
}

void app_model_key_tap(int index)
{
    const app_key_t *table = NULL;
    switch (s_app) {
    case ROTARY_APP_FREECAD:  table = KEYS_FREECAD;  break;
    case ROTARY_APP_BLENDER:  table = KEYS_BLENDER;  break;
    case ROTARY_APP_KDENLIVE: table = KEYS_KDENLIVE; break;
    case ROTARY_APP_VSCODIUM: table = KEYS_VSCODIUM; break;
    case ROTARY_APP_GIMP:     table = KEYS_GIMP;     break;
    case ROTARY_APP_INKSCAPE: table = KEYS_INKSCAPE; break;
    case ROTARY_APP_LIBREOFFICE: table = KEYS_LIBREOFFICE; break;
    case ROTARY_APP_FIREFOX:  table = KEYS_FIREFOX;  break;
    case ROTARY_APP_CHROMIUM: table = KEYS_CHROMIUM; break;
    default: return;
    }
    if (index < 0 || index >= 8) {
        return;
    }
    ESP_LOGI(TAG, "key: %s", table[index].name);
    send(table[index].chords, table[index].count);
}

void app_model_toggle_step(void)
{
    if (s_app == ROTARY_APP_KDENLIVE) {
        s_step_second = !s_step_second;
        changed();
    }
}

app_axis_t app_model_axis(void)        { return s_axis[rot_slot()]; }
int32_t    app_model_frame(void)       { return s_frame; }
bool       app_model_step_is_second(void) { return s_step_second; }

int app_model_cuts(const int32_t **out)
{
    *out = s_cuts;
    return s_cut_count;
}

const char *app_model_axis_mode_text(void) { return s_mode_text; }
const char *app_model_angle_text(void)     { return s_angle_text; }
const char *app_model_timecode_text(void)  { return s_tc_text; }
const char *app_model_step_text(void)      { return s_step_text; }
const char *app_model_scroll_step_text(void) { return s_scroll_text; }

void app_model_set_changed_callback(app_model_changed_cb_t cb)
{
    s_changed_cb = cb;
    reformat();
}
