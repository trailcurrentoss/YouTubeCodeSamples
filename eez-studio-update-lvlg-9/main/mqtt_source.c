/*
 * mqtt_source — the dashboard's real data, subscribed from Headwaters.
 *
 * Headwaters is the server in the rig. It owns the physical CAN bus and
 * republishes what it decodes onto MQTT, so this panel never touches CAN
 * itself -- it subscribes to two topics and feeds dash_data:
 *
 *   local/energy/status   battery_voltage, battery_percent,
 *                         consumption_watts, solar_watts, charge_type
 *   local/water/status    fresh, grey, black   (percent full)
 *
 * The energy payload is a MERGED state object: Headwaters keeps one struct and
 * republishes all of it whenever any single CAN frame updates part of it. So
 * every message carries every field it has seen so far, and a field that has
 * not arrived yet is simply absent rather than zero. That is why each lookup
 * below is optional and nothing is defaulted to 0 -- a missing
 * consumption_watts means "not known", and treating it as "no load" would put
 * a confident wrong number on the screen.
 *
 * Device only: the simulator gets its numbers from sim_feed() in dash_data.c.
 */

#ifndef EEZ_LVGL_SIMULATOR

#include "broker_cfg.h"
#include "dash_data.h"

#include <string.h>

#include "cJSON.h"
#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "mqtt_source";

#define TOPIC_ENERGY "local/energy/status"
#define TOPIC_WATER  "local/water/status"

static esp_mqtt_client_handle_t client;

/* Headwaters publishes charge_type as a word from the MPPT's own enum, and
 * "unknown" / "off" mean it is not charging. Anything else is a real charge
 * stage worth showing. */
static bool charging_word(const char *w)
{
    return w && *w
        && strcmp(w, "unknown") != 0
        && strcmp(w, "off") != 0
        && strcmp(w, "not charging") != 0;
}

static void handle_energy(const cJSON *root)
{
    const cJSON *v    = cJSON_GetObjectItemCaseSensitive(root, "battery_voltage");
    const cJSON *pct  = cJSON_GetObjectItemCaseSensitive(root, "battery_percent");
    const cJSON *load = cJSON_GetObjectItemCaseSensitive(root, "consumption_watts");
    const cJSON *sol  = cJSON_GetObjectItemCaseSensitive(root, "solar_watts");
    const cJSON *chg  = cJSON_GetObjectItemCaseSensitive(root, "charge_type");

    if (cJSON_IsNumber(v)) {
        const int32_t mv = (int32_t)(v->valuedouble * 1000.0 + 0.5);
        const int32_t soc_tenths = cJSON_IsNumber(pct)
                                 ? (int32_t)(pct->valuedouble * 10.0 + 0.5)
                                 : -1;          /* -1 = fall back to the curve */
        const int32_t load_w = cJSON_IsNumber(load) ? (int32_t)load->valuedouble : 0;
        const char *state = (cJSON_IsString(chg) && charging_word(chg->valuestring))
                          ? chg->valuestring : NULL;
        dash_data_set_battery(mv, soc_tenths, load_w, state);
    }

    if (cJSON_IsNumber(sol)) {
        dash_data_set_solar((int32_t)sol->valuedouble);
    }
}

static void handle_water(const cJSON *root)
{
    const cJSON *fresh = cJSON_GetObjectItemCaseSensitive(root, "fresh");
    const cJSON *grey  = cJSON_GetObjectItemCaseSensitive(root, "grey");
    if (cJSON_IsNumber(fresh) && cJSON_IsNumber(grey)) {
        dash_data_set_tanks((int32_t)fresh->valuedouble,
                            (int32_t)grey->valuedouble);
    }
}

static void on_data(const esp_mqtt_event_handle_t ev)
{
    /* Payloads arrive as a length-counted buffer with no NUL, and a large one
     * can be split across events. Anything chunked is dropped rather than
     * parsed half-way; these two topics are far below the buffer size. */
    if (ev->current_data_offset != 0 || ev->data_len != ev->total_data_len) {
        ESP_LOGW(TAG, "ignoring chunked payload (%d of %d bytes)",
                 ev->data_len, ev->total_data_len);
        return;
    }

    cJSON *root = cJSON_ParseWithLength(ev->data, (size_t)ev->data_len);
    if (!root) {
        ESP_LOGW(TAG, "unparseable JSON on %.*s", ev->topic_len, ev->topic);
        return;
    }

    if (ev->topic_len == (int)strlen(TOPIC_ENERGY) &&
        strncmp(ev->topic, TOPIC_ENERGY, (size_t)ev->topic_len) == 0) {
        handle_energy(root);
    } else if (ev->topic_len == (int)strlen(TOPIC_WATER) &&
               strncmp(ev->topic, TOPIC_WATER, (size_t)ev->topic_len) == 0) {
        handle_water(root);
    }

    cJSON_Delete(root);
}

static void on_mqtt(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg; (void)base;
    const esp_mqtt_event_handle_t ev = (esp_mqtt_event_handle_t)data;

    switch ((esp_mqtt_event_id_t)id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "connected to broker");
        esp_mqtt_client_subscribe(client, TOPIC_ENERGY, 0);
        esp_mqtt_client_subscribe(client, TOPIC_WATER, 0);
        dash_data_set_link(true, "HEADWATERS");
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "disconnected");
        dash_data_set_link(false, NULL);
        break;

    case MQTT_EVENT_DATA:
        on_data(ev);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "mqtt error");
        break;

    default:
        break;
    }
}

static void build_uri(char *out, size_t len, const char *host)
{
    /* mqtts on 8883, not mqtt on 1883: the Headwaters broker runs a TLS
     * listener with allow_anonymous false. Aimed at 1883 in plain MQTT it
     * accepts the socket and drops it, which surfaces as "Connection reset by
     * peer" and says nothing about TLS. */
    snprintf(out, len, "mqtts://%s:8883", host);
}

void mqtt_source_start(void)
{
    broker_cfg_t c;
    const bool configured = broker_cfg_get(&c);
    if (!configured) {
        ESP_LOGW(TAG, "broker not configured; open the Headwaters screen on "
                      "the panel to enter host, username and password");
        dash_data_set_link(false, NULL);
        return;
    }

    static char uri[BROKER_HOST_MAX + 24];
    build_uri(uri, sizeof(uri), c.host);

    static char user[BROKER_USER_MAX];
    static char pass[BROKER_PASS_MAX];
    snprintf(user, sizeof(user), "%s", c.user);
    snprintf(pass, sizeof(pass), "%s", c.pass);

    const esp_mqtt_client_config_t cfg = {
        .broker.address.uri = uri,
        /* The broker's certificate is self-signed and no CA ships on the
         * panel, so the CN check is skipped here and verification itself is
         * disabled by CONFIG_ESP_TLS_INSECURE +
         * CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY in sdkconfig.defaults. Both
         * halves are required: without the sdkconfig flags the handshake
         * fails inside esp-tls with 0x8017 before this flag is consulted.
         * Same arrangement the Fireside firmware uses against this broker. */
        .broker.verification.skip_cert_common_name_check = true,
        .credentials.client_id               = "trailcurrent-powerdash",
        .credentials.username                = user,
        .credentials.authentication.password = pass,
        .session.keepalive  = CONFIG_DASH_MQTT_KEEPALIVE,
        .network.timeout_ms = CONFIG_DASH_MQTT_TIMEOUT_MS,
    };

    client = esp_mqtt_client_init(&cfg);
    if (!client) {
        ESP_LOGE(TAG, "esp_mqtt_client_init failed");
        return;
    }
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, on_mqtt, NULL);
    esp_mqtt_client_start(client);
    ESP_LOGI(TAG, "client started, broker %s", uri);
}

/* Called when the settings change. Tear the old client down first: starting a
 * second one would leave two clients racing to publish the same link state. */
void mqtt_source_restart(void)
{
    if (client) {
        esp_mqtt_client_stop(client);
        esp_mqtt_client_destroy(client);
        client = NULL;
        dash_data_set_link(false, NULL);
    }
    mqtt_source_start();
}

#else  /* simulator build */

typedef int mqtt_source_placeholder;

#endif /* EEZ_LVGL_SIMULATOR */
