#pragma once

#include <Arduino.h>

#include "ThermistorConstants.h"
#include "Temperature.h"

class Thermistor {
public:
    Thermistor(uint8_t analogPin,
               float seriesResistor,
               float nominalResistance,
               float nominalTemperature,
               float betaCoefficient);

    Temperature readTemperatureC();
    Temperature readTemperatureK();
    Temperature readTemperatureF();

    Temperature::SensorStatus getStatus() const;

private:
    uint8_t _pin;
    float _seriesResistor;
    float _nominalResistance;
    float _nominalTemperature;
    float _beta;
    enum Temperature::SensorStatus _sensorStatus = Temperature::INVALID;
};