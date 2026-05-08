#pragma once

#include "../storage/data/temperature/TemperatureLifetimeStats.h"
#include "../storage/data/temperature/TemperatureDailyStats.h"
#include "../storage/data/humidity/HumidityLifetimeStats.h"
#include "../storage/data/humidity/HumidityDailyStats.h"
#include "Thermistor.h"
#include "DHT_Sensor.h"
#include "../storage/eeprom/EEPROM_25LC040A.h"

void printEnvironmentStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, 
    TemperatureDailyStats &td, TemperatureLifetimeStats &tl, 
    HumidityDailyStats &hd, HumidityLifetimeStats &hl);

void printTemperature(int16_t temp);

void saveTemperatureLifetimeRecord(EEPROM_25LC040A &eeprom, int16_t maxTemp, int16_t minTemp, TemperatureLifetimeStats &life);

void rememberTemperatureDailyRecord(int16_t maxTemp, int16_t minTemp, TemperatureDailyStats &day);
void rememberHumidityDailyRecord(uint16_t maxHum, uint16_t minHum, HumidityDailyStats &day);

void printLine(const __FlashStringHelper* label, int16_t value, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);
void printLine(const __FlashStringHelper* label, uint16_t value, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);
void printDecimalNumber(int16_t temp, const __FlashStringHelper *symbol);