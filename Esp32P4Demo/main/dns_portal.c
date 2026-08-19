/*
 * Minimal captive-portal DNS responder.
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "dns_portal.h"

static const char *TAG = "dns_portal";

#define DNS_PORT        53
#define DNS_MAX_LEN     512

/* Flags for a standard, successful, authoritative-ish response. */
#define DNS_QR_RESPONSE 0x8000
#define DNS_AA          0x0400
#define DNS_OPCODE_MASK 0x7800

#define DNS_TYPE_A      1
#define DNS_CLASS_IN    1

typedef struct __attribute__((packed)) {
    uint16_t id;
    uint16_t flags;
    uint16_t qd_count;
    uint16_t an_count;
    uint16_t ns_count;
    uint16_t ar_count;
} dns_header_t;

/* Answer record using a name-compression pointer back to the question at
 * offset 12, which is where the question always starts in our replies. */
typedef struct __attribute__((packed)) {
    uint16_t name_ptr;
    uint16_t type;
    uint16_t klass;
    uint32_t ttl;
    uint16_t rd_length;
    uint32_t address;
} dns_answer_t;

static TaskHandle_t s_task;
static int          s_sock = -1;
static volatile bool s_running;

static void dns_task(void *arg)
{
    uint8_t rx[DNS_MAX_LEN];
    uint8_t tx[DNS_MAX_LEN];

    while (s_running) {
        struct sockaddr_in from = {0};
        socklen_t from_len = sizeof(from);

        const int len = recvfrom(s_sock, rx, sizeof(rx), 0,
                                 (struct sockaddr *)&from, &from_len);
        if (len < (int)sizeof(dns_header_t)) {
            continue;
        }

        const dns_header_t *q_hdr = (const dns_header_t *)rx;

        /* Only answer ordinary queries with exactly one question. */
        if ((ntohs(q_hdr->flags) & (DNS_QR_RESPONSE | DNS_OPCODE_MASK)) != 0 ||
            ntohs(q_hdr->qd_count) != 1) {
            continue;
        }

        /* Walk the QNAME label sequence to find where the question ends. */
        int pos = sizeof(dns_header_t);
        while (pos < len && rx[pos] != 0) {
            pos += rx[pos] + 1;              /* length byte + label */
            if (pos >= len) {
                break;
            }
        }
        pos += 1;                            /* the terminating zero byte */
        if (pos + 4 > len) {
            continue;                        /* truncated QTYPE/QCLASS */
        }

        uint16_t qtype;
        memcpy(&qtype, &rx[pos], sizeof(qtype));
        pos += 4;                            /* QTYPE + QCLASS */

        if (ntohs(qtype) != DNS_TYPE_A) {
            continue;                        /* only A records are useful here */
        }

        /* Current SoftAP address, read each time so it stays correct even if
         * the netif is reconfigured. */
        esp_netif_ip_info_t ip = {0};
        esp_netif_t *ap = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
        if (ap == NULL || esp_netif_get_ip_info(ap, &ip) != ESP_OK) {
            continue;
        }

        if (pos + (int)sizeof(dns_answer_t) > (int)sizeof(tx)) {
            continue;
        }

        memcpy(tx, rx, pos);                 /* header + original question */
        dns_header_t *r_hdr = (dns_header_t *)tx;
        r_hdr->flags    = htons(DNS_QR_RESPONSE | DNS_AA);
        r_hdr->an_count = htons(1);
        r_hdr->ns_count = 0;
        r_hdr->ar_count = 0;

        dns_answer_t answer = {
            .name_ptr  = htons(0xC000 | sizeof(dns_header_t)),
            .type      = htons(DNS_TYPE_A),
            .klass     = htons(DNS_CLASS_IN),
            .ttl       = htonl(0),           /* never cache; the portal is transient */
            .rd_length = htons(4),
            .address   = ip.ip.addr,
        };
        memcpy(&tx[pos], &answer, sizeof(answer));

        sendto(s_sock, tx, pos + sizeof(answer), 0,
               (struct sockaddr *)&from, from_len);
    }

    close(s_sock);
    s_sock = -1;
    s_task = NULL;
    vTaskDelete(NULL);
}

esp_err_t dns_portal_start(void)
{
    if (s_task != NULL) {
        return ESP_OK;
    }

    s_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s_sock < 0) {
        ESP_LOGE(TAG, "Could not create the DNS socket");
        return ESP_FAIL;
    }

    const struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(DNS_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    if (bind(s_sock, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ESP_LOGE(TAG, "Could not bind UDP port %d", DNS_PORT);
        close(s_sock);
        s_sock = -1;
        return ESP_FAIL;
    }

    s_running = true;
    if (xTaskCreate(dns_task, "dns_portal", 3072, NULL, 4, &s_task) != pdPASS) {
        s_running = false;
        close(s_sock);
        s_sock = -1;
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Captive portal DNS running");
    return ESP_OK;
}

void dns_portal_stop(void)
{
    s_running = false;
    if (s_sock >= 0) {
        shutdown(s_sock, SHUT_RDWR);   /* unblock recvfrom so the task exits */
    }
}
