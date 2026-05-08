#pragma once

#include "Battery.h"
#include "MemoryProfiler.h"
#include "ACS712.h"
#include "Thermistor.h"

void printBatteryStats(Battery &battery, ACS712 &acs712);

void printRamStats(MemoryProfiler &ram);

void printSystemStats(Battery &battery, ACS712 &acs712, MemoryProfiler &ram, Thermistor &therm);

void printSystemTemperature(Thermistor &therm);

bool isUsbPowered(uint16_t voltage);