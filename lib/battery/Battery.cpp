#include "Battery.h"

Battery::Battery(uint8_t pin, uint32_t r1, uint32_t r2)
    : _pin(pin), _r1(r1), _r2(r2) {
    _multiplier = (5000UL * (_r1 + _r2)) / (1023UL * _r2);
}

void Battery::begin() {
    _filteredV = 0;
}

uint16_t Battery::readVoltage() {
    return (uint16_t)(analogRead(_pin) * _multiplier);
}

uint16_t Battery::applyFilter(uint16_t v) {
    if (_filteredV == 0) _filteredV = v;
    
    _filteredV = (v + (_filteredV * 3)) >> 2; 
    
    return _filteredV;
}

uint8_t Battery::readPercent() {
    uint16_t v = applyFilter(readVoltage());

    if (v >= 8400) return 100;
    if (v <= 6000) return 0;

    static const uint16_t volt_points[] = {6000, 7200, 7500, 7700, 8000, 8400};
    static const uint8_t percent_points[] = {0, 20, 40, 60, 80, 100};

    uint8_t i = 0;
    while (v > volt_points[i + 1]) i++;

    uint32_t num = (uint32_t)(v - volt_points[i]) * (percent_points[i+1] - percent_points[i]);
    uint16_t den = volt_points[i+1] - volt_points[i];
    
    return percent_points[i] + (uint8_t)(num / den);
}