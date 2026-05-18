#pragma once

#include "IHardware.h"

// --- ANALOG PINS ---
#define PIN_SYSTEM_TEMP_THERMISTOR   A0
#define PIN_BATTERY_VOLTAGE_DIVIDER  A1
#define PIN_ACS712                   A2
// #define PIN_EMPTY                 A3
// #define PIN_I2C_SDA               A4
// #define PIN_I2C_SCL               A5
// #define PIN_EMPTY                 A6
// #define PIN_EMPTY                 A7

// --- DIGITAL PINS ---
// #define PIN_D0                    0  // TX USB
// #define PIN_D1                    1  // RX USB
#define PIN_INT_RTC                  2
#define PIN_INT_MANUAL               3
// #define PIN_EMPTY                 4
// #define PIN_EMPTY                 5
// #define PIN_EMPTY                 6
// #define PIN_EMPTY                 7
#define PIN_SCREEN_CONTROLLER        8
#define PIN_DHT                      9
#define PIN_EEPROM_SS                10
// #define PIN_SPI_MOSI              11
// #define PIN_SPI_MISO              12
// #define PIN_SPI_SCK               13