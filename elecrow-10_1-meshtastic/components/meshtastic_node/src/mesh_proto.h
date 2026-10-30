#pragma once

/*
 * Meshtastic protobuf field numbers and enum values used by this node.
 *
 * Numbers follow the public Meshtastic protocol definitions
 * (github.com/meshtastic/protobufs) so the official apps and other nodes
 * interoperate with us. Only the subset this node reads or writes is listed.
 */

/* --- PortNum --- */
#define PORT_TEXT_MESSAGE   1
#define PORT_POSITION       3
#define PORT_NODEINFO       4
#define PORT_ROUTING        5
#define PORT_ADMIN          6
#define PORT_TELEMETRY      67
#define PORT_TRACEROUTE     70

/* --- ToRadio --- */
#define TORADIO_PACKET          1
#define TORADIO_WANT_CONFIG_ID  3
#define TORADIO_DISCONNECT      4
#define TORADIO_HEARTBEAT       7

/* --- FromRadio --- */
#define FROMRADIO_ID                 1
#define FROMRADIO_PACKET             2
#define FROMRADIO_MY_INFO            3
#define FROMRADIO_NODE_INFO          4
#define FROMRADIO_CONFIG             5
#define FROMRADIO_CONFIG_COMPLETE_ID 7
#define FROMRADIO_REBOOTED           8
#define FROMRADIO_CHANNEL            10
#define FROMRADIO_QUEUE_STATUS       11
#define FROMRADIO_METADATA           13

/* Special want_config nonces used by newer apps for two-phase sync. */
#define NONCE_ONLY_CONFIG 69420
#define NONCE_ONLY_NODES  69421

/* --- MeshPacket --- */
#define MP_FROM          1   /* fixed32 */
#define MP_TO            2   /* fixed32 */
#define MP_CHANNEL       3
#define MP_DECODED       4   /* Data */
#define MP_ENCRYPTED     5
#define MP_ID            6   /* fixed32 */
#define MP_RX_TIME       7   /* fixed32 */
#define MP_RX_SNR        8   /* float */
#define MP_HOP_LIMIT     9
#define MP_WANT_ACK      10
#define MP_RX_RSSI       12  /* int32 */
#define MP_HOP_START     15
#define MP_PKI_ENCRYPTED 17
#define MP_NEXT_HOP      18
#define MP_RELAY_NODE    19

/* --- Data --- */
#define DATA_PORTNUM       1
#define DATA_PAYLOAD       2
#define DATA_WANT_RESPONSE 3
#define DATA_DEST          4   /* fixed32 */
#define DATA_SOURCE        5   /* fixed32 */
#define DATA_REQUEST_ID    6   /* fixed32 */
#define DATA_REPLY_ID      7   /* fixed32 */
#define DATA_BITFIELD      9

/* --- Routing --- */
#define ROUTING_ERROR_REASON 3
#define ROUTING_ERR_NONE           0
#define ROUTING_ERR_NO_ROUTE       1
#define ROUTING_ERR_MAX_RETRANSMIT 5

/* --- User --- */
#define USER_ID         1
#define USER_LONG_NAME  2
#define USER_SHORT_NAME 3
#define USER_MACADDR    4
#define USER_HW_MODEL   5
#define USER_ROLE       7
#define USER_PUBLIC_KEY 8

#define HW_MODEL_PRIVATE_HW 255

/* --- NodeInfo --- */
#define NI_NUM        1
#define NI_USER       2
#define NI_SNR        4   /* float */
#define NI_LAST_HEARD 5   /* fixed32 */
#define NI_CHANNEL    7
#define NI_HOPS_AWAY  9

/* --- MyNodeInfo --- */
#define MYINFO_NODE_NUM        1
#define MYINFO_REBOOT_COUNT    8
#define MYINFO_MIN_APP_VERSION 11
#define MYINFO_DEVICE_ID       12
#define MYINFO_PIO_ENV         13
#define MYINFO_NODEDB_COUNT    15

/* --- DeviceMetadata --- */
#define META_FIRMWARE_VERSION      1
#define META_DEVICE_STATE_VERSION  2
#define META_CAN_SHUTDOWN          3
#define META_HAS_WIFI              4
#define META_HAS_BLUETOOTH         5
#define META_ROLE                  7
#define META_HW_MODEL              9
#define META_HAS_PKC               11

/* --- QueueStatus --- */
#define QS_RES            1
#define QS_FREE           2
#define QS_MAXLEN         3
#define QS_MESH_PACKET_ID 4

/* --- Channel / ChannelSettings --- */
#define CH_INDEX    1
#define CH_SETTINGS 2
#define CH_ROLE     3
#define CHROLE_DISABLED  0
#define CHROLE_PRIMARY   1
#define CHROLE_SECONDARY 2

#define CHS_PSK              2
#define CHS_NAME             3
#define CHS_ID               4   /* fixed32 */
#define CHS_UPLINK_ENABLED   5
#define CHS_DOWNLINK_ENABLED 6
#define CHS_MODULE_SETTINGS  7

/* --- Config oneof --- */
#define CFG_DEVICE     1
#define CFG_POSITION   2
#define CFG_POWER      3
#define CFG_NETWORK    4
#define CFG_DISPLAY    5
#define CFG_LORA       6
#define CFG_BLUETOOTH  7
#define CFG_SECURITY   8
#define CFG_SESSIONKEY 9

#define DEVCFG_ROLE                   1
#define DEVCFG_NODE_INFO_BCAST_SECS   7
#define DEVROLE_CLIENT 0

#define LORA_USE_PRESET     1
#define LORA_MODEM_PRESET   2
#define LORA_REGION         7
#define LORA_HOP_LIMIT      8
#define LORA_TX_ENABLED     9
#define LORA_TX_POWER       10
#define LORA_CHANNEL_NUM    11
#define LORA_RX_BOOSTED     13

#define REGION_UNSET  0
#define REGION_US     1
#define REGION_JP     5
#define REGION_ANZ    6
#define REGION_KR     7
#define REGION_TW     8
#define REGION_TH     12
#define REGION_MY_919 17
#define REGION_SG_923 18
#define REGION_PH_915 21
#define REGION_BR_902 26

/* ModemPreset */
#define PRESET_LONG_FAST     0
#define PRESET_LONG_SLOW     1
#define PRESET_MEDIUM_SLOW   3
#define PRESET_MEDIUM_FAST   4
#define PRESET_SHORT_SLOW    5
#define PRESET_SHORT_FAST    6
#define PRESET_LONG_MODERATE 7
#define PRESET_SHORT_TURBO   8
#define PRESET_LONG_TURBO    9
#define PRESET_MEDIUM_TURBO  16

#define BT_ENABLED   1
#define BT_MODE      2
#define BT_FIXED_PIN 3
#define BT_MODE_RANDOM_PIN 0
#define BT_MODE_FIXED_PIN  1
#define BT_MODE_NO_PIN     2

/* AdminMessage.ConfigType, used by get_config_request */
#define CFGTYPE_DEVICE     0
#define CFGTYPE_POSITION   1
#define CFGTYPE_POWER      2
#define CFGTYPE_NETWORK    3
#define CFGTYPE_DISPLAY    4
#define CFGTYPE_LORA       5
#define CFGTYPE_BLUETOOTH  6
#define CFGTYPE_SECURITY   7
#define CFGTYPE_SESSIONKEY 8

/* --- AdminMessage --- */
#define ADM_GET_CHANNEL_REQ    1
#define ADM_GET_CHANNEL_RESP   2
#define ADM_GET_OWNER_REQ      3
#define ADM_GET_OWNER_RESP     4
#define ADM_GET_CONFIG_REQ     5
#define ADM_GET_CONFIG_RESP    6
#define ADM_GET_MODCFG_REQ     7
#define ADM_GET_MODCFG_RESP    8
#define ADM_GET_METADATA_REQ   12
#define ADM_GET_METADATA_RESP  13
#define ADM_SET_OWNER          32
#define ADM_SET_CHANNEL        33
#define ADM_SET_CONFIG         34
#define ADM_SET_MODCFG         35
#define ADM_REMOVE_BY_NODENUM  38
#define ADM_SET_TIME_ONLY      43
#define ADM_BEGIN_EDIT         64
#define ADM_COMMIT_EDIT        65
#define ADM_REBOOT_SECONDS     97
#define ADM_NODEDB_RESET       100
#define ADM_SESSION_PASSKEY    101
