#pragma once

#include <Arduino.h>
#include "SystemState.h"

class SleepMode {
public:
    SleepMode(uint8_t pin, uint16_t debounceMs = 50);

    void begin();
    void update();
    void enable();

    SystemState getState();

private:
    static void isrHandler();

    static volatile bool _interruptFlag;

    static SleepMode* _instance;

    void attachWakeInterruptors();
    void detachWakeInterruptors();

    uint8_t _pin;
    uint16_t _debounceMs;

    SystemState _state;
};