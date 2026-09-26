# Bruce — adapted for ESP32-S3 + OTFTSPI GMT020-02-8P

This repository is a fork/vendor of [Bruce](https://github.com/pr3y/Bruce)
(upstream commit `a59213f`) with a **new board definition** added for a generic
**ESP32-S3** dev board driving an **OTFTSPI GMT020-02-8P** display
(2.0", 240x320, ST7789V, 4-wire SPI) with three navigation buttons.

**No Bruce core code was modified.** The adaptation is entirely additive — it
follows Bruce's supported "add a board" path (three files + a board manifest),
so it stays easy to rebase on upstream.

## What was added

| File | Purpose |
|------|---------|
| `boards/gmt020-02-8p/pins_arduino.h` | Pin map + display config (TFT_eSPI active; LovyanGFX fallback block) |
| `boards/gmt020-02-8p/interface.cpp` | 3-button input handler, power/brightness stubs |
| `boards/gmt020-02-8p/gmt020-02-8p.ini` | PlatformIO build env (+ commented LovyanGFX env) |
| `boards/gmt020-02-8p/connections.md` | Wiring table, build/flash, tuning guide |
| `boards/_boards_json/gmt020-02-8p.json` | PlatformIO board manifest (ESP32-S3, N16R8) |
| `platformio.ini` | `gmt020-02-8p` added as the default build env |

## Build

```bash
pio run -e gmt020-02-8p -t upload
```

See [`boards/gmt020-02-8p/connections.md`](boards/gmt020-02-8p/connections.md)
for the wiring, controls, display-backend switch, and first-boot tuning.

## Key decisions

- **Display backend: TFT_eSPI first**, with a drop-in **LovyanGFX** fallback
  that mirrors the config this panel was already proven on. TFT_eSPI is Bruce's
  default/most-tested path and natively supports the ST7789V; LovyanGFX is one
  toggle away if the panel misbehaves.
- **Target: N16R8** (16MB flash, 8MB OPI PSRAM) — full Bruce build.
- **No radios/SD/touch/audio declared** (none fitted); those subsystems stay
  dormant and free GPIOs are exposed for future external modules. WiFi, BLE and
  BadUSB are fully available.
