#pragma once

#include <Arduino.h>

class Battery {
public:
    Battery(uint8_t pin, float r1, float r2);

    void begin();

    float readVoltage();
    float readFilteredVoltage();
    uint8_t readPercent();

private:
    uint8_t _pin;
    float _r1, _r2;

    float adcToVoltage(uint16_t adc);
    float applyFilter(float v);

    static const uint8_t N = 10;
    float _samples[N];
    uint8_t _index = 0;
    bool _filled = false;
};