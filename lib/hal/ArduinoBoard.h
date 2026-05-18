#include "IHardware.h"

class ArduinoBoard : public IHardware {
public:
    void pinMode(uint8_t pin, uint8_t mode) override { ::pinMode(pin, mode); }

    int digitalRead(uint8_t pin) override { return ::digitalRead(pin); }
    void digitalWrite(uint8_t pin, uint8_t val) override { ::digitalWrite(pin, val); }

    uint16_t analogRead(uint8_t pin) override { return ::analogRead(pin); }

    void delay(uint32_t ms) override { ::delay(ms); }
    uint32_t millis() override { ::millis(); }
};