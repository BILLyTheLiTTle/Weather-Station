#include "DHT_Sensor.h"

DHT_Sensor::DHT_Sensor(uint8_t pin, SensorType type) {
    _pin = pin;
    _type = type;
}

int16_t DHT_Sensor::getTemperature() {
    int16_t chk = (_type == 22) ? _dht.read22(_pin) : _dht.read11(_pin);

    if (chk == DHTLIB_OK) {
        float temp = _dht.getTemperature();
        
        if (temp < -40.0f || temp > 80.0f) {
            _lastTemp = INVALID_TEMPERATURE;
            return INVALID_TEMPERATURE;
        }
        _lastTemp = temp * 100;
        return (int16_t)(temp * 100);
    }
    _lastTemp = INVALID_TEMPERATURE;
    return INVALID_TEMPERATURE;
}

uint16_t DHT_Sensor::getHumidity() {
    float hum = _dht.getHumidity();

    if (_lastTemp == INVALID_TEMPERATURE) {
        _lastHum = INVALID_HUMIDITY;
        return INVALID_HUMIDITY;
    }
    
    // if (hum < 0 || hum > 100) {
        int16_t chk = (_type == 22) ? _dht.read22(_pin) : _dht.read11(_pin);
        if (chk != DHTLIB_OK) {
            _lastHum = INVALID_HUMIDITY;
            return INVALID_HUMIDITY;
        }
        hum = _dht.getHumidity();
    // }

    if (hum >= 0.0f && hum <= 100.0f) { // TODO verify it is not normal for humidity to go above 100%
        _lastHum = hum * 100;
        return (uint16_t)(hum * 100);
    }
    
    return INVALID_HUMIDITY;
}