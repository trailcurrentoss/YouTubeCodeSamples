/*
 * broker_cfg - where the panel keeps its Headwaters broker settings.
 *
 * Host, username and password, entered on PageBroker and stored on the device
 * rather than compiled in. That is deliberate:
 *
 *   - a credential in sdkconfig.defaults would be committed to the repository,
 *     and a secret committed once has to be treated as compromised even after
 *     it is deleted;
 *   - a credential in sdkconfig alone is gitignored, but then pointing the
 *     panel at a different rig needs a rebuild and a cable, which is not
 *     something the person holding it can do.
 *
 * So the settings live in NVS beside the Wi-Fi credentials, and the screen is
 * the way in. Same split as wifi_port: this header and the simulator backend
 * are plain C and live in main/UI/ so the EEZ Studio simulator can compile
 * them; the NVS-backed device implementation lives in main/.
 */

#ifndef BROKER_CFG_H
#define BROKER_CFG_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BROKER_HOST_MAX  64
#define BROKER_USER_MAX  48
#define BROKER_PASS_MAX  64

typedef struct {
    char host[BROKER_HOST_MAX];
    char user[BROKER_USER_MAX];
    char pass[BROKER_PASS_MAX];
} broker_cfg_t;

void broker_cfg_init(void);

/* Copies the stored settings out. Returns false when nothing is configured
 * yet, in which case *out still holds the defaults (host prefilled, empty
 * credentials) so the screen has something sensible to show. */
bool broker_cfg_get(broker_cfg_t *out);

/* Persists and returns true on success. An empty host or username is rejected
 * -- the broker sets allow_anonymous false, so those cannot be optional. */
bool broker_cfg_set(const broker_cfg_t *in);

/* Re-point the MQTT client at whatever is now stored. Called after a
 * successful set(). A no-op in the simulator, which has no broker. */
void broker_cfg_apply(void);

/* Short uppercase status for the header: "NOT CONFIGURED", "SAVED", ... */
void broker_cfg_status_text(char *out, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* BROKER_CFG_H */
