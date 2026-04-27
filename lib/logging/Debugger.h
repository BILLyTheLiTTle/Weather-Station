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

    // 1. Για κανονικές μεταβλητές (int, float, long κτλ)
    template <typename T>
    void log(T msg) {
      if (_enabled) Serial.println(msg);
    }

    // 2. Ειδικά για κείμενα στη Flash μέσω F("...") - ΜΗΔΕΝ RAM
    void log(const __FlashStringHelper* msg) {
      if (_enabled) Serial.println(msg);
    }

    // 3. Για Label και Τιμή (π.χ. dbg.logValue(F("Temp"), s))
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