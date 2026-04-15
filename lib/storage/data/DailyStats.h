#pragma once

#include <Arduino.h>

struct DailyStats {
    uint16_t dayIndex;

    float minTemp;
    float maxTemp;

    uint8_t minHour;
    uint8_t minMinute;

    uint8_t maxHour;
    uint8_t maxMinute;
};