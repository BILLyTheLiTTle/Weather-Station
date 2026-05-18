#pragma once

#ifndef COMPILING_FOR_PC
  #include <Arduino.h>

  #include <Wire.h>
  
  #include "SSD1306Ascii.h"
  #include "SSD1306AsciiWire.h"
#else
  #include <stdint.h>
  #include <cmath>

  #define HIGH 0x1
  #define LOW  0x0

  #define INPUT 0x0
  #define OUTPUT 0x1
  #define INPUT_PULLUP 0x2

#endif

class IHardware {
public:
    virtual ~IHardware() {}

    virtual void pinMode(uint8_t pin, uint8_t mode) = 0;

    virtual int digitalRead(uint8_t pin) = 0;
    virtual void digitalWrite(uint8_t pin, uint8_t val) = 0;

    virtual uint16_t analogRead(uint8_t pin) = 0;

    virtual void delay(uint32_t ms) = 0;
    virtual uint32_t millis() = 0;
};