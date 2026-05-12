#pragma once

#include "Arduino.h"

void formatNumber(char* buffer, int16_t value);
void formatNumber(char* buffer, uint16_t value);
void formatDateTime(char* buffer, uint8_t d, uint8_t m, uint16_t y, uint8_t h, uint8_t min);