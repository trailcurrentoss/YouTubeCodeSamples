/*
 * rotary_usb — TinyUSB composite device: HID keyboard + one CDC-ACM port.
 * See rotary_usb.h for the interface layout and the companion protocol.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "esp_app_desc.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"

#include "class/hid/hid_device.h"
#include "tinyusb.h"
#include "tinyusb_cdc_acm.h"
#include "tinyusb_default_config.h"

#include "rotary_usb.h"

static const char *TAG = "usb";

/* ------------------------------------------------------------------------ *
 * Descriptors
 * ------------------------------------------------------------------------ */

enum {
    ITF_CDC = 0,      /* CDC uses two interfaces: control + data */
    ITF_CDC_DATA,
    ITF_HID,
    ITF_COUNT,
};

enum {
    STR_LANG = 0,
    STR_MANUFACTURER,
    STR_PRODUCT,
    STR_SERIAL,
    STR_CDC,
    STR_HID,
    STR_COUNT,
};

#define EP_CDC_NOTIF  0x81
#define EP_CDC_OUT    0x02
#define EP_CDC_IN     0x82
#define EP_HID_IN     0x83

static const uint8_t s_hid_report_desc[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(),
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_DESC_LEN)

static const uint8_t s_config_desc[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_COUNT, 0, CONFIG_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_CDC_DESCRIPTOR(ITF_CDC, STR_CDC, EP_CDC_NOTIF, 8,
                       EP_CDC_OUT, EP_CDC_IN, 64),
    TUD_HID_DESCRIPTOR(ITF_HID, STR_HID, HID_ITF_PROTOCOL_KEYBOARD,
                       sizeof(s_hid_report_desc), EP_HID_IN, 8, 5),
};

/*
 * Espressif's VID with the PID TinyUSB's own default scheme gives a
 * CDC + HID device (0x4000 | CDC<<0 | HID<<2). The companion finds the
 * device by the interface string, not by these numbers.
 */
static const tusb_desc_device_t s_device_desc = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    /* Composite with an IAD (the CDC function). */
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0x303A,
    .idProduct          = 0x4005,
    .bcdDevice          = 0x0100,
    .iManufacturer      = STR_MANUFACTURER,
    .iProduct           = STR_PRODUCT,
    .iSerialNumber      = STR_SERIAL,
    .bNumConfigurations = 1,
};

static char s_serial[13];   /* filled from the chip's MAC at init */

static const char *s_strings[STR_COUNT] = {
    [STR_LANG]         = (const char[]){ 0x09, 0x04 },
    [STR_MANUFACTURER] = "TrailCurrent",
    [STR_PRODUCT]      = "Rotary HID",
    [STR_SERIAL]       = s_serial,
    [STR_CDC]          = "Rotary HID companion",
    [STR_HID]          = "Rotary HID keyboard",
};

/* ------------------------------------------------------------------------ *
 * HID callbacks TinyUSB requires
 * ------------------------------------------------------------------------ */

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return s_hid_report_desc;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t *buffer, uint16_t reqlen)
{
    (void)instance; (void)report_id; (void)report_type;
    (void)buffer; (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize)
{
    /* LED state (caps lock etc.) -- not used. */
    (void)instance; (void)report_id; (void)report_type;
    (void)buffer; (void)bufsize;
}

/* ------------------------------------------------------------------------ *
 * Serial output: protocol lines and the log share the one CDC port
 * ------------------------------------------------------------------------ */

static SemaphoreHandle_t s_tx_lock;

static void cdc_write(const char *buf, size_t len)
{
    if (!tud_cdc_n_connected(0) || !s_tx_lock) {
        return;
    }
    /* Never block the caller on USB: a full buffer drops the rest of the
     * line. Logging must not be able to stall the LVGL or USB task. */
    if (xSemaphoreTake(s_tx_lock, 0) != pdTRUE) {
        return;
    }
    tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, (const uint8_t *)buf, len);
    tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
    xSemaphoreGive(s_tx_lock);
}

static void proto_send(const char *fmt, ...)
{
    char line[64];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(line, sizeof(line) - 1, fmt, ap);
    va_end(ap);
    if (n < 0) {
        return;
    }
    if (n > (int)sizeof(line) - 2) {
        n = sizeof(line) - 2;
    }
    line[n++] = '\n';
    cdc_write(line, n);
}

static vprintf_like_t s_prev_vprintf;

/* Log to the previous sink (UART0) AND to the CDC port. */
static int log_vprintf(const char *fmt, va_list ap)
{
    va_list copy;
    va_copy(copy, ap);
    char buf[256];
    int n = vsnprintf(buf, sizeof(buf), fmt, copy);
    va_end(copy);
    if (n > 0) {
        cdc_write(buf, n < (int)sizeof(buf) ? (size_t)n : sizeof(buf) - 1);
    }
    return s_prev_vprintf ? s_prev_vprintf(fmt, ap) : n;
}

/* ------------------------------------------------------------------------ *
 * Reboot into ROM download mode, for flashing
 * ------------------------------------------------------------------------ */

static void bootloader_task(void *arg)
{
    (void)arg;
    ESP_LOGW(TAG, "rebooting into ROM download mode");
    vTaskDelay(pdMS_TO_TICKS(50));   /* let the log line out */
    tud_disconnect();
    vTaskDelay(pdMS_TO_TICKS(100));  /* host sees the detach */

    /*
     * TinyUSB routed the internal PHY to USB-OTG by setting
     * RTCCNTL.usb_conf.sw_hw_usb_phy_sel. That register is in the RTC
     * domain and survives esp_restart(), so without clearing it the ROM
     * would come up on OTG instead of the USB-Serial/JTAG port esptool
     * expects. Zero hands the PHY back to the hardware default.
     */
    RTCCNTL.usb_conf.sw_hw_usb_phy_sel = 0;
    REG_WRITE(RTC_CNTL_OPTION1_REG, RTC_CNTL_FORCE_DOWNLOAD_BOOT);
    esp_restart();
}

static void request_bootloader(void)
{
    static bool requested;
    if (!requested) {
        requested = true;
        xTaskCreate(bootloader_task, "usb_boot", 3072, NULL, 10, NULL);
    }
}

/* ------------------------------------------------------------------------ *
 * Companion protocol (host -> device)
 * ------------------------------------------------------------------------ */

static rotary_usb_app_cb_t s_app_cb;
static void               *s_app_ctx;
static rotary_usb_time_cb_t s_time_cb;
static void                *s_time_ctx;

void rotary_usb_set_time_callback(rotary_usb_time_cb_t cb, void *ctx)
{
    s_time_cb  = cb;
    s_time_ctx = ctx;
}

const char *rotary_app_name(rotary_app_t app)
{
    switch (app) {
    case ROTARY_APP_FREECAD:  return "freecad";
    case ROTARY_APP_BLENDER:  return "blender";
    case ROTARY_APP_KDENLIVE: return "kdenlive";
    case ROTARY_APP_VSCODIUM: return "vscodium";
    case ROTARY_APP_GIMP:     return "gimp";
    case ROTARY_APP_INKSCAPE: return "inkscape";
    case ROTARY_APP_LIBREOFFICE: return "libreoffice";
    case ROTARY_APP_FIREFOX:  return "firefox";
    case ROTARY_APP_CHROMIUM: return "chromium";
    default:                  return "other";
    }
}

static void handle_line(char *line)
{
    /* Trim trailing whitespace ('\r' from terminals). */
    size_t n = strlen(line);
    while (n && (line[n - 1] == '\r' || line[n - 1] == ' ')) {
        line[--n] = '\0';
    }
    if (n == 0) {
        return;
    }

    if (strncmp(line, "app ", 4) == 0) {
        const char *name = line + 4;
        rotary_app_t app = ROTARY_APP_OTHER;
        if (strcmp(name, "freecad") == 0)       { app = ROTARY_APP_FREECAD; }
        else if (strcmp(name, "blender") == 0)  { app = ROTARY_APP_BLENDER; }
        else if (strcmp(name, "kdenlive") == 0) { app = ROTARY_APP_KDENLIVE; }
        else if (strcmp(name, "vscodium") == 0) { app = ROTARY_APP_VSCODIUM; }
        else if (strcmp(name, "gimp") == 0)     { app = ROTARY_APP_GIMP; }
        else if (strcmp(name, "inkscape") == 0) { app = ROTARY_APP_INKSCAPE; }
        else if (strcmp(name, "libreoffice") == 0) { app = ROTARY_APP_LIBREOFFICE; }
        else if (strcmp(name, "firefox") == 0)  { app = ROTARY_APP_FIREFOX; }
        else if (strcmp(name, "chromium") == 0) { app = ROTARY_APP_CHROMIUM; }
        proto_send("@app %s", rotary_app_name(app));
        if (s_app_cb) {
            s_app_cb(app, s_app_ctx);
        }
    } else if (strncmp(line, "time ", 5) == 0) {
        int h, m, sec;
        if (sscanf(line + 5, "%d:%d:%d", &h, &m, &sec) == 3 && s_time_cb) {
            s_time_cb(h, m, sec, s_time_ctx);
        }
    } else if (strcmp(line, "ping") == 0) {
        proto_send("@pong");
    } else if (strcmp(line, "bootloader") == 0) {
        request_bootloader();
    } else {
        ESP_LOGW(TAG, "unknown command '%s'", line);
    }
}

static char   s_rx_line[64];
static size_t s_rx_len;

static void cdc_rx_cb(int itf, cdcacm_event_t *event)
{
    (void)event;
    uint8_t buf[64];
    size_t got = 0;
    if (tinyusb_cdcacm_read(itf, buf, sizeof(buf), &got) != ESP_OK) {
        return;
    }
    for (size_t i = 0; i < got; i++) {
        const char c = (char)buf[i];
        if (c == '\n') {
            s_rx_line[s_rx_len] = '\0';
            handle_line(s_rx_line);
            s_rx_len = 0;
        } else if (s_rx_len < sizeof(s_rx_line) - 1) {
            s_rx_line[s_rx_len++] = c;
        }
        /* An over-long line is truncated rather than overrunning. */
    }
}

static uint32_t s_line_baud;

static void cdc_line_coding_cb(int itf, cdcacm_event_t *event)
{
    (void)itf;
    s_line_baud = event->line_coding_changed_data.p_line_coding->bit_rate;
}

static void cdc_line_state_cb(int itf, cdcacm_event_t *event)
{
    (void)itf;
    const bool dtr = event->line_state_changed_data.dtr;
    if (dtr) {
        s_rx_len = 0;
        proto_send("@hello rotary_hid %s", esp_app_get_description()->version);
    } else if (s_line_baud == 1200) {
        /* The "1200 baud touch": open at 1200, close. Same convention the
         * Arduino core uses, so generic tooling can trigger it. */
        request_bootloader();
    }
}

/* ------------------------------------------------------------------------ *
 * HID typing task
 * ------------------------------------------------------------------------ */

#define TYPE_QUEUE_LEN  64
/* The keyboard endpoint is polled every 5 ms (bInterval in s_config_desc),
 * so 8 ms down and 8 ms up guarantees the host sees both edges. */
#define KEY_HOLD_MS     8
#define KEY_GAP_MS      8

static QueueHandle_t s_type_q;

static bool wait_hid_ready(void)
{
    for (int i = 0; i < 50; i++) {
        if (tud_hid_ready()) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    return false;
}

static void hid_task(void *arg)
{
    (void)arg;
    rotary_chord_t c;
    for (;;) {
        if (xQueueReceive(s_type_q, &c, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (!tud_mounted()) {
            continue;
        }
        if (tud_suspended()) {
            tud_remote_wakeup();
        }
        uint8_t keys[6] = { c.key, 0, 0, 0, 0, 0 };
        if (wait_hid_ready()) {
            tud_hid_keyboard_report(0, c.mods, c.key ? keys : NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(KEY_HOLD_MS));
        if (wait_hid_ready()) {
            tud_hid_keyboard_report(0, 0, NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(KEY_GAP_MS));
    }
}

esp_err_t rotary_usb_type(const rotary_chord_t *chords, size_t count)
{
    if (!tud_mounted()) {
        return ESP_ERR_INVALID_STATE;
    }
    /* All or nothing: a half-typed sequence (e.g. "R X" without the
     * number and Enter) would leave the app mid-command. */
    if (uxQueueSpacesAvailable(s_type_q) < count) {
        return ESP_ERR_TIMEOUT;
    }
    for (size_t i = 0; i < count; i++) {
        xQueueSend(s_type_q, &chords[i], 0);
    }
    return ESP_OK;
}

bool rotary_usb_mounted(void)             { return tud_mounted(); }
bool rotary_usb_companion_connected(void) { return tud_cdc_n_connected(0); }

/* ------------------------------------------------------------------------ */

esp_err_t rotary_usb_init(rotary_usb_app_cb_t on_app, void *ctx)
{
    s_app_cb  = on_app;
    s_app_ctx = ctx;

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(s_serial, sizeof(s_serial), "%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    s_tx_lock = xSemaphoreCreateMutex();
    s_type_q  = xQueueCreate(TYPE_QUEUE_LEN, sizeof(rotary_chord_t));
    ESP_RETURN_ON_FALSE(s_tx_lock && s_type_q, ESP_ERR_NO_MEM, TAG, "alloc");

    tinyusb_config_t cfg = TINYUSB_DEFAULT_CONFIG();
    cfg.descriptor.device            = &s_device_desc;
    cfg.descriptor.string            = s_strings;
    cfg.descriptor.string_count      = STR_COUNT;
    cfg.descriptor.full_speed_config = s_config_desc;
    ESP_RETURN_ON_ERROR(tinyusb_driver_install(&cfg), TAG, "tinyusb install");

    const tinyusb_config_cdcacm_t acm = {
        .cdc_port                     = TINYUSB_CDC_ACM_0,
        .callback_rx                  = cdc_rx_cb,
        .callback_line_state_changed  = cdc_line_state_cb,
        .callback_line_coding_changed = cdc_line_coding_cb,
    };
    ESP_RETURN_ON_ERROR(tinyusb_cdcacm_init(&acm), TAG, "cdc init");

    ESP_RETURN_ON_FALSE(xTaskCreate(hid_task, "usb_hid", 3072, NULL, 5, NULL) == pdPASS,
                        ESP_ERR_NO_MEM, TAG, "hid task");

    s_prev_vprintf = esp_log_set_vprintf(log_vprintf);

    ESP_LOGI(TAG, "USB up: HID keyboard + CDC, serial %s", s_serial);
    return ESP_OK;
}
