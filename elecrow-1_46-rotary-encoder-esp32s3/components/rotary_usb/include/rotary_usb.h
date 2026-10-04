/*
 * rotary_usb — the device's USB face.
 *
 * One composite USB device, two functions:
 *
 *   HID keyboard   the shortcuts and macro keys sent to the focused app
 *   CDC (serial)   line protocol with the host companion, AND the ESP log
 *
 * Why one serial port and not a second one for the log: the ESP32-S3's
 * USB-OTG has only five IN FIFOs including EP0's. Each CDC port needs two
 * IN endpoints (notification + data) and the keyboard one, so two CDC
 * ports plus HID does not fit. The companion picks its own lines out of the
 * stream (see "Companion protocol") and ignores log lines.
 *
 * The USB-C port has only one PHY, and TinyUSB takes it over from the
 * USB-Serial/JTAG peripheral `idf.py flash` normally talks to. To flash, the
 * firmware reboots itself into ROM download mode when asked -- a
 * `bootloader` line, or opening the port at 1200 baud -- and the ROM comes
 * back up on USB-Serial/JTAG. scripts/flash.sh does this for you.
 *
 * Companion protocol (ASCII, one command per line, '\n' terminated). Device
 * lines start with "@" so they can never be mistaken for a log line:
 *
 *   host -> device   app <freecad|blender|kdenlive|vscodium|gimp|inkscape|
 *                         libreoffice|firefox|chromium|other>
 *                    time <HH:MM:SS>                (local time, 24 h)
 *                    ping
 *                    bootloader
 *   device -> host   @hello rotary_hid <version>    (when the port opens)
 *                    @app <name>                    (acknowledges `app`)
 *                    @pong
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Modifier bits, as in the HID boot keyboard report. */
#define ROTARY_MOD_CTRL   0x01
#define ROTARY_MOD_SHIFT  0x02
#define ROTARY_MOD_ALT    0x04
#define ROTARY_MOD_GUI    0x08

/** One key press: modifiers held while one HID usage is tapped. */
typedef struct {
    uint8_t mods;
    uint8_t key;     /* HID keyboard usage (HID_KEY_* in class/hid/hid.h) */
} rotary_chord_t;

typedef enum {
    ROTARY_APP_OTHER = 0,
    ROTARY_APP_FREECAD,
    ROTARY_APP_BLENDER,
    ROTARY_APP_KDENLIVE,
    ROTARY_APP_VSCODIUM,
    ROTARY_APP_GIMP,
    ROTARY_APP_INKSCAPE,
    ROTARY_APP_LIBREOFFICE,
    ROTARY_APP_FIREFOX,
    ROTARY_APP_CHROMIUM,
} rotary_app_t;

/**
 * Called when the companion reports a focus change. Runs on the USB task --
 * NOT the LVGL task -- so take the display lock before touching the UI.
 */
typedef void (*rotary_usb_app_cb_t)(rotary_app_t app, void *ctx);

/**
 * Start the USB device. Logs move to CDC 1 once this returns, so anything
 * logged before it is only visible over UART0.
 */
esp_err_t rotary_usb_init(rotary_usb_app_cb_t on_app, void *ctx);

/**
 * Called when the companion sends the time of day. Runs on the USB task --
 * take the display lock before touching anything the UI reads.
 */
typedef void (*rotary_usb_time_cb_t)(int hour, int minute, int second, void *ctx);
void rotary_usb_set_time_callback(rotary_usb_time_cb_t cb, void *ctx);

/** True once the host has configured the device (the HID is usable). */
bool rotary_usb_mounted(void);

/** True while the companion has CDC 0 open (DTR asserted). */
bool rotary_usb_companion_connected(void);

/**
 * Queue a sequence of chords to be typed, in order, each pressed and
 * released. Non-blocking: the HID task types them. Returns ESP_ERR_TIMEOUT
 * if the queue is full, ESP_ERR_INVALID_STATE if not mounted.
 */
esp_err_t rotary_usb_type(const rotary_chord_t *chords, size_t count);

const char *rotary_app_name(rotary_app_t app);

#ifdef __cplusplus
}
#endif
