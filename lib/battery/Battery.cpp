#include "Battery.h"

Battery::Battery(uint8_t pin, float r1, float r2)
: _pin(pin), _r1(r1), _r2(r2) {}

void Battery::begin() {
    // pinMode(_pin, INPUT);

    for (int i = 0; i < N; i++) {
        _samples[i] = 0;
    }
}

float Battery::adcToVoltage(int adc) {
    float vA0 = (adc * 5.0) / 1023.0;
    return vA0 * (_r1 + _r2) / _r2;
}

/* ===== LOW PASS + MOVING AVERAGE HYBRID ===== */
float Battery::applyFilter(float v) {
    _samples[_index] = v;
    _index = (_index + 1) % N;

    if (_index == 0) _filled = true;

    int count = _filled ? N : _index;

    float sum = 0;
    for (int i = 0; i < count; i++) {
        sum += _samples[i];
    }

    float avg = sum / count;

    // extra low-pass smoothing
    static float filtered = 0;
    filtered = 0.85 * filtered + 0.15 * avg;

    return filtered;
}

float Battery::readVoltage() {
    int adc = analogRead(_pin);
    float v = adcToVoltage(adc);
    return v;
}

float Battery::readFilteredVoltage() {
    float v = readVoltage();
    return applyFilter(v);
}

int Battery::readPercent() {
    float v = readFilteredVoltage();

    if (v >= 8.4) return 100;
    if (v <= 6.0) return 0;

    float x = (v - 6.0) / (8.4 - 6.0); // normalize 0..1

    // realistic + UI fast feel curve
    float curved = x * x * (3.0 - 2.0 * x);  // smoothstep
    curved = pow(curved, 0.75);              // UX boost

    return (int)(curved * 100);
}