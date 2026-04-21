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

    /* * WARNING: Avoid attaching a LOW-level interrupt during initialization.
     * 1. CPU Starvation: A LOW-level interrupt is level-triggered. If the pin is LOW 
     * at startup, the ISR fires continuously (put a `Serial.println(0)` in `isrHandler` to see the problem), preventing the main loop from executing.
     * 2. Serial Corruption: Continuous ISR execution disrupts UART timing and buffer 
     * handling, leading to "frozen" or corrupted Serial output (undefined behavior).
     * 3. Signal Noise: Mechanical bouncing during power-up can trigger a cascade of 
     * interrupts before the voltage stabilizes.
     *
     * During my experiments I was falling to case No1.
     * 
     * Best Practice: Sample the state manually via digitalRead() during begin() 
     * and only attach the LOW interrupt right before entering Power-Down sleep.
     */
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
    attachWakeInterruptors();

    // Reenable interrupts to start listening the outer world
    interrupts(); 
    sleep_cpu(); 

    // Here Arduino wakes up again and continues from here
    sleep_disable();
    detachWakeInterruptors();
    ADCSRA = adcsra_save; // Restore ADC state and re-enable it
    
    _state = SystemState::ACTIVE;
}

    void SleepMode::attachWakeInterruptors() {
        // Internal clock stops during sleep so it cannot detect voltage changing (RISING, FALLING, CHANGE).
        // Without the clock CPU is blind to voltage changes. This applies to old Arduino (Uno, Nano, Mega)
        attachInterrupt(digitalPinToInterrupt(_pin), isrHandler, LOW);
    }

    void SleepMode::detachWakeInterruptors() {
        detachInterrupt(digitalPinToInterrupt(_pin));
    }

SystemState SleepMode::getState() {
    return _state;
}