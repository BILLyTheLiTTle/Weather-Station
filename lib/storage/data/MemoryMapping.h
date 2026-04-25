#pragma once

#include <float.h>
#include <Arduino.h>
#include "./temperature/TemperatureLifetimeStats.h"
#include "./temperature/TemperatureDailyStats.h"


const uint16_t VERSION_ADDRESS = 0x000;
uint8_t version = 1; // Max value should be 254 because 255 (0xFF) is written from factory reset as well.

// Memory mapping for current data
uint16_t lifetimeStatsAddress = VERSION_ADDRESS + sizeof(uint8_t);
// uint16_t dailyStatsAddress = lifetimeStatsAddress + sizeof(TemperatureLifetimeStats);
