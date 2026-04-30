#include "ACS712.h"

ACS712::ACS712(int8_t pin, uint16_t sens, uint16_t capacity) {
    _pin = pin;
    _sensitivity = sens;
    _capacityMAH = capacity;
    _vOffsetRaw = 555; // The raw value of ADC with no power to my ACS (could vary per ACS)
}

void ACS712::calibrate() {
    uint32_t sum = 0;
    const uint16_t calSamples = 500;

    for (uint16_t i = 0; i < calSamples; i++) {
        sum += analogRead(_pin);
        delay(1);
    }
    _vOffsetRaw = (uint16_t)(sum / calSamples);
}

uint32_t ACS712::getCurrentMA() {
    uint32_t sum = 0;
    const uint16_t samples = 200;

    for (uint16_t i = 0; i < samples; i++) {
        sum += analogRead(_pin);
    }
    uint16_t avgRaw = (uint16_t)(sum / samples);

    uint16_t diffRaw;
    if (avgRaw > _vOffsetRaw) {
        diffRaw = avgRaw - _vOffsetRaw;
    } else {
        diffRaw = _vOffsetRaw - avgRaw;
    }

    if (diffRaw < 3) return 0; 

    uint64_t numerator = (uint64_t)diffRaw * 5000000ULL;
    uint64_t denominator = 1024ULL * _sensitivity;
    
    return (uint32_t)(numerator / denominator);
}

uint32_t ACS712::getRemainingMinutes() {
    uint32_t currentMA = getCurrentMA();

    if (currentMA < 5) {
        return 99999; 
    }

    return (_capacityMAH * 60UL) / currentMA;
}