#pragma once

#include "../storage/data/temperature/TemperatureLifetimeStats.h"
#include "../storage/data/temperature/TemperatureDailyStats.h"
#include "../storage/data/humidity/HumidityLifetimeStats.h"
#include "../storage/data/humidity/HumidityDailyStats.h"
#include "Thermistor.h"
#include "DHT_Sensor.h"
#include "../storage/eeprom/EEPROM_25LC040A.h"
#include "DS3231.h"

class EnvironmentManager {
public:
    EnvironmentManager() {}

    void printEnvironmentStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc,
                               TemperatureDailyStats &td, TemperatureLifetimeStats &tl, 
                               HumidityDailyStats &hd, HumidityLifetimeStats &hl);

    void saveTemperatureLifetimeRecord(EEPROM_25LC040A &eeprom, DS3231 &rtc, int16_t maxTemp, int16_t minTemp, TemperatureLifetimeStats &life);
    void saveHumidityLifetimeRecord(EEPROM_25LC040A &eeprom, DS3231 &rtc, int16_t maxHum, int16_t minHum, HumidityLifetimeStats &life);
    
    void rememberTemperatureDailyRecord(DS3231 &rtc, int16_t maxTemp, int16_t minTemp, TemperatureDailyStats &day);
    void rememberHumidityDailyRecord(DS3231 &rtc, uint16_t maxHum, uint16_t minHum, HumidityDailyStats &day);

private:
    void printLine(const __FlashStringHelper* label, int16_t value, bool isTemperature, 
                   uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);
    
    void printTemperature(int16_t temp);
    void printHumidity(uint16_t hum);
    void printDecimalNumber(int16_t val, const __FlashStringHelper *symbol);
    void printDate(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);
    void printTemperatureStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc, TemperatureDailyStats &td, TemperatureLifetimeStats &tl);
    void printHumidityStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc, HumidityDailyStats &hd, HumidityLifetimeStats &hl);
};