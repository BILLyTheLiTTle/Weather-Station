#include "SleepMode.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>

volatile bool SleepMode::_interruptFlag = false;
SleepMode* SleepMode::_instance = nullptr;

SleepMode::SleepMode(uint8_t pin, uint16_t debounceMs)
    : _pin(pin),
      _debounceMs(debounceMs),
      _state(SystemState::SLEEP)
{}

void SleepMode::begin() {
    _instance = this;

    pinMode(_pin, INPUT_PULLUP);

    // IMPORTANT: start with FALLING only (stable wake trigger)
    // attachInterrupt(digitalPinToInterrupt(_pin), isrHandler, FALLING);

    _state = (digitalRead(_pin) == LOW)
                ? SystemState::ACTIVE
                : SystemState::SLEEP;
}

void SleepMode::isrHandler() {
    // ultra minimal ISR: NO millis, NO logic
    _interruptFlag = true;
}

void SleepMode::update() {
    // truth comes ONLY from pin
    bool isLow = (digitalRead(_pin) == LOW);

    _state = isLow ? SystemState::ACTIVE : SystemState::SLEEP;

    if (_interruptFlag) {
        _interruptFlag = false;
    }
}

void SleepMode::enable() {
    if (digitalRead(_pin) == LOW) {
        return; 
    }

    // Serial activity can prevent deep sleep or cause immediate wake-up.
    // Ensure the Serial buffer is fully empty before sleeping.
    if (Serial) { 
        Serial.flush(); 
    }

    // Save ADC settings and power it down
    byte adcsra_save = ADCSRA;
    ADCSRA = 0; 

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
    // Disable every interrupt
    noInterrupts(); 
    
    sleep_enable();
    attachInterrupt(digitalPinToInterrupt(_pin), isrHandler, LOW); // Το LOW είναι πιο αξιόπιστο για ξύπνημα από Power Down

    // Reenable interrupts to start listening the outer world
    interrupts(); 
    sleep_cpu(); 

    // Here Arduino wakes up again and continues from here
    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(_pin));
    ADCSRA = adcsra_save; // Restore ADC state and re-enable it
    
    _state = SystemState::ACTIVE;
}

SystemState SleepMode::getState() {
    return _state;
}