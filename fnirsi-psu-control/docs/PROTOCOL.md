# FNIRSI DC Power Supply — Serial Protocol

Recovered by decompiling the vendor Windows application (`FNIRSI Power supply.exe`,
.NET Framework 4.7.2 / WinForms, software version V1.0.0.0, released 2023-12-21).
Everything below is transcribed from the recovered `Serial`, `CmdMoudle`, `Command`
and `SerialData` classes.

The device presents itself as a USB CDC virtual COM port.

## Link settings

| Setting | Value |
|---|---|
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| RTS | asserted (`RtsEnable = true`) |
| Read timeout | 500 ms |
| Baud | one of 9600, 19200, 38400, 57600, 115200 |

The vendor app defaults its baud dropdown to **9600**.

> ### ⚠ Hazard: `connect` and `enter bootloader` differ by one bit
>
> ```
> connect      F1 C1 00 01 01 02
> bootloader   F1 C0 00 01 01 02
>                 ^^
> ```
>
> The checksum covers `REG`, `LEN` and the payload — **not** the type byte — so
> both frames carry the identical checksum `0x02`. A single flipped bit in the
> type byte silently turns "hello" into "enter bootloader", and the device has
> no way to detect it.
>
> Because of this, `enterBootloader` is defined in the codec but is **not
> reachable** from the UI or over IPC in this application. Do not re-expose it
> without a very good reason.
>
> If the supply does end up in firmware/BOOT mode, it presents as a USB mass
> storage volume and waits for a firmware file. Nothing is written unless you
> copy one there — power-cycle the unit (remove USB **and** DC input, then
> reconnect) and it returns to normal.

## Frame format

Same layout in both directions; only the start byte differs.

```
+-------+------+------+------+---------------+------+
| START | TYPE | REG  | LEN  | PAYLOAD[LEN]  | CKS  |
+-------+------+------+------+---------------+------+
```

* `START` — **`0xF1` on frames the host sends, `0xF0` on frames the device
  sends.** The vendor software never checked this byte — it walked the receive
  buffer using the length field alone — so the distinction is invisible in the
  decompiled source but is unambiguous on the wire.
* `TYPE` — frame class (see below).
* `REG` — register / command id.
* `LEN` — payload length in bytes.
* `PAYLOAD` — `LEN` bytes, little-endian.
* `CKS` — `(REG + LEN + Σ PAYLOAD) & 0xFF`.

**The checksum does not cover `0xF1` or `TYPE`.** Total frame length is `LEN + 5`.

Multiple frames may arrive in a single serial read. Split them by walking the
buffer and consuming `buffer[i+3] + 5` bytes at a time.

### TYPE values

| TYPE | Direction | Meaning |
|---|---|---|
| `0xC1` | host → device | Session control (connect / disconnect) |
| `0xC0` | host → device | Enter bootloader for firmware upgrade |
| `0xB0` | host → device | Set baud rate |
| `0xA1` | host → device | Read register |
| `0xB1` | host → device | Write register |
| `0xF0` | device → host | Response / unsolicited status (observed; the parser ignores `TYPE` on receive and dispatches purely on `REG`) |

A read request is always `F1 A1 <REG> 01 00 <CKS>` — one zero payload byte,
not a zero-length payload.

## Host → device commands

Taken verbatim from `Command.cs`; byte sequences expanded with the checksum.

| Name | Bytes | Meaning |
|---|---|---|
| CMD_1 | `F1 C1 00 01 01 02` | Connect — hand control to the PC |
| CMD_2 | `F1 C1 00 01 00 01` | Disconnect — return control to the front panel |
| CMD_3 | `F1 A1 FF 01 00 00` | Read **all** state (see full-status blob) |
| CMD_4 | `F1 A1 C0 01 00 C1` | Read input voltage |
| CMD_5 | `F1 A1 DE 01 00 DF` | Read product model string |
| CMD_6 | `F1 A1 E0 01 00 E1` | Read firmware version string |
| CMD_7 | `F1 A1 E1 01 00 E2` | Read device address (**used as the connect probe**) |
| CMD_8 | `F1 B1 DB 01 00 DC` | Output OFF |
| CMD_9 | `F1 B1 DB 01 01 DD` | Output ON |
| CMD_10 | `F1 B1 D8 01 01 DA` | Metering (Ah/Wh accumulation) ON |
| CMD_11 | `F1 B1 D8 01 00 D9` | Metering OFF |
| CMD_12 | `F1 A1 D6 01 00 D7` | Read brightness |
| CMD_13 | `F1 B0 00 01 <n> <n+1>` | Set baud rate, `n` = 1..5 → 9600/19200/38400/57600/115200 |
| CMD_14 | `F1 A1 C3 01 00 C4` | Read output V / I / P |
| CMD_15 | `F1 C0 00 01 01 02` | **Enter bootloader** (firmware upgrade) |
| CMD_16 | `F1 A1 DF 01 00 E0` | Read hardware/extra string (`0xDF`) |
| CMD_17 | `F1 A1 C1 01 00 C2` | Read voltage setpoint |
| CMD_18 | `F1 A1 C2 01 00 C3` | Read current setpoint |

### Writable registers (`TYPE = 0xB1`)

| REG | Payload | Meaning |
|---|---|---|
| `0xC1` (193) | float32 LE | Voltage setpoint (V) |
| `0xC2` (194) | float32 LE | Current setpoint (A) |
| `0xC5` / `0xC6` | float32 LE | Memory group M1 — voltage / current |
| `0xC7` / `0xC8` | float32 LE | Memory group M2 — voltage / current |
| `0xC9` / `0xCA` | float32 LE | Memory group M3 — voltage / current |
| `0xCB` / `0xCC` | float32 LE | Memory group M4 — voltage / current |
| `0xCD` / `0xCE` | float32 LE | Memory group M5 — voltage / current |
| `0xCF` / `0xD0` | float32 LE | Memory group M6 — voltage / current |
| `0xD1` (209) | float32 LE | OVP threshold (V) |
| `0xD2` (210) | float32 LE | OCP threshold (A) |
| `0xD3` (211) | float32 LE | OPP threshold (W) |
| `0xD4` (212) | float32 LE | OTP threshold (°C) |
| `0xD5` (213) | float32 LE | LVP threshold (V) |
| `0xD6` (214) | uint8 | Display brightness, 1..14 |
| `0xD8` (216) | uint8 | Metering enable, 0/1 |
| `0xDB` (219) | uint8 | Output enable, 0/1 |

The memory-group register is computed as `0xC3 + 2 * id + (isVoltage ? 0 : 1)`
for `id` in 1..6.

Note the OTP threshold is edited as an integer in the UI but is still
transmitted as a float32.

## Device → host responses

Dispatched on `REG` alone.

| REG | Payload | Meaning |
|---|---|---|
| `0xC0` (192) | float32 | Input voltage (V) |
| `0xC3` (195) | 3 × float32 | Output voltage (V), output current (A), output power (W) |
| `0xC4` (196) | float32 | System temperature (°C) |
| `0xD9` (217) | float32 | Accumulated charge (Ah) |
| `0xDA` (218) | float32 | Accumulated energy (Wh) |
| `0xDB` (219) | uint8 | Output state — 1 = on, 0 = off |
| `0xDC` (220) | uint8 | Protection status (see table) |
| `0xDD` (221) | uint8 | Regulation mode — **1 = CV, 0 = CC** |
| `0xDE` (222) | ASCII | Product model |
| `0xDF` (223) | ASCII | Hardware/extra string (read but unused by the vendor UI) |
| `0xE0` (224) | ASCII | Firmware version |
| `0xE1` (225) | uint8 | Device address |
| `0xE2` (226) | float32 | Maximum settable voltage |
| `0xE3` (227) | float32 | Maximum settable current |
| `0xFF` (255) | 140 bytes | Full status blob |

### Protection status codes (`0xDC`, and byte 108 of the blob)

| Code | Meaning |
|---|---|
| 0 | Normal |
| 1 | OVP — over-voltage |
| 2 | OCP — over-current |
| 3 | OPP — over-power |
| 4 | OTP — over-temperature |
| 5 | LVP — under-voltage |

Any non-zero code means the device has **already disconnected its output**.

### Full-status blob (`REG = 0xFF`)

Payload offsets, all little-endian:

| Offset | Type | Field |
|---|---|---|
| 0 | float32 | Input voltage (V) |
| 4 | float32 | Voltage setpoint (V) |
| 8 | float32 | Current setpoint (A) |
| 12 | float32 | Output voltage (V) |
| 16 | float32 | Output current (A) |
| 20 | float32 | Output power (W) |
| 24 | float32 | System temperature (°C) |
| 28, 32 | float32 | M1 voltage, M1 current |
| 36, 40 | float32 | M2 voltage, M2 current |
| 44, 48 | float32 | M3 voltage, M3 current |
| 52, 56 | float32 | M4 voltage, M4 current |
| 60, 64 | float32 | M5 voltage, M5 current |
| 68, 72 | float32 | M6 voltage, M6 current |
| 76 | float32 | OVP threshold (V) |
| 80 | float32 | OCP threshold (A) |
| 84 | float32 | OPP threshold (W) |
| 88 | float32 | OTP threshold (°C) |
| 92 | float32 | LVP threshold (V) |
| 96 | uint8 | Display brightness |
| 97 | uint8 | *(read by the vendor app into an unused field)* |
| 98 | uint8 | Metering enabled |
| 99 | float32 | Accumulated charge (Ah) |
| 103 | float32 | Accumulated energy (Wh) |
| 107 | uint8 | Output enabled |
| 108 | uint8 | Protection status |
| 109 | uint8 | Regulation mode (1 = CV, 0 = CC) |
| 110 | uint8 | *(not read by the vendor app)* |
| 111 | float32 | Maximum settable voltage |
| 115 | float32 | Maximum settable current |
| 119 | float32 | OVP maximum |
| 123 | float32 | OCP maximum |
| 127 | float32 | OPP maximum |
| 131 | float32 | OTP maximum |
| 135 | float32 | LVP maximum |

The vendor app reads through offset 138, so the payload is at least 139 bytes.

## Connection handshake

Exactly as the vendor app performs it:

1. Open the port at the chosen baud.
2. Send `CMD_1` (connect).
3. Up to **two** times, one second apart, send `CMD_7` (read device address).
4. When a `0xE1` response arrives whose address byte equals the configured
   device address (vendor default **1**, range 1..255), the link is confirmed.
5. On confirmation, send in order: `CMD_13` (baud), `CMD_5` (model),
   `CMD_6` (firmware), `CMD_16` (`0xDF`), `CMD_3` (read all).
6. If no matching address arrives within two attempts, close and report
   "Connection failed".

On disconnect the app sends `CMD_2` before closing the port, handing control
back to the front panel.

## Timing — verified on hardware

Two behaviours that are not visible in the decompiled source but matter a great
deal in practice. Both were confirmed against a DPS-150 running firmware V1.2.

**The supply drops back-to-back commands.** It is a small MCU that is already
busy streaming measurements. Frames sent with no gap are silently discarded —
and there is no error, no NAK, nothing: the command simply does not happen.
Leave at least ~120 ms between *every* outgoing frame.

This bites hardest on the "turn the output on" sequence, which is three frames
(`0xC1` set voltage, `0xC2` set current, `0xDB` enable). Sent as a burst, only
the voltage write lands: the output stays off, and a client that trusts its own
command will report the output as on when it is not. Measured on hardware —
with pacing the output comes up at the requested voltage; without it, nothing
happens at all.

**The set-baud-rate command deafens it for a few hundred milliseconds.** After
`CMD_13` the supply reinitialises its serial handling and ignores everything
that arrives for roughly half a second.

Together these explain a long-standing vendor-software annoyance. Its connect
routine sends

```
CMD_13 (baud), CMD_5 (model), CMD_6 (firmware), CMD_16 (0xDF), CMD_3 (read all)
```

back-to-back, so the four commands after the baud command land inside the dead
window and are lost — which is why Product Model and Firmware Version so often
stay blank. Waiting ~600 ms after `CMD_13`, then issuing the reads spaced out
and retrying any that do not answer, populates them reliably.

## Device address

Register `0xE1`, one byte. The supply's front panel shows it as three digits
and the usable range is **1–255**, matching the single wire byte. The vendor
software required the user to type the matching address before it would
connect, and reported only "Connection failed" on a mismatch — with no hint of
what the device actually wanted.

Since the supply reports its address in the probe response regardless, a client
can simply accept whatever comes back, or at minimum name the observed value in
the error.

## Notes and quirks

* The device streams `0xC3`, `0xC4`, `0xDB`, `0xDC`, `0xDD`, `0xD9`, `0xDA`
  updates on its own once connected — the vendor app never polls on a timer.
  It only issues `CMD_3` on connect and after saving a memory group.
* `0xDE` / `0xE0` strings are decoded with `Encoding.Default` (the Windows ANSI
  code page). In practice these are plain ASCII.
* Setting voltage and current are two separate writes; the vendor app sends
  voltage first, then current.
* Turning the output on also re-sends the current V and I setpoints.
* Vendor defaults before the device reports its real limits: 24 V / 5 A. The
  real limits arrive at blob offsets 111/115 or via `0xE2` / `0xE3`.
