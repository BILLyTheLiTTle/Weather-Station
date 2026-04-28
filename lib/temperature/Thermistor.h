#pragma once

#include <Arduino.h>

#include "ThermistorConstants.h"
#include "Temperature.h"

class Thermistor {
public:
    Thermistor(uint8_t analogPin,
               uint16_t seriesResistor,
               uint16_t nominalResistance,
               int8_t nominalTemperature,
               uint16_t betaCoefficient);

    // returns temprature value as centiCelsius to avoid floats
    Temperature readTemperatureC();
    Temperature readTemperatureK();
    Temperature readTemperatureF();

    Temperature::SensorStatus getStatus() const;

private:
    uint8_t _pin;
    uint16_t _seriesResistor;
    uint16_t _nominalResistance;
    int8_t _nominalTemperature;
    uint16_t _beta;
    enum Temperature::SensorStatus _sensorStatus = Temperature::INVALID;
};