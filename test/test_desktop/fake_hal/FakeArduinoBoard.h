#include "../hal/IHardware.h"

class FakeArduinoBoard : public IHardware {
public:
    uint16_t analogReadValue = 512; // Η τιμή που θα "επιστρέφει" το analogRead
    
    uint16_t analogRead(uint8_t pin) override { 
        return analogReadValue; 
    }
    
    void delay(uint32_t ms) override { 
        // We need to actual delay in unit tests
        (void (0));
    }
    
    
};