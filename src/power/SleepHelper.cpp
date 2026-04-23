#pragma once

#include "SleepHelper.h"

void enterConditionalSleep(SleepMode &flag){
    // 1. Constantly update the switch state to catch real-time changes
    flag.update();

    // If the state is SLEEP, enter low-power mode immediately.
    // We don't wait for the interval timer to trigger sleep.
    if (flag.getState() == SystemState::SLEEP) {
        Serial.println(F("System going to sleep..."));
        flag.enable(); 
        
        // Execution resumes here after the interrupt wakes up the MCU.
        // Update state immediately to reflect the wake-up trigger.
        flag.update();
    }
}