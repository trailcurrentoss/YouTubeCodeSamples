# Meshtastic messenger — Elecrow CrowPanel Advance 10.1" (ESP32-P4)

A standalone [Meshtastic](https://meshtastic.org) node with its own touchscreen
UI. The Elecrow SX1262 LoRa module that ships for the panel's wireless-module
socket carries the mesh; you read and send messages on the 10.1" display, and no
phone is needed.

The node also speaks the Meshtastic phone API over Bluetooth, so the official
Meshtastic apps can still connect to it and see the same messages.

Stock Meshtastic firmware does not run on the ESP32-P4, so this project
implements the node itself (`components/meshtastic_node`). It talks to other
Meshtastic devices over the air on the default LongFast channel.

## Hardware

- Elecrow CrowPanel Advance 10.1" HMI, ESP32-P4 + ESP32-C6 (V1.1 or later)
- Elecrow SX1262 LoRa wireless module (915 MHz), plugged into the wireless-module
  socket on the back of the panel, antenna attached

Attach the antenna before powering the panel: the module transmits at up to
22 dBm.

The socket's IRQ and RESET lines moved between board revisions (V1.0/V1.1:
GPIO53/54, V1.2: GPIO27/28). The firmware works out which one it is on at boot,
reading only inputs to do it. It can be forced under
`idf.py menuconfig` → Meshtastic node.

Bluetooth uses the ESP32-C6 coprocessor through ESP-Hosted. The C6 firmware
Elecrow ships (ESP-Hosted 2.12.3) already includes the BLE controller; it does
not need reflashing.

## Screens

| Page | What it does |
|---|---|
| Messages | The LongFast channel plus up to seven direct-message threads. Tap the compose box to type; your messages show "Relayed by the mesh" / "Delivered" when acknowledged. |
| Nodes | Every node heard, most recent first: name, node ID, last heard, SNR, hop count. **Message** opens a direct thread. |
| Settings | Rename this node (long and 4-character short name). Set the radio to match other nodes: region, modem preset, frequency slot, hop limit, TX power, and the primary channel's name and key. The resulting frequency is shown before you press **Apply radio settings**. |

## Building

The UI is designed in [EEZ Studio](https://github.com/eez-open/studio); its C
output is generated, not hand-written.

1. Open `GUI/elecrow-10_1-meshtastic/elecrow-10_1-meshtastic.eez-project` in EEZ
   Studio and press **Build** (Ctrl+B). This writes `main/ui/`.
2. Build and flash with ESP-IDF 5.5:

   ```bash
   idf.py set-target esp32p4
   idf.py build
   idf.py -p <serial-port> flash monitor
   ```

   `<serial-port>` is the panel's USB-C **UART** port (a CH340), for example
   `/dev/ttyUSB0` on Linux.

Without step 1 the firmware still builds, but the panel only shows a message
asking for the EEZ Studio export.

After changing `sdkconfig.defaults*`, regenerate the build config so the change
takes effect: `rm sdkconfig && idf.py build`.

## Region and channel

The default region is US (906.875 MHz, LongFast) with the default channel key,
so the node talks to any stock Meshtastic device on default settings.

Two nodes only hear each other when they agree on all of these:

- **Region** — US, JP, ANZ, KR, TW, TH, MY_919, SG_923, PH_915 or BR_902. These
  are the Meshtastic regions inside 902-928 MHz; the others are refused because
  the module and antenna are 915 MHz parts.
- **Modem preset** — any of the ten presets those regions offer (LongFast,
  MediumFast, ShortTurbo and so on).
- **Frequency slot** — *Auto* picks the slot from the channel name, as stock
  firmware does. A node set to a numbered slot needs the same number here.
- **Primary channel name and key** — leave the name blank to use the preset's
  name. The key is shown in base64, as in the Meshtastic apps; `AQ==` is the
  default key and an empty key turns encryption off.

Set them on the Settings page, or from the Meshtastic app over Bluetooth. The
region the node starts in can also be chosen in `menuconfig`. TX power is capped
at the region's legal limit and at the module's 22 dBm.

## Limits

- The node does not enforce duty-cycle limits. That matters only in TH, which
  allows 10%; keep traffic light there.

- Direct messages use the channel key. Meshtastic 2.5+ public-key (PKI)
  direct messages addressed to this node cannot be decrypted.
- The Bluetooth link uses no PIN, so anyone nearby can pair with the node.
- The screen keeps the last 20 messages per conversation and lists the 40
  most recently heard nodes; messages are not stored across restarts.
- Text fonts cover Latin-1; emoji and other scripts show as boxes.
- The on-screen keyboard is LVGL's built-in English layout.

## Layout

```
components/
  bsp_*            CrowPanel display, touch, I2C, backlight
  meshtastic_node  LoRa radio driver, mesh routing, phone API over BLE
GUI/               EEZ Studio project + fonts
main/
  main.c           board bring-up (display + touch first, coprocessor last)
  mesh_ui.c        conversation store and screen painter
  actions.c        EEZ Studio action handlers
  ui/              EEZ Studio output (generated — do not edit)
```
