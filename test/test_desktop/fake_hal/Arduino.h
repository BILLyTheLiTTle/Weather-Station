#pragma once
#include <cstddef>
#include <cstdint>

using std::size_t;

#define HIGH 0x1
#define LOW  0x0
#define INPUT_PULLUP 0x2
#define INPUT 0x0
#define OUTPUT 0x1

#define A0 14
#define A1 15
#define A2 16
#define A3 17

// --- Διόρθωση για τον MemoryProfiler ---
// Ορίζουμε τους τύπους ακριβώς όπως τους κάνει extern ο κώδικάς σου
extern int __bss_end;
extern int *__brkval;
inline uint16_t SP = 0xFFFF;

// --- Προσθήκη βασικών συναρτήσεων που έλειπαν ---
inline void pinMode(uint8_t pin, uint8_t mode) {}
inline void digitalWrite(uint8_t pin, uint8_t val) {}
inline int digitalRead(uint8_t pin) { return HIGH; }
inline uint16_t analogRead(uint8_t pin) { return 0; }

inline void delay(uint32_t ms) {}
inline uint32_t millis() { return 0; }

class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper*>(string_literal))