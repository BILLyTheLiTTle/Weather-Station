#include "Battery.h"

Battery::Battery(uint8_t pin, uint32_t r1, uint32_t r2)
    : _pin(pin), _r1(r1), _r2(r2), _index(0), _filled(false), _filteredV(0) {}

void Battery::begin() {
    for (uint8_t i = 0; i < N; i++) _samples[i] = 0;
}

// Μετατροπή σε mV (milliVolts) χωρίς float
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

    // 2. Low Pass Part (0.85 * filtered + 0.15 * avg)
    // Με ακέραιους: (filtered * 85 + avg * 15) / 100
    if (_filteredV == 0) _filteredV = avg; // Αρχικοποίηση
    _filteredV = ((uint32_t)_filteredV * 85 + (uint32_t)avg * 15) / 100;

    return _filteredV;
}

uint8_t Battery::readPercent() {
    // uint16_t v = applyFilter(readVoltage());
    uint16_t v = readVoltage();

    if (v >= 8400) return 100;
    if (v <= 6000) return 0;

    // Κανονικοποίηση x = (v - 6000) / 2400 (σε κλίμακα 0-1000 για ακρίβεια)
    uint32_t x = ((uint32_t)(v - 6000) * 1000) / 2400;

    // Smoothstep: x * x * (3000 - 2 * x) / 1000000
    // Αυτό αντικαθιστά το x * x * (3 - 2x)
    uint32_t curved = (x * x * (3000 - 2 * x)) / 1000000;

    // Αντί για pow(curved, 0.75), χρησιμοποιούμε μια Look-up Table 11 σημείων
    // για να δώσουμε αυτό το "UX Boost" που θέλεις
    static const uint8_t ux_boost_lut[] = {0, 15, 28, 40, 52, 63, 73, 82, 90, 96, 100};
    
    // Το curved είναι 0-1000, άρα διαιρούμε με 100 για να βρούμε το index (0-10)
    uint8_t idx = curved / 100;
    if (idx >= 10) return 100;

    // Γραμμική παρεμβολή ανάμεσα στα σημεία της LUT για ομαλότητα 1%
    uint8_t p1 = ux_boost_lut[idx];
    uint8_t p2 = ux_boost_lut[idx + 1];
    uint8_t weight = curved % 100;

    return p1 + ((p2 - p1) * weight) / 100;
}