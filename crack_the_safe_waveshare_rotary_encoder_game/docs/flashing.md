# Flashing both chips

The Waveshare ESP32-S3-Knob-Touch-LCD-1.8 has **two microcontrollers**, and
Crack the Safe puts firmware on both:

| Chip | What it runs | Firmware in this repo |
|---|---|---|
| **ESP32-S3** | The game: screen, touch, knob, haptics, wired sound | the project root (`main/`, `components/`) |
| **ESP32-U4WDH** (the "co-processor") | Bluetooth headphones | `coprocessor/` |

There is one USB-C port. Which chip your computer talks to depends on **which
way up the plug is**.

## 1. The cable

Use a USB-C cable that carries **data**. Many charge-only cables power the
board but never show up on the computer -- if nothing appears at all in step 2,
try another cable before anything else.

USB-C plugs are symmetric, so there is no "right way up" printed on them. Plug
in, check which chip answered (step 2), and if it is the wrong one:

1. Unplug the cable from the **board** (the computer end can stay put).
2. Turn the plug over, 180 degrees.
3. Plug it back in and check again.

Once you know which way reaches which chip, put a small mark on one face of
the plug -- a dot of marker or a sticker -- and note "dot up = S3". It saves
the guesswork every time after.

## 2. Which chip is connected?

The two chips reach the computer through different USB hardware, so they show
up differently.

| | ESP32-S3 (the game) | ESP32-U4WDH (Bluetooth) |
|---|---|---|
| USB hardware | the S3's own USB | a CH340 USB-serial chip |
| USB ID (`lsusb`) | `303a:1001` Espressif | `1a86:7523` QinHeng Electronics CH340 |
| Linux port | `/dev/ttyACM0` | `/dev/ttyUSB0` |
| macOS port | `/dev/cu.usbmodem*` | `/dev/cu.wchusbserial*` or `/dev/cu.usbserial-*` |
| Windows (Device Manager → Ports) | "USB Serial Device (COMn)" | "USB-SERIAL CH340 (COMn)" |

The numbers on the ports (`ACM0`, `USB0`, `COM5`) can differ on your machine;
the **kind** of port is what tells them apart.

On Linux, one command shows which is plugged in:

```bash
ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

To be certain, ask the chip itself. `esptool` ships with ESP-IDF:

```bash
esptool.py --port /dev/ttyACM0 chip_id     # the S3: "Chip is ESP32-S3"
esptool.py --port /dev/ttyUSB0 chip_id     # the co-processor: "Chip is ESP32-U4WDH"
```

**Check this before every flash.** Each firmware is built for one chip; esptool
refuses a mismatch, but only after you have spent the time to find out.

## 3. Set up ESP-IDF

ESP-IDF 5.4 or newer (tested on 5.5.2). In every new terminal:

```bash
. $IDF_PATH/export.sh
```

## 4. Flash the ESP32-S3 (the game)

Plug in so the S3 answers (`/dev/ttyACM0`), then from the project root:

```bash
idf.py set-target esp32s3     # first build on a fresh clone only
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

`monitor` shows the log; quit it with `Ctrl+]`. A good boot ends with
`main: up`.

If flashing cannot connect, hold the **BOOT** button while plugging the cable
in, release it, and run the flash command again.

The game needs an EEZ Studio export in `main/ui/` (it is committed, so a fresh
clone already has one). After changing the screens, export again (Ctrl+B in
EEZ Studio) before building.

## 5. Flash the ESP32-U4WDH (Bluetooth)

This **replaces Waveshare's factory firmware** on that chip. The factory image
is specific to your board (it holds its own calibration), so save it first --
you only need to do this once, before the first flash.

Turn the plug over so the co-processor answers (`/dev/ttyUSB0`), then from the
project root:

```bash
# Once: back up the factory firmware (about a minute). firmware_backup/ is
# git-ignored -- keep the file somewhere safe as well.
mkdir -p firmware_backup
esptool.py --port /dev/ttyUSB0 -b 921600 read_flash 0 ALL firmware_backup/esp32_u4wdh_factory_4MB.bin

# Build and flash the Bluetooth firmware
cd coprocessor
idf.py set-target esp32       # first build on a fresh clone only
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

A good boot logs `cts_bt: -> READY 1`.

The order does not matter: either chip can be flashed first. Until the
co-processor has this firmware, the game runs normally and its Headphones
screen says **Bluetooth is not available**.

## 6. Check both together

Turn the plug back to the S3 side (or unplug it and power the board any other
way). On the instructions screen touch **Headphones**: the status should read
**Put headphones in pairing mode** or **Searching...**. If it says
**Bluetooth is not available**, the co-processor is still on its factory
firmware, or did not start -- flash it again and watch its log.

## Putting the factory firmware back

```bash
# Co-processor (plug turned to the /dev/ttyUSB0 side)
esptool.py --port /dev/ttyUSB0 -b 921600 write_flash 0 firmware_backup/esp32_u4wdh_factory_4MB.bin
```

The image includes the bootloader and partition table, so this restores the
chip exactly as it shipped.

## Troubleshooting

| Symptom | Likely cause |
|---|---|
| Nothing appears in step 2 | A charge-only cable, or the plug not fully seated |
| The wrong chip answers | The plug is the other way up -- turn it over |
| `Could not open /dev/tty...: port is busy` | A monitor or another program has the port open; close it. Also happens if the plug was turned over mid-command |
| `This chip is ESP32, not ESP32-S3` (or the reverse) | Flashing the wrong project for the connected chip -- check step 2 |
| Flash will not start on the S3 | Hold BOOT while plugging in, then flash |
| Headphones screen: **Bluetooth is not available** | The co-processor is on factory firmware, or not running |
| No sound from the 3.5 mm jack while headphones are connected | Expected: connected headphones take the sound and the jack is muted |
