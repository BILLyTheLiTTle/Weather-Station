#include "EEPROM_25LC040A.h"
#include "./data/MemoryMapping.h"

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

// --- SINGLE BYTE OPERATIONS (The Foundation) ---

void EEPROM_25LC040A::writeByte(uint16_t addr, uint8_t data) {
    writeEnable(); // Allow writing

    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    csLow();

    // Handle 9th bit of address (A8) inside the instruction
    uint8_t instruction = WRITE;
    if (addr & 0x0100) instruction |= 0x08; 

    SPI.transfer(instruction);
    SPI.transfer(addr & 0xFF); // Send the rest 8 bits
    SPI.transfer(data);

    csHigh();
    SPI.endTransaction();
    
    waitWrite(); // Wait for the EEPROM to finish physical storage
}

uint8_t EEPROM_25LC040A::readByte(uint16_t addr) {
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    csLow();

    uint8_t instruction = READ;
    if (addr & 0x0100) instruction |= 0x08; 

    SPI.transfer(instruction);
    SPI.transfer(addr & 0xFF);
    uint8_t data = SPI.transfer(0x00);

    csHigh();
    SPI.endTransaction();
    
    return data;
}

// --- MULTI-BYTE OPERATIONS (The Reusable Layer) ---

void EEPROM_25LC040A::writeBytes(uint16_t addr, const void *data, uint16_t len) {
    const uint8_t *ptr = (const uint8_t*)data;

    for (uint16_t i = 0; i < len; i++) {
        // Reuse the single byte function to bypass pagination logic
        writeByte(addr + i, ptr[i]);
    }
}

void EEPROM_25LC040A::readBytes(uint16_t addr, void *data, uint16_t len) {
    uint8_t *ptr = (uint8_t*)data;
    
    // Note: We don't reuse readByte here because the 25LC040A supports 
    // "Sequential Read". It's much faster to keep CS LOW and read everything.
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    csLow();

    uint8_t instruction = READ;
    if (addr & 0x0100) instruction |= 0x08; 

    SPI.transfer(instruction);
    SPI.transfer(addr & 0xFF);

    for (uint16_t i = 0; i < len; i++) {
        ptr[i] = SPI.transfer(0x00);
    }

    csHigh();
    SPI.endTransaction();
}

void EEPROM_25LC040A::factoryReset() {    
    for (uint16_t i = 0; i < 512; i++) {
        writeByte(i, 0xFF);
    }
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

void EEPROM_25LC040A::saveDailyTemperature(uint8_t index, const TemperatureDailyStats &data) {
    uint16_t addr = dailyStatsAddress + index * sizeof(TemperatureDailyStats);
    writeBytes(addr, &data, sizeof(TemperatureDailyStats));
}

void EEPROM_25LC040A::saveDailyTemperature(const TemperatureDailyStats &data) {
    saveDailyTemperature(0, data);
}

void EEPROM_25LC040A::loadDailyTemperature(uint8_t index, TemperatureDailyStats &data) {
    uint16_t addr = dailyStatsAddress + index * sizeof(TemperatureDailyStats);
    readBytes(addr, &data, sizeof(TemperatureDailyStats));
}

void EEPROM_25LC040A::loadDailyTemperature(TemperatureDailyStats &data) {
    loadDailyTemperature(0, data);
}

// -------------------- LIFETIME --------------------

void EEPROM_25LC040A::saveLifetimeTemperature(const TemperatureLifetimeStats &data) {
    writeBytes(lifetimeStatsAddress, &data, sizeof(TemperatureLifetimeStats));
}

void EEPROM_25LC040A::loadLifetimeTemperature(TemperatureLifetimeStats &data) {
    readBytes(lifetimeStatsAddress, &data, sizeof(TemperatureLifetimeStats));
}

// -------------------- OTHER --------------------
void EEPROM_25LC040A::storeDataVersion() {
    writeByte(VERSION_ADDRESS, version);
}

uint8_t EEPROM_25LC040A::loadDataVersion() {
    return readByte(VERSION_ADDRESS);
}
