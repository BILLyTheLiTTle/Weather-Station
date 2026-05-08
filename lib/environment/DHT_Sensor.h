#pragma once

#include "DHTStable.h"

class DHT_Sensor {
private:
    DHTStable _dht;
    uint8_t _pin;
    uint8_t _type; // 11 for DHT11, 22 for DHT22

public:
    // Constructor
    DHT_Sensor(uint8_t pin, uint8_t type);

    int16_t getTemperature(); // return centiCelsius to avoid floats
    uint16_t getHumidity(); // return percentage x100 to avoid floats

    static const int16_t INVALID_VALUE = DHTLIB_INVALID_VALUE;
};