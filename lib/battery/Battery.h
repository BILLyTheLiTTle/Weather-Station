#pragma once

#include <Arduino.h>

// Number of samples for the Moving Average part of the filter
#define N 10 

class Battery {
public:
    /**
     * @param pin Analog pin (e.g., A0)
     * @param vcc_r Resistance connected to V+ in Ohms
     * @param gnd_r Resistance connected to GND in Ohms
     */
    Battery(uint8_t pin, uint16_t vcc_r, uint16_t gnd_r);

    void begin();

    // Returns raw battery voltage in millivolts (mV)
    uint16_t readVoltage();

    // Returns battery percentage (0-100) using optimized Smoothstep & LUT
    uint8_t readPercent();

    static constexpr uint16_t UPPER_BOUND_VOLTAGE = 8400; //mV
    static constexpr uint16_t LOWER_BOUND_VOLTAGE = 6000; //mV

private:
    uint8_t  _pin;
    uint16_t _vcc_r;
    uint16_t _gnd_r;

    // Filter variables
    uint16_t _samples[N];    // Array for Moving Average
    uint8_t  _index;         // Current position in the array
    bool     _filled;        // Flag to check if the buffer is full
    uint16_t _filteredV;     // Stores the result of the Low Pass part

    uint16_t _old_voltage = 0;
    static constexpr uint16_t VOLTAGE_THRESHOLD = 500; //mV

    // Internal hybrid filter logic (Moving Average + Exponential Smoothing)
    uint16_t applyFilter(uint16_t v);
};