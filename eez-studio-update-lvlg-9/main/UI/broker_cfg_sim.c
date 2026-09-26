/*
 * broker_cfg_sim - the simulator's settings store.
 *
 * Kept in RAM for the life of the process. There is no NVS under Emscripten,
 * and persisting to browser storage would only make the simulator behave
 * differently from the panel in a way nobody asked for. Entering settings,
 * saving them and seeing the status change all work; they simply do not
 * survive a restart.
 */

#ifdef EEZ_LVGL_SIMULATOR

#include "broker_cfg.h"

#include <stdio.h>
#include <string.h>

static broker_cfg_t s;
static bool         s_saved;
static char         s_status[24] = "NOT CONFIGURED";

static void copy_str(char *dst, size_t dstlen, const char *src)
{
    if (!dst || dstlen == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    size_t n = strlen(src);
    if (n >= dstlen) n = dstlen - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
}

void broker_cfg_init(void)
{
    memset(&s, 0, sizeof(s));
    copy_str(s.host, sizeof(s.host), "headwaters.local");
    s_saved = false;
    copy_str(s_status, sizeof(s_status), "NOT CONFIGURED");
}

bool broker_cfg_get(broker_cfg_t *out)
{
    if (!out) return false;
    *out = s;
    return s_saved;
}

bool broker_cfg_set(const broker_cfg_t *in)
{
    if (!in || in->host[0] == '\0' || in->user[0] == '\0') {
        copy_str(s_status, sizeof(s_status), "HOST/USER REQUIRED");
        return false;
    }
    s = *in;
    s_saved = true;
    copy_str(s_status, sizeof(s_status), "SAVED");
    return true;
}

void broker_cfg_apply(void) { /* no broker in the simulator */ }

void broker_cfg_status_text(char *out, size_t len)
{
    if (!out || len == 0) return;
    copy_str(out, len, s_status);
}

#else

typedef int broker_cfg_sim_placeholder;

#endif /* EEZ_LVGL_SIMULATOR */
