# ESP32-S3 Industrial RS485/CAN — Antenna A/B Test

An ESP-IDF firmware that quantifies the difference between the **onboard patch
ceramic antenna** and an **external SMA antenna on the IPEX connector** of the
Waveshare **ESP32-S3-RS485-CAN** industrial control board.

It runs a repeatable measurement campaign, stores each run in a dedicated NVS
partition, and prints a side-by-side comparison with the dB delta.

---

## Read this first: the two antennas are not switchable in software

The original goal for this project was to verify that either antenna could be
used **without any physical changes to the board**. That turns out not to be
possible on this hardware, and it is worth stating plainly before you wire
anything up.

The ESP32-S3's single RF pin feeds a **0 Ω link** near the IPEX socket. That link
selects *either* the patch ceramic antenna *or* the IPEX Gen1 connector — never
both, and never under software control. There is no RF switch, no GPIO, and no
"plugging in the pigtail disconnects the patch" switched connector (that
behaviour exists on some switched U.FL parts, but this board does not use one).

Waveshare's own documentation says so in two places:

> ⑧. IPEX Gen 1 connector — *Switch to the external antenna via resoldering an
> onboard resistor*

> **Q: How to use the onboard external antenna?**
> **A:** Change the resistor in the below image from vertical soldering
> (default) to horizontal soldering.

— [ESP32-S3-RS485-CAN — Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-RS485-CAN)

Waveshare also sells the board as two distinct SKUs — `ESP32-S3-RS485-CAN`
(patch antenna, link fitted vertically) and `ESP32-S3-RS485-CAN-U` (external
antenna, link fitted horizontally, ships with a 2.4 GHz 4 dBi SMA antenna).
Same PCB, different factory link position.

**What this means for the test:** the two configurations cannot be compared
within one power cycle. The firmware is built around that constraint — each run
is persisted to flash so it survives the power-down, rework, and reboot needed
to get to the other configuration. See [Test procedure](#test-procedure).

**If you leave the link in its factory position and just screw an antenna onto
the SMA pigtail, you are measuring the patch antenna with a disconnected cable
dangling off the board.** The numbers will look plausible. They will be wrong.
The `report` command's sanity checks will not catch this for you — only the
soldering iron will.

### Where the link is, and what it looks like

Open the DIN-rail enclosure. On the antenna end of the PCB you have three
landmarks in a row:

```
   [ red ceramic ]   [ 0R link ]   [ gold IPEX ]
      patch ant.      <-- here      socket
   ------------- ESP32-S3 shield can above -------------
```

The link sits in the narrow gap **between** the red-brown ceramic patch antenna
(item ⑦ on the wiki's board photo) and the gold IPEX Gen1 socket (item ⑧),
tucked right under the bottom edge of the ESP32-S3 module's shield can. It is a
**0402 part, roughly 1 mm long** — you will want a loupe or a phone macro lens.
Waveshare's FAQ image marks it with a red box.

### "Can I just add the external antenna without moving the link?"

No — and it is worse than a no-op.

With the link in the factory position, the IPEX centre pin terminates on a
**dead stub trace** at an unpopulated pad. It is not connected to the
transmitter. The only coupling across that open 0402 gap is a few hundredths of
a picofarad, which at 2.4 GHz is thousands of ohms against a 50 Ω system — the
leaked signal is orders of magnitude below the direct path. None of the external
antenna's gain is available to you.

But the antenna is still physically there: a resonant 2.4 GHz radiator a few
centimetres from the patch, tied to it through a coax stub. That makes it a
**parasitic element** — it absorbs, re-radiates and detunes the patch antenna,
in a direction that is not predictable and usually not helpful. Measured
degradation of several dB versus the bare patch is entirely normal.

So a run done this way does not compare two antennas. It compares one antenna
against itself with something spoiling it.

**Confirming which position your link is in, without a microscope:**

```
antenna> scan     # note the mean RSSI, with the antenna attached
                  # unscrew the SMA antenna / unplug the pigtail from IPEX
antenna> scan     # note it again
```

- Numbers barely move, or improve → the RF feed is on the **patch**; the link is
  in the factory position.
- Numbers collapse by 10–20 dB → the RF feed is on the **IPEX**; the link has
  been moved.

**Never bridge both positions with solder.** It splits the RF feed into two
mismatched loads, wrecks the VSWR, and performs worse than either antenna alone.
It is the tempting shortcut when the part is too small to move cleanly.

If you do move it, use hot air rather than an iron: flux, ~250–280 °C, tweezers.

### The realistic option: buy the right SKU

For most people, reworking a 1 mm 0402 in an RF path is not a reasonable ask.
Waveshare's own answer is to sell the board pre-configured — the `-U` variant is
the same PCB with the link already in the external position, shipped with the
SMA antenna. **There is no field-configurable path**, and the SMA connector on a
stock board is not a feature you can opt into.

---

## What it measures

Each run is four stages, in this order:

| Stage | What it does | Why |
|---|---|---|
| 1. AP census | Full active scan, all channels, disconnected | Count of distinct BSSIDs heard and their mean RSSI is an antenna-sensitivity proxy that does not depend on any single access point behaving itself |
| 2. Associate | Connect to the reference AP, time to DHCP lease | A weak path shows up as retried association long before it shows up as a failed one |
| 3. RSSI series | N samples of the connected AP's RSSI | The headline number — mean, standard deviation, median, 10th percentile, min/max |
| 4. ICMP echo | N pings to the gateway (or a configured host) | RSSI is **receive-side only**. A broken transmit path — a badly-terminated pigtail, say — shows up here as loss and RTT, and nowhere else |

Also recorded per run: BSSID, channel, and max TX power, so the report can warn
you if the two runs are not actually comparable.

### Why the paired per-BSSID census exists

Comparing the *mean* RSSI of two scans is only valid if both scans heard the
same set of APs — and they usually don't. The weak tail of a scan drops in and
out between consecutive scans regardless of which antenna is fitted, and losing
a couple of −85 dBm neighbours drags the mean several dB.

So each run also stores the **strongest 16 APs by BSSID**. The report matches
them across the two runs and averages the per-AP differences:

```
  --- paired per-BSSID census (same AP measured in both runs) ---
    a4:2b:b0:1c:44:e1   -33 ->  -35 dBm  -2
    e8:9f:80:2a:11:03   -58 ->  -61 dBm  -3
    ...
    11 AP(s) matched, mean paired delta -2.64 dB
```

Every term in that average is the same radio measured twice, so AP-set churn
cancels out entirely. **Trust this over `Scan mean RSSI`.** If the two disagree
badly, the unpaired mean is the one that's lying.

### Why the 10th percentile and standard deviation are in there

A single mean RSSI figure hides the thing that actually breaks industrial
deployments: the depth of the fades. Two antennas can post the same mean and
behave completely differently — one holding steady at −67 dBm, the other
oscillating between −55 and −80 as someone walks past. The `p10` column is the
deep-fade tail, and the standard deviation tells you whether the difference
between your two runs is a real gain or just the noise you measured it in.

The report calls this out explicitly:

```
NOTE: that gap (0.84 dB) is smaller than the mean per-run
      standard deviation (2.31 dB). Treat it as noise, not
      as a difference -- take longer runs or move further
      from the AP to open up the gap.
```

---

## Hardware

| | |
|---|---|
| Board | Waveshare ESP32-S3-RS485-CAN (ESP32-S3R8, 16 MB flash, 8 MB octal PSRAM) |
| Antenna A | Onboard patch ceramic antenna (factory default) |
| Antenna B | 2.4 GHz SMA antenna via IPEX Gen1 → SMA pigtail |
| Console | USB Type-C |
| Power | USB Type-C, or the 7–36 V screw terminal |

Nothing on the RS485 or CAN side is used — this is purely a radio test — so the
screw terminals can be left unwired.

---

## Build and flash

```bash
. ~/esp/v5.5.2/esp-idf/export.sh

idf.py set-target esp32s3
idf.py menuconfig          # → "Antenna A/B Test Configuration"
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

Set at minimum the **Reference AP SSID** and **password** under
*Antenna A/B Test Configuration*. Defaults for everything else give a 60-second
RSSI run and 50 pings, which is a reasonable starting point.

### Console transport

`sdkconfig.defaults` selects **USB Serial/JTAG**, which is correct if the Type-C
port goes to the ESP32-S3's native USB peripheral — the port then enumerates as
`/dev/ttyACM*`. If your unit instead enumerates as `/dev/ttyUSB*`, it has a
USB-UART bridge; switch the console over:

```
idf.py menuconfig
  → Component config → ESP System Settings → Channel for console output
  → Default: UART0
```

The firmware picks the matching REPL backend at compile time, so no code change
is needed.

---

## Test procedure

The comparison is only as good as the discipline around it. RSSI moves several
dB if you rotate the board, and a person standing between board and AP is worth
about 3–5 dB on its own.

**Before you start**

1. Put the board somewhere it can stay **completely undisturbed** for both runs
   — same position, same orientation, same height, same cable dress. Tape it
   down. You will be soldering on it between runs, so plan a way to lift it out
   and put it back in exactly the same place, or do the rework in situ.
2. Pick a reference AP that is **not** in the same room. At two metres from the
   AP everything works and both antennas will read within a decibel of each
   other. Somewhere around −65 to −80 dBm is where the difference becomes
   legible.
3. Lock the AP to a **fixed channel** if you can. Band steering or auto-channel
   between runs invalidates the comparison — the report will warn you, but only
   after you have wasted the run.
4. Keep people and moving metal out of the path for the duration.

**Run A — onboard patch antenna (factory)**

```
antenna> measure onboard
```

Roughly 75 seconds at defaults. Leave the room if you can.

**Rework**

1. Power the board down completely.
2. Rotate the 0 Ω link next to the IPEX socket from vertical to horizontal
   (see the Waveshare wiki FAQ image).
3. Fit the IPEX → SMA pigtail and screw on the antenna.
4. Put the board back exactly where it was. Power up.

The stored slot A result survives this — it is in the `results` NVS partition,
which `idf.py flash` does not touch. (`idf.py erase-flash` *does*.)

**Run B — external SMA antenna**

```
antenna> measure external
antenna> report
```

---

## Commands

| Command | Description |
|---|---|
| `measure onboard` | Full campaign, stored in slot A |
| `measure external` | Full campaign, stored in slot B |
| `report` | Both slots side by side, with deltas and sanity warnings |
| `show <slot>` | One stored run in full |
| `scan` | One-off AP census — quick check that the RF path is alive at all |
| `monitor -t 30 -i 500` | Live RSSI stream, one line per sample |
| `clear [slot\|all]` | Erase stored results |
| `help` | Full command list |

`onboard` also accepts `a`, `internal`, `ceramic`; `external` also accepts `b`,
`sma`, `ipex`.

### `monitor` is the one to use on camera

`measure`/`report` is the rigorous path. `monitor` is the one that reads well in
a video — stream live RSSI and walk away from the AP, and the difference between
a good and a bad antenna path is visible in real time as the numbers diverge.

---

## Example output

```
=== A/B comparison ===
                          onboard   external      delta
  ---------------------- ---------- ---------- ----------
  RSSI mean                  -71.34     -64.08      +7.26 dB
  RSSI median                   -71        -64         +7 dB
  RSSI 10th pct                 -76        -67         +9 dB
  RSSI std deviation           3.11       1.94      -1.17 dB
  Scan mean RSSI             -78.92     -73.55      +5.37 dB
  APs heard                      11         17         +6
  Associate time               1830       1204       -626 ms
  ICMP replies                   47         50         +3
  ICMP RTT avg                    9          4         -5 ms

  External antenna is +7.26 dB relative to the onboard patch.
```

Interpretation, in order of how much you should trust it:

- **RSSI mean / median delta** — the headline. Positive means the external
  antenna hears the AP better. Check it against the standard deviation before
  believing a small number.
- **APs heard** — a blunt but honest sensitivity measure. Hearing six more
  access points is hard to argue with.
- **RSSI std deviation going *down*** — often the more valuable result than raw
  gain. A steadier link is what you want on a DIN rail in a cabinet.
- **ICMP loss / RTT** — the only figures here that reflect the *transmit* path.

---

## Project layout

| File | Role |
|---|---|
| [main/main.c](main/main.c) | Init, banner, console REPL bring-up |
| [main/wifi_link.c](main/wifi_link.c) | esp_wifi wrapper — scan, connect, RSSI, gateway |
| [main/rf_bench.c](main/rf_bench.c) | The measurement campaign, statistics, ICMP probe |
| [main/results.c](main/results.c) | NVS slot storage and the comparison report |
| [main/console_cmds.c](main/console_cmds.c) | Console command registration |
| [main/Kconfig.projbuild](main/Kconfig.projbuild) | menuconfig options |
| [partitions.csv](partitions.csv) | Adds a dedicated 16 KB `results` NVS partition |

### Configuration knobs

All under *Antenna A/B Test Configuration* in `menuconfig`:

| Option | Default | Notes |
|---|---|---|
| Reference AP SSID / password | — | Must be set |
| Association timeout | 15000 ms | A failed connect is recorded, not fatal |
| RSSI samples per run | 300 | 300 × 200 ms = 60 s |
| RSSI sample interval | 200 ms | Below the ~100 ms beacon interval you just duplicate readings |
| ICMP echo count / interval | 50 / 200 ms | 0 disables the ping stage |
| ICMP target | *(blank)* | Blank pings the DHCP gateway, keeping the test on the local RF link rather than your internet uplink |
| Max APs per scan | 48 | Census upper bound |

---

## Notes on measurement validity

- **Wi-Fi power save is disabled** (`esp_wifi_set_ps(WIFI_PS_NONE)`). Modem
  sleep parks the radio between beacons and injects variance into the RSSI
  series that has nothing to do with the antenna.
- **RSSI is quantised to 1 dBm** by the ESP32-S3, which is why the report leans
  on the mean over hundreds of samples rather than any single reading.
- **The scan is taken while disconnected** so it is not competing with an active
  association for airtime.
- **`report` refuses to draw conclusions across mismatched runs** — it warns if
  the two runs used different BSSIDs, different channels, or different max TX
  power.
- **Antenna gain figures on datasheets are peak, not average.** A 4 dBi
  omnidirectional whip beats a patch ceramic in the horizontal plane and can
  lose to it straight overhead. If your AP is on the floor above, orient the
  external antenna accordingly and expect the result to change.

## Reference

- [ESP32-S3-RS485-CAN — Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-RS485-CAN)
- [Industrial ESP32-S3 Control Board product page](https://www.waveshare.com/esp32-s3-rs485-can.htm)
- [ESP-IDF Wi-Fi driver API](https://docs.espressif.com/projects/esp-idf/en/v5.5.2/esp32s3/api-reference/network/esp_wifi.html)
