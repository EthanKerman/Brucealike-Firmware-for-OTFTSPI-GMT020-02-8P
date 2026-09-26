#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "soc/soc_caps.h"
#include <stdint.h>

// ============================================================================
// Bruce board: ESP32-S3 DevKit + OTFTSPI GMT020-02-8P
//   Display : 2.0" 240x320 ST7789V, 4-wire SPI, 3V3 logic
//   Input   : 3 tactile buttons (UP / SELECT / DOWN), active LOW + pullups
// See boards/gmt020-02-8p/connections.md for wiring and backend switching.
// ============================================================================

#ifndef DEVICE_NAME
#define DEVICE_NAME "ESP32-S3 GMT020-02-8P"
#endif

// ---------------------------------------------------------------------------
// USB
// ---------------------------------------------------------------------------
#define USB_VID 0x303a
#define USB_PID 0x1001

// ---------------------------------------------------------------------------
// UART0
// ---------------------------------------------------------------------------
static const uint8_t TX = 43;
static const uint8_t RX = 44;

// ---------------------------------------------------------------------------
// I2C / Grove-style pins (free; used as defaults for optional external
// IR/RF/NFC modules and BadUSB serial). No I2C peripheral is fitted onboard.
// ---------------------------------------------------------------------------
#define GROVE_SDA 17
#define GROVE_SCL 18
static const uint8_t SDA = GROVE_SDA;
static const uint8_t SCL = GROVE_SCL;

// ---------------------------------------------------------------------------
// Secondary / expansion SPI bus (NOT the display bus).
// Exposed on free GPIOs so an external CC1101 / NRF24 / W5500 can be wired
// later. No module is declared present (see .ini: no USE_*_VIA_SPI), so these
// stay dormant and cost no runtime behaviour.
// ---------------------------------------------------------------------------
#define SPI_SCK_PIN 14
#define SPI_MOSI_PIN 13
#define SPI_MISO_PIN 9
#define SPI_SS_PIN 21

static const uint8_t SS = SPI_SS_PIN;
static const uint8_t MOSI = SPI_MOSI_PIN;
static const uint8_t SCK = SPI_SCK_PIN;
static const uint8_t MISO = SPI_MISO_PIN;

// ---------------------------------------------------------------------------
// No SD card on this board.
// ---------------------------------------------------------------------------
#define SDCARD_CS -1
#define SDCARD_SCK -1
#define SDCARD_MISO -1
#define SDCARD_MOSI -1

// ===========================================================================
// DISPLAY -- default backend: TFT_eSPI  (ST7789V, 4-wire SPI)
//
// Proven wiring:
//   SDA/MOSI -> G11    SCL/SCLK -> G12    CS -> G10
//   DC       -> G5     RST      -> G4     BL -> 3V3 (always on)
// ===========================================================================
#define USER_SETUP_LOADED
#define ST7789_DRIVER 1
// GMT020-02-8P is an IPS-style panel; colours are typically inverted. If the
// image comes out colour-negative, change this to TFT_INVERSION_OFF (or toggle
// bruceConfig.colorInverted at runtime from interface.cpp).
#define TFT_INVERSION_ON 1

#define TFT_WIDTH 240
#define TFT_HEIGHT 320

#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS 10
#define TFT_DC 5
#define TFT_RST 4
#define TFT_MISO -1
#define TFT_BL -1 // Backlight hardwired to 3V3 -> no PWM/brightness control
#define TFT_BACKLIGHT_ON HIGH

#define TOUCH_CS -1 // No touch panel
#define SMOOTH_FONT 1
#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 20000000

// ---------------------------------------------------------------------------
// Display setup (common to all backends)
// ---------------------------------------------------------------------------
#define HAS_SCREEN 1
#define ROTATION 0 // 0 = native portrait 240(w) x 320(h). Try 2 to flip 180.
#define MINBRIGHT 1
#define BACKLIGHT -1

// Font sizes
#define FP 1
#define FM 2
#define FG 3

// ===========================================================================
// LovyanGFX FALLBACK (known-good driver for this exact panel).
//
// To switch backends:
//   1. Comment out the whole "DISPLAY -- default backend: TFT_eSPI" block above
//      (everything from `#define USER_SETUP_LOADED` down to SPI_READ_FREQUENCY).
//   2. Uncomment the block below.
//   3. Use the `gmt020-02-8p-lovyan` env in gmt020-02-8p.ini (adds the lib and
//      -DUSE_LOVYANGFX). Do NOT define USE_LOVYANGFX here as well; one place.
// This mirrors the config already proven working on this board.
// ---------------------------------------------------------------------------
// #define LOVYAN_PANEL   Panel_ST7789
// #define LOVYAN_BUS     Bus_SPI
// #define LOVYAN_SPI_BUS 1
//
// #define TFT_SPI_HOST   SPI3_HOST      // HSPI / SPI3 host on the ESP32-S3
// #define TFT_SPI_MODE   0
// #define TFT_WRITE_FREQ 40000000
// #define TFT_READ_FREQ  15000000
// #define TFT_SPI_3WIRE  false          // 4-wire SPI: a dedicated DC line exists
// #define TFT_USE_LOCK   true
//
// #define TFT_SCLK 12
// #define TFT_MOSI 11
// #define TFT_MISO -1
// #define TFT_DC   5
// #define TFT_CS   10
// #define TFT_RST  4
// #define TFT_BUSY_PIN -1
//
// #define TFT_WIDTH      240
// #define TFT_HEIGHT     320
// #define TFT_OFFSET_X   0
// #define TFT_OFFSET_Y   0
// #define TFT_INVERTION  1              // ST7789V IPS: inversion on
// #define TFT_RGB_ORDER  0             // 0 = BGR, 1 = RGB (flip if colours wrong)
// #define TFT_MEM_WIDTH  240
// #define TFT_MEM_HEIGHT 320
// ===========================================================================

// ---------------------------------------------------------------------------
// Buttons: UP / SELECT / DOWN, active LOW, driven with INPUT_PULLUP.
//   UP  -> PrevPress    DOWN -> NextPress    SELECT -> SelPress
//   SELECT long-press -> EscPress (back)      UP+DOWN -> reboot
// ---------------------------------------------------------------------------
#define HAS_BTN 1
#define BTN_ALIAS "\"Sel\""
#define BTN_PIN 7 // SELECT (primary OK button)
#define BTN_ACT LOW
#define SEL_BTN 7
#define UP_BTN 6
#define DW_BTN 8

// ---------------------------------------------------------------------------
// Optional RGB status LED (many S3 devkits wire a WS2812 to GPIO48). Declared
// only as a pin; HAS_RGB_LED is intentionally left undefined so nothing drives
// it unless you enable it.
// ---------------------------------------------------------------------------
#define RGB_LED 48

// ---------------------------------------------------------------------------
// Infrared / RF default pin choices (external modules only; nothing fitted).
// ALLOW_ALL_GPIO_FOR_IR_RF (set in .ini) lets you re-pick these at runtime.
// ---------------------------------------------------------------------------
#define TXLED 1
#define RXLED 2
#define LED_ON HIGH
#define LED_OFF LOW

#define IR_TX_PINS '{{"GPIO1", 1}, {"GPIO2", 2}, {"GPIO21", 21}, {"Grove SDA", GROVE_SDA}}'
#define IR_RX_PINS '{{"GPIO2", 2}, {"GPIO1", 1}, {"GPIO21", 21}, {"Grove SCL", GROVE_SCL}}'
#define RF_TX_PINS '{{"GPIO1", 1}, {"GPIO2", 2}, {"GPIO21", 21}, {"Grove SDA", GROVE_SDA}}'
#define RF_RX_PINS '{{"GPIO2", 2}, {"GPIO1", 1}, {"GPIO21", 21}, {"Grove SCL", GROVE_SCL}}'

// ---------------------------------------------------------------------------
// Sub-GHz / 2.4GHz radio pin references (used ONLY if a module is enabled in
// the .ini via -DUSE_CC1101_VIA_SPI / -DUSE_NRF24_VIA_SPI, which it is not).
// ---------------------------------------------------------------------------
#define CC1101_GDO0_PIN 1
#define CC1101_SS_PIN SPI_SS_PIN
#define CC1101_MOSI_PIN SPI_MOSI_PIN
#define CC1101_SCK_PIN SPI_SCK_PIN
#define CC1101_MISO_PIN SPI_MISO_PIN

#define NRF24_CE_PIN 2
#define NRF24_SS_PIN SPI_SS_PIN
#define NRF24_MOSI_PIN SPI_MOSI_PIN
#define NRF24_SCK_PIN SPI_SCK_PIN
#define NRF24_MISO_PIN SPI_MISO_PIN

// ---------------------------------------------------------------------------
// FM radio reset pin default (Si4713 module; not fitted)
// ---------------------------------------------------------------------------
#define FM_RSTPIN 40

// ---------------------------------------------------------------------------
// BadUSB (native USB HID)
// ---------------------------------------------------------------------------
#define USB_as_HID 1
#define BAD_TX GROVE_SDA
#define BAD_RX GROVE_SCL

// ---------------------------------------------------------------------------
// Deep-sleep wake on SELECT button
// ---------------------------------------------------------------------------
#define DEEPSLEEP_WAKEUP_PIN 7
#define DEEPSLEEP_PIN_ACT LOW

#endif /* Pins_Arduino_h */
