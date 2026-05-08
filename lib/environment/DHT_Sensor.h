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

    static const int16_t INVALID_TEMPERATURE = DHTLIB_INVALID_VALUE;
    static const uint16_t INVALID_HUMIDITY = 0xFFFF;

private:
    DHTStable _dht;
    uint8_t _pin;
    SensorType _type;
    int16_t _lastTemp = INVALID_TEMPERATURE;
    uint16_t _lastHum = INVALID_HUMIDITY;
};