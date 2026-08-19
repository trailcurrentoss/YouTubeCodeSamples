# Elecrow CrowPanel Advance ESP32-P4 — Board-Specific Lessons

Hardware-specific traps and non-obvious details for the Elecrow **CrowPanel
Advance 10.1" ESP32-P4 HMI** board. Everything here is tied to the physical
hardware — the ESP32-P4 SoC, the ESP32-C6 WiFi co-processor over ESP-Hosted,
the EK79007 MIPI-DSI panel, the GT911 touch controller, the PDM MEMS mic,
the SD card wiring, the STC8H1K battery MCU, and the board's per-revision
pin drift (V1.0 vs V1.1 vs V1.2).

For generic ESP-IDF, LVGL, EEZ Studio, and app-level lessons that aren't
board-specific, see the companion [LessonsLearned.md](LessonsLearned.md).
Cross-references below use the section numbers from that source document.

Each entry is: **symptom → root cause → fix**.

---

## 1. ESP32-P4 has no on-die radio — WiFi comes via ESP-Hosted

(Source: LessonsLearned.md §5)

**Symptom.** `esp_wifi.h` fails to compile with
`CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM undeclared`,
`CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM undeclared`, etc.

**Root cause.** ESP32-P4 does not have integrated WiFi. Elecrow's
CrowPanel Advance carries an ESP32-C6 co-processor connected over SDIO;
the P4 talks to it via **ESP-Hosted** and drives the WiFi APIs through
**esp_wifi_remote**. Neither component is a default ESP-IDF dependency;
enabling them requires both idf_component.yml entries AND the matching
Kconfig flags.

**Fix.**

`main/idf_component.yml`:
```yaml
dependencies:
  espressif/esp_hosted:
    rules:
      - if: target in [esp32p4, esp32h2]
    version: ~2.7.0
  espressif/esp_wifi_remote:
    rules:
      - if: target in [esp32p4, esp32h2]
    version: ^0.16.1
```

`sdkconfig.defaults.esp32p4`:
```
CONFIG_SLAVE_IDF_TARGET_ESP32C6=y
CONFIG_ESP_HOSTED_ENABLED=y
CONFIG_ESP_HOSTED_P4_DEV_BOARD_NONE=y
CONFIG_ESP_HOSTED_SDIO_1_BIT_BUS=y
```

The `SDIO_1_BIT_BUS` value is board-specific for the CrowPanel Advance —
matches Elecrow's Lesson17-Wi-Fi_function example. See lesson 5 below for
the correct 4-bit V1.2 pin map.

---

## 2. MIPI-DSI DMA on ESP32-P4 needs specific L2 cache + MMU config

(Source: LessonsLearned.md §6)

**Symptom.** Screen boots blue and flickers rapidly (couple of Hz). No
error in the serial log; the LVGL task appears to be running.

**Root cause.** The EK79007 panel is driven over MIPI-DSI at 900 Mbps/lane
and the LVGL frame buffer is DMA'd to it from PSRAM. On ESP32-P4 that DMA
path is sensitive to the L2 cache line size and the MMU page size; the
Kconfig defaults land on values that misalign the transfers, producing
partial writes to the panel and rapid buffer swaps that read as blue
flicker.

**Fix.** In `sdkconfig.defaults.esp32p4`, mirror Elecrow's Lesson09 exactly:

```
CONFIG_CACHE_L2_CACHE_128KB=y
CONFIG_CACHE_L2_CACHE_LINE_64B=y
CONFIG_MMU_PAGE_SIZE_64KB=y
CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY=y
```

Also make sure the full PSRAM boot-init block is present (SPIRAM_MODE_HEX,
SPEED_200M, BOOT_INIT, PRE_CONFIGURE_MEMORY_PROTECTION, etc.). If any of
those are missing PSRAM either doesn't come up or comes up in a mode the
DMA controller can't use.

**How to know it's this and not something else.** If the panel is showing
ANY color at all (blue, black-with-flicker, streaks), the LDO + I2C + touch
+ panel + backlight init chain worked. The remaining failure mode after
that is DMA alignment. Serial log will typically be silent because the
LVGL task IS running — it's just producing corrupt frames.

---

## 3. Backlight ON only AFTER `ui_init()` populates the frame buffer

(Source: LessonsLearned.md §8)

**Symptom.** Brief flash of noise / random colors at boot before the UI
appears.

**Root cause.** `display_init()` (in `bsp_illuminate.c`) leaves the
backlight OFF at the end. If you call `set_lcd_blight(80)` before
`ui_init()`, the panel powers on displaying whatever was in the frame
buffer's PSRAM at reset — uninitialized garbage. Then `ui_init()` starts
populating the widget tree and the renderer overwrites it.

**Fix.** Order in `app_main`:

```c
display_init();                          // backlight left off
if (lvgl_port_lock(0)) {
    ui_init();                           // paints the initial screen
    lvgl_port_unlock();
}
set_lcd_blight(80);                      // backlight on AFTER paint
```

---

## 4. Flash size: default 2 MB is not enough — this board has 16 MB

(Source: LessonsLearned.md §10)

**Symptom.** Partition table generation fails:
```
Partitions tables occupies 5.1MB of flash (5308416 bytes) which does not
fit in configured flash size 2MB
```

**Root cause.** ESP-IDF's default `CONFIG_ESPTOOLPY_FLASHSIZE_2MB=y`. The
CrowPanel Advance carries 16 MB flash.

**Fix.** `sdkconfig.defaults`:

```
CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y
CONFIG_ESPTOOLPY_FLASHSIZE="16MB"
```

---

## 5. The CrowPanel BSP owns LDO, I2C, touch, LCD, and LVGL port init — in that order

(Source: LessonsLearned.md §13)

**Symptom (avoided).** Skipping any step in the init chain leaves the
panel dark, unresponsive, or crashing on first LVGL call.

**Root cause.** The CrowPanel Advance ESP32-P4 needs a specific bring-up
sequence, encoded in Elecrow's `bsp_*` components:

1. **`init_ldo()`** — `esp_ldo_acquire_channel` for LDO3 (2500 mV, panel
   analog) and LDO4 (3300 mV, panel digital). The MIPI-DSI panel won't
   respond without both rails up.
2. **`i2c_init()`** — I2C master bus for the touch controller.
3. **`touch_init()`** — GT911 touch chip probe over I2C.
4. **`display_init()`** — MIPI-DSI bus, EK79007 panel bring-up, then
   `lvgl_port_init()` + `lvgl_port_add_disp_dsi()` + `lvgl_port_add_touch()`.
   Leaves backlight OFF.
5. **`ui_init()`** — EEZ Studio's generated screen creation. Needs
   `lvgl_port_lock()`.
6. **`set_lcd_blight(80)`** — backlight ON.

The BSPs live in `components/` (copied verbatim from Elecrow's
Lesson09-LVGL_Lighting_Control example). Do NOT reorder these.

**Key detail.** `display_init()` internally calls `lvgl_port_init()`
which spawns the LVGL renderer task. That means from step 4 onward, every
LVGL call from `app_main` (or any other task) MUST take the lock.

**LDO channel ownership.** LDO3 = panel analog (2500 mV). LDO4 = panel
digital (3300 mV). Neither is available for SD card power (see lesson 12)
or any other subsystem — stealing either breaks the display.

---

## 6. ESP-Hosted resets the whole P4 if the C6 slave isn't responding

(Source: LessonsLearned.md §16)

**Symptom.** Board boots, screen briefly renders, then boots again ~10
seconds later. Repeat forever. Looks like a fast blue flicker to a naive
eye; on the serial log:
```
I (3820) H_SDIO_DRV: Open data path at slave
I (13590) transport: Not able to connect with ESP-Hosted slave device
I (15138) H_SDIO_DRV: Host is reseting itself, to avoid any sdio race condition
I (15138) os_wrapper_esp: Restarting host
ESP-ROM:esp32p4-eco2-20240710
...
```

**Root cause.** ESP32-P4 has no on-die radio; WiFi comes from an
ESP32-C6 co-processor over SDIO via ESP-Hosted. If the C6 doesn't
respond to the SDIO handshake (missing firmware, wrong board revision,
bus wiring), ESP-Hosted's default behavior is to `esp_restart()` the P4
and try again with the slave. This is intentional in ESP-Hosted — it
assumes the P4 is fresh out of a race condition — but it produces a boot
loop when the underlying C6 issue is persistent.

**Diagnostic fix — decouple boot from WiFi.** In `main.c`, do NOT call
`wifi_task_start()` from `app_main`. Defer WiFi initialization until the
user actually needs it (e.g., taps Rescan on PageWifiSetup). Then the
boot renders cleanly no matter what state the C6 is in, and the user can
see the reset loop only when they trigger the WiFi scan.

```c
// actions.c
void action_wifi_scan(lv_event_t *e) {
    (void)e;
    wifi_task_start();          // idempotent
    wifi_task_request_scan();
}
```

**Real fix.** The C6 slave needs its ESP-Hosted slave firmware flashed.
On Elecrow's CrowPanel Advance the C6 is accessible via a separate USB
port on the board or via the P4's USB-CDC bridge — check
`CrowPanel-*/example/V1.2/Upgrade P4 to C6 firmware/` in the vendor
materials for the exact flashing procedure.

---

## 7. ESP-Hosted SDIO pin config is board-revision-specific

(Source: LessonsLearned.md §18)

**Symptom.** With ESP-Hosted enabled at default pins, transport init fails:
```
I (3820) H_SDIO_DRV: Open data path at slave
I (13590) transport: Not able to connect with ESP-Hosted slave device
```
Serial log shows `sdio_wrapper: GPIOs: CLK[18] CMD[19] D0[14] D1[15] Slave_Reset[54]` — which is a mix of V1.0 pins with wrong reset. C6 is never actually reset because GPIO 54 isn't wired to its reset line.

**Root cause.** ESP-Hosted's Kconfig defaults are for a generic dev board, not the CrowPanel Advance. Elecrow's own tutorial
(`CrowPanel-Advanced-*/example/V1.0/Upgrade P4 to C6 firmware/Tutorial on Upgrading the C6 Firmware via P4.pdf`)
lists the correct pin config per board revision. **The 7"/9"/10.1" V1.1 and V1.2 boards use 4-bit SDIO with GPIOs 14/15/16/17 (D3..D0), CMD=19, CLK=18, and reset on GPIO 32** — completely different from ESP-Hosted's defaults.

**Fix.** In `sdkconfig.defaults.esp32p4`:

```
CONFIG_ESP_HOSTED_SDIO_HOST_INTERFACE=y
CONFIG_ESP_HOSTED_SDIO_SLOT_1=y
CONFIG_ESP_HOSTED_SDIO_4_BIT_BUS=y
CONFIG_ESP_HOSTED_SDIO_BUS_WIDTH=4
CONFIG_ESP_HOSTED_SDIO_CLOCK_FREQ_KHZ=40000
CONFIG_ESP_HOSTED_PRIV_SDIO_PIN_CMD_SLOT_1=19
CONFIG_ESP_HOSTED_PRIV_SDIO_PIN_CLK_SLOT_1=18
CONFIG_ESP_HOSTED_PRIV_SDIO_PIN_D0_SLOT_1=17
CONFIG_ESP_HOSTED_PRIV_SDIO_PIN_D1_4BIT_BUS_SLOT_1=16
CONFIG_ESP_HOSTED_PRIV_SDIO_PIN_D2_4BIT_BUS_SLOT_1=15
CONFIG_ESP_HOSTED_PRIV_SDIO_PIN_D3_4BIT_BUS_SLOT_1=14
CONFIG_ESP_HOSTED_SDIO_GPIO_RESET_SLAVE=32
```

**Reference table (per Elecrow tutorial):**

| Board | SDIO | CMD | CLK | D0 | D1 | D2 | D3 | RESET |
|---|---|---|---|---|---|---|---|---|
| 7/9/10.1" V1.0 | 1-bit | 19 | 18 | 14 | 15 | - | - | 32 |
| **7/9/10.1" V1.1+V1.2** | **4-bit** | **19** | **18** | **17** | **16** | **15** | **14** | **32** |
| 5" V1.0 | 4-bit | 54 | 53 | 52 | 51 | 50 | 49 | 20 |

Check the silkscreen for the revision number. Ours reads "V1.2".

---

## 8. ESP-Hosted host/slave version must match — pin to Elecrow's tested version

(Source: LessonsLearned.md §19)

**Symptom.** SDIO pins are correct, transport reaches "Open data path at slave" and "Card init success", but then never sees the ESP_PRIV_IF INIT event and eventually gives up:
```
I (8926) H_SDIO_DRV: Open data path at slave
I (13590) transport: Not able to connect with ESP-Hosted slave device
```

**Root cause.** ESP-Hosted host firmware (on the P4) and slave firmware (on the C6) speak a versioned RPC protocol. If the versions are on opposite sides of a breaking change, the SDIO transport comes up (card enumerates) but the higher-level RPC handshake never completes.

Default `~2.7.0` in `main/idf_component.yml` is too old for the C6 firmware Elecrow ships on the CrowPanel Advance (which is V2.12.3 per the vendor "Tutorial on Upgrading the C6 Firmware via P4" PDF).

**Fix.**

```yaml
espressif/esp_hosted:
  rules:
    - if: target in [esp32p4, esp32h2]
  version: "~2.12.3"
espressif/esp_wifi_remote:
  rules:
    - if: target in [esp32p4, esp32h2]
  version: "*"
```

Bumping to `~2.12.3` (or `*` to always pull latest) matches what Elecrow tested with the slave firmware they ship. `esp_wifi_remote` can float — it's chosen to be compatible with the resolved esp_hosted version automatically.

**When you flash a NEW C6 firmware** (via `host_performs_slave_ota`), keep the host version aligned with whatever the slave build was cut from — check `Advance_P4_On-Board_ESP32C6_Firmware_V<X.Y.Z>_<date>/` filename in the vendor materials.

---

## 9. sys_evt task stack overflows during ESP-Hosted scan event dispatch

(Source: LessonsLearned.md §20)

**Symptom.** ESP-Hosted transport comes up, `wifi station started` fires, then immediately:
```
I (9936) RPC_WRAP: ESP Event: wifi station started
Guru Meditation Error: Core 0 panic'ed (Stack protection fault).
Detected in task "sys_evt" at 0x4ff0c18a
Stack pointer: 0x4ff3cd70
Stack bounds: 0x4ff3cd80 - 0x4ff3d870
```
Stack pointer is BELOW the lower bound — classic overflow. ~2816 byte default stack for `sys_evt` isn't enough.

**Root cause.** ESP-Hosted's RPC handlers marshal WiFi events (scan results, connection state) on the ESP event loop task (`sys_evt`). `wifi_ap_record_t` is ~200 bytes; a batch of them plus event-fanout closure state overruns the default stack.

**Fix.** Two Kconfig changes in `sdkconfig.defaults`:

```
CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE=6144
```
Bumps the sys_evt task stack from 2816 to 6144.

AND — in `sdkconfig.defaults.esp32p4`:
```
CONFIG_ESP_HOSTED_DFLT_TASK_FROM_SPIRAM=y
```
Moves ESP-Hosted's own worker task stacks to PSRAM. Internal RAM is tight when WiFi + LVGL + mbedtls all want internal buffers; SPIRAM has 32 MB of headroom.

**Complement (from Fireside):** Reserve internal RAM so DMA-critical allocations don't get pushed to PSRAM:
```
CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL=131072
CONFIG_MBEDTLS_INTERNAL_MEM_ALLOC=n
CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC=y
CONFIG_MBEDTLS_DYNAMIC_BUFFER=y
```
128 KB internal reserve is the sweet spot from the [Fireside project](../../Product/TrailCurrentFireside) — proven ESP32-P4 + ESP-Hosted config.

---

## 10. `esp_wifi_get_config()` over `esp_wifi_remote` returns empty SSID

(Source: LessonsLearned.md §25)

**Symptom.** User connects to WiFi, GOT_IP fires, we save credentials by reading them back via `esp_wifi_get_config(WIFI_IF_STA, &cur)` and writing `cur.sta.ssid` + `cur.sta.password` to NVS. Next boot, `nvs_get_str("wifi_ssid")` returns success but the value is empty, so auto-connect never runs.

**Root cause.** With `esp_wifi_remote` (WiFi on the C6 slave via ESP-Hosted), the wifi_config lives on the slave, not locally. `esp_wifi_get_config` proxies the call over SDIO — some slave versions return an empty `sta.ssid` field even though the actual connection is up and working. It's a data-plane / control-plane mismatch that gets fixed intermittently in newer esp_wifi_remote releases.

**Fix.** Don't rely on the round-trip. Save the credentials at the exact moment you receive them from the user (in `wifi_task_connect`), not later on GOT_IP:

```c
void wifi_task_connect(const char *ssid, const char *psk) {
    // ... esp_wifi_set_config + esp_wifi_connect ...
    app_save_wifi_creds(ssid, psk ? psk : "");   // trust the caller's values
}
```

The caller (the LVGL keyboard-submit action) has the actual strings the user typed. Save those. The GOT_IP handler can stay as a sanity confirmation but shouldn't be the persistence path.

---

## 11. WiFi-before-touch silently kills LVGL touch dispatch

(Source: LessonsLearned.md §26)

**Symptom.** The board boots. The screen lights up. The UI is visible. WiFi
connects. MQTT connects. But touching any button — anywhere on any page —
does nothing. No log line, no crash, no error. Just dead touch. The
`ACT:` log lines that would fire on button tap never appear.

**How we know it's not the touch driver.** We put a background task in
`app_main` that polls the GT911 touch chip directly every 100 ms, bypassing
LVGL entirely. Every finger tap on the screen produced a valid coordinate
in the log (x/y correctly ranged 0–1024 × 0–600). So the touch chip, the
I2C bus, and the GT911 driver were all healthy. The break was between the
GT911 driver and LVGL's own touch-polling timer — LVGL never asked the
touch driver for coordinates.

**Root cause.** The bring-up order in `app_main` had WiFi first:
`esp_hosted_init()` → `esp_wifi_start()` → wait 2 seconds → then LDO / I2C /
touch / display. That specific ordering leaves LVGL's touch input-device
(indev) polling timer inert. LVGL's display refresh runs (the screen paints
correctly), and LVGL's other timers run (the clock updates), but the timer
that calls `lvgl_port_touchpad_read()` never fires. Nothing crashes because
nothing tried to do anything invalid; it's just that a background timer
that _should_ have been created and scheduled during `lvgl_port_add_touch`
ends up in a state where it never ticks. Our best guess is that
`esp_hosted_init` + `esp_wifi_start` reshape either the GPIO ISR table or
the FreeRTOS timer service in a way that later `lvgl_port_add_touch` calls
tolerate but that leaves the read-timer stranded. We stopped digging once
we found the fix — the ordering _is_ documented by the vendor as the fix.

**Fix.** Reorder `app_main` to match Elecrow's vendor Lesson16
(`example/V1.2/idf-code/Lesson16_Get_weather_via_WiFi/main/main.c`) — the
canonical known-good sequence for display + WiFi on this board:

```
1. NVS
2. LDO3 (2500 mV) + LDO4 (3300 mV)
3. i2c_init()          — touch bus
4. touch_init()        — GT911
5. display_init()      — MIPI-DSI + starts LVGL task + adds touch indev
6. set_lcd_blight(80)
7. ui_init()           — build widgets, under lvgl_port_lock()
8. esp_netif_init + esp_event_loop_create_default
9. esp_hosted_init() + esp_wifi_init + esp_wifi_start
10. app_state / MQTT / anything WiFi-driven
```

The rule in plain terms: **display and touch come up FIRST, WiFi comes up
LAST.** The moment you invert that order on this board, touch dispatch is
gone.

**How to spot this fault fast next time.** If the screen renders but no
widget reacts to taps, don't chase the touch driver — it's fine. Grep for
`ACT:` (or whatever prefix your action handlers log). If nothing shows up
even after twenty taps, the failure is LVGL's indev timer, not the touch
chip. Check that `touch_init` + `display_init` happen before
`esp_hosted_init` / `esp_wifi_start` in `app_main`.

---

## 12. GT911 INT pin makes touch feel like you have to hold for a full second

(Source: LessonsLearned.md §28)

**Symptom.** The board boots, WiFi comes up, MQTT connects, the UI renders,
buttons _do_ respond — but only if you press hard and hold your finger down
for roughly a second. Quick taps are silently ignored. Swipes register
inconsistently. It's not dead touch (lesson 11) — it's laggy, deliberate,
"industrial equipment from 2003" touch.

**Root cause.** `esp_lvgl_port`'s `lvgl_port_add_touch` inspects the touch
handle's `int_gpio_num`. If it's a real GPIO (not `GPIO_NUM_NC`), the port
puts the LVGL indev in `LV_INDEV_MODE_EVENT`:

```c
// managed_components/espressif__esp_lvgl_port/src/lvgl9/esp_lvgl_port_touch.c
if (touch_ctx->handle->config.int_gpio_num != GPIO_NUM_NC) {
    lv_indev_set_mode(indev, LV_INDEV_MODE_EVENT);
}
```

Event mode means LVGL only reads the GT911 when the INT GPIO edge fires
and wakes the LVGL task via `lvgl_port_task_wake`. That's ideal on boards
where the INT line is clean and edge-triggered per touch event. On the
CrowPanel Advance 10.1" the INT line is _not_ clean — it appears to only
fire reliably after sustained pressure, and quick taps produce either no
edge at all or an edge too brief for the GT911 to have registered valid
coordinates by the time LVGL reads them. Net effect: taps are silently
dropped unless held.

Because the GT911 driver still populates its internal registers correctly,
a direct-polling diagnostic task (like the one used to isolate lesson 11)
_would_ see every tap. It's specifically the INT-triggered read path in
`esp_lvgl_port` that's broken on this board.

**Fix.** In `components/bsp_display/bsp_display.c`, set the touch config's
`int_gpio_num` to `GPIO_NUM_NC` even though the schematic wires INT to
GPIO 42:

```c
esp_lcd_touch_config_t tp_cfg = {
    .x_max = H_size,
    .y_max = V_size,
    .rst_gpio_num = Touch_GPIO_RST,
    .int_gpio_num = GPIO_NUM_NC,      // was Touch_GPIO_INT (42)
    ...
};
```

That forces `esp_lvgl_port` to leave the indev in the default timer-polled
mode, so LVGL reads the GT911 over I2C on every LVGL timer tick — 5 ms in
this project (`bsp_illuminate.c`'s `lvgl_cfg.timer_period_ms`). Taps become
instant.

**Why leaving the INT pin unconnected is safe.** The GT911 driver uses the
INT pin during init to select between the primary (0x5D) and backup (0x14)
I2C addresses. `touch_init()` already handles that with a fallback: try
primary → on failure retry with backup. Skipping the INT-driven address
selection just means we take the fallback path if the chip happens to have
come up on the backup address, which costs one extra I2C probe at boot.

**How to spot this fault fast next time.** Touch responds but only to slow,
deliberate presses; quick taps are ignored. That's not the touch driver,
not the I2C bus, not LVGL locking — that's `int_gpio_num` set in
`esp_lcd_touch_config_t`. This is different from lesson 11 (dead touch,
_no_ response). Lesson 11 = ordering; this one = event vs polling mode.

**Every Elecrow reference example does this.** Their Lesson09 sets
`int_gpio_num = GPIO_NUM_NC` — the version of `bsp_display.c` we started
from had it wired to GPIO 42, presumably as an "in case someone wants to
use it" placeholder. Don't. On this board specifically, keep it NC.

**Follow-up trap: polling mode is still coarse at LVGL's 30 ms default.**
Fixing `int_gpio_num` gets touch to _respond_, but the tap still feels like
it needs more finger pressure than a typical capacitive screen. That's
because `CONFIG_LV_INDEV_DEF_READ_PERIOD` defaults to 30 ms, and the LVGL
indev only reads the GT911 on that timer. A light, quick tap can start
_and end_ inside a single 30 ms window — LVGL never sees the press-then-
release transition, so no click fires. The user compensates by pressing
harder / holding longer, which flattens the finger against the glass and
gives GT911 more contact time for the 30 ms poll to catch. The fix is
another one-line Kconfig change in `sdkconfig.defaults`:

```
CONFIG_LV_INDEV_DEF_READ_PERIOD=10
```

Three I2C reads per 30 fps frame instead of one. GT911's I2C traffic is
trivially small (a few bytes per read at 400 kHz), and no perceptible CPU
cost even under LVGL rendering load. After both changes (INT=NC _and_
10 ms polling) capacitive touch feels the way you'd expect — no learned
"press harder" behavior needed.

**When you still need more sensitivity than polling can give you.** The
GT911 itself has a configurable touch threshold at register 0x8053
(`Screen_Touch_Level`) inside its config table (0x8047–0x80FE). Lower =
more sensitive. Factory default from Elecrow is around 0x50; dropping to
0x30 makes it noticeably more finger-friendly. **Doing this safely requires
reading the entire config table, patching the byte, recomputing the
checksum at 0x80FF, and writing back with the fresh flag at 0x8100** — get
any part wrong and the chip either rejects the write (silent) or bricks
its config until reset (recoverable, but scary). Only reach for this after
INT=NC + 10 ms polling proves insufficient; the polling change alone has
solved every "feels too heavy" report we've had on this board so far.

---

## 13. Elecrow V1.0 factory sourcecode is not safe to copy on a V1.2 board

(Source: LessonsLearned.md §29)

**Symptom.** You port an "Elecrow factory example" pattern into the
project — SD card init, LDO acquire, PDM mic, codec register writes —
and one of two things happens:
- Something else on the board goes sluggish/broken (touch, display).
- The peripheral you're wiring up is silently non-functional (returns
  ESP_OK but no data ever arrives, or clocks glitch).

The bug is almost never in your code. The bug is that you read the wrong
folder in `CrowPanel-Advanced-.../factory_sourcecode/V1.0/...`. Elecrow
respins the board revisions (`V1.0`, `V1.1`, `V1.2`) with different pin
maps, LDO channel assignments, and codec routing. The V1.0 factory BSP
in particular uses:

- SDMMC slot-0 IO-MUX defaults (no explicit pins) + LDO channel 4 power
  control — **but V1.2 wires SD to explicit GPIOs 43/44/39 with no LDO
  fiddling** (lesson 14). Copying V1.0's `sd_pwr_ctrl_new_on_chip_ldo(4)`
  onto a V1.2 board steals LDO4 from the MIPI-DSI panel digital rail
  (lesson 5) and every touch/button on the UI goes molasses-slow.
- PDM microphone on the same I2S peripheral as the speaker TX, with
  default clock-config macros — **but V1.2 puts PDM RX on I2S_NUM_0 with
  specific `dn_sample_mode`/`bclk_div`/`hp_en` fields** (lesson 15).
  The default macros return silent audio; every capture yields ±0.

**Rule.** Match example-folder revision to the physical board silkscreen
revision. On this project the target is **V1.2**, so the canonical
reference is `CrowPanel-Advanced-.../example/V1.2/idf-code/Lesson<N>-*/`,
NOT `factory_sourcecode/V1.0/`. The `V1.0` folder should be treated as
a completely different product.

If the V1.2 folder doesn't have an example for what you need, look at
the V1.1 folder next (revisions 1.1 and 1.2 share most pin assignments —
see lesson 7 for SDIO specifically). Only fall back to V1.0 as a last
resort, and diff the two revisions' `board_config.h` line-by-line
before trusting any pin.

**Cross-references.**
- Lesson 5 explains what LDO3 (2500 mV, panel analog) and LDO4
  (3300 mV, panel digital) power on this board — stealing either
  breaks the display.
- Lesson 7 documents the same rev-specific pin drift for ESP-Hosted
  SDIO (7"/9"/10.1" V1.1+V1.2 use GPIOs 14/15/16/17/18/19/32; V1.0 is
  different).
- Lessons 14 and 15 document the specific V1.2 SD and mic fixes that
  fell out of applying this rule.

---

## 14. SD card init on V1.2 — explicit pins, no LDO acquisition

(Source: LessonsLearned.md §30)

**Symptom.** After wiring up an SD card reader following the ESP32-P4
factory ev-board BSP pattern (`bsp_sdcard_mount()` in
`factory_sourcecode/V1.0/.../espressif__esp32_p4_function_ev_board/`),
the SD may or may not mount, but the on-screen buttons become
noticeably sluggish. The MQTT link starts dropping PINGRESPs
(`E (…) mqtt_client: No PING_RESP, disconnected`), WiFi RSSI polls
timeout (`W (…) rpc_core: Timeout waiting for Resp for
[0x126](Req_WifiStaGetApInfo)`), and `getaddrinfo` starts returning
EAI_AGAIN. In short: everything on the board simultaneously degrades
after SD init runs.

**Root cause.** The factory BSP does:
```c
sd_pwr_ctrl_ldo_config_t ldo_config = { .ldo_chan_id = 4 };
sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;
sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
host.pwr_ctrl_handle = pwr_ctrl_handle;
```

Acquires ESP32-P4 on-chip LDO channel 4 for SD card power. But on the
CrowPanel Advance V1.2, LDO4 is the MIPI-DSI panel digital rail (see
lesson 5). Two drivers now fight for control of the same LDO. The
resulting brownout/rail-modulation degrades every downstream peripheral
that depends on stable panel timing, including LVGL's touch polling
loop and the display refresh — the panel visibly still works, but the
whole board runs like it's underwater.

**Fix.** Mirror `example/V1.2/idf-code/Lesson08-SD_Card_File_Reading/`
which explicitly does NOT touch LDO at all — SD power on V1.2 is
board-wired. Use explicit slot pins (Slot 0 is not on IO MUX for this
variant) and enable internal pull-ups:

```c
sdmmc_host_t host = SDMMC_HOST_DEFAULT();
host.slot = SDMMC_HOST_SLOT_0;
host.max_freq_khz = 10000;   // 10 MHz — HIGHSPEED is unstable here
/* NO host.pwr_ctrl_handle = ... — leave it NULL */

sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
slot_config.clk   = 43;
slot_config.cmd   = 44;
slot_config.d0    = 39;
slot_config.width = 1;
slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
slot_config.cd    = SDMMC_SLOT_NO_CD;
slot_config.wp    = SDMMC_SLOT_NO_WP;
```

Reference: `main/sd_config.c`.

**Trap.** If you mount the SD successfully but subsequent `fopen`s
return NULL despite `readdir` listing the file, enable FATFS long-file-name
support (`CONFIG_FATFS_LFN_HEAP=y` + `CONFIG_FATFS_MAX_LFN=255` +
`CONFIG_FATFS_API_ENCODING_UTF_8=y`) or files with base name >8 chars
show up as their DOS 8.3 short name.

---

## 15. PDM mic on V1.2 — MUST bind I2S_NUM_0, needs full clk_cfg + slot_cfg

(Source: LessonsLearned.md §31)

**Symptom.** PTT button press fires the capture path, `audio_capture_start()`
returns `false`, and the user sees a "Mic unavailable." toast. The
`i2s_new_channel(&chan_cfg, NULL, &s_rx_chan)` call silently succeeds
but `i2s_channel_init_pdm_rx_mode()` doesn't produce a working channel —
or it does, but every read returns silence (±0).

**Root cause.** Two independent bugs, both easy to hit if you take
patterns from `factory_sourcecode/V1.0/` or from the ESP-IDF PDM RX
default macros:

1. **PDM RX must be on `I2S_NUM_0` on ESP32-P4.** The P4's PDM peripheral
   only binds to port 0. If your speaker TX is already on `I2S_NUM_0`,
   the PDM RX `i2s_new_channel` accepts your `I2S_NUM_1` request but
   the returned channel doesn't clock anything. Symptom: silent failure
   downstream at capture-read time.
2. **The default PDM macros produce wrong config for this MEMS mic.**
   `I2S_PDM_RX_CLK_DEFAULT_CONFIG(rate)` and `I2S_PDM_RX_SLOT_DEFAULT_CONFIG(...)`
   don't set `dn_sample_mode`, `bclk_div`, `slot_mask`, or the high-pass
   filter. The mic on V1.2 needs specific values or capture returns silence.

**Fix.** Put speaker TX on `I2S_NUM_1`, PDM RX on `I2S_NUM_0`, and copy
the config block from
`example/V1.2/idf-code/Lesson11-Playback_After_Recording/peripheral/bsp_mic/bsp_mic.c`
verbatim:

```c
/* PDM RX on I2S_NUM_0 — speaker TX elsewhere (NUM_1) */
i2s_chan_config_t rx_chan_cfg = {
    .id = I2S_NUM_0,
    .role = I2S_ROLE_MASTER,
    .dma_desc_num = 6,
    .dma_frame_num = 256,
    .auto_clear = true,
    .intr_priority = 0,
};

i2s_pdm_rx_config_t pdm_cfg = {
    .clk_cfg = {
        .sample_rate_hz = 16000,
        .clk_src        = I2S_CLK_SRC_DEFAULT,
        .mclk_multiple  = I2S_MCLK_MULTIPLE_256,
        .dn_sample_mode = I2S_PDM_DSR_8S,    /* required */
        .bclk_div       = 8,                  /* required */
    },
    .slot_cfg = {
        .data_bit_width     = I2S_DATA_BIT_WIDTH_16BIT,
        .slot_bit_width     = I2S_SLOT_BIT_WIDTH_AUTO,
        .slot_mode          = I2S_SLOT_MODE_MONO,
        .slot_mask          = I2S_PDM_SLOT_LEFT,   /* required */
        .hp_en              = true,                /* required */
        .hp_cut_off_freq_hz = 35.5f,
        .amplify_num        = 1,
    },
    .gpio_cfg = { .clk = 24, .din = 26, .invert_flags = { .clk_inv = false } },
};
```

Note that GPIOs 24 (CLK) and 26 (DATA) are shared with the V1.0 board
even though many other pins differ — the PDM mic wiring itself didn't
change between revisions.

Reference: `main/audio.c` (`init_pdm_rx_channel`).

**Trap.** If the alarm playback path also lives on I2S_NUM_0 (as some
inherited code does), TX will keep working after you move PDM RX to
NUM_0 because the alarm task holds the channel serially, but you'll
have to swap the TX code to `I2S_NUM_1` before enabling the mic. If
you don't, one or the other silently breaks.

---

## 16. Adding modest BSS shifts main task stack into TCM → nvs_flash_init aborts

(Source: LessonsLearned.md §32 / §49)

**Symptom.** Fresh build, flash succeeds, board boots into the bootloader,
loads the app, prints:

```
I (1811) main_task: Started on CPU0
I (1811) esp_psram: Reserving pool of 128K of internal memory for DMA/internal allocations
I (1821) main_task: Calling app_main()
I (1821) MAIN: CrowPanel P4 boot (UI present, WiFi LAST)

assert failed: spi_flash_disable_interrupts_caches_and_other_cpu cache_utils.c:127 (esp_task_stack_is_sane_cache_disabled())
Core  0 register dump:
MEPC    : 0x4ff09688  RA      : 0x4ff0963a  SP      : 0x30100d30  GP      : 0x4ff15c80
...
```

Then reboots and loops. Display stays black. The build that produced
this crash differs from the previous working build only by a few KB of
extra BSS (in our case a 4 KB static scratch buffer for streaming audio
volume scaling).

**Root cause.** ESP-IDF's `esp_task_stack_is_sane_cache_disabled()`
asserts that the current task's stack lives in a memory region that
remains accessible when the flash cache is disabled. This check fires
from every NVS / SPI-flash operation. It considers the 384 KB internal
SRAM pool (`4FF40000..4FFA0000`) safe. It does **not** consider TCM
(`30100068..30101FFF`, only 7 KB on the P4) safe.

Under normal conditions ESP-IDF places the 3584-byte main task stack in
internal SRAM. If you push BSS layout — the internal SRAM pool can
suddenly have less contiguous free space at the moment the main task is
created, and IDF falls back to allocating the stack in TCM (the only
other "internal" region that fits). The stack pointer then points at
TCM addresses (`0x30100D30`, etc.), and the very first NVS call
(`nvs_flash_init()` in app_main) trips the assert.

The crash log stack dump's `SP` value is the give-away.

**Fix.** In `sdkconfig.defaults`:

```
CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192
```

Doubling the requested main-task stack size forces IDF to allocate from
the 384 KB internal SRAM pool (TCM is only 7 KB and can't hold an 8 KB
stack). It also gives more headroom for the deep `nvs_flash_init` call
chain that runs early.

Regenerate `sdkconfig` from defaults after the change:

```bash
rm sdkconfig && idf.py reconfigure
grep CONFIG_ESP_MAIN_TASK_STACK_SIZE sdkconfig    # verify =8192
```

**Trap.** `CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY=y` in this project's
sdkconfig lets IDF place stacks in PSRAM as a further fallback. If TCM
is also full, main task's stack lands in PSRAM instead, and the same
`esp_task_stack_is_sane_cache_disabled()` assert fires (PSRAM disappears
when cache is disabled). Bumping the stack size fixes both cases at once.

**How to spot this fast.** In the panic register dump:
- `SP` starts with `0x4FF` → stack in internal SRAM, this lesson doesn't apply.
- `SP` starts with `0x301` → stack in TCM → this lesson.
- `SP` starts with `0x48…` (or wherever PSRAM lives) → stack in PSRAM → this lesson.

---

## 17. Audio DMA source buffer must be in guaranteed-DMA-safe internal memory

(Source: LessonsLearned.md §43)

**The problem.** The streaming playback path allocated its scratch
buffer as a `static int16_t s_stream_scratch[2048]` in BSS. On most
builds this landed in internal SRAM and worked fine. But as the project
grew, one build placed it in PSRAM instead. Result: intermittent loud
static during Peregrine replies — the audio was decoded, streamed,
and volume-scaled correctly, but the DMA hardware couldn't read the
buffer reliably from PSRAM through the cache.

**The cause.** ESP32-P4 audio DMA needs its source memory to be
either DMA-capable internal SRAM, or the driver needs special
cache-coherency handling for PSRAM. Static BSS variables land wherever
the linker puts them — which can shift as unrelated code changes.
Silent bug.

**The fix.** Allocate the scratch buffer explicitly with
`heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT)`
at boot in `audio_init()`. Log the address (should start with `0x4Fxx`
for internal SRAM on P4, not `0x48xx` or `0x30xx`) so you can visually
confirm it's in the right region.

**Rule of thumb.** For anything that hands memory to a hardware
peripheral (I2S, SPI, camera, etc.) use `heap_caps_malloc` with the
matching CAP flag, not static BSS. Don't trust the linker to put your
buffer somewhere DMA-safe.

---

## 18. PDM microphone `amplify_num > 1` slows the peripheral instead of boosting signal

(Source: LessonsLearned.md §46)

**The problem.** The CrowPanel's PDM MEMS mic produced a very quiet
signal — Whisper transcribed every capture as dot-strings. The V1.2
vendor lesson uses `slot_cfg.amplify_num = 1` (no amplification). We
tried `amplify_num = 8` to boost the signal. Result: mic signal did
get louder, but capture rate dropped to ~32% of real-time. A user
holding the button for 5 seconds only got 1.6 seconds of captured
audio, badly truncating every sentence.

**The cause.** The hardware amplifier stage in the PDM peripheral
appears to add per-sample processing overhead. At `amplify_num > 1`
the peripheral can't keep up with the requested sample rate and
silently drops samples.

**The fix.** Keep `amplify_num = 1` on the hardware. Amplify in software
after the samples come out of `i2s_channel_read`:

```c
#define MIC_SW_GAIN 8

int16_t *s = (int16_t *)chunk;
for (size_t i = 0; i < samples; i++) {
    int32_t v = (int32_t)s[i] * MIC_SW_GAIN;
    if (v >  32767) v =  32767;
    if (v < -32768) v = -32768;
    s[i] = (int16_t)v;
}
```

Fast, doesn't slow the peripheral, and you can tune the gain freely
without hardware constraints.

**Rule of thumb.** Hardware audio-processing knobs are often free-
looking but expensive-in-practice. Try software post-processing first.

---

## 19. Whisper's small-model + this board's cheap MEMS mic is the accuracy floor of a voice terminal

(Source: LessonsLearned.md §50)

**The finding.** After tuning every knob we could — mic gain, Whisper
model (base.en → small.en), initial_prompt hint with likely words,
release-tail padding — the CrowPanel's PDM MEMS microphone still
produced transcripts that garbled proper nouns:

- Said: "What is the capital of Illinois?"
- Whisper heard: `"What is that?"` / `"What a count man."` /
  `"Whistle."` / `"is all in my"` / (various)

**Why it happens.** Two compounding limits:
1. The MEMS mic on this board has poor SNR — it's designed for
   wake-word detection at close range, not general dictation.
2. Whisper base.en (74 MB) is fast but misses proper nouns even on
   good audio. Small.en (466 MB) is much better but the mic is still
   the bottleneck; the model can only work with what it hears.

**What works around it.** Chat mode (text input) — same LLM, same
prompt, no Whisper in the loop — gives perfect answers. Voice-assistant
mode with an external USB microphone works far better than the CrowPanel's
onboard mic (external analog USB mic > MEMS).

**What doesn't work.** Adding an `initial_prompt` with likely words
biases Whisper toward specific vocabulary but doesn't fix "Illinois"
recognition. Boosting mic gain past a certain point creates clipping
that makes recognition worse, not better. Cranking Whisper model size
larger keeps helping but hits a wall around small.en on this mic.

**Practical implications.** This board can't do open-domain voice
dictation well through the onboard mic. Realistic uses of the voice
terminal:
- Push-to-talk on a limited command vocabulary where an intent-matcher's
  regex layer handles it before Whisper even sees the audio
- External I²S codec + high-quality mic on the CrowPanel's exposed
  headers as a future hardware revision
- Wake-word + short-utterance mode using a purpose-tuned model

**Not-a-bug.** The pipeline architecture is sound. The mic hardware is
the limit.

---

## 20. Battery gauge is an STC8H1K + TP4059 combo — voltage lookup, not real fuel gauging

(Source: LessonsLearned.md §51)

**What the STC8H1KXX actually is.** The STC8H1K series (STCmicro) is a
generic 8-bit 8051-derivative microcontroller with a built-in 12-bit
ADC. Elecrow bought a cheap general-purpose MCU (~$0.30 in volume, DFN-8
package) and wrote custom firmware on it to:

- Drive the TP4059 (a basic linear CC/CV Li-ion charger IC)
- Read battery voltage on one of its ADC channels
- Publish a byte block on I²C for the host to consume

That's the entire "fuel gauge." No dedicated SOC algorithm hardware.
No coulomb counter. No temperature compensation. No learning. Whatever's
in that `bat_level_pct` field is just Elecrow's firmware doing a naive
lookup on the terminal-voltage ADC reading — likely a straight linear
map from 3.3 V → 0 % to 4.2 V → 100 %. That's why plug/unplug snaps 40
points.

**What a real fuel gauge IC does.** For reference, purpose-built fuel
gauges like the MAX17048 ($1.50), BQ27441 ($1.75), STC3115 ($1.20), or
LC709203F ($0.90) do dramatically more inside one chip:

| Feature | STC8+TP4059 | MAX17048 | BQ27441 |
|---|---|---|---|
| SOC algorithm | Voltage lookup | ModelGauge m5 (adaptive) | Impedance Track (learning) |
| Coulomb counter | No | No (OCV+drift model) | Yes |
| Load compensation | No | Yes | Yes |
| Temp compensation | No | Yes | Yes |
| Aging/health tracking | No | Partial | Yes |
| Battery model learning | No | Yes | Yes |
| Steady-state accuracy | ±10-15 % | ±3 % | ±1 % |
| Snap on plug/unplug | 40+ pts | ~0 pts | ~0 pts |

The MAX17048 in particular is what most consumer devices use for
single-cell Li-ion — it delivers 3% accuracy without a current shunt.
The BQ27441 delivers 1% accuracy but needs a shunt.

**Why Elecrow chose this.** Straightforward cost engineering: an
STC8H1K08 + TP4059 combo costs Elecrow ~$0.60 in parts vs. ~$2 for a
proper charger + fuel gauge combo. On a board that already carries an
ESP32-P4, 32 MB PSRAM, a 10" DSI panel, and a WiFi coprocessor, the
battery subsystem is the one place they could shave a dollar. And most
customers plug the panel into wall power full-time — the battery is an
optional-use nicety, not a must-be-accurate feature.

**What you can do about it if you care.** Three paths, escalating:

1. Ship what you have, know that it reads ~5-10% low under load, and
   call it good. Fine for a display where "roughly how much battery is
   left" is enough.

2. Add a MAX17048 (~$0.90-1.50) to the I²C bus in a board rev. Same
   I²C bus the STC8 already sits on. Different address (0x36). Firmware
   picks it up automatically — STC8 continues to run the charger,
   MAX17048 does the SoC reporting. You'd get 3% accuracy with no
   changes to how you use the pack. This is the recommended v2 respin.

3. Full BMS chip like the BQ25185 (charger + gauge combined) — single
   chip that replaces both the TP4059 and the STC8H1K, exposing a
   proper I²C register set from TI. More work (schematic change,
   different footprint) but eliminates the vendor firmware entirely and
   gets to <1% SOC accuracy.

Given the board is already fielded, path 1 is the pragmatic answer. If
you're planning a hardware revision anyway, path 2 is a ~$1 BOM add for
a very visible quality improvement.

---

## 21. ESP-Hosted RPC hang: SDIO stays alive, C6 slave goes silent, nothing recovers

**Symptom.** Device runs cleanly for hours or days. Then MQTT drops, the
topbar shows `0 dBm`, buttons feel sluggish (~100 ms lag per tap),
alarm/status data on the home screen freezes on its last known values, and
a "Connection Lost" toast appears. The screen never advances to the WiFi
setup wizard, so the app looks alive but nothing responds to network
events. Serial log, every ~10 s indefinitely:

```
W (548725) rpc_core: Timeout waiting for Resp for [0x126](Req_WifiStaGetApInfo)
E (548725) rpc_core: Response not received for [0x126](Req_WifiStaGetApInfo)
E (555862) esp-tls: couldn't get hostname for :headwaters.local: getaddrinfo() returns 202, addrinfo=0x0
E (555862) esp-tls: Failed to open new connection
W (555880) MQTT: Disconnected. err_type=1 transport_sock_errno=119 tls_last_esp_err=0x8001 ...
I (555890) APP_STATE: MQTT state: disconnected
```

The tell is the combination:
- `rpc_core: Timeout waiting for Resp` — RPCs go out over SDIO but no
  response ever comes back.
- `getaddrinfo() returns 202` (EAI_FAIL) — the network stack can't even
  resolve hostnames.
- **No `WIFI_EVENT_STA_DISCONNECTED`.** If the C6 had cleanly dropped the
  AP, the P4 would receive that event, `wifi_setup.c` would transition to
  `WIFI_SETUP_STATE_FAILED`, and `app_state.c` would bounce to the WiFi
  wizard. The wizard never appears — meaning the C6 didn't cleanly
  disconnect, it just stopped answering.
- **No `H_SDIO_DRV: Host is reseting itself`.** Lesson 6 describes a
  transport-level failure where the SDIO handshake breaks and ESP-Hosted
  auto-restarts the P4 (via `CONFIG_ESP_HOSTED_TRANSPORT_RESTART_ON_FAILURE=y`).
  That is NOT this failure mode. SDIO writes are still going through — the
  RPC request reaches the C6. The C6's application task that would answer
  it has hung.

**Root cause.** ESP-Hosted's transport-restart safety net only fires on
SDIO-layer failures (`sdio_is_write_buffer_available` timing out for
`max_retry_sdio_not_responding` iterations). There is a whole class of
failures above that layer where the SDIO peripheral is fine, packets are
flowing, but the C6 firmware's RPC handler task is stuck (crashed
assertion that didn't reset the chip, deadlock, watchdog-triggered task
kill, buffer exhaustion in a specific event path — under sustained
high-rate MQTT traffic combined with WiFi RSSI polling every 5 s, one of
these will eventually be tickled).

When this happens:
- `rpc_core` logs the timeout for every request and returns NULL to the
  caller. No retry, no reset, no event posted.
- `esp_wifi_sta_get_ap_info()` returns FAIL. Our RSSI cache goes to 0.
  UI paints "0 dBm" (which the user reads as "not associated").
- `mqtt_client` sees the socket die and tries to reconnect. `getaddrinfo`
  can't reach a DNS server (WiFi packets aren't moving even though the P4
  thinks the association is up). Reconnect fails every ~17 s.
- Every failed reconnect fires `MQTT_EVENT_DISCONNECTED`, which grabs the
  LVGL lock to call `set_var_mqtt_connected(false)` — which in turn calls
  `paint_notif_badge()` (walks 13 badges, refreshes the toaster, evaluates
  the alarm bitmaps). That's the ~100 ms button-slowness the user feels.
- The C6 doesn't reboot on its own, so no `ESP_HOSTED_EVENT_CP_INIT`
  event ever fires to signal recovery is needed.

There is no path out of this state in the default ESP-Hosted config. The
device is stuck until a hardware power cycle.

**Fix — enable the C6 heartbeat + monitor it + auto-restart on timeout.**

Espressif ships a reference example at
`managed_components/espressif__esp_hosted/examples/host_hosted_events/`
that documents the recovery pattern. Three ESP-Hosted events matter:

| Event | Meaning | Action |
| --- | --- | --- |
| `ESP_HOSTED_EVENT_CP_HEARTBEAT` | C6 alive at time T | Reset a one-shot timeout timer |
| `ESP_HOSTED_EVENT_TRANSPORT_FAILURE` | SDIO layer detected slave unresponsive (already logged as `E: SDIO slave unresponsive`) | Trigger recovery immediately |
| `ESP_HOSTED_EVENT_CP_INIT` (unexpected — after the first one at boot) | C6 rebooted spontaneously | Trigger recovery immediately |

Heartbeat interval is set via `esp_hosted_configure_heartbeat(true, N_SEC)`
— the C6 sends `RPC_ID__Event_Heartbeat` every N_SEC, which the wrap
layer translates to `ESP_HOSTED_EVENT_CP_HEARTBEAT`. Pick N such that
missing 3–4 heartbeats fits comfortably inside the timeout window
(10 s heartbeat + 45 s timeout = tolerates 4 missed heartbeats before
recovery).

Recovery has two options:
1. **In-place**: `example_wifi_sta_netif_close()` → `example_wifi_deinit_sta()`
   → `esp_hosted_deinit()` → `esp_hosted_init()` (which drives the C6
   reset GPIO because `CONFIG_ESP_HOSTED_SLAVE_RESET_ON_EVERY_HOST_BOOTUP=y`)
   → `esp_wifi_init()` → `esp_wifi_start()` → `esp_wifi_connect()`. Keeps
   the UI alive and running. Fragile if the SDIO peripheral state is
   also compromised.
2. **`esp_restart()`**. UI blinks for ~3 s, comes back with state
   resumed from NVS. Guaranteed to recover because both the P4 boots
   fresh AND the reset-slave-on-boot Kconfig drives the C6 reset GPIO
   from `ensure_slave_bus_ready` on the next `esp_hosted_init()`. This
   is what Espressif's example calls `DO_HOST_RESET` mode.

For this project we chose option 2. The hang should be rare (every few
days of uptime under sustained MQTT load), so a 3-second UI blink every
few days is an acceptable price for automatic recovery. Option 1 is worth
revisiting if the failure rate turns out to be more frequent than "rare",
because keeping the display + audio + LVGL alive during recovery is a
better UX than a full reboot.

**Implementation** lives in `main/wifi_health.c`. Sketch:

```c
#include "esp_event.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_hosted.h"
#include "esp_hosted_event.h"

#define HEARTBEAT_SEC          10
#define HEARTBEAT_TIMEOUT_US   (45LL * 1000 * 1000)

static const char *TAG = "WIFI_HEALTH";
static esp_timer_handle_t s_hb_timer = NULL;
static bool s_saw_first_init = false;

static void arm_or_restart_hb_timer(void) {
    if (!s_hb_timer) return;
    if (esp_timer_is_active(s_hb_timer))
        esp_timer_restart(s_hb_timer, HEARTBEAT_TIMEOUT_US);
    else
        esp_timer_start_once(s_hb_timer, HEARTBEAT_TIMEOUT_US);
}

static void reboot_with_reason(const char *reason) {
    ESP_LOGE(TAG, "C6 wedge detected (%s) — rebooting for recovery", reason);
    /* Give UART a chance to flush the reason. */
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_restart();
}

static void hb_timeout_cb(void *arg) {
    (void)arg;
    reboot_with_reason("no heartbeat from C6 in 45 s");
}

static void on_hosted_event(void *arg, esp_event_base_t base,
                             int32_t id, void *data) {
    (void)arg; (void)base; (void)data;
    switch (id) {
    case ESP_HOSTED_EVENT_CP_HEARTBEAT:
        arm_or_restart_hb_timer();
        break;
    case ESP_HOSTED_EVENT_CP_INIT:
        if (!s_saw_first_init) { s_saw_first_init = true; break; }
        reboot_with_reason("unexpected C6 init event (slave crashed)");
        break;
    case ESP_HOSTED_EVENT_TRANSPORT_FAILURE:
        reboot_with_reason("SDIO transport failure");
        break;
    default:
        break;
    }
}

void wifi_health_init(void) {
    esp_event_handler_register(ESP_HOSTED_EVENT, ESP_EVENT_ANY_ID,
                               &on_hosted_event, NULL);
    const esp_timer_create_args_t args = {
        .callback = hb_timeout_cb,
        .name = "hb_watchdog",
    };
    esp_timer_create(&args, &s_hb_timer);
    esp_timer_start_once(s_hb_timer, HEARTBEAT_TIMEOUT_US);
    esp_hosted_configure_heartbeat(true, HEARTBEAT_SEC);
    ESP_LOGI(TAG, "heartbeat=%ds, timeout=%llds, action=esp_restart",
             HEARTBEAT_SEC, HEARTBEAT_TIMEOUT_US / 1000000);
}
```

Called from `main.c` right after `esp_wifi_start()`.

Also worth doing (small, related): guard `set_var_mqtt_connected()` with
a "did the value actually change?" check so the failing-reconnect loop
doesn't call `paint_notif_badge()` (and grab the LVGL lock for ~100 ms)
every 17 s while the recovery timer is counting down. This shaves the
"buttons feel slow" symptom before the reboot kicks in.

**Why this had to be added on top of ESP-Hosted rather than fixed inside
it.** The RPC layer intentionally does not retry or auto-recover — the
philosophy is "the transport tells the app when the slave is gone, the
app decides recovery." `ESP_HOSTED_EVENT_TRANSPORT_FAILURE` covers the
transport half. Detecting application-layer hang (SDIO OK, RPC handler
stuck) needs the heartbeat, and only the app knows what "we've lost the
C6 long enough to warrant recovery" means for its use case. So the
heartbeat monitor is not a workaround for a missing feature — it's the
integration point ESP-Hosted expects you to write.

---

## 22. Why this board took so much longer than the Waveshare Fireside

(Source: LessonsLearned.md §27)

This project started life as a port of the TrailCurrent Fireside firmware,
which runs on a Waveshare ESP32-P4 development board. Same chip family,
similar peripherals, totally different manufacturer. Porting Fireside to
the Elecrow CrowPanel Advance 10.1" took vastly longer than any earlier
Fireside development iteration on the Waveshare hardware. Documenting why,
because next time we port to a third board we should not repeat this.

### 1. Waveshare hides the boot order inside a wrapper; Elecrow makes you own it.

Waveshare ships a Board Support Package (BSP) with a single call:
`bsp_display_start_with_config(&cfg)`. You pass a struct with buffer sizes
and rotation flags. That one function _internally_ brings up LDO rails, I2C,
the touch chip, the MIPI-DSI panel, LVGL, and attaches the touch as an
LVGL input device — in the exact right order. You literally _cannot_ get
the order wrong because you don't own the order. On the Waveshare board,
"WiFi first or WiFi last" is a non-question; the BSP has already done
display + touch before your `app_main` gets a chance to call anything.

Elecrow's BSP is _not_ wrapped. Instead you get discrete functions:
`i2c_init()`, `touch_init()`, `display_init()`, `set_lcd_blight()`. You
must call them yourself, in the right order, in your `app_main`. There is
no compile-time check, no runtime warning if you get it wrong. The
vendor's Lesson09 shows the correct order, but nothing prevents you from
inserting a WiFi block ahead of it and shipping code that quietly breaks
touch.

We inherited the Fireside habit of "start WiFi as early as possible so the
UI can show connection status" — a habit that was safe on Waveshare because
Waveshare's wrapper had already done display + touch before your code
started. That same habit, applied line-for-line to Elecrow, silently
disabled touch and gave us no obvious clue.

### 2. Elecrow's WiFi lives on a separate chip; Waveshare's is on-die.

The ESP32-P4 has no radio of its own. Waveshare's board wires the P4 to
an external ESP32-C6 co-processor using a well-integrated helper library.
Elecrow does the same thing but with an older version of that library
(`esp_hosted 2.7.x` — the Waveshare examples we were used to use 2.12.x).
Older `esp_hosted` requires you to manually call `esp_hosted_init()`;
newer versions auto-initialize. So on top of the ordering trap we were
also debugging a "did it initialize?" question, and every serial log had
another ~2 seconds of SDIO transport chatter that we had to mentally
filter out before we could see anything else.

### 3. Elecrow's touch pipeline can look healthy while being broken.

On both boards, the GT911 touch chip is initialized identically: reset
line, INT line to set the I2C address, then `esp_lcd_touch_new_i2c_gt911`.
On Waveshare, if the driver reports "GT911 ID = 0x39,0x31,0x31" you know
touch is working. On Elecrow, that ID line _also_ prints when touch is
about to silently fail because LVGL's polling won't run. The healthy-looking
init log misled us into checking the wrong things (I2C addresses, INT
pulldown level, driver `driver_data` field, etc.) for hours before we
realized the driver was doing exactly what it was supposed to and the
break was one layer up.

### 4. We tried to reason about the problem instead of using the working example.

The single fastest thing we could have done — the thing that _did_ crack
it in the end — was flash Elecrow's Lesson09 firmware to the board. It
took under a minute. It immediately proved that hardware, touch driver,
display, and LVGL were all fine, and that the break was in our
application's `app_main`. Instead we spent hours re-checking sdkconfig
values, comparing the compat wrapper against native BSP, decompiling
factory firmware in our heads, etc. — all of it downstream of information
Lesson09 would have given us in 60 seconds. **The rule going forward: when
a vendor ships working example firmware for their board, flash it first,
confirm the feature you care about works, and only then start comparing
to your project. You save yourself from chasing hypotheses about hardware
that is provably fine.**

### 5. Multiple stale backups and untracked git state made "what changed?" impossible to answer.

Because this project is untracked in git (all files show as `??` in
`git status`), we could not do a `git diff` against "the last working
state". Two `preport_backup_*` directories in `/tmp/` gave partial
snapshots but weren't comprehensive. When the user said "it was working an
hour ago", we had no reliable way to identify what had changed between
"working" and "now". Committing to git before every experimental change
would have collapsed a lot of that uncertainty.

### 6. Multi-agent sessions edited things without a shared plan.

Across the debugging session, several files (`sdkconfig.defaults`,
`sdkconfig.defaults.esp32p4`, `main.c`) were rewritten by different agents
attempting different theories. Each rewrite was reasonable in isolation
but the cumulative effect was that the "known-good" state kept moving
under us. Setting a firm ground truth (Lesson09 flashed onto the board;
that's the baseline) and only diffing outward from it would have removed
this drift.

### The single-sentence takeaway

**When porting firmware from Board A to Board B (same chip family,
different vendor), start from Board B's vendor example and add features
one at a time — do NOT start from Board A's finished firmware and try to
patch it to Board B.** The whole "WiFi killed touch" fault would have been
avoided if the first commit of this project had been an unmodified copy of
Elecrow's Lesson09.

---

## Adding new board-specific lessons

Structure each entry as **symptom → root cause → fix** with a heading that
names the topic. If the lesson is hardware-specific to the CrowPanel Advance
(BSP init order, board revision pin drift, on-board peripheral quirks,
STC8 fuel-gauge behavior, GT911 touch specifics, ESP-Hosted+C6 issues,
etc.) it belongs here. Generic ESP-IDF / LVGL / EEZ Studio / build-system
lessons belong in [LessonsLearned.md](LessonsLearned.md).
