/*
Pins:
1 (CS) -> D10
2 (SO) -> D12
3 (WP) -> 5V
4 (VSS) -> GND
5 (SI) -> D11
6 (SCK) -> D13
7 (HOLD) -> 5V
8 (VCC) -> 5V

Almost identical with this wiring (https://wolles-elektronikkiste.de/wp-content/uploads/2022/03/Wiring_25LC512_Arduino_Nano.png)
apart from WP(D9), HOLD(5V) pins.

ChatGPT implementation discussion: https://chatgpt.com/c/69dfb460-452c-832d-a170-bb0289e106c1
*/

#pragma once

#include <Arduino.h>
#include <SPI.h>
#include "../storage/data/temperature/TemperatureLifetimeStats.h"
#include "../storage/data/temperature/TemperatureDailyStats.h"

class EEPROM_25LC040A {
public:
    EEPROM_25LC040A(uint8_t csPin);

    void begin();

    void readBytes(uint16_t addr, void *data, uint16_t len);
    void writeBytes(uint16_t addr, const void *data, uint16_t len);
    void writeByte(uint16_t addr, uint8_t data);
    uint8_t readByte(uint16_t addr);

    void factoryReset();

    void initSystem(uint8_t maxDays);

    // void saveDailyTemperature(uint8_t index, const TemperatureDailyStats &data);
    // void saveDailyTemperature(const TemperatureDailyStats &data);

    // void loadDailyTemperature(uint8_t index, TemperatureDailyStats &data);
    // void loadDailyTemperature(TemperatureDailyStats &data);

    void saveLifetimeTemperature(const TemperatureLifetimeStats &data);
    void loadLifetimeTemperature(TemperatureLifetimeStats &data);

    void storeDataVersion();
    uint8_t loadDataVersion();

private:
    uint8_t _cs;

    void csLow();
    void csHigh();

    void writeEnable();
    void waitWrite();
};