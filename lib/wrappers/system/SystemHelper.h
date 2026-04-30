#pragma once
#include "Battery.h"
#include "MemoryProfiler.h"
#include "ACS712.h"

void printBatteryStats(Battery &battery, ACS712 &acs712);

void printRamStats(MemoryProfiler &ram);

void printSystemStats(Battery &battery, ACS712 &acs712, MemoryProfiler &ram);

bool isUsbPowered(uint16_t voltage);