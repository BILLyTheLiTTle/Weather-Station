#include "Debugger.h"

Debugger::Debugger(bool enabled) {
  _enabled = enabled;
}

void Debugger::begin(uint32_t baud) {
  if (_enabled) {
    Serial.begin(baud);
    Serial.println(F(">>> Debugger Ready"));
  }
}

void Debugger::setEnabled(bool enabled) {
  _enabled = enabled;
}

void Debugger::separator() {
  if (_enabled) Serial.println(F("--------------------------"));
}

// Αρχικοποίηση αντικειμένου
Debugger dbg(true);