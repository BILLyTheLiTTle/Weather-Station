#pragma once

#include <Arduino.h>

class Debugger {
  private:
    bool _enabled;

  public:
    Debugger(bool enabled = true);
    void begin(uint32_t baud = 9600);
    void setEnabled(bool enabled);
    void separator();

    // For variable (int, float, long, etc)
    template <typename T>
    void log(T msg) {
      if (_enabled) Serial.println(msg);
    }

    // For literals in Flash
    void log(const __FlashStringHelper* msg) {
      if (_enabled) Serial.println(msg);
    }

    // For label in Flash and a variable
    template <typename T>
    void log(const __FlashStringHelper* label, T value) {
      if (_enabled) {
        Serial.print(label);
        Serial.print(F(": "));
        Serial.println(value);
      }
    }
};

extern Debugger dbg;