#pragma once

#include "SleepHelper.h"

void enterConditionalSleep(SleepMode &mode){
    // 1. Constantly update the switch state to catch real-time changes
    mode.update();

    // If the state is SLEEP, enter low-power mode immediately.
    // We don't wait for the interval timer to trigger sleep.
    if (mode.getState() == SystemState::SLEEP) {
        Serial.println(F("System going to sleep..."));
        mode.enable(); 
        
        // Execution resumes here after the interrupt wakes up the MCU.
        // Update state immediately to reflect the wake-up trigger.
        mode.update();
    }
}