#pragma once

#include <float.h>
#include <Arduino.h>

struct DailyStats {
    uint16_t dayIndex;

    float minTemp = FLT_MAX;
    float maxTemp = FLT_MIN;

    // float minHumidity = FLT_MAX;
    // float maxHumidity = FLT_MIN;

    uint8_t minDay = 29;
    uint8_t minMonth = 12;
    uint16_t minYear = 1900;
    uint8_t minHour = 23;
    uint8_t minMinute = 59;

    uint8_t maxDay = 29;
    uint8_t maxMonth = 12;
    uint16_t maxYear = 1900;
    uint8_t maxHour = 23;
    uint8_t maxMinute = 59;
};