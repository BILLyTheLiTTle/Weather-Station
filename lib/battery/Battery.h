#pragma once

#include <Arduino.h>

class Battery {
public:
    /**
     * @param pin Analog pin (e.g., A0)
     * @param r1 Resistance connected to V+ in Ohms
     * @param r2 Resistance connected to GND in Ohms
     */
    Battery(uint8_t pin, uint32_t r1, uint32_t r2);

    void begin();

    // Returns voltage in millivolts (e.g., 7400 instead of 7.4V)
    uint16_t readVoltage();

    // Returns battery percentage (0-100) using a Look-Up Table
    uint8_t readPercent();

private:
    uint8_t  _pin;
    uint32_t _r1;
    uint32_t _r2;
    uint32_t _multiplier; // Pre-calculated scaling factor
    uint16_t _filteredV;  // Last filtered voltage value

    // Internal smoothing filter (Exponential Moving Average)
    uint16_t applyFilter(uint16_t v);
};