#pragma once

#include <Arduino.h>

class Battery {
public:
    Battery(uint8_t pin, float r1, float r2);

    void begin();

    float readVoltage();
    float readFilteredVoltage();
    int readPercent();

private:
    uint8_t _pin;
    float _r1, _r2;

    float adcToVoltage(int adc);
    float applyFilter(float v);

    static const int N = 10;
    float _samples[N];
    int _index = 0;
    bool _filled = false;
};