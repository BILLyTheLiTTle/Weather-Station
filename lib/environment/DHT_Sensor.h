#pragma once

#include "DHTStable.h"

class DHT_Sensor {
public:
    enum SensorType : uint8_t {
        DHT11 = 11,
        DHT22 = 22
    };
    DHT_Sensor(uint8_t pin, SensorType type);

    int16_t getTemperature(); // return centiCelsius to avoid floats
    uint16_t getHumidity(); // return percentage x100 to avoid floats

    static const int16_t INVALID_VALUE = DHTLIB_INVALID_VALUE;

private:
    DHTStable _dht;
    uint8_t _pin;
    SensorType _type;


};