#pragma once

#include <Arduino.h>
#include "ACS712_Model.h"

class ACS712 {
  private:
    int8_t   _pin;
    uint16_t _sensitivity;  // mV/A (185 για τον 05B)
    uint16_t _vOffsetRaw;   // Raw τιμή ADC (κέντρο)
    uint16_t _capacityMAH;  // Χωρητικότητα μπαταρίας σε mAh

  public:
    ACS712(int8_t pin, uint16_t sens, uint16_t capacity);
    
    void     calibrate();
    uint32_t getCurrentMA();    
    uint32_t getRemainingMinutes();
};