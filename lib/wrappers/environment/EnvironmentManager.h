#pragma once

#include "../storage/data/temperature/TemperatureLifetimeStats.h"
#include "../storage/data/temperature/TemperatureDailyStats.h"
#include "../storage/data/humidity/HumidityLifetimeStats.h"
#include "../storage/data/humidity/HumidityDailyStats.h"
#include "Thermistor.h"
#include "DHT_Sensor.h"
#include "../storage/eeprom/EEPROM_25LC040A.h"
#include "DS3231.h"
#include "Formatter.h"

class EnvironmentManager {
public:
    EnvironmentManager() {}

    void printEnvironmentStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc,
                               TemperatureDailyStats &td, TemperatureLifetimeStats &tl, 
                               HumidityDailyStats &hd, HumidityLifetimeStats &hl);

    bool saveTemperatureLifetimeRecord(EEPROM_25LC040A &eeprom, DS3231 &rtc, int16_t maxTemp, int16_t minTemp, TemperatureLifetimeStats &life);
    bool saveHumidityLifetimeRecord(EEPROM_25LC040A &eeprom, DS3231 &rtc, int16_t maxHum, int16_t minHum, HumidityLifetimeStats &life);
    
    bool rememberTemperatureDailyRecord(DS3231 &rtc, int16_t maxTemp, int16_t minTemp, TemperatureDailyStats &day);
    bool rememberHumidityDailyRecord(DS3231 &rtc, uint16_t maxHum, uint16_t minHum, HumidityDailyStats &day);

    int16_t getCurrentTemp();
    uint16_t getCurrentHum();

private:
    void printLine(const __FlashStringHelper* label, int16_t value, bool isTemperature, 
                   uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);
    
    void printTemperature(int16_t temp);
    void printHumidity(uint16_t hum);
    void printDate(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);
    void printTemperatureStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc, TemperatureDailyStats &td, TemperatureLifetimeStats &tl);
    void printHumidityStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc, HumidityDailyStats &hd, HumidityLifetimeStats &hl);

    int16_t _currentTemp;
    uint16_t _currentHumidity;

    char bufferedValue[10];
    char bufferedDateTime[20];

    static const uint8_t RESET_HOUR = 0;
    static const uint8_t RESET_MINUTE_LOWER_BOUND = 5;
    static const uint8_t RESET_MINUTE_UPPER_BOUND = 25;
};