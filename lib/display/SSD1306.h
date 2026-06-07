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
#include "Formatter.h"

enum Page : uint8_t {
    PAGE_CURRENT_STATS = 0,
    PAGE_WEATHER_PREDICTION,
    PAGE_DAILY_TEMPERATURE_STATS,
    PAGE_DAILY_HUMIDITY_STATS,
    PAGE_LIFETIME_TEMPERATURE_STATS,
    PAGE_LIFETIME_HUMIDITY_STATS,
    PAGE_SYSTEM_STATS,
    PAGE_COUNT // This will have value of 7, which represents the number of available Pages!
};

class SSD1306 {
private:
    SSD1306AsciiWire _oled;
    ButtonSensor navigationButton;
    Page currentPage = PAGE_CURRENT_STATS;
    char bufferedValue[10];
    char bufferedDateTime[20];

public:
    SSD1306(uint8_t navigationControl);

    void begin();
    void showBootMessage();
    void updateTime(int hours, int minutes, int seconds);
    void showError(const __FlashStringHelper* msg);
    void showCurrentStats(int16_t temp, uint16_t hum, uint32_t pres);
    void showDailyTemperatureStats(TemperatureDailyStats &stats);
    void showDailyHumidityStats(HumidityDailyStats &stats);
    void showLifetimeTemperatureStats(TemperatureLifetimeStats &stats);
    void showLifetimeHumidityStats(HumidityLifetimeStats &stats);
    void showWeatherPrediction(const char* forecast, const char* wind, const char* timeframe, bool isIcy);
    void showSystemStats(Battery &battery, ACS712 &acs712, MemoryProfiler &ram, Thermistor &therm);

    void clear();

    void sleep();
    void wake();

    Page readControls();

};

extern SSD1306 display;