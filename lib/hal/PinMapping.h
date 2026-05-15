#include "IHardware.h"

struct Pin {
    static constexpr uint8_t SYSTEM_TEMP_THERMISTOR = A0;
    static constexpr uint8_t BATTERY_VOLTAGE_DIVIDER = A1;
    static constexpr uint8_t ACS712 = A2;
    // static constexpr uint8_t EMPTY = A3;
    // static constexpr uint8_t I2C_SDA = A4;
    // static constexpr uint8_t I2C_SCL = A5;
    // static constexpr uint8_t EMPTY = A6;
    // static constexpr uint8_t EMPTY = A7;
    
    // static constexpr uint8_t D0 = 0; // TX USB
    // static constexpr uint8_t D1 = 1; // RX USB
    static constexpr uint8_t INT_RTC = 2;
    static constexpr uint8_t INT_MANUAL = 3;
    // static constexpr uint8_t EMPTY = 4;
    // static constexpr uint8_t EMPTY = 5;
    // static constexpr uint8_t EMPTY = 6;
    // static constexpr uint8_t EMPTY = 7;
    static constexpr uint8_t SCREEN_COTROLLER = 8;
    static constexpr uint8_t DHT = 9;
    static constexpr uint8_t EEPROM_SS = 10;
    // static constexpr uint8_t SPI_MOSI = 11;
    // static constexpr uint8_t SPI_MISO = 12;
    // static constexpr uint8_t SPI SCK = 13; 
};