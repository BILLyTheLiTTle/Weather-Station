#pragma once

#include <stdint.h>
#include "ACS712_Model.h"
#include "../hal/IHardware.h"

class ACS712 {
  private:
    IHardware* _hw;
    int8_t     _pin;
    ACS712_Model _sensitivity; // mV/A (185 for 05B)
    uint16_t   _vOffsetRaw; // // Raw ADC value with no load
    uint16_t   _capacityMAH;
    
    static constexpr uint16_t CALIBRATION_SAMPLES = 500;
    static constexpr uint8_t  MEASUREMENT_SAMPLES = 200;

  public:
    ACS712(IHardware* hw, int8_t pin, ACS712_Model sens, uint16_t capacity);
    
    void     calibrate();
    uint32_t getCurrentMA();    
    uint32_t getRemainingMinutes();
};