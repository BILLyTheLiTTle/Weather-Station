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
#include "data/DailyStats.h"
#include "data/LifetimeStats.h"

class EEPROM_25LC040A {
public:
    EEPROM_25LC040A(uint8_t csPin);

    void begin();

    void readBytes(uint16_t addr, void *data, uint16_t len);
    void writeBytes(uint16_t addr, const void *data, uint16_t len);
    void EEPROM_25LC040A::writeByte(uint16_t addr, uint8_t data);
    uint8_t EEPROM_25LC040A::readByte(uint16_t addr);
    void EEPROM_25LC040A::factoryReset();

    void initSystem(uint8_t maxDays);

    void saveDaily(uint8_t index, const DailyStats &data);
    void saveDaily(const DailyStats &data);

    void loadDaily(uint8_t index, DailyStats &data);
    void loadDaily(DailyStats &data);

    void saveLifetime(const LifetimeStats &data);
    void loadLifetime(LifetimeStats &data);

    void nextDay();
    uint8_t getIndex();

private:
    uint8_t _cs;
    uint8_t _maxDays;
    uint8_t _index;

    const uint16_t LIFETIME_ADDR = 0x0000;

    uint16_t baseDailyAddr();

    void csLow();
    void csHigh();

    void writeEnable();
    void waitWrite();
};