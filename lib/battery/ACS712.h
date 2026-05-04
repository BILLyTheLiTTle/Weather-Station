#pragma once

#include <Arduino.h>
#include "ACS712_Model.h"

class ACS712 {
  private:
    int8_t   _pin;
    ACS712_Model _sensitivity;  // mV/A (185 για τον 05B)
    uint16_t _vOffsetRaw;   // Raw τιμή ADC (κέντρο)
    uint16_t _capacityMAH;  // Χωρητικότητα μπαταρίας σε mAh

    static constexpr uint16_t CALIBRATION_SAMPLES = 500;
    static constexpr uint8_t MEASUREMENT_SAMPLES = 200;

  public:
    ACS712(int8_t pin, ACS712_Model sens, uint16_t capacity);
    
    void     calibrate();
    uint32_t getCurrentMA();    
    uint32_t getRemainingMinutes();
};