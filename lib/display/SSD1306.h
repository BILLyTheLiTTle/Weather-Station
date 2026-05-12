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
#include "controller/ButtonSensor.h"

enum Page : uint8_t {
    PAGE_CURRENT_STATS = 0,
    PAGE_DAILY_TEMPERATURE_STATS,
    PAGE_DAILY_HUMIDITY_STATS,
    PAGE_LIFETIME_TEMPERATURE_STATS,
    PAGE_LIFETIME_HUMIDITY_STATS,
    PAGE_SYSTEM_STATS,
    PAGE_COUNT // This will have value of 6, which represents the number of available Pages!
};

class SSD1306 {
private:
    SSD1306AsciiWire _oled;
    ButtonSensor navigationButton;
    Page currentPage = PAGE_CURRENT_STATS;

    void formatNumber(char* buffer, int16_t value);
    void formatNumber(char* buffer, uint16_t value);
    void formatDateTime(char* buffer, uint8_t d, uint8_t m, uint16_t y, uint8_t h, uint8_t min);

public:
    SSD1306(uint8_t navigationControl);

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

    Page readControls();

};

extern SSD1306 display;