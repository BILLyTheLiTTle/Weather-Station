#include "Thermistor.h"
#include <math.h>

Thermistor::Thermistor(uint8_t analogPin,
                       float seriesResistor,
                       float nominalResistance,
                       float nominalTemperature,
                       float betaCoefficient)
    : _pin(analogPin),
      _seriesResistor(seriesResistor),
      _nominalResistance(nominalResistance),
      _nominalTemperature(nominalTemperature),
      _beta(betaCoefficient) {}

Temperature Thermistor::readTemperatureC() {
    const float manualCalibration = 1;//1.0688;
    uint16_t adc = analogRead(_pin);

    if (adc <= 0) {
        _sensorStatus = Temperature::SHORT_TO_GND;
        return {-273.15, _sensorStatus};
    }

    if (adc >= 1023) {
        _sensorStatus = Temperature::SHORT_TO_VCC;
        return {-273.15, _sensorStatus};
    }

    float resistance = _seriesResistor * (adc / (1023.0 - adc));

    float steinhart;
    steinhart = resistance / _nominalResistance;      // (R/Ro)
    steinhart = log(steinhart);                       // ln(R/Ro)
    steinhart /= _beta;                               // 1/B * ln(...)
    steinhart += 1.0 / (_nominalTemperature + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                      // Invert
    steinhart -= 273.15;                              // K -> C

    _sensorStatus = Temperature::OK;
    return {steinhart * manualCalibration, _sensorStatus};
}

Temperature Thermistor::readTemperatureK() {
    return {(readTemperatureC().value + 273.15), _sensorStatus};
}

Temperature Thermistor::readTemperatureF() {
    return {(readTemperatureC().value * 9.0 / 5.0 + 32.0), _sensorStatus};
}

Temperature::SensorStatus Thermistor::getStatus() const {
    return _sensorStatus;
}