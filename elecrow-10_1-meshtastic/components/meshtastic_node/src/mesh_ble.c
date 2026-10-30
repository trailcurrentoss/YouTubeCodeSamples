/*
 * BLE transport for the Meshtastic phone API.
 *
 * The P4 has no radio of its own: NimBLE runs here and reaches the ESP32-C6's
 * BLE controller over ESP-Hosted (HCI over SDIO, "VHCI"). The GATT layout is
 * the Meshtastic service the apps scan for.
 */

#include <string.h>
#include "esp_hosted.h"
#include "esp_check.h"
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "mesh_internal.h"

static const char *TAG = "mesh_ble";

static const ble_uuid128_t SVC_UUID = BLE_UUID128_INIT(
    0xfd, 0xea, 0x73, 0xe2, 0xca, 0x5d, 0xa8, 0x9f, 0x1f, 0x46, 0xa8, 0x15, 0x18, 0xb2, 0xa1, 0x6b);
static const ble_uuid128_t TORADIO_UUID = BLE_UUID128_INIT(
    0xe7, 0x01, 0x44, 0x12, 0x66, 0x78, 0xdd, 0xa1, 0xad, 0x4d, 0x9e, 0x12, 0xd2, 0x76, 0x5c, 0xf7);
static const ble_uuid128_t FROMRADIO_UUID = BLE_UUID128_INIT(
    0x02, 0x00, 0x12, 0xac, 0x42, 0x02, 0x78, 0xb8, 0xed, 0x11, 0x93, 0x49, 0x9e, 0xe6, 0x55, 0x2c);
static const ble_uuid128_t FROMNUM_UUID = BLE_UUID128_INIT(
    0x53, 0x44, 0xe3, 0x47, 0x75, 0xaa, 0x70, 0xa6, 0x66, 0x4f, 0x00, 0xa8, 0x8c, 0xa1, 0x9d, 0xed);

static uint16_t s_fromnum_handle;
static uint16_t s_conn = BLE_HS_CONN_HANDLE_NONE;
static uint8_t  s_own_addr_type;
static uint32_t s_fromnum;
static char     s_name[16];

/*
 * A FromRadio longer than MTU-1 is fetched with Read + Read Blob requests,
 * and NimBLE calls the access callback for each one (trimming by offset
 * itself). Keep the current item until all its pieces have been read so a
 * blob request doesn't pop the next queued packet.
 */
static uint8_t s_fr_buf[MESH_FROMRADIO_MAX];
static size_t  s_fr_len;
static int     s_fr_blobs_left;

static void advertise(void);

/* --- GATT --- */

static int chr_access(uint16_t conn, uint16_t attr, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    const ble_uuid_t *uuid = ctxt->chr->uuid;

    if (ble_uuid_cmp(uuid, &TORADIO_UUID.u) == 0 && ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        uint8_t buf[MESH_FROMRADIO_MAX];
        uint16_t len = 0;
        if (ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &len) != 0) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        phone_handle_toradio(buf, len);
        return 0;
    }

    if (ble_uuid_cmp(uuid, &FROMRADIO_UUID.u) == 0 && ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        size_t chunk = ble_att_mtu(conn) - 1;
        if (s_fr_blobs_left > 0) {
            s_fr_blobs_left--;
        } else {
            s_fr_len = phone_get_fromradio(s_fr_buf, sizeof(s_fr_buf));
            s_fr_blobs_left = chunk ? s_fr_len / chunk : 0;
        }
        return os_mbuf_append(ctxt->om, s_fr_buf, s_fr_len) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (ble_uuid_cmp(uuid, &FROMNUM_UUID.u) == 0 && ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        uint8_t le[4] = { s_fromnum, s_fromnum >> 8, s_fromnum >> 16, s_fromnum >> 24 };
        return os_mbuf_append(ctxt->om, le, sizeof(le)) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

static const struct ble_gatt_svc_def SERVICES[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &SVC_UUID.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            { .uuid = &TORADIO_UUID.u, .access_cb = chr_access,
              .flags = BLE_GATT_CHR_F_WRITE },
            { .uuid = &FROMRADIO_UUID.u, .access_cb = chr_access,
              .flags = BLE_GATT_CHR_F_READ },
            { .uuid = &FROMNUM_UUID.u, .access_cb = chr_access, .val_handle = &s_fromnum_handle,
              .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY },
            { 0 },
        },
    },
    { 0 },
};

void mesh_ble_notify_fromnum(uint32_t num)
{
    s_fromnum = num;
    if (s_conn != BLE_HS_CONN_HANDLE_NONE) ble_gatts_chr_updated(s_fromnum_handle);
}

/* --- GAP --- */

static int gap_event(struct ble_gap_event *ev, void *arg)
{
    switch (ev->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (ev->connect.status == 0) {
            s_conn = ev->connect.conn_handle;
            s_fr_blobs_left = 0;
            phone_connected();
            ble_gattc_exchange_mtu(s_conn, NULL, NULL);
            ESP_LOGI(TAG, "phone connected (handle %u)", s_conn);
        } else {
            advertise();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "phone disconnected (reason 0x%x)", ev->disconnect.reason);
        s_conn = BLE_HS_CONN_HANDLE_NONE;
        phone_disconnected();
        advertise();
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        if (s_conn == BLE_HS_CONN_HANDLE_NONE) advertise();
        break;
    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "MTU %u", ev->mtu.value);
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG, "subscribe handle %u notify %d", ev->subscribe.attr_handle, ev->subscribe.cur_notify);
        break;
    default:
        break;
    }
    return 0;
}

static void advertise(void)
{
    struct ble_hs_adv_fields adv = {0};
    adv.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    adv.uuids128 = &SVC_UUID;
    adv.num_uuids128 = 1;
    adv.uuids128_is_complete = 1;
    int rc = ble_gap_adv_set_fields(&adv);
    if (rc) ESP_LOGE(TAG, "adv fields rc=%d", rc);

    struct ble_hs_adv_fields rsp = {0};
    rsp.name = (const uint8_t *)s_name;
    rsp.name_len = strlen(s_name);
    rsp.name_is_complete = 1;
    rsp.tx_pwr_lvl_is_present = 1;
    rsp.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    rc = ble_gap_adv_rsp_set_fields(&rsp);
    if (rc) ESP_LOGE(TAG, "scan rsp rc=%d", rc);

    struct ble_gap_adv_params params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
    };
    rc = ble_gap_adv_start(s_own_addr_type, NULL, BLE_HS_FOREVER, &params, gap_event, NULL);
    if (rc && rc != BLE_HS_EALREADY) ESP_LOGE(TAG, "adv start rc=%d", rc);
}

static void on_sync(void)
{
    ble_hs_util_ensure_addr(0);
    ble_hs_id_infer_auto(0, &s_own_addr_type);
    uint8_t addr[6] = {0};
    ble_hs_id_copy_addr(s_own_addr_type, addr, NULL);
    ESP_LOGI(TAG, "advertising as \"%s\" (%02x:%02x:%02x:%02x:%02x:%02x)", s_name,
             addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    advertise();
}

static void on_reset(int reason)
{
    ESP_LOGW(TAG, "NimBLE host reset, reason %d", reason);
}

static void host_task(void *param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

esp_err_t mesh_ble_start(const char *device_name)
{
    strlcpy(s_name, device_name, sizeof(s_name));

    /* Safe to call when WiFi already brought the transport up. */
    esp_hosted_connect_to_slave();
    ESP_RETURN_ON_ERROR(esp_hosted_bt_controller_init(), TAG, "C6 BT controller init");
    ESP_RETURN_ON_ERROR(esp_hosted_bt_controller_enable(), TAG, "C6 BT controller enable");
    ESP_RETURN_ON_ERROR(nimble_port_init(), TAG, "nimble init");

    ble_hs_cfg.sync_cb = on_sync;
    ble_hs_cfg.reset_cb = on_reset;
    ble_hs_cfg.sm_io_cap = BLE_HS_IO_NO_INPUT_OUTPUT;
    ble_hs_cfg.sm_bonding = 0;

    ble_svc_gap_init();
    ble_svc_gatt_init();
    int rc = ble_gatts_count_cfg(SERVICES);
    if (rc == 0) rc = ble_gatts_add_svcs(SERVICES);
    if (rc) {
        ESP_LOGE(TAG, "GATT register rc=%d", rc);
        return ESP_FAIL;
    }
    ble_svc_gap_device_name_set(s_name);
    ble_att_set_preferred_mtu(512);

    nimble_port_freertos_init(host_task);
    return ESP_OK;
}

bool mesh_ble_connected(void)
{
    return s_conn != BLE_HS_CONN_HANDLE_NONE;
}

const char *mesh_ble_name(void)
{
    return s_name;
}
