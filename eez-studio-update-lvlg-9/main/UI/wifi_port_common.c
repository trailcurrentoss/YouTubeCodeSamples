/*
 * wifi_port_common — the parts of the port that are identical in both builds.
 * Always compiled, in both the firmware and the simulator.
 */

#include "wifi_port.h"

const char *wifi_auth_name(wifi_auth_t auth)
{
    switch (auth) {
    case TCWIFI_AUTH_OPEN: return "OPEN";
    case TCWIFI_AUTH_WEP:  return "WEP";
    case TCWIFI_AUTH_WPA:  return "WPA";
    case TCWIFI_AUTH_WPA2: return "WPA2";
    default:             return "?";
    }
}
