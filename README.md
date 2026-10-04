# TrailCurrent YouTube Code Samples

Code, firmware and CAD from TrailCurrent YouTube videos. Each folder is a
separate project with its own README covering what it does, what hardware it
needs, and how to build and run it.

These are demonstration projects. They are meant to be read, built and
adapted, not dropped into a product unchanged.

## Samples

| Sample | Runs on | What it is |
|---|---|---|
| **Desktop & Linux** | | |
| [TrailShell](CustomLinuxDesktop/README.md) | Waveshare PocketTerm35 (Raspberry Pi 5) | A touch-first desktop shell for a 3.5" 640×480 screen with a built-in keyboard: status bar, swipe-down shade, launcher and task switcher. |
| [LinuxTouch](LinuxTouch/README.md) | Waveshare PocketTerm35 (Raspberry Pi 5) | A phone-style shell on top of full Raspberry Pi OS, with a home screen, one fullscreen app at a time and a card switcher. *In development.* |
| [FNIRSI PSU Control](fnirsi-psu-control/README.md) | Linux, Windows, macOS | An unofficial cross-platform desktop app for FNIRSI DC bench power supplies (tested on a DPS-150), built by reverse-engineering the Windows-only vendor app. *Beta.* |
| [Genie NPU installer](install-genie-npu.sh) | Radxa Dragon Q6A | A one-file script that runs Llama 3.2 1B on the Qualcomm NPU and serves it as an Ollama-compatible API. See [below](#genie-npu-installer). |
| **ESP32 displays** | | |
| [Off-Grid Power Dashboard](eez-studio-update-lvlg-9/README.md) | Waveshare ESP32-P4 7" touch | A battery, solar and tank dashboard showing the new features in EEZ Studio 0.29 and LVGL 9. It also runs in EEZ Studio's simulator. |
| [Headwaters Map Viewer](elecrow-esp32-p4-10-in/README.md) | Elecrow CrowPanel Advance 10.1" ESP32-P4 | Walks through Wi-Fi setup and token sign-in, then shows map tiles from a [TrailCurrent Headwaters](https://github.com/trailcurrentoss/TrailCurrentHeadwaters) gateway. |
| **Rotary macro pads** | | |
| [Rotary Macro Pad, 2.1"](makerfab-2_1-rotary-encoder-esp32s3/README.md) | Makerfabs MaTouch ESP32-S3 Rotary 2.1" | A USB keyboard with a round touchscreen in a rotary ring. Its keys change to suit the app you're using: FreeCAD, Blender, Kdenlive, VS Code, GIMP, Inkscape, LibreOffice and browsers. |
| [Rotary Macro Pad, 1.28"](elecrow-1_28-rotary-encoder-esp32s3/README.md) | Elecrow CrowPanel 1.28" Rotary (ESP32-S3) | The same macro pad, redrawn for a 240×240 screen. |
| [Rotary Macro Pad, 1.46"](elecrow-1_46-rotary-encoder-esp32s3/README.md) | Elecrow CrowPanel 1.46" Rotary (ESP32-S3) | The same macro pad, redrawn for a 360×360 screen. |
| **ESP32 boards** | | |
| [ESP32-P4-WIFI6 Demo](Esp32P4Demo/README.md) | Waveshare ESP32-P4-WIFI6 | Wi-Fi setup from a phone, a web dashboard, microSD file management, microphone and speaker, a camera, and push-to-talk to the Peregrine voice assistant. |
| [Antenna A/B Test](Esp32IndustrialSmaTest/README.md) | Waveshare ESP32-S3-RS485-CAN | Measures the onboard ceramic antenna against an external SMA antenna and reports the difference in dB. |
| **CAD** | | |
| [Torsion Table CAD](TorsionTableCAD/README.md) | FreeCAD + LowRider 4 CNC | CAM jobs for cutting the plywood ribs of a torsion box table. |

The three macro pads share one companion program, which runs on your computer
and tells the pad which app has focus. The pads identify as the same USB
device, so one install works with any of them.

## Getting the code

```bash
git clone https://github.com/trailcurrentoss/YouTubeCodeSamples.git
cd YouTubeCodeSamples
```

FreeCAD files (`*.FCStd`) are stored with [Git LFS](https://git-lfs.com/).
If you want the CAD, install Git LFS first, or run `git lfs pull` after cloning.

Most samples need a specific toolchain, for example a particular ESP-IDF
version. The sample's README says which. Start there rather than assuming the
same setup works for every folder.

## Genie NPU installer

[`install-genie-npu.sh`](install-genie-npu.sh) sets up a local AI chat server
on a **Radxa Dragon Q6A**. It runs on the board's Qualcomm Hexagon NPU instead
of the CPU, at roughly 12 tokens per second.

Start from a stock Radxa OS R2 (or newer) image. Log in as the `radxa` user
(not root) and run:

```bash
bash install-genie-npu.sh
```

It needs internet access and will ask for your `sudo` password. The script:

1. Installs `btop` (to watch CPU load) and `jq` (to read the streamed replies).
2. Downloads the NPU build of Llama 3.2 1B, about 1.3 GB.
3. Changes two settings so the server sits near 0% CPU when idle instead of
   keeping three cores busy.
4. Installs `genie_server.py` from
   [TrailCurrent Peregrine](https://github.com/trailcurrentoss/TrailCurrentPeregrine)
   as a systemd service that starts on every boot.
5. Waits up to two minutes for the model to warm up.

When it finishes, the server answers on `http://localhost:11434` using the
same API as Ollama, so tools that talk to Ollama can use it. The script prints
a `curl` command you can paste to try it.

To use a longer context window, set `CTX_LENGTH` first, for example
`CTX_LENGTH=2048 bash install-genie-npu.sh`. Only context lengths that Radxa
publishes a model bundle for will work.

## License

MIT. See [LICENSE](LICENSE).
