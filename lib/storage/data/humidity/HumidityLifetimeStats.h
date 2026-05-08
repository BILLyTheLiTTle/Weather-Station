#pragma once

#include <Arduino.h>

struct __attribute__((packed)) HumidityLifetimeStats {
    uint16_t minHum = UINT16_MAX;
    uint16_t maxHum = 0;

    uint32_t minDay : 5;
    uint32_t minMonth : 4;
    uint32_t minYear : 12;
    uint32_t minHour : 5;
    uint32_t minMinute : 6;

    uint32_t maxDay : 5;
    uint32_t maxMonth : 4;
    uint32_t maxYear : 12;
    uint32_t maxHour : 5;
    uint32_t maxMinute : 6;
};