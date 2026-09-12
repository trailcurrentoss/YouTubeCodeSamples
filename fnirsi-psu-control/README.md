# FNIRSI PSU Control

> # ⚠ BETA: USE AT YOUR OWN RISK
>
> **This software is not supported by, affiliated with, or endorsed by FNIRSI.**
>
> It is an independent, unofficial example built by reverse-engineering the
> vendor's Windows application, created so that the work can be shared back
> with FNIRSI in case they wish to release something official along these
> lines. It is beta software that controls a mains-powered device capable of
> delivering real current into real hardware.
>
> There is no warranty of any kind. You are responsible for anything you
> connect to the supply and anything that happens to it.

A cross-platform desktop application for FNIRSI DC bench power supplies,
written in Electron so it runs on Linux, Windows and macOS. The original
vendor software is Windows-only.

Developed and verified against a **DPS-150** (hardware V1.0, firmware V1.2).

---

## Status

| Area | State |
|---|---|
| Protocol codec | Complete, 30 unit tests, validated against live hardware captures |
| Connect / disconnect | Verified on hardware, with device-address auto-detect |
| Live measurements | Verified — V, I, P, input voltage, temperature, CV/CC, protection |
| Setpoints, output on/off | **Verified on hardware** — output on/off and live setpoint changes |
| Memory groups M1–M6 | Implemented — recall and store |
| Metering (Ah/Wh), brightness | Implemented |
| Protection limits | Implemented — OVP, OCP, OPP, OTP, LVP |
| Program (list) mode | Implemented — auto and manual, not yet exercised under load |
| Voltage / current sweeps | Implemented, not yet exercised under load |
| Recording, chart, CSV export | Implemented |
| Firmware update | Implemented as a deliberately gated multi-step flow |
| Localisation | 7 languages ported from the vendor software |

Everything marked "implemented" but not "verified" has been written against a
protocol that *is* hardware-verified, but the feature itself has not yet been
run end-to-end on a bench. Treat those as beta within a beta.

### Controlling the output

The output is toggled from either of two places, which always agree: the
**OUTPUT** button in the header bar, and the switch beside the setpoints on the
Basic page. Setpoints can be changed at any time, with the output on or off;
turning the output on re-sends both values first.

## Requirements

- Node.js 18 or newer
- A FNIRSI supply on a USB CDC serial port
- Linux: your user must be able to read the port. On most distributions that
  means membership of the `dialout` group:
  ```sh
  sudo usermod -aG dialout "$USER"   # log out and back in afterwards
  ```

## Running

```sh
npm install
npm start
```

## Tests

```sh
npm test
```

The protocol suite runs without any hardware attached. It pins the command
encoding byte-for-byte against the vendor software's own constants and decodes
real frames captured from a DPS-150.

## Packaging

```sh
npm run dist:linux     # AppImage + deb
npm run dist:win       # NSIS installer
npm run dist:mac       # dmg
```

Cross-compiling is not supported for the native serial module — build each
target on its own platform, or in a matching container.

## Connecting

1. Pick the serial port. On Linux the supply appears as `/dev/ttyACM*` and
   usually reports its manufacturer as *Artery*.
2. Leave **Auto-detect** ticked for the device address, or untick it and enter
   the address shown on the supply's own display (1–255).
3. Press **Connect**.

If the address does not match, the app tells you which address the supply
actually reported rather than just saying "connection failed".

While connected the supply hands control to the application. Disconnecting
returns control to the front panel.

## Firmware update

The firmware flow is deliberately staged, and the bootloader command is gated
in the main process on a file having been chosen — it cannot be reached by a
stray click:

1. **Choose file…** — select the firmware image.
2. **Update Firmware** — asks for confirmation, then puts the supply into
   update mode, where it disconnects and appears as a USB drive.
3. **Copy to…** — you pick the drive; the app copies the image there. It never
   guesses at a mount point.
4. Power-cycle the supply.

Nothing is written to the supply until step 3. If you enter update mode by
mistake, just power-cycle it — it returns to normal operation.

> **Why the gating matters.** In this protocol the "connect" and "enter
> bootloader" frames differ by a single bit, and the checksum does not cover
> the byte that differs — both frames carry the identical checksum:
>
> ```
> connect      F1 C1 00 01 01 02
> bootloader   F1 C0 00 01 01 02
> ```
>
> See [docs/PROTOCOL.md](docs/PROTOCOL.md).

## Layout

```
src/shared/protocol.js   Pure protocol codec — no I/O, fully unit tested
src/shared/i18n.js       UI strings in 7 languages
src/main/device.js       Serial port ownership, handshake, framing, pacing
src/main/main.js         Electron main process and IPC surface
src/main/preload.js      contextBridge API exposed to the renderer
src/renderer/            UI (plain HTML/CSS/JS, no framework)
docs/PROTOCOL.md         The protocol, as recovered and verified
test/protocol.test.js    Protocol tests, including live hardware captures
```

The renderer runs with `contextIsolation` on, `nodeIntegration` off and a
restrictive CSP. It has no Node access and cannot touch the serial port
directly; every device command is name-checked in the main process.

## How this was built

The vendor application is a .NET Framework 4.7.2 WinForms binary. It was
decompiled with [ILSpy](https://github.com/icsharpcode/ILSpy), which recovers
near-original C#. The protocol was transcribed from the recovered `Serial`,
`CmdMoudle`, `Command` and `SerialData` classes, then checked against real
traffic — which is how the two discrepancies below came to light.

**Things the vendor source gets away with but does not document:**

- Device replies start with `0xF0`, not the `0xF1` the host sends. The vendor
  parser never checks the start byte at all — it walks the buffer using only
  the length field — so the difference is invisible in the source.
- The supply drops commands sent back-to-back, and goes deaf for a few hundred
  milliseconds after the set-baud command. The vendor connect routine sends its
  five init commands in a burst and loses the four after the baud command,
  which is why its Product Model and Firmware Version fields are so often
  blank. This app waits, spaces the reads out, and retries.

No artwork, icons or other assets from the vendor application are reused. The
UI here is original CSS. The translated UI strings were carried over so the
application remains usable in the same seven languages.

## Licence

MIT. See the beta warning at the top: this is unofficial software and comes
with no warranty.
