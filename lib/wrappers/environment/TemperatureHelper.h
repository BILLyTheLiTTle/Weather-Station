#pragma once

#include "../storage/data/temperature/TemperatureLifetimeStats.h"
#include "../storage/data/temperature/TemperatureDailyStats.h"
#include "Thermistor.h"
#include "../storage/eeprom/EEPROM_25LC040A.h"

void printTemperature(Thermistor &therm, EEPROM_25LC040A &eeprom, TemperatureDailyStats &d, TemperatureLifetimeStats &l);
void printLine(const __FlashStringHelper* label, float value, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);

void saveTemperatureLifetimeRecord(EEPROM_25LC040A &eeprom, float maxTemp, float minTemp, TemperatureLifetimeStats &life);

void rememberTemperatureDailyRecord(float maxTemp, float minTemp, TemperatureDailyStats &day);