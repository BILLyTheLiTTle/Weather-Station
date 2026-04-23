#pragma once

#include "./data/temperature/TemperatureLifetimeStats.h"
#include "./data/temperature/TemperatureDailyStats.h"
#include "Thermistor.h"
#include "./eeprom/EEPROM_25LC040A.h"

void printTemperature(Thermistor &therm, EEPROM_25LC040A &eeprom, TemperatureDailyStats d, TemperatureLifetimeStats l);

void storeTemperatureStats(EEPROM_25LC040A &eeprom, float maxTemp, float minTemp, TemperatureDailyStats &day, TemperatureLifetimeStats &life);