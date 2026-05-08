#include "DHT_Sensor.h"

DHT_Sensor::DHT_Sensor(uint8_t pin, uint8_t type) {
    _pin = pin;
    _type = type;
}

int16_t DHT_Sensor::getTemperature() {
    int16_t chk;
    if (_type == 22) {
        chk = _dht.read22(_pin);
    } else {
        chk = _dht.read11(_pin);
    }

    if (chk == DHTLIB_OK) {
        return _dht.getTemperature() * 100;
    } else {
        return INVALID_VALUE;
    }
}

uint16_t DHT_Sensor::getHumidity() {
    if (_dht.getHumidity() == 0) { 
        if (_type == 22) {
            _dht.read22(_pin);
        }
        else {
            _dht.read11(_pin);
        }
    }
    
    return (_dht.getHumidity() != 0) ? _dht.getHumidity() * 100 : INVALID_VALUE;
}