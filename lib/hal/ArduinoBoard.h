#include "IHardware.h"

class ArduinoBoard : public IHardware {
public:
    uint16_t analogRead(uint8_t pin) override { return ::analogRead(pin); }
    void delay(uint32_t ms) override { ::delay(ms); }
};