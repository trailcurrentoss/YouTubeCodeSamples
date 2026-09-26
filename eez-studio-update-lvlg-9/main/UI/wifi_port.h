/*
 * wifi_port — the seam between the Wi-Fi setup screen and whatever is
 * actually holding the radio.
 *
 * There are two implementations and the UI cannot tell them apart:
 *
 *   wifi_port_sim.c  (simulator)  canned scan results and scripted outcomes.
 *   wifi_port_esp.c  (device)     esp_wifi_remote, which talks to the
 *                                 ESP32-C6 over SDIO. The P4 has no radio of
 *                                 its own, so even the "real" path is already
 *                                 going through an abstraction — which is why
 *                                 adding a second one costs almost nothing.
 *
 * This header is deliberately free of both LVGL and ESP-IDF, so it compiles
 * for xtensa/riscv and for Emscripten unchanged.
 *
 * THREADING CONTRACT -- read this before changing anything here.
 *
 * On the device there are two threads in play and they are not the same one:
 *
 *   LVGL task   calls every function in this header. It also holds the
 *               display lock while it does so, so nothing here may block for
 *               long or issue a radio RPC directly.
 *   worker task owned by the port. It is the ONLY thread that touches
 *               esp_wifi_*, NVS, or the scan results.
 *
 * Therefore every function below is non-blocking: requests are queued for the
 * worker, and results are COPIED OUT under a mutex rather than returned as
 * pointers into shared state. That is why wifi_port_ap() takes an out
 * parameter instead of returning a pointer, and why the status text is copied
 * into a caller buffer.
 *
 * The simulator has no threads and implements the same shapes trivially.
 */

#ifndef WIFI_PORT_H
#define WIFI_PORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_MAX_APS     20      /* matches the row count on PageWifi */
#define WIFI_SSID_MAX    33      /* 32 octets + NUL, per 802.11        */
#define WIFI_PASS_MAX    65

typedef enum {
    TCWIFI_AUTH_OPEN = 0,
    TCWIFI_AUTH_WEP,
    TCWIFI_AUTH_WPA,
    TCWIFI_AUTH_WPA2,             /* also covers WPA3 / mixed mode */
} wifi_auth_t;

typedef enum {
    WIFI_STATE_IDLE = 0,
    WIFI_STATE_SCANNING,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_FAILED,
} wifi_state_t;

typedef struct {
    char        ssid[WIFI_SSID_MAX];
    int8_t      rssi;            /* dBm, negative */
    wifi_auth_t auth;
} wifi_ap_t;

void wifi_port_init(void);

/* Advance time-based work. now_ms is any monotonic millisecond clock.
 * Cheap and non-blocking; safe to call from the UI pump. */
void wifi_port_tick(uint32_t now_ms);

/* Queue a scan. Returns immediately; results appear via wifi_port_ap_count(). */
void wifi_port_scan(void);

int  wifi_port_ap_count(void);

/* Copies entry `index` into *out. Returns false when out of range. The copy
 * is deliberate: the underlying array is rewritten by the worker thread, so a
 * borrowed pointer could be overwritten while the UI is still drawing it. */
bool wifi_port_ap(int index, wifi_ap_t *out);

/* Queue a connect. Returns immediately. */
void wifi_port_connect(const char *ssid, wifi_auth_t auth, const char *pass);

/* Queue a disconnect and clear stored credentials. */
void wifi_port_forget(void);

wifi_state_t wifi_port_state(void);

/* Copies a short, already-uppercased status into `out` -- "SCANNING",
 * "CONNECTED", "BAD PASSWORD". Always NUL-terminates. */
void wifi_port_status_text(char *out, size_t len);

/* Human label for a security type, e.g. "WPA2". Always non-NULL. */
const char *wifi_auth_name(wifi_auth_t auth);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_PORT_H */
