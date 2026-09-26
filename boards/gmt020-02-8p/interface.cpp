#include "core/powerSave.h"
#include <esp_sleep.h>
#include <interface.h>

// ============================================================================
// Bruce board interface: ESP32-S3 DevKit + OTFTSPI GMT020-02-8P
//
// Input: 3 tactile buttons, active LOW, INPUT_PULLUP.
//   UP  (G6)  -> PrevPress / UpPress
//   DOWN(G8)  -> NextPress / DownPress
//   SEL (G7)  -> SelPress (short click)   /   EscPress (long press >700ms)
//   UP + DOWN held together -> power off (deep sleep, wake on SELECT)
//
// The display backlight is hardwired to 3V3, so brightness control is a no-op.
// There is no battery and no charger, so getBattery()/isCharging() are stubs.
// ============================================================================

#define SEL_LONGPRESS_MS 700
#define POWEROFF_HOLD_MS 2000

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    pinMode(UP_BTN, INPUT_PULLUP);
    pinMode(SEL_BTN, INPUT_PULLUP);
    pinMode(DW_BTN, INPUT_PULLUP);
}

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   No battery gauge on this board.
***************************************************************************************/
int getBattery() { return 0; }

/***************************************************************************************
** Function name: isCharging()
** Description:   No charger on this board.
***************************************************************************************/
bool isCharging() { return false; }

/*********************************************************************
** Function: _setBrightness
** location: settings.cpp
** Backlight is tied to 3V3 (always on) -> nothing to control.
**********************************************************************/
void _setBrightness(uint8_t brightval) { (void)brightval; }

/*********************************************************************
** Function: InputHandler
** Handles PrevPress, NextPress, SelPress, EscPress, AnyKeyPress.
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = 0;
    static unsigned long sel_down_ms = 0;
    static bool sel_was_down = false;
    static bool sel_long_fired = false;

    bool up = (digitalRead(UP_BTN) == BTN_ACT);
    bool dw = (digitalRead(DW_BTN) == BTN_ACT);
    bool sel = (digitalRead(SEL_BTN) == BTN_ACT);

    // --- SELECT: short click = Select, long press = Esc ---
    // Evaluated on every call so press duration is timed accurately.
    if (sel) {
        if (!sel_was_down) {
            sel_was_down = true;
            sel_long_fired = false;
            sel_down_ms = millis();
        } else if (!sel_long_fired && (millis() - sel_down_ms > SEL_LONGPRESS_MS)) {
            sel_long_fired = true; // fire Esc once while still held
            if (wakeUpScreen()) return;
            AnyKeyPress = true;
            EscPress = true;
        }
    } else {
        if (sel_was_down && !sel_long_fired) {
            // released before the long-press threshold -> Select
            sel_was_down = false;
            if (wakeUpScreen()) return;
            AnyKeyPress = true;
            SelPress = true;
        }
        sel_was_down = false;
    }

    // --- UP / DOWN: menu navigation, rate-limited ---
    if (millis() - tm < 200 && !LongPress) return;

    if (up || dw) {
        tm = millis();
        if (wakeUpScreen()) return;
        AnyKeyPress = true;
        if (up) {
            PrevPress = true;
            UpPress = true;
        }
        if (dw) {
            NextPress = true;
            DownPress = true;
        }
    }
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Sleep the panel and enter deep sleep; wake on SELECT.
**********************************************************************/
void powerOff() {
    esp_sleep_enable_ext0_wakeup((gpio_num_t)SEL_BTN, BTN_ACT);
    esp_deep_sleep_start();
}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Hold UP + DOWN together for ~2s to power off (deep sleep).
**********************************************************************/
void checkReboot() {
    if (digitalRead(UP_BTN) == BTN_ACT && digitalRead(DW_BTN) == BTN_ACT) {
        unsigned long t0 = millis();
        while (digitalRead(UP_BTN) == BTN_ACT && digitalRead(DW_BTN) == BTN_ACT) {
            if (millis() - t0 > POWEROFF_HOLD_MS) {
                // wait for release so the buttons don't immediately re-wake
                while (digitalRead(UP_BTN) == BTN_ACT || digitalRead(DW_BTN) == BTN_ACT);
                delay(150);
                powerOff();
            }
            delay(10);
        }
    }
}
