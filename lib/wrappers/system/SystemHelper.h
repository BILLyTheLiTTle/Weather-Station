#pragma once
#include "Battery.h"
#include "MemoryProfiler.h"

void printBatteryPercentage(Battery &battery);

void printRamStats(MemoryProfiler &ram);

void printSystemStats(Battery &battery, MemoryProfiler &ram);

bool isUsbPowered(uint16_t voltage);