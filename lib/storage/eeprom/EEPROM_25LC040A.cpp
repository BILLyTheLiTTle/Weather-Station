#include "EEPROM_25LC040A.h"

// SPI commands
#define WREN  0x06
#define WRITE 0x02
#define READ  0x03
#define RDSR  0x05

// -------------------- CONSTRUCTOR --------------------

EEPROM_25LC040A::EEPROM_25LC040A(uint8_t csPin) {
    _cs = csPin;
}

// -------------------- INIT --------------------

void EEPROM_25LC040A::begin() {
    pinMode(_cs, OUTPUT);
    csHigh();
    SPI.begin();
}

// -------------------- CHIP SELECT --------------------

void EEPROM_25LC040A::csLow() {
    digitalWrite(_cs, LOW);
}

void EEPROM_25LC040A::csHigh() {
    digitalWrite(_cs, HIGH);
}

// -------------------- WRITE CONTROL --------------------

void EEPROM_25LC040A::writeEnable() {
    csLow();
    SPI.transfer(WREN);
    csHigh();
}

void EEPROM_25LC040A::waitWrite() {
    csLow();
    SPI.transfer(RDSR);

    while (SPI.transfer(0x00) & 0x01) {
        // wait write in progress
    }

    csHigh();
}

// -------------------- LOW LEVEL IO --------------------

void EEPROM_25LC040A::writeBytes(uint16_t addr, const void *data, uint16_t len) {
    writeEnable();

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

    csLow();

    SPI.transfer(WRITE);
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);

    const uint8_t *ptr = (const uint8_t*)data;
    for (uint16_t i = 0; i < len; i++) {
        SPI.transfer(ptr[i]);
    }

    csHigh();

    SPI.endTransaction();

    waitWrite();
}

void EEPROM_25LC040A::readBytes(uint16_t addr, void *data, uint16_t len) {
    csLow();
    SPI.transfer(READ);
    SPI.transfer(addr >> 8);
    SPI.transfer(addr & 0xFF);

    uint8_t *ptr = (uint8_t*)data;
    for (uint16_t i = 0; i < len; i++) {
        ptr[i] = SPI.transfer(0x00);
    }

    csHigh();
}

// -------------------- MEMORY MAP --------------------

uint16_t EEPROM_25LC040A::baseDailyAddr() {
    return sizeof(LifetimeStats);
}

// -------------------- RING BUFFER CONTROL --------------------

void EEPROM_25LC040A::initSystem(uint8_t maxDays) {
    _maxDays = maxDays;
    _index = 0;
}

uint8_t EEPROM_25LC040A::getIndex() {
    return _index;
}

void EEPROM_25LC040A::nextDay() {
    _index = (_index + 1) % _maxDays;
}

// -------------------- DAILY --------------------

void EEPROM_25LC040A::saveDaily(uint8_t index, const DailyStats &data) {
    uint16_t addr = baseDailyAddr() + index * sizeof(DailyStats);
    writeBytes(addr, &data, sizeof(DailyStats));
}

void EEPROM_25LC040A::saveDaily(const DailyStats &data) {
    saveDaily(0, data);
}

void EEPROM_25LC040A::loadDaily(uint8_t index, DailyStats &data) {
    uint16_t addr = baseDailyAddr() + index * sizeof(DailyStats);
    readBytes(addr, &data, sizeof(DailyStats));
}

void EEPROM_25LC040A::loadDaily(DailyStats &data) {
    loadDaily(0, data);
}

// -------------------- LIFETIME --------------------

void EEPROM_25LC040A::saveLifetime(const LifetimeStats &data) {
    writeBytes(LIFETIME_ADDR, &data, sizeof(LifetimeStats));
}

void EEPROM_25LC040A::loadLifetime(LifetimeStats &data) {
    readBytes(LIFETIME_ADDR, &data, sizeof(LifetimeStats));
}