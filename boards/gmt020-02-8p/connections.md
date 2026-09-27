# ESP32-S3 DevKit + OTFTSPI GMT020-02-8P

A Bruce board definition for a generic **ESP32-S3** dev board driving an
**OTFTSPI GMT020-02-8P** panel (2.0", 240x320, **ST7789V**, 4-wire SPI, 3V3 logic)
with three tactile navigation buttons.

## Wiring

### Display (SPI) — proven working on this board

| Panel pin | ESP32-S3 GPIO | Notes                          |
|-----------|---------------|--------------------------------|
| GND       | GND           |                                |
| VCC       | 3V3           |                                |
| SCL (SCLK)| **GPIO12**    | SPI clock                      |
| SDA (MOSI)| **GPIO11**    | SPI data                       |
| RST (RES) | **GPIO4**     |                                |
| DC        | **GPIO5**     |                                |
| CS        | **GPIO10**    |                                |
| BL        | 3V3           | Backlight always on (no PWM)   |

### Buttons — active LOW, `INPUT_PULLUP`

| Button  | ESP32-S3 GPIO | Bruce action                          |
|---------|---------------|---------------------------------------|
| UP      | **GPIO6**     | `PrevPress` (menu up)                  |
| SELECT  | **GPIO7**     | `SelPress` (click) / `EscPress` (hold) |
| DOWN    | **GPIO8**     | `NextPress` (menu down)                |

Controls:
- **UP / DOWN** — move through menus.
- **SELECT (short click)** — confirm / enter.
- **SELECT (hold >0.7s)** — back / escape.
- **UP + DOWN (hold ~2s)** — power off (deep sleep; wake on SELECT).

## Building

```bash
pio run -e gmt020-02-8p            # default backend: TFT_eSPI
pio run -e gmt020-02-8p -t upload
```

Target MCU is assumed to be **N16R8** (16MB flash, 8MB OPI PSRAM). If your
module differs, adjust `board_build.arduino.memory_type` and
`board_build.partitions` in `gmt020-02-8p.ini` and `flash_size` in
`boards/_boards_json/gmt020-02-8p.json`.

## Display backends

Bruce's display HAL (`lib/HAL/display/`) supports several backends. This board
ships with **TFT_eSPI** (Bruce's default, most-tested path, native ST7789V).

A **LovyanGFX** fallback — the driver this panel was originally proven on — is
provided as a drop-in. To switch:

1. In `pins_arduino.h`: comment out the `DISPLAY -- default backend: TFT_eSPI`
   block and uncomment the `LovyanGFX FALLBACK` block below it.
2. In `gmt020-02-8p.ini`: uncomment the `[env:gmt020-02-8p-lovyan]` env (it adds
   `-DUSE_LOVYANGFX` and the LovyanGFX `#develop` dependency — the released
   1.2.7 is not compatible with the current Arduino 3.3.x core).
3. Build `-e gmt020-02-8p-lovyan`.

## First-boot display tuning

If the image is wrong, adjust in `pins_arduino.h` (no core changes needed):

- **Colours look inverted / negative** → toggle `TFT_INVERSION_ON` ↔
  `TFT_INVERSION_OFF` (TFT_eSPI) or `TFT_INVERTION 1` ↔ `0` (LovyanGFX).
- **Red/blue swapped** → set `TFT_RGB_ORDER` (LovyanGFX) or add
  `#define TFT_RGB_ORDER TFT_RGB` (TFT_eSPI).
- **Orientation** → change `ROTATION` (0/1/2/3).
- **A few pixels shifted** → set `TFT_OFFSET_X/Y` (LovyanGFX); for a full 240x320
  ST7789V the offset is 0.

## What this board does NOT include

No SD card, touch, audio codec, battery gauge, or dedicated IR / sub-GHz / NFC
hardware — none is fitted. Those subsystems are left dormant (no `USE_*_VIA_SPI`
module is declared). Free expansion GPIOs and `ALLOW_ALL_GPIO_FOR_IR_RF` are
exposed so external modules can be wired and selected at runtime later, keeping
the freed flash/RAM available. WiFi, BLE and BadUSB (native USB HID) — the
features the ESP32-S3 supports natively — are fully available.
