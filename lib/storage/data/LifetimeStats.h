#pragma once

#include <Arduino.h>

struct LifetimeStats {
    float minTemp;
    float maxTemp;

    uint16_t minDay;
    uint8_t minHour;
    uint8_t minMinute;

    uint16_t maxDay;
    uint8_t maxHour;
    uint8_t maxMinute;
};