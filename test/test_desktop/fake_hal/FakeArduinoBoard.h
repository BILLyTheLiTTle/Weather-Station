#include "../hal/IHardware.h"

class FakeArduinoBoard : public IHardware {
public:
    uint16_t analogReadValue = 512;
    
    int digitalReadValue = 0;
    uint8_t lastWrittenPin = 0;
    uint8_t lastWrittenValue = 0;

    uint8_t lastPinMode = 0;

    uint32_t currentTimeMs = 0;

    uint16_t analogRead(uint8_t pin) override { 
        return analogReadValue; 
    }
    
    void delay(uint32_t ms) override { 
        // We need to actual delay in unit tests
        (void (0));
    }

    uint32_t millis() override { return currentTimeMs; }
    
    int digitalRead(uint8_t pin) override {
        return digitalReadValue;
    }
    
    void digitalWrite(uint8_t pin, uint8_t val) override {
        lastWrittenPin = pin;
        lastWrittenValue = val;
    }

    void pinMode(uint8_t pin, uint8_t mode) override {
        // Απλά καταγράφουμε ότι κλήθηκε η pinMode με αυτές τις παραμέτρους
        lastPinMode = mode; 
    }
};