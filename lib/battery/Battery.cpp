#include "Battery.h"

Battery::Battery(uint8_t pin, uint32_t r1, uint32_t r2)
    : _pin(pin), _r1(r1), _r2(r2), _index(0), _filled(false), _filteredV(0) {}

void Battery::begin() {
    for (uint8_t i = 0; i < N; i++) _samples[i] = 0;
}

// Convert ADC reading to mV (milliVolts) without using float
uint16_t Battery::readVoltage() {
    uint32_t adc = analogRead(_pin);
    // V_pin = (adc * 5000) / 1023
    uint32_t v_pin = (adc * 5000UL) / 1023UL;
    // V_bat = V_pin * (R1 + R2) / R2
    uint32_t v_bat = (v_pin * (_r1 + _r2)) / _r2;
    return (uint16_t)v_bat;
}

uint16_t Battery::applyFilter(uint16_t v) {
    // 1. Moving Average Part
    _samples[_index] = v;
    _index = (_index + 1) % N;
    if (_index == 0) _filled = true;
    uint8_t count = _filled ? N : _index;

    uint32_t sum = 0;
    for (uint8_t i = 0; i < count; i++) sum += _samples[i];
    uint16_t avg = sum / count;

    // 2. Low Pass Part (Exponential Smoothing: 0.85 * filtered + 0.15 * avg)
    // Using integer math: (filtered * 85 + avg * 15) / 100
    if (_filteredV == 0) _filteredV = avg; // Initial seeding
    _filteredV = ((uint32_t)_filteredV * 85 + (uint32_t)avg * 15) / 100;

    return _filteredV;
}

uint8_t Battery::readPercent() {
    uint16_t v = readVoltage();

    if (_old_voltage == 0) {
        _old_voltage = v;
        _filteredV = v;
    } else if (abs((int16_t)_old_voltage - (int16_t)v) < VOLTAGE_THRESHOLD) {
        v = applyFilter(v);
    }

    _old_voltage = v;

    if (v >= 8400) return 100;
    if (v <= 6000) return 0;

    // Normalization: x = (v - 6000) / 2400 (scale to 0-1000 for precision)
    uint32_t x = ((uint32_t)(v - 6000) * 1000) / 2400;

    // Smoothstep curve: x * x * (3 - 2x)
    // Scaled to integer: (x^2 * (3000 - 2x)) / 1000000
    uint32_t curved = (x * x * (3000 - 2 * x)) / 1000000;

    // UX Boost Look-up Table (replaces pow(curved, 0.75))
    // Provides a more "natural" feel to the percentage drop
    static const uint8_t ux_boost_lut[] = {0, 15, 28, 40, 52, 63, 73, 82, 90, 96, 100};
    
    // Map curved value (0-1000) to LUT index (0-10)
    uint8_t idx = curved / 100;
    if (idx >= 10) return 100;

    // Linear Interpolation between LUT points for 1% granularity
    uint8_t p1 = ux_boost_lut[idx];
    uint8_t p2 = ux_boost_lut[idx + 1];
    uint8_t weight = curved % 100;

    return p1 + ((p2 - p1) * weight) / 100;
}