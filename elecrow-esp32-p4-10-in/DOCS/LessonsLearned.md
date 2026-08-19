# Lessons Learned

Traps and non-obvious details discovered while building the Headwaters Map
Viewer firmware on the Elecrow **CrowPanel Advance 10.1" ESP32-P4 HMI**
board with ESP-IDF v5.5.2, LVGL 8.4.x, and EEZ Studio as the GUI source of
truth.

Each entry is: **symptom → root cause → fix**. Add new entries as they
surface — the whole point is that the next person (or the next agent) can
skip the debugging cycle.

---

## 1. EEZ Studio owns `ui/`; nothing else touches it

**Symptom.** Files landed in `main/` alongside handwritten application
code; every re-export from EEZ Studio would then either overwrite them or
need manual reconciliation.

**Root cause.** Default `.eez-project` `destinationFolder` was set to a
path that put the export inside the `main` component. That mixes generated
and hand-written code in one directory — the moment EEZ Studio is opened
again and Ctrl+B is hit, the boundary is lost.

**Fix.**
- `.eez-project` `settings.build.destinationFolder = "../../ui"` (relative
  to the `.eez-project` file at `GUI/elecrow-esp32-p4-10-in/`).
- `ui/` at project root is its own ESP-IDF component, with its own
  `CMakeLists.txt`.
- `main/` REQUIRES `ui`.
- Every subsequent EEZ Studio export drops files into `ui/` — no manual
  moves.

---

## 2. `ui/` component must be `WHOLE_ARCHIVE`

**Symptom.** Link fails with `undefined reference to action_wifi_scan` (and
every other `action_*`), even though `main/actions.c` compiles those symbols
into `libmain.a`.

**Root cause.** `ui/screens.c` references the `action_*` symbols that live
in the sibling `main` component. ESP-IDF's link line puts components in a
specific order and `--gc-sections` does on-demand archive resolution —
`libui.a` gets scanned before `libmain.a`, screens.c.obj hasn't been pulled
in yet (dead-code scan), and by the time `libmain.a` is fully processed,
the linker no longer has an outstanding request for `action_*`.

**Fix.** `ui/CMakeLists.txt`:

```cmake
idf_component_register(
    SRCS ${UI_SRCS}
    INCLUDE_DIRS "."
    REQUIRES lvgl__lvgl
    WHOLE_ARCHIVE
)
```

`WHOLE_ARCHIVE` forces every object in `libui.a` to be linked in
unconditionally, so the `action_*` references are live and get resolved
from `libmain.a` when it's scanned.

---

## 3. EEZ Studio emits `#include <lvgl/lvgl.h>`; ESP-IDF exposes `lvgl.h`

**Symptom.** Every `.c` under `ui/` fails to compile with
`fatal error: lvgl/lvgl.h: No such file or directory`.

**Root cause.** EEZ Studio's C templates inherit the SquareLine Studio
include convention `#include <lvgl/lvgl.h>`. ESP-IDF's `lvgl__lvgl`
managed component exposes the header as plain `lvgl.h` — no `lvgl/`
subdirectory prefix. This is a fixed template pattern in EEZ Studio; you
can't tell it to emit `"lvgl.h"` instead without editing the export
templates.

**Fix.** A four-line shim at `ui/lvgl/lvgl.h`:

```c
/* Angle-bracket form skips the current-file's directory in the header
 * search — otherwise this shim recursively includes itself. */
#include <lvgl.h>
```

**Trap inside the fix.** The shim's own `#include` MUST use angle
brackets (`<lvgl.h>`), NOT quotes (`"lvgl.h"`). Quoted-form includes search
the current file's directory FIRST, which for this shim is `ui/lvgl/` —
and the file named `lvgl.h` in that directory is the shim itself. Result:
infinite recursion, `error: #include nested depth 200 exceeds maximum of
200`. Angle-bracket form skips the current-file directory and resolves via
`-I` include paths, hitting the real `managed_components/lvgl__lvgl/lvgl.h`.

---

## 4. EEZ Studio's export lowercases action names to snake_case

**Symptom.** Handwritten `action_WifiScan()` in `main/actions.c` compiles,
but the link fails with `undefined reference to action_wifi_scan` because
`ui/screens.c` calls the lowercased name.

**Root cause.** The `.eez-project` declares actions in the author's
casing (`WifiScan`, `MapZoomIn`, etc.). EEZ Studio's C generator lowercases
and snake_cases these to `action_wifi_scan`, `action_map_zoom_in`, etc.
The declarations in `ui/actions.h` are `extern void action_wifi_scan(...)`;
the C definitions in `main/actions.c` MUST match.

**Fix.** Always define the action handlers in snake_case in `main/actions.c`:

```c
void action_wifi_scan(lv_event_t *e) { ... }
void action_map_zoom_in(lv_event_t *e) { ... }
```

**Reference table.** Whatever the `.eez-project` calls `SomeCamelCase`
becomes `action_some_camel_case` in C. Same rule for variables:
`get_var_X` / `set_var_X` where X is the snake_case name.

---

## 5. ESP32-P4 has no on-die radio — WiFi comes via ESP-Hosted

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
matches Elecrow's Lesson17-Wi-Fi_function example.

---

## 6. MIPI-DSI DMA on ESP32-P4 needs specific L2 cache + MMU config

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

## 7. LVGL is not thread-safe — every cross-thread call needs `lvgl_port_lock()`

**Symptom.** Screen looks OK for a fraction of a second, then goes blue
and flickers. Or: a widget briefly renders correctly, then the whole page
becomes garbled.

**Root cause.** `display_init()` inside `bsp_illuminate.c` calls
`lvgl_port_init()` — which spawns the LVGL renderer task **immediately**.
From that point on, any LVGL API call from another thread (`app_main`,
WiFi callbacks, HTTP tasks) races the renderer. Widget-tree corruption,
buffer tearing, all the usual symptoms — on MIPI-DSI double-buffering it
looks like blue flicker.

**Fix.** Wrap every LVGL API call from a non-LVGL thread:

```c
#include "esp_lvgl_port.h"

if (lvgl_port_lock(0)) {
    lv_label_set_text(objects.my_label, "hello");
    lv_obj_clear_flag(objects.my_widget, LV_OBJ_FLAG_HIDDEN);
    lvgl_port_unlock();
}
```

**Where this hides.**
- `ui_init()` from `app_main` — needs the lock.
- Every `app_set_*` function in `app_state.c` — called from wifi_task,
  auth_task, tile_task worker threads.
- Every `lv_scr_load` / `loadScreen` call from a task or event handler
  that isn't the LVGL task itself.

**Where locking is NOT needed.** LVGL event callbacks
(`lv_obj_add_event_cb`) run on the LVGL task, so anything called from
inside an event handler already holds the lock implicitly. Elecrow's
Lesson09 gets away without any explicit locks because its only LVGL calls
outside `app_main` are inside event callbacks — a pattern that doesn't
survive contact with WiFi / MQTT / HTTP callbacks.

**Trap.** `lvgl_port_lock(0)` uses `portMAX_DELAY` under the hood — the
`0` timeout means "wait forever". A non-zero timeout returns false on
timeout, in which case the LVGL call must be skipped. Prefer `0` unless
you have a real deadline.

---

## 8. Backlight ON only AFTER `ui_init()` populates the frame buffer

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

## 9. `sdkconfig` is generated from `sdkconfig.defaults` only when it doesn't exist

**Symptom.** Added a new `CONFIG_X=y` to `sdkconfig.defaults`, ran
`idf.py build`, but `sdkconfig` still shows `# CONFIG_X is not set` and
the compile still fails as if the flag isn't set.

**Root cause.** ESP-IDF only applies `sdkconfig.defaults` values to keys
that aren't already present in `sdkconfig`. Once `sdkconfig` exists, new
default entries are silently ignored on subsequent builds — the file
becomes sticky.

**Fix.** When adding or changing anything in `sdkconfig.defaults` or
`sdkconfig.defaults.esp32p4`:

```bash
rm sdkconfig
idf.py build
```

Or `idf.py reconfigure` / `idf.py fullclean`. The rebuild picks up the
new defaults.

**Never edit `sdkconfig` directly.** It's regenerated by ESP-IDF and the
change will vanish. `sdkconfig.defaults` is the source of truth.

---

## 10. Flash size: default 2 MB is not enough for this project

**Symptom.** Partition table generation fails:
```
Partitions tables occupies 5.1MB of flash (5308416 bytes) which does not
fit in configured flash size 2MB
```

**Root cause.** ESP-IDF's default `CONFIG_ESPTOOLPY_FLASHSIZE_2MB=y`. The
CrowPanel Advance carries 16 MB flash, and our `partitions.csv` allocates
4 MB for the factory app + 1 MB for SPIFFS.

**Fix.** `sdkconfig.defaults`:

```
CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y
CONFIG_ESPTOOLPY_FLASHSIZE="16MB"
```

---

## 11. Every Montserrat size referenced in the `.eez-project` must be enabled in Kconfig

**Symptom.** `ui/styles.c` fails to compile:
```
error: 'lv_font_montserrat_32' undeclared (first use in this function);
did you mean 'lv_font_montserrat_22'?
```

**Root cause.** EEZ Studio's export references LVGL's built-in Montserrat
fonts by symbol name (`&lv_font_montserrat_<size>`), but LVGL only compiles
in the sizes explicitly enabled via Kconfig. Font sizes referenced by any
style in the `.eez-project` must have their Kconfig flag set.

**Fix.** Enable the full range in `sdkconfig.defaults`:

```
CONFIG_LV_FONT_MONTSERRAT_8=y
CONFIG_LV_FONT_MONTSERRAT_10=y
...
CONFIG_LV_FONT_MONTSERRAT_48=y
```

Costs ~10 KB per size in the binary; still 65% free in a 4 MB app
partition after enabling all of them, so no reason to trim.

---

## 12. `-Werror=format-truncation` fires on small `snprintf` targets

**Symptom.** Build fails:
```
error: '%s' directive output may be truncated writing up to 128 bytes
into a region of size 64 [-Werror=format-truncation=]
```

**Root cause.** GCC statically computes the maximum length of a `%s`
argument (based on the source array's declared size) and warns if the
destination buffer might overflow. ESP-IDF promotes this warning to an
error by default. A `char msg[80]` receiving a URL of up to 128 chars
plus a fixed prefix trips it.

**Fix.** Size destination buffers based on the source constants:

```c
// Bad:
char msg[80];
snprintf(msg, sizeof(msg), "Could not reach %s (status %d)", url, status);

// Good:
char msg[APP_MAX_URL_LEN + 40];
snprintf(msg, sizeof(msg), "Could not reach %s (status %d)", url, status);
```

Alternative: use `snprintf` with an explicit precision (`%.60s`) to bound
the argument length. Cleaner in most cases to size the destination.

---

## 13. The CrowPanel BSP owns LDO, I2C, touch, LCD, and LVGL port init — in that order

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
LVGL call from `app_main` (or any other task) MUST take the lock. See
lesson 7.

---

## 15. `LV_MEM_SIZE_KILOBYTES=32` is way too small for a multi-page project

**Symptom.** Boot log ends with:
```
I (1822) LVGL: Starting LVGL task
E (1822) lcd_panel: esp_lcd_panel_swap_xy(...): swap_xy is not supported...
Guru Meditation Error: Core 0 panic'ed (Load access fault). Exception was unhandled.
MEPC : 0x40032f74  ...  MTVAL : 0x00000022  MCAUSE : 0x00000005
```
`addr2line` decodes `0x40032f74` to
`lv_obj_mark_layout_as_dirty at src/core/lv_obj_pos.c:290` — dereferencing
a NULL `lv_obj_t *obj` inside widget construction.

**Root cause.** LVGL's default `lv_obj_class_create_obj` returns `NULL`
when the internal heap is exhausted, and `lv_obj_class_init_obj(NULL)`
then dereferences NULL. LVGL's Kconfig default `LV_MEM_SIZE_KILOBYTES=32`
(a fixed 32 KB pool, no growth) is fine for Lesson09's three-button demo
but the `.eez-project` here instantiates 6 screens with hundreds of
widgets — 32 KB runs out during the first screen's build.

**Fix.** Route LVGL through the system allocator so it can spill into
PSRAM (via ESP-IDF's `CONFIG_SPIRAM_USE_MALLOC=y`, already set):

```
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEM_CUSTOM_INCLUDE="stdlib.h"
```

**Trap inside the fix.** The Kconfig only exposes `LV_MEM_CUSTOM` and
`LV_MEM_CUSTOM_INCLUDE` — NOT `LV_MEM_CUSTOM_ALLOC` /
`LV_MEM_CUSTOM_FREE` / `LV_MEM_CUSTOM_REALLOC`. LVGL's Kconfig
hard-codes `malloc()`/`free()`/`realloc()` when custom is enabled; only
the include path is configurable. Setting the ALLOC/FREE/REALLOC symbols
produces `warning: unknown kconfig symbol` and they're silently ignored.

**Alternative fix.** Bumping `CONFIG_LV_MEM_SIZE_KILOBYTES=128` (the
Kconfig max) is enough for many projects but caps at 128 KB — the
`_CUSTOM` route above is unbounded.

---

## 16. ESP-Hosted resets the whole P4 if the C6 slave isn't responding

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
ESP32-C6 co-processor over SDIO via ESP-Hosted (see lesson 5). If the
C6 doesn't respond to the SDIO handshake (missing firmware, wrong
board revision, bus wiring), ESP-Hosted's default behavior is to
`esp_restart()` the P4 and try again with the slave. This is
intentional in ESP-Hosted — it assumes the P4 is fresh out of a race
condition — but it produces a boot loop when the underlying C6 issue
is persistent.

**Diagnostic fix — decouple boot from WiFi.** In `main.c`, do NOT call
`wifi_task_start()` from `app_main`. Defer WiFi initialization until the
user actually needs it (e.g., taps Rescan on PageWifiSetup). Then the
boot renders cleanly no matter what state the C6 is in, and the user can
see the reset loop only when they trigger the WiFi scan.

```c
// main.c — WiFi bring-up moved out of app_main
// Boot flow now stops after ui_init + backlight; wifi_task_start() moves
// into action_wifi_scan (lazy, first-tap init).

// actions.c
void action_wifi_scan(lv_event_t *e) {
    (void)e;
    wifi_task_start();          // idempotent
    wifi_task_request_scan();
}
```

**Real fix (still open).** The C6 slave needs its ESP-Hosted slave
firmware flashed. On Elecrow's CrowPanel Advance the C6 is accessible
via a separate USB port on the board or via the P4's USB-CDC bridge —
check `CrowPanel-*/example/V1.2/Upgrade P4 to C6 firmware/` in the
vendor materials for the exact flashing procedure. TODO once we've
walked through it.

---

## 17. Serial capture from an already-booted device shows nothing

**Symptom.** `stty -F /dev/ttyUSB0 115200 raw -echo -icanon; timeout 8
cat /dev/ttyUSB0` returns zero bytes even though the device is running.

**Root cause.** The ESP-IDF app boots in <2 seconds. If your capture
starts AFTER the boot logs have already been printed, and the app then
sits quiet (LVGL task rendering, no log messages), stdout on the serial
port is idle. `cat` blocks with no data.

**Fix.** Reset the chip immediately before capturing:

```bash
esptool.py --port /dev/ttyUSB0 --before default_reset --after hard_reset run &
sleep 1
stty -F /dev/ttyUSB0 115200 raw -echo -icanon
timeout 8 cat /dev/ttyUSB0 > /tmp/serial.log
wait
```

`esptool.py run` sends a reset without erasing anything, and the boot
logs land in the capture window.

**Trap.** `esptool.py chip_id` (used to probe the chip) leaves the P4 in
DOWNLOAD mode (bootloader waiting for esptool) — you'll see
`waiting for download` repeated instead of app output. Use
`esptool.py run` to reset back to the flashed app.

---

## 18. ESP-Hosted SDIO pin config is board-revision-specific

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

## 19. ESP-Hosted host/slave version must match — pin to Elecrow's tested version

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

## 20. sys_evt task stack overflows during ESP-Hosted scan event dispatch

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

## 21. EEZ Studio's `ui_init` always loads the first-declared page (SCREEN_ID_MAIN)

**Symptom.** After boot the display briefly shows a wrong page (blue background, no widgets), then flickers as our code switches to the intended page. Touch is briefly unresponsive.

**Root cause.** EEZ Studio's generated `ui_init()` hard-codes `loadScreen(SCREEN_ID_MAIN)` at the end. `SCREEN_ID_MAIN` maps to `userPages[0]` — the FIRST page in the `.eez-project`, which for us is the leftover legacy "Main" page. `createAtStart: true` on other pages is ignored by the generator; only the position of the page in `userPages[]` matters.

Then our own `app_show_screen(APP_SCREEN_WIFI_SETUP)` fires ANOTHER `lv_scr_load_anim` with a 200 ms fade-in. Two overlapping animations + our lock/unlock churn = buffer flicker + starved touch polling.

**Fix — three parts.**

1. **Use `lv_scr_load` (instant, no anim) for the first screen switch**, only animate subsequent transitions. In `app_state.c`:
```c
static bool s_initial_screen_loaded = false;
static void load_screen(app_screen_t s) {
    lv_obj_t *scr = screen_for(s);
    if (!s_initial_screen_loaded) {
        lv_scr_load(scr);              // cancels the MAIN fade cleanly
        s_initial_screen_loaded = true;
    } else {
        lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
    }
}
```

2. **Wait past the MAIN fade before switching.** In `app_main`, after `ui_init()`:
```c
vTaskDelay(pdMS_TO_TICKS(300));   // let MAIN's 200 ms fade finish
app_show_screen(APP_SCREEN_WIFI_SETUP);
```

3. **Real fix (cleaner but requires an EEZ Studio re-export):** rename/reorder the `.eez-project` so the desired boot page IS the first entry in `userPages[]`. Then `SCREEN_ID_MAIN` naturally maps to the right screen and steps 1-2 aren't needed.

---

## 22. Hide widget rows and control indicators that don't reflect current state

**Symptom.** WiFi setup screen renders 8 blank rows with signal bars and lock icons. Spinner spins. Nothing is actually scanning.

**Root cause.** The `.eez-project` authors 8 placeholder rows for future scan results. Without any scan having happened they render with empty SSID text — visible strip-shaped panels with all the chrome but no meaning. The spinner spins because it's a Spinner widget (always animates by JSON default).

**Fix.** On screen entry, hide unpopulated rows and non-active indicators:
```c
app_set_scan_results(NULL, 0);      // hides all 8 rows
app_set_wifi_scanning(false);       // hides the spinner
app_set_wifi_status("Tap Rescan to search for networks");
```

Reveal them only when the data / activity is real. Same rule for any status pill or activity indicator: hide when it doesn't mean anything, show when it does. Keep the JSON-authored placeholder visible in EEZ Studio's canvas (that's the design template), but the device's runtime state is authoritative for what the user sees.

**This is state, not geometry** — allowed under the canvas-device divergence rule (skill Gate 5).

---

## 23. Headwaters API keys don't use the "Bearer " prefix; session tokens do

**Symptom.** ESP-Hosted WiFi works, DHCP lease is good, request goes out to `https://<host>/api/auth/check` with `Authorization: Bearer <token>`. Server returns 401. esp_http_client logs:
```
W (6422) HTTP_CLIENT: This request requires authentication, but does not provide header information for that
E (6422) HTTP_CLIENT: Error response
```
and returns `ESP_ERR_NOT_SUPPORTED` because it tried to auto-retry with Basic auth and gave up.

**Root cause.** Headwaters accepts TWO distinct token formats via `authMiddleware` in `containers/backend/src/routes/auth.js`:

| Token type | Prefix | Header format | Validate endpoint |
|---|---|---|---|
| Session token | none | `Authorization: Bearer <session_token>` | `GET /api/auth/check` |
| API key | `rv_` | `Authorization: <api_key>` (**no Bearer**) | any middleware-protected endpoint |

`/api/auth/check` is session-only — it looks up the header value (after stripping "Bearer ") in the `sessions` collection. An API key wrapped in "Bearer " won't match a session record → 401.

**Fix.** Probe the token by prefix, pick the right header format + endpoint:

```c
bool is_api_key = strncmp(token, "rv_", 3) == 0;
const char *path = is_api_key ? "/api/maps/current" : "/api/auth/check";
if (is_api_key)
    snprintf(auth_hdr, sizeof(auth_hdr), "%s", token);       // raw
else
    snprintf(auth_hdr, sizeof(auth_hdr), "Bearer %s", token); // wrapped
esp_http_client_set_header(c, "Authorization", auth_hdr);
```

Any middleware-protected endpoint works for API-key probes; `/api/maps/current` is a good choice because a 200 = maps loaded and 404 = no map bundle yet — both mean auth succeeded, only 401/403 = auth failed. Widen the success check:

```c
if ((status >= 200 && status < 300) || status == 404) {
    // auth succeeded
} else if (status == 401 || status == 403) {
    // token rejected
}
```

**Where to get an API key:** Headwaters PWA → Settings → API Keys → Create. It's a `rv_...` string, ~67 chars. Copy the full value into the SD config file `token=` line.

---

## 24. FATFS defaults to DOS 8.3 filenames; enable LFN to see the real name

**Symptom.** SD card mounts cleanly, `readdir("/sdcard")` returns entries, but the filename you saved as `headwaters.conf` shows up as `HEADWA~1.CON` and `fopen("/sdcard/headwaters.conf")` returns NULL:
```
I (1920) config_sd: Files at /sdcard:
I (1920) config_sd:   HEADWA~1.CON
I (1920) config_sd: No /sdcard/headwaters.conf on SD card
```

**Root cause.** FATFS in ESP-IDF defaults to `CONFIG_FATFS_LFN_NONE=y` — long-file-name support is OFF, so the driver reports only DOS 8.3 short names. `headwaters.conf` (>8 chars in base name, >3 in extension) becomes `HEADWA~1.CON` and `fopen` on the long name doesn't match.

**Fix.** In `sdkconfig.defaults`:

```
CONFIG_FATFS_LFN_HEAP=y
CONFIG_FATFS_MAX_LFN=255
CONFIG_FATFS_API_ENCODING_UTF_8=y
```

`LFN_HEAP` allocates the LFN scratch buffer from heap (safe for our stack budget). After enabling, `readdir` returns the true long name and `fopen("/sdcard/headwaters.conf")` matches.

**Trap.** As a defensive fallback, iterate `readdir` and match filenames case-insensitively via `strcasecmp`. Even with LFN enabled, FAT is case-insensitive on the wire but ESP-IDF's VFS layer treats paths as case-sensitive on open. A file saved as `Headwaters.conf` (mixed case) may still fail `fopen("/sdcard/headwaters.conf")` in edge cases.

---

## 25. `esp_wifi_get_config()` over `esp_wifi_remote` returns empty SSID

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

## 14. `EXTRA_COMPONENT_DIRS` accepts single-component directories

**Symptom (avoided by knowledge).** Uncertainty about whether `ui/`
(a directory with its own `CMakeLists.txt` and source files) works as a
top-level component.

**Root cause / detail.** `EXTRA_COMPONENT_DIRS` in the top-level
`CMakeLists.txt` can point to either:
- A parent directory containing multiple components (like `components/`
  → each subdir with its own CMakeLists.txt is a component), OR
- A single component directory directly.

**Fix.** Top-level `CMakeLists.txt`:

```cmake
set(EXTRA_COMPONENT_DIRS "components" "ui")
```

`components/` provides the four `bsp_*` components as subdirs. `ui/` IS
itself a component (its own `CMakeLists.txt` calls
`idf_component_register`). Both forms coexist in one list.

---

## 26. WiFi-before-touch silently kills LVGL touch dispatch

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

## 27. Why this board took so much longer than the Waveshare Fireside

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

## 28. GT911 INT pin makes touch feel like you have to hold for a full second

**Symptom.** The board boots, WiFi comes up, MQTT connects, the UI renders,
buttons _do_ respond — but only if you press hard and hold your finger down
for roughly a second. Quick taps are silently ignored. Swipes register
inconsistently. It's not dead touch (lesson 26) — it's laggy, deliberate,
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
a direct-polling diagnostic task (like the one used to isolate lesson 26)
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
`esp_lcd_touch_config_t`. This is different from lesson 26 (dead touch,
_no_ response). Lesson 26 = ordering; lesson 28 = event vs polling mode.

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

## 29. Elecrow V1.0 factory sourcecode is not safe to copy on a V1.2 board

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
  fiddling** (lesson 30). Copying V1.0's `sd_pwr_ctrl_new_on_chip_ldo(4)`
  onto a V1.2 board steals LDO4 from the MIPI-DSI panel digital rail
  (lesson 13) and every touch/button on the UI goes molasses-slow.
- PDM microphone on the same I2S peripheral as the speaker TX, with
  default clock-config macros — **but V1.2 puts PDM RX on I2S_NUM_0 with
  specific `dn_sample_mode`/`bclk_div`/`hp_en` fields** (lesson 31).
  The default macros return silent audio; every capture yields ±0.

**Rule.** Match example-folder revision to the physical board silkscreen
revision. On this project the target is **V1.2**, so the canonical
reference is `CrowPanel-Advanced-.../example/V1.2/idf-code/Lesson<N>-*/`,
NOT `factory_sourcecode/V1.0/`. The `V1.0` folder should be treated as
a completely different product.

If the V1.2 folder doesn't have an example for what you need, look at
the V1.1 folder next (revisions 1.1 and 1.2 share most pin assignments —
see lesson 18 for SDIO specifically). Only fall back to V1.0 as a last
resort, and diff the two revisions' `board_config.h` line-by-line
before trusting any pin.

**Cross-references.**
- Lesson 13 explains what LDO3 (2500 mV, panel analog) and LDO4
  (3300 mV, panel digital) power on this board — stealing either
  breaks the display.
- Lesson 18 documents the same rev-specific pin drift for ESP-Hosted
  SDIO (7"/9"/10.1" V1.1+V1.2 use GPIOs 14/15/16/17/18/19/32; V1.0 is
  different).
- Lessons 30 and 31 document the specific V1.2 SD and mic fixes that
  fell out of applying this rule.

---

## 30. SD card init on V1.2 — explicit pins, no LDO acquisition

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
lesson 13). Two drivers now fight for control of the same LDO. The
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
return NULL despite `readdir` listing the file, see lesson 24 — FATFS
long-file-name support (`CONFIG_FATFS_LFN_HEAP=y`) needs to be enabled
or files with base name >8 chars show up as their DOS 8.3 short name.

---

## 31. PDM mic on V1.2 — MUST bind I2S_NUM_0, needs full clk_cfg + slot_cfg

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

## 32. Adding modest BSS shifts main task stack into TCM → nvs_flash_init aborts

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

The crash log stack dump's `SP` value is the give-away: `0x301xxxxx` =
TCM, `0x4FFxxxxx` = internal SRAM. Once you know to read that field,
this diagnosis is a 10-second job.

**Fix.** In `sdkconfig.defaults`:

```
CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192
```

Doubling the requested main-task stack size forces IDF to allocate from
the 384 KB internal SRAM pool (TCM is only 7 KB and can't hold an 8 KB
stack). It also gives more headroom for the deep `nvs_flash_init` +
`fireside_config_init` + `alarms_init` call chain that runs early.

Regenerate `sdkconfig` from defaults after the change (adding a new
`CONFIG_*` key doesn't retroactively appear in an existing `sdkconfig` —
see lesson 17):

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
- `SP` starts with `0x301` → stack in TCM → this lesson.
- `SP` starts with `0x480` (or wherever PSRAM lives) → stack in PSRAM.
- `SP` starts with `0x4FF` → stack in internal SRAM, this lesson doesn't apply.

---

## 33. WAV response body streaming: static scratch, not malloc-per-chunk

**Symptom.** Streamed audio (e.g., Peregrine's `/api/voice` reply WAV
being played into I2S TX as the HTTP body arrives) sounds like pure
noise / static — not "distorted speech," not silence, but random
crackle from start to finish. The response arrives fine (headers,
content length correct), the transcript header is present, the pipeline
otherwise works.

**Root cause.** The naive playback path per HTTP chunk (esp_http_client
delivers ~4 KB chunks) is:

```c
int16_t *scratch = malloc(byte_len);        // per chunk
scale_volume(scratch, pcm, byte_len);
i2s_channel_write(tx, scratch, byte_len);
free(scratch);
```

At ~10 chunks/sec for a 5-second Piper reply that's ~50 malloc/free
cycles in ~5 seconds, on a heap already under load from LVGL + WiFi
+ MQTT. The heap fragments, one allocation eventually returns NULL,
the chunk gets dropped, and subsequent chunks stream over stale data
still sitting in the DMA ring buffer → the DAC clocks out random
samples → static.

**Fix.** Allocate one static scratch buffer at BSS size and loop-write
larger chunks through it:

```c
#define STREAM_SCRATCH_BYTES 4096   /* matches esp_http_client default */
static int16_t s_stream_scratch[STREAM_SCRATCH_BYTES / 2];

bool audio_play_stream_write(const void *pcm, size_t byte_len) {
    byte_len &= ~(size_t)1;                 /* even bytes only */
    const int16_t *src = pcm;
    size_t remaining = byte_len / 2;
    while (remaining > 0) {
        size_t take = remaining;
        if (take > STREAM_SCRATCH_BYTES / 2) take = STREAM_SCRATCH_BYTES / 2;
        for (size_t i = 0; i < take; i++) s_stream_scratch[i] = scale(src[i]);
        i2s_channel_write(tx, s_stream_scratch, take * 2, &bw, timeout);
        src += take;
        remaining -= take;
    }
    return true;
}
```

No allocation on the hot path. Static-BSS placement means the buffer
lives in internal SRAM (mind the DMA-safety constraints below) and no
fragmentation is possible.

Reference: `main/audio.c` (`audio_play_stream_write`).

**Trap 1.** Don't skip the volume scaling to "avoid the scratch buffer
entirely" — you'll clip on Piper output (see lesson 34).

**Trap 2.** If your static buffer somehow lands in PSRAM (large size +
`CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL` threshold exceeded, or explicit
`__attribute__((section))` misuse), `i2s_channel_write` from PSRAM is
generally OK on the P4 but cache coherency is fragile. Prefer keeping
it small enough (≤ 4 KB matches `CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=16384`)
that it stays in internal SRAM.

---

## 34. Piper TTS peaks hotter than pre-baked ChatterboxTTS assets

**Symptom.** The Peregrine `/api/voice` reply plays through the same
amp + speaker as the on-device alarm phrases, at the same volume knob
setting. Alarms sound normal. The reply distorts — pure clipping, not
static or noise, just harsh clipping on every peak. Turning down the
volume works but then the alarms are inaudible.

**Root cause.** Two different TTS pipelines produce audio at different
peak amplitude levels:

- Alarm phrases were generated by ChatterboxTTS (ComfyUI-based) with
  gain settings that leave ~6–10 dB of headroom below full scale.
  Samples peak around ±16000..±20000.
- Piper TTS (running on Peregrine) normalizes its output much closer to
  full scale — most consonants and vowels hit or exceed ±32000. On the
  CrowPanel's class-D amp this immediately clips.

The user experience is: reply is unusable at normal alarm volume; if
you turn down for the reply, you can't hear the alarms.

**Fix.** Apply a fixed attenuation to the streamed reply BEFORE it hits
the DAC, so a single volume knob is well-behaved for both paths. In
`audio.c`:

```c
static uint8_t s_stream_loudness_pct = 50;   /* -6 dB starting point */

bool audio_play_stream_write(const void *pcm, size_t byte_len) {
    int32_t gain = (int32_t)s_volume_pct * (int32_t)s_stream_loudness_pct;
    for (each sample) {
        int32_t v = ((int32_t)src[i] * gain) / 10000;
        clamp(v, -32768, 32767);
        scratch[i] = (int16_t)v;
    }
    i2s_channel_write(...);
}
```

Combined gain = `volume × loudness / 10000`. At volume=100, loudness=50 →
50% (-6 dB) of the raw Piper output. That's roughly where Piper's peaks
end up equivalent to ChatterboxTTS's peaks.

**Rule.** Any time you mix two independently-generated audio sources
that go through the same amp, one of them will need level-matching.
Do it in-firmware (per-source attenuation) rather than trying to
normalize on the server — server-side normalization requires a re-deploy
per tuning iteration; firmware attenuation can be exposed as a slider.

**Trap.** Don't confuse this with static/noise (lesson 33). Clipping is
loud and harsh but the words are still recognizable. Static is random
crackle with no intelligible signal.

---

## 35. Bash heredoc `$(cat ...)` expands on the SSH remote host, not locally

**Symptom.** You SSH into the target board and paste a heredoc that
injects a value from a local file:

```bash
sudo tee /etc/systemd/system/foo.service.d/token.conf <<EOF
[Service]
Environment=FOO_TOKEN=$(cat ~/.foo-token)
EOF
```

Systemd then treats `FOO_TOKEN` as empty (the drop-in file shows
`Environment=FOO_TOKEN=` with a trailing `=` and nothing after). If
the value goes through validation (bearer-token comparison, config
lookup, etc.) it fails silently and the operator wonders why the token
"isn't working."

**Root cause.** Inside a heredoc, `$(cat ~/.foo-token)` is expanded by
the shell that's executing the heredoc — which is the **remote** shell
you SSH'd into, not your local one. If `~/.foo-token` only exists on
your workstation, the remote `cat` fails with "No such file or
directory" (usually visible in the terminal) and the substitution yields
an empty string.

**Fix.** Read the value into a local variable BEFORE the ssh call, then
interpolate the local variable into the ssh command:

```bash
TOKEN=$(cat ~/.foo-token | tr -d '\n')       # local expansion
ssh -t user@host "
    printf '[Service]\nEnvironment=FOO_TOKEN=%s\n' '$TOKEN' |
      sudo tee /etc/systemd/system/foo.service.d/token.conf > /dev/null &&
    sudo systemctl daemon-reload &&
    sudo systemctl restart foo.service
"
```

`$TOKEN` is expanded by the LOCAL shell before ssh sends the string over
the wire. The remote shell sees the literal hex value.

**Trap.** `tr -d '\n'` is important — without it the token has a
trailing newline that most consumers accept but some (openssl HMAC,
bearer-token constant-time compares) do NOT.

---

## 36. EEZ Studio silently drops LVGLSliderWidget with `previewValue` fields

**Symptom.** You add a slider widget to a page via `.eez-project` JSON
edit. Gates pass (JSON valid, no dangling references, no canvas-device
divergence). You open the project in EEZ Studio, navigate to the page,
and the slider doesn't appear on the canvas. Nothing else on the page
changed. C-side symbols (`objects.<slider>`) DO get exported after
Ctrl+B and DO work on-device, but the canvas preview shows nothing.

**Root cause.** The EEZ Studio skill's schema crib sheet lists
`LVGLSliderWidget` fields as including `previewValue` and
`previewValueLeft` (LVGL 8.4-era). EEZ Studio's C generator accepts
these — the exported `screens.c` compiles and runs. But EEZ Studio's
own canvas renderer treats them as unknown fields on this widget shape
and silently drops the widget from the visual layout. Classic Mode B
canvas-device divergence: device sees it, canvas doesn't.

**Fix.** Mirror the shape of an existing working slider in the project
(e.g., `slider_screen_brightness` on PageSettings, which EEZ Studio
renders correctly). That shape does NOT carry `previewValue` /
`previewValueLeft`. Strip both fields from your new slider and re-open
in EEZ Studio.

```python
sld = find_node(root, "my_new_slider")
for k in ("previewValue", "previewValueLeft"):
    sld.pop(k, None)
```

**Rule.** The skill's crib sheet is a starting point, not the truth. If
a widget you added doesn't render in the canvas, diff its JSON against
a working sibling of the same type in the same project and strip
whatever extra keys yours has. The working sibling is authoritative.

---

## 37. `idf.py monitor` needs a TTY; use `stty` + `cat` from a subprocess

**Symptom.** Running `idf.py monitor` from a non-interactive subprocess
(script, background bash, CI job) exits immediately with:

```
Error: Monitor requires standard input to be attached to TTY. Try using a different terminal.
idf_monitor failed with exit code 1
```

**Root cause.** `idf_monitor.py` uses `curses`/`readchar` for its
interactive commands (menu key, GDB, panic decode, etc.) and refuses to
run if `sys.stdin` isn't a TTY. It's a hard failure — no `--no-tty`
flag exists.

**Fix.** For simple boot-log capture / event streaming, skip
`idf.py monitor` and read the raw serial device directly:

```bash
stty -F /dev/ttyUSB0 115200 raw -echo -icanon
cat /dev/ttyUSB0
```

You lose panic-backtrace ELF decoding and GDB integration, but for
diagnosing boot crashes, live-tailing logs, or feeding output into
`grep --line-buffered` for monitoring, plain `cat` is fine.

**Trap.** `cat /dev/ttyUSB0` will exit whenever the USB device
disconnects momentarily (esptool flash, board reboot, USB re-enumerate).
Wrap in a retry loop so a transient disconnect doesn't kill the reader:

```bash
while true; do
    cat /dev/ttyUSB0 2>/dev/null
    sleep 0.5
done
```

**Trap 2.** For panic backtrace decoding, hand the addresses to
`riscv32-esp-elf-addr2line -e build/<project>.elf 0x4ff09688` manually.
Or reserve `idf.py monitor` for when the user has an interactive
terminal available.

---

## 38. Volume-runtime slider survives, boot-time NVS restore doesn't

**Symptom.** You add a UI slider that adjusts an audio parameter live,
and want it to persist across reboots. Naive implementation:

- Slider CHANGED handler: read value, apply to runtime, `nvs_set_i32` +
  `nvs_commit`. Works fine (called from LVGL task, safe stack).
- Boot init in `ptt_actions_init()` (called from main task under LVGL
  lock): `nvs_open` + `nvs_get_i32` to restore last value. **Boots
  crashes at `nvs_flash_init()` or later NVS call with the
  `esp_task_stack_is_sane_cache_disabled` assert** (lesson 32).

**Root cause.** Same as lesson 32 — the boot-time NVS restore runs from
main task, and main task's stack may be in TCM. Once you bump
`CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192` (lesson 32), the crash goes away
and boot-time NVS reads work again.

But before diagnosing the stack-in-TCM issue, the practical workaround
is: **skip the boot-time restore, paint the slider at the compile-time
default, and let the slider CHANGED handler pick up the persisted value
the first time the user interacts with it.**

**Fix (post-lesson-32).** Once main task has enough stack, boot-time NVS
restore is safe:

```c
void ptt_actions_init(void) {
    peregrine_voice_init(...);
    nvs_handle_t h;
    uint8_t pct = audio_get_stream_loudness_pct();
    if (nvs_open("prg", NVS_READONLY, &h) == ESP_OK) {
        int32_t v = 0;
        if (nvs_get_i32(h, "loud", &v) == ESP_OK && v >= 0 && v <= 100) {
            pct = (uint8_t)v;
            audio_set_stream_loudness_pct(pct);
        }
        nvs_close(h);
    }
    lv_slider_set_value(objects.slider, pct, LV_ANIM_OFF);
    ...
}
```

**Rule.** When a boot-time NVS read causes a stack-sanity panic, the
right fix is lesson 32 (bump stack), not "skip NVS on boot forever."
Skipping is a stopgap; the stack bump is the real fix.

---

## 39. Peregrine web_chat.py runs under the assistant venv, not system Python

**Symptom.** You extend `peregrine-chat.service` (Peregrine's LAN-facing
web UI) with a new endpoint that imports `faster_whisper` and/or `piper`.
Everything compiles, the service starts, listens on the right port. A
POST to the new endpoint returns HTTP 500. Journal shows:

```
[voice] pipeline error: No module named 'faster_whisper'
```

**Root cause.** The `peregrine-chat.service` unit historically pointed
at `/usr/bin/python3` — because the old web UI was pure stdlib. But
the voice-terminal endpoint imports the same ML libraries that
`voice-assistant.service` uses, which live in the
`/home/trailcurrent/assistant-env` venv (created for `voice-assistant`
and populated with faster-whisper, piper, openWakeWord, etc.). System
Python has none of them.

**Fix.** Change the `ExecStart` in
`config/peregrine-chat.service` to use the venv Python:

```ini
ExecStart=/home/trailcurrent/assistant-env/bin/python3 /home/trailcurrent/web_chat.py
```

`web_chat.py` was stdlib-only before this change; switching to the
venv is purely additive (all stdlib modules remain available).

**Trap.** Adding a new Python source file to Peregrine's `src/` also
requires adding it to `deploy.sh`'s `[2/6]` step — the deploy script
scp's an explicit file list, not `src/*.py`, so newly created modules
(e.g., `stt.py`) silently fail to deploy until you edit the script.
Cross-reference `docs/software-releases.md` for the canonical list of
"software release" files.

---

## 40. SD `environment.conf` is the standard way to inject secrets on this project

**Symptom.** The board needs a bearer token / URL / MQTT credential that
is too long to type comfortably on the touch keyboard (~64 hex chars).
Users get frustrated typing and re-typing on a resistive touchscreen.

**Fix / convention.** Read config from `/sdcard/environment.conf` on
boot, then fall back to on-screen entry if the SD card / file / key is
missing. File format is `KEY=VALUE` per line with `#` comments:

```
# Peregrine voice terminal
PEREGRINE_URL=http://peregrine.local:8081
PEREGRINE_VOICE_TOKEN=8cb78a5aad346241e6d5c5c19d7f2b48058eb602798752951a37bfbbacfe4f57
```

Boot-time reader (see `main/sd_config.c`) mounts SD, opens the file,
parses lines, applies recognized keys, unmounts. Unrecognized keys are
logged and ignored — the same file can carry Headwaters MQTT creds,
tile-server URLs, etc.

**Rules that fell out of implementing this.**

1. **FATFS LFN required** (lesson 24) — without
   `CONFIG_FATFS_LFN_HEAP=y` + `CONFIG_FATFS_MAX_LFN=255` +
   `CONFIG_FATFS_API_ENCODING_UTF_8=y`, `fopen("/sdcard/environment.conf")`
   fails because FATFS reports `ENVIRO~1.CON`.
2. **SD init must match board revision** (lessons 29, 30) — never copy
   from `factory_sourcecode/V1.0/`; the V1.2 pattern from
   `example/V1.2/idf-code/Lesson08-*/` is the reference.
3. **Fall back to on-screen entry, don't hard-require SD** — some boards
   won't have a card. If the SD read produces no keys, the on-screen
   settings-page textareas still work.
4. **Boot-time SD mount + read runs on main task** — same
   stack-sanity concern as lesson 32 applies (though SD driver ops
   don't disable the flash cache the same way NVS does, so this hasn't
   been directly observed).

---

## 41. If the audio reply is long, stream it — don't wait for the whole answer

**The problem.** The first version of the /api/voice endpoint waited for
the language model to write the entire answer, then ran text-to-speech
on the whole thing, then sent one big audio file back to the panel. For
a 5-second answer the user waited ~8 seconds of silence before anything
started playing. For a 30-second answer they waited ~35 seconds. It
felt broken even though it wasn't.

**The fix.** Stream everything: as soon as the language model produces
a complete sentence (or even a clause ending in a comma), synthesize
that sentence to audio and start sending it to the panel. The panel
plays audio as it arrives. Now the user hears the first words within a
second or two of finishing their question, regardless of how long the
full answer will be.

**How.** On the server (`web_chat.py`) the endpoint sends the WAV
header first, then reads the language model's output stream sentence-
by-sentence. Each sentence is fed to Piper, and the resulting audio
bytes are written to the HTTP response as they're produced. On the
panel (`peregrine_voice.c`) the HTTP client just writes each chunk
straight into the I2S output as it arrives.

**Gotcha.** Streaming responses don't have a Content-Length header
(you don't know the size until you're done). Some HTTP clients treat
the socket-close-at-end as "incomplete data" (`ESP_ERR_HTTP_INCOMPLETE_DATA`).
Trust `status=200 + we received body bytes` as success, not the client's
transport-level warning.

---

## 42. A slider that saves to flash on every pixel of drag will corrupt audio playback

**The problem.** The panel had an on-screen slider for tuning the reply
loudness live. Every time the slider fired VALUE_CHANGED (many times
per drag) the handler wrote to NVS flash. Meanwhile the streaming audio
was playing. Result: loud static every time the user touched the slider
during playback, and persistent audio corruption in subsequent replies.

**The cause.** Every `nvs_commit()` briefly disables the SPI-flash
cache to write to flash. If the audio DMA is reading its source buffer
through cache at that moment, it reads garbage. The garbage sits in
the DMA ring and plays as loud static.

**The fix.** Split the slider handler into two events:
- **VALUE_CHANGED** — fires per pixel. Update the audio gain in RAM
  (single uint8_t write) and repaint the numeric label. **No flash.**
- **RELEASED** — fires once when the finger lifts. Commit the final
  value to NVS.

Both events point to the same LVGL action; the handler discriminates
on `lv_event_get_code(e)`. See `action_set_peregrine_loudness` in
`actions.c` (before we removed the slider — pattern still applies to
any live-tuning slider that persists).

**Rule of thumb.** Any slider whose value needs to persist should
write NVS only on release (or a debounced timer), never per-pixel.
"How often does this write to flash?" is a question worth asking every
time a UI control touches persistent storage.

---

## 43. Audio DMA source buffer must be in guaranteed-DMA-safe internal memory

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

**Two related failure modes that were red herrings:**
- Malloc-per-chunk (allocate 4 KB, scale, write, free, repeat 10x/sec)
  fragmented the heap enough that allocations started failing after a
  few seconds → the same "static then silence" symptom. Static buffer
  fixed this. But the underlying DMA/PSRAM issue was separate.
- The panel's amp powering up mid-DMA-underrun could produce a click
  and audible artifacts, mistaken for static.

**Rule of thumb.** For anything that hands memory to a hardware
peripheral (I2S, SPI, camera, etc.) use `heap_caps_malloc` with the
matching CAP flag, not static BSS. Don't trust the linker to put your
buffer somewhere DMA-safe.

---

## 44. Whisper hallucinates coherent-looking phrases on silence — filter them

**The problem.** When the panel captured near-silence (short button
tap, or a mic that wasn't picking up much), Whisper base.en didn't
return an empty string. It returned things like:

- `"You"`
- `"Thanks for watching!"`
- `"..."`, `". . . . . ."`
- `"the"`, `"a"`, `"and"`
- `"Music"`

These are Whisper's known hallucinations — patterns baked into the
model from its training data (a lot of YouTube captions). When the
mic input is too quiet or the clip is too short, the model outputs
one of these instead of "I don't know."

**Downstream damage.** The language model treats "Thanks for watching!"
as a real message and generates a "You're welcome, what would you like
to do next?" response. That takes 25+ seconds on the on-device NPU and
produces a menu the user never asked for. Every accidental button tap
turns into a half-minute of wasted CPU and confusing audio.

**The fix.** In the voice endpoint, check the transcript against a
list of known hallucinations before sending it to the language model.
If it matches, treat it as silence and return a fast "I didn't catch
that." reply.

```python
_WHISPER_HALLUCINATIONS = frozenset({
    "you", "you.", "thanks for watching", "thanks for watching!",
    "thank you.", "thanks.", "music", "music playing",
    ".", "!", "?", "...", "bye", "okay", "ok",
    "uh", "um", "hmm", "the", "a", "and",
})

def _is_whisper_hallucination(text):
    if not text:
        return True
    # Only punctuation/whitespace = classic silence hallucination
    if not any(ch.isalnum() for ch in text):
        return True
    t = text.strip().lower().rstrip(".,!?;:")
    return t in _WHISPER_HALLUCINATIONS
```

See `_is_whisper_hallucination` in `web_chat.py`. Broad-punctuation
check catches `. . . . . . .` variants (Whisper often outputs these on
low-signal mic input).

**Rule of thumb.** Every speech-to-text pipeline that feeds a language
model needs a "did they actually say anything?" gate before spending
NPU/GPU time on the reply.

---

## 45. Bigger Whisper models need a writable model cache

**The problem.** Whisper `base.en` (~74 MB) is fast but often misheard
speech on the CrowPanel's PDM MEMS microphone. Upgrading to `small.en`
(~466 MB) would help. Setting `WHISPER_SIZE=small.en` triggered a
Whisper library download of the model on first use — but the download
failed with `[Errno 30] Read-only file system` because Peregrine's
`/home` (where the HuggingFace cache lives) is mounted read-only in
production.

**The workaround.** Remount `/home` read-write, trigger the download,
then continue. The read-only mount reverts on next reboot but the
downloaded model file persists on disk:

```bash
sudo mount -o remount,rw /home
/home/trailcurrent/assistant-env/bin/python3 -c "
from faster_whisper import WhisperModel
WhisperModel('small.en', device='cpu', compute_type='int8')
"
# Then flip WHISPER_SIZE in the systemd drop-in and restart
```

**Rule of thumb.** When adding a new model to an offline-first product,
plan for the download path. Either: bake the model into the image at
build time, or make the cache directory writable so the runtime can
fetch on demand. Silent failure on a read-only mount is confusing.

---

## 46. PDM microphone `amplify_num > 1` slows the peripheral instead of boosting signal

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

## 47. Getting real timing numbers is worth 10× the code

**The problem.** For a week the voice pipeline felt "slow but I don't
know where." Sometimes 5 seconds, sometimes 30, sometimes 60 (timeout).
Guessing where the slowness lived — WiFi? Whisper? NPU? Piper? Playback?
— produced a lot of wasted fixes.

**The fix.** Add explicit timing markers around each stage of the
pipeline and log them together at the end. The panel's request path
became:

```
[PRG-VOICE] timing (ms): open=41 upload=217 first_body=1773 total=6300
                          wait_llm=1514 play=4526 status=200
```

Suddenly it was obvious:
- `open + upload` low → network handshake and post are fast
- `wait_llm` = 1.5 s → Whisper + LLM are fast when they run
- `play` = 4.5 s → matches the real duration of the reply audio (this
  isn't overhead, it's the reply playing in real time)
- Big `first_body` numbers without corresponding `play` numbers = LLM
  is churning on a bad transcript (silence, hallucination) for too long

The 60-second EAGAINs became visible as "upload finished, first_body
never arrived" — a WiFi transport problem, not a Peregrine slowness
problem. Peregrine's own log confirmed the request completed
server-side but the response bytes never reached the client.

**Rule of thumb.** When a distributed system feels slow, instrument
first, guess second. Five lines of `esp_timer_get_time()` calls
around the stages of a request are cheaper than a day of "let me try
another optimization."

---

## 48. When Peregrine's CPU spikes for a long time on voice, don't add a token cap — stream

**The problem.** First-generation voice endpoint sent Whisper transcript
→ waited for the whole LLM answer → ran TTS on the whole thing →
returned. On a bad transcript (Whisper hallucinating a question) the
LLM would generate a 100-token menu that took 25+ seconds on the NPU.
Peregrine's CPU pegged at 70% for the duration. The panel timed out.

**The wrong fix.** Cap the LLM response at 60 tokens (`num_predict`).
This bounded the wait time but produced obviously-truncated answers.

**The right fix.** Stream the LLM output (lesson 41). The user hears
each sentence as soon as it's generated, and the LLM can take as long
as it wants for a complex answer — it just extends the tail of the
playback. Latency = time-to-first-sentence, not time-to-full-response.

**Rule of thumb.** If a pipeline stage is slow, prefer streaming over
truncating. Truncation destroys quality; streaming just changes when
the user perceives progress.

---

## 49. `esp_task_stack_is_sane_cache_disabled()` panic means main task's stack landed in TCM (or PSRAM)

**The symptom.** Board boots to `main_task: Calling app_main()`, prints
the first log line from app_main, then panics:

```
assert failed: spi_flash_disable_interrupts_caches_and_other_cpu
  cache_utils.c:127 (esp_task_stack_is_sane_cache_disabled())
```

Then boots again. Bootloop.

**What's happening.** The main task's stack got placed in a memory
region that's not accessible when the SPI-flash cache is disabled.
That check fires on every NVS or flash-write operation. `nvs_flash_init`
(usually one of the first things in app_main) trips it and crashes.

**How to diagnose fast.** Read the SP value in the panic dump:
- `SP = 0x4FF…` → stack in internal SRAM. **NOT this bug.** Look
  elsewhere.
- `SP = 0x301…` → stack in TCM (Tightly Coupled Memory, 7 KB region
  on P4). **This bug.**
- `SP = 0x48…` → stack in PSRAM. **This bug.**

**The fix.** In `sdkconfig.defaults`:

```
CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192
```

Doubling the stack size forces IDF to allocate from the big internal
SRAM pool (TCM is only 7 KB and can't hold an 8 KB stack, PSRAM allocs
fall back to internal too). Regenerate `sdkconfig`:

```
rm sdkconfig && idf.py reconfigure
```

**Rule of thumb.** If you add a modest static array to a project and it
suddenly bootloops on `nvs_flash_init`, the added BSS pushed the main
task's stack out of internal SRAM. Bump the stack size, don't shrink
the BSS.

---

## 50. Whisper's small-model + a cheap MEMS mic is the accuracy floor of a voice terminal

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
mode with the Jabra USB microphone works far better than the CrowPanel's
onboard mic (external analog USB mic > MEMS).

**What doesn't work.** Adding an `initial_prompt` with likely words
biases Whisper toward specific vocabulary but doesn't fix "Illinois"
recognition. Boosting mic gain past a certain point creates clipping
that makes recognition worse, not better. Cranking Whisper model size
larger keeps helping but hits a wall around small.en on this mic.

**Practical implications.** This project can't do open-domain voice
dictation well through the onboard mic. Realistic uses of the voice
terminal:
- Push-to-talk on a limited command vocabulary (turn on lights, what's
  the temperature, etc.) where the intent-matcher's regex layer
  handles it before Whisper even sees the audio
- External I²S codec + high-quality mic on the CrowPanel's exposed
  headers as a future hardware revision
- Wake-word + short-utterance mode using a purpose-tuned model

**Not-a-bug.** The pipeline architecture (streaming, DMA-safe audio,
runtime tuning, everything else in lessons 41-49) is sound. The mic
hardware is the limit.

---

## 51. Battery Measurement
What the STC8H1KXX actually is
The STC8H1K series (STCmicro) is a generic 8-bit 8051-derivative microcontroller with a built-in 12-bit ADC. Elecrow bought a cheap general-purpose MCU (~$0.30 in volume, DFN-8 package) and wrote custom firmware on it to:

Drive the TP4059 (a basic linear CC/CV Li-ion charger IC)
Read battery voltage on one of its ADC channels
Publish a byte block on I²C for the host to consume
That's the entire "fuel gauge." No dedicated SOC algorithm hardware. No coulomb counter. No temperature compensation. No learning. Whatever's in that bat_level_pct field is just Elecrow's firmware doing a naive lookup on the terminal-voltage ADC reading — likely a straight linear map from 3.3 V → 0 % to 4.2 V → 100 %. That's why plug/unplug snaps 40 points.

What a real fuel gauge IC does
For reference, purpose-built fuel gauges like the MAX17048 ($1.50), BQ27441 ($1.75), STC3115 ($1.20), or LC709203F ($0.90) do dramatically more inside one chip:

Feature	STC8+TP4059	MAX17048	BQ27441
SOC algorithm	Voltage lookup	ModelGauge m5 (adaptive)	Impedance Track (learning)
Coulomb counter	No	No (OCV+drift model)	Yes
Load compensation	No	Yes	Yes
Temp compensation	No	Yes	Yes
Aging/health tracking	No	Partial	Yes
Battery model learning	No	Yes	Yes
Steady-state accuracy	±10-15 %	±3 %	±1 %
Snap on plug/unplug	40+ pts	~0 pts	~0 pts
The MAX17048 in particular is what most consumer devices use for single-cell Li-ion — it delivers 3% accuracy without a current shunt. The BQ27441 (or its siblings BQ27621, BQ27621) delivers 1% accuracy but needs a shunt.

Why Elecrow chose this
Straightforward cost engineering: an STC8H1K08 + TP4059 combo costs Elecrow ~$0.60 in parts vs. ~$2 for a proper charger + fuel gauge combo. On a board that already carries an ESP32-P4, 32 MB PSRAM, a 10" DSI panel, and a WiFi coprocessor, the battery subsystem is the one place they could shave a dollar. And most customers plug the panel into wall power full-time — the battery is an optional-use nicety, not a must-be-accurate feature.

What you can do about it if you care
Three paths, escalating:

Ship the Tier 1 + 2 work I just did, know that it reads ~5-10% low under load, and call it good. Fine for a display where "roughly how much battery is left" is enough.

Add a MAX17048 (~$0.90-1.50) to the I²C bus in a board rev. Same I²C bus the STC8 already sits on. Different address (0x36). Fireside firmware picks it up automatically — STC8 continues to run the charger, MAX17048 does the SoC reporting. You'd get 3% accuracy with no changes to how you use the pack. This is what I'd do in a v2 respin.

Full BMS chip like the BQ25185 (charger + gauge combined) — single chip that replaces both the TP4059 and the STC8H1K, exposing a proper I²C register set from TI. More work (schematic change, different footprint) but eliminates the vendor firmware entirely and gets to <1% SOC accuracy.

Given the board is already fielded, path 1 is the pragmatic answer. If you're planning a hardware revision anyway, path 2 is a ~$1 BOM add for a very visible quality improvement.

---

## Adding new lessons

Structure each entry as **symptom → root cause → fix** with a heading that
names the topic. Group by area if helpful (build, display, WiFi, GUI).
Cross-reference by number so entries can grow organically without renaming.

If the lesson is a rediscovery of something in the `eezstudio` skill or
similar, note that — future sessions can then decide whether to trust the
skill or the local lesson.
