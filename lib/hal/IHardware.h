#pragma once

#ifndef COMPILING_FOR_PC
  #include <Arduino.h>
#else
  #include <stdint.h>
  #include <cmath>
#endif

class IHardware {
public:
    virtual ~IHardware() {}
    virtual uint16_t analogRead(uint8_t pin) = 0;
    virtual void delay(uint32_t ms) = 0;
};