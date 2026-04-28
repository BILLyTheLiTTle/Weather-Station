#pragma once

#include "../storage/data/temperature/TemperatureLifetimeStats.h"
#include "../storage/data/temperature/TemperatureDailyStats.h"
#include "Thermistor.h"
#include "../storage/eeprom/EEPROM_25LC040A.h"

void printTemperatureStats(Thermistor &therm, EEPROM_25LC040A &eeprom, TemperatureDailyStats &d, TemperatureLifetimeStats &l);
void printLine(const __FlashStringHelper* label, int16_t value, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);
void printTemperature(int16_t temp);

void saveTemperatureLifetimeRecord(EEPROM_25LC040A &eeprom, int16_t maxTemp, int16_t minTemp, TemperatureLifetimeStats &life);

void rememberTemperatureDailyRecord(int16_t maxTemp, int16_t minTemp, TemperatureDailyStats &day);