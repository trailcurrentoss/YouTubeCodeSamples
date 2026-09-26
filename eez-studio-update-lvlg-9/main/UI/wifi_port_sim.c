/*
 * wifi_port_sim — the simulator's radio.
 *
 * The whole file is inside the EEZ_LVGL_SIMULATOR guard, so on the device it
 * compiles to nothing and the linker never sees it. It still lives in
 * main/UI/ because the full simulator only copies the export folder into its
 * container.
 *
 * This is the answer to the simulator's one real shortcoming: it has no
 * controls, no way to type a value in and watch the UI react. So the controls
 * go in the fake backend instead. The canned list below is chosen to be
 * awkward on purpose -- a 32-character SSID, an open network, a weak one --
 * because those are the rows that break a layout, and standing in a kitchen
 * waiting for a neighbour's AP to appear is a poor way to find that out.
 *
 * The scripted failure matters just as much. On a bench, making a connection
 * fail on demand means deliberately typing the wrong password and waiting for
 * a real timeout. Here, "TrailCurrent-Guest" always rejects the password and
 * "Barn-2G" always times out, so the error paths are reachable every single
 * run.
 */

#ifdef EEZ_LVGL_SIMULATOR

#include "wifi_port.h"

#include <string.h>

/* Deliberately unpleasant test data. */
static const wifi_ap_t CANNED[] = {
    { "Headwaters",                       -42, TCWIFI_AUTH_WPA2 },
    { "TrailCurrent-Guest",               -55, TCWIFI_AUTH_WPA2 },
    { "ThisIsAThirtyTwoCharacterSSID!!",  -61, TCWIFI_AUTH_WPA2 },
    { "Barn-2G",                          -67, TCWIFI_AUTH_WPA  },
    { "campground-wifi",                  -74, TCWIFI_AUTH_OPEN },
    { "NETGEAR47",                        -81, TCWIFI_AUTH_WEP  },
    { "Pixel_1234",                       -86, TCWIFI_AUTH_WPA2 },
    { "shed",                             -89, TCWIFI_AUTH_WPA2 },
};
#define CANNED_N ((int)(sizeof(CANNED) / sizeof(CANNED[0])))

/* Long enough that a UI built against it needs a spinner and an empty state,
 * which is the point -- an instant fake scan lets you design something that
 * only works when the answer is already there. */
#define SIM_SCAN_MS     2500u
#define SIM_CONNECT_MS  3000u

static struct {
    wifi_state_t state;
    uint32_t     deadline;
    bool         timing;
    int          n_aps;
    char         ssid[WIFI_SSID_MAX];
    char         status[24];
    bool         will_fail;
    bool         fail_is_timeout;
} s;

static void set_status(const char *t)
{
    strncpy(s.status, t, sizeof(s.status) - 1);
    s.status[sizeof(s.status) - 1] = '\0';
}

void wifi_port_init(void)
{
    memset(&s, 0, sizeof(s));
    s.state = WIFI_STATE_IDLE;
    set_status("IDLE");
}

void wifi_port_scan(void)
{
    s.state    = WIFI_STATE_SCANNING;
    s.n_aps    = 0;
    s.timing   = false;          /* armed on the next tick, which has the clock */
    s.deadline = 0;
    set_status("SCANNING");
}

void wifi_port_connect(const char *ssid, wifi_auth_t auth, const char *pass)
{
    (void)auth;

    s.ssid[0] = '\0';
    if (ssid) {
        strncpy(s.ssid, ssid, sizeof(s.ssid) - 1);
        s.ssid[sizeof(s.ssid) - 1] = '\0';
    }

    /* Scripted outcomes, so every branch of the UI is reachable on demand. */
    s.fail_is_timeout = (strcmp(s.ssid, "Barn-2G") == 0);
    s.will_fail = s.fail_is_timeout ||
                  (strcmp(s.ssid, "TrailCurrent-Guest") == 0) ||
                  (pass == NULL) || (pass[0] == '\0');

    s.state    = WIFI_STATE_CONNECTING;
    s.timing   = false;
    s.deadline = 0;
    set_status("CONNECTING");
}

void wifi_port_forget(void)
{
    s.state = WIFI_STATE_IDLE;
    s.ssid[0] = '\0';
    set_status("IDLE");
}

void wifi_port_tick(uint32_t now_ms)
{
    if (s.state != WIFI_STATE_SCANNING && s.state != WIFI_STATE_CONNECTING) {
        return;
    }
    if (!s.timing) {
        s.timing   = true;
        s.deadline = now_ms + (s.state == WIFI_STATE_SCANNING ? SIM_SCAN_MS
                                                             : SIM_CONNECT_MS);
        return;
    }
    if ((int32_t)(now_ms - s.deadline) < 0) {
        return;
    }

    s.timing = false;
    if (s.state == WIFI_STATE_SCANNING) {
        s.n_aps = CANNED_N < WIFI_MAX_APS ? CANNED_N : WIFI_MAX_APS;
        s.state = WIFI_STATE_IDLE;
        set_status("SCAN DONE");
    } else if (s.will_fail) {
        s.state = WIFI_STATE_FAILED;
        set_status(s.fail_is_timeout ? "NO REPLY" : "BAD PASSWORD");
    } else {
        s.state = WIFI_STATE_CONNECTED;
        set_status("CONNECTED");
    }
}

int wifi_port_ap_count(void) { return s.n_aps; }

/* Copies out, matching the device backend's contract. There are no threads
 * here, but the two backends must present the same shape or code written
 * against the simulator will be subtly wrong on hardware. */
bool wifi_port_ap(int index, wifi_ap_t *out)
{
    if (!out || index < 0 || index >= s.n_aps) return false;
    *out = CANNED[index];
    return true;
}

wifi_state_t wifi_port_state(void) { return s.state; }

void wifi_port_status_text(char *out, size_t len)
{
    if (!out || len == 0) return;
    strncpy(out, s.status, len - 1);
    out[len - 1] = '\0';
}

#else  /* device build — this translation unit is empty by design */

typedef int wifi_port_sim_placeholder;

#endif /* EEZ_LVGL_SIMULATOR */
