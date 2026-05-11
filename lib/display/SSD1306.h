#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "../storage/data/temperature/TemperatureLifetimeStats.h"
#include "../storage/data/temperature/TemperatureDailyStats.h"
#include "../storage/data/humidity/HumidityLifetimeStats.h"
#include "../storage/data/humidity/HumidityDailyStats.h"
#include "Battery.h"
#include "ACS712.h"
#include "MemoryProfiler.h"
#include "Thermistor.h"

enum Page : uint8_t {
    PAGE_CURRENT_STATS = 0,
    PAGE_DAILY_STATS,
    PAGE_LIFETIME_STATS,
    PAGE_SYSTEM_STATS
};

class SSD1306 {
private:
    SSD1306AsciiWire _oled;

public:
    void begin();
    void showBootMessage();
    void updateTime(int hours, int minutes, int seconds);
    void showError(const __FlashStringHelper* msg);
    void showCurrentStats(int16_t temp, uint16_t hum);
    void showDailyTemperatureStats(TemperatureDailyStats &stats);
    void showDailyHumidityStats(HumidityDailyStats &stats);
    void showLifetimeTemperatureStats(TemperatureLifetimeStats &stats);
    void showLifetimeHumidityStats(HumidityLifetimeStats &stats);
    void showSystemStats(Battery &battery, ACS712 &acs712, MemoryProfiler &ram, Thermistor &therm);

    void clear();
    // void print(const __FlashStringHelper* msg);
    // void println(const __FlashStringHelper* msg);
    // void print(int16_t value);
    // void println(int16_t value);
    // void print(uint16_t value);
    // void println(uint16_t value);
    // void print(int8_t value);
    // void println(int8_t value);
    // void print(uint8_t value);
    // void println(uint8_t value);

};

extern SSD1306 display;