#pragma once
#include <cstdint>

class SPISettings {
public:
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {}
};

class SPIClass {
public:
    void begin() {}
    // Διορθώθηκε: Τώρα δέχεται SPISettings αντί για int
    void beginTransaction(SPISettings settings) {} 
    uint8_t transfer(uint8_t data) { return 0; }
    void endTransaction() {}
};

inline SPIClass SPI;

#define MSBFIRST 1
#define SPI_MODE0 0