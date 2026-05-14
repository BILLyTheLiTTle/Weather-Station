#include "PowerSaver.h"
#include "SSD1306.h"
#include "Debugger.h"

static uint32_t lastReadTime = 0;

void enterConditionalSleep(DS3231 &rtc, SleepMode &mode){
    // 1. Constantly update the switch state to catch real-time changes
    mode.update();

    // If the state is SLEEP, enter low-power mode immediately.
    // We don't wait for the interval timer to trigger sleep.
    if (mode.getState() == SystemState::SLEEP) {
        rtc.setRecurringMinutesAlarm(INTERVAL_BETWEEN_RTC_WAKEUPS);
        display.sleep();

        DBG_LN(F("System going to sleep..."));
        mode.enable();

        lastReadTime = 0;
        
        // Execution resumes here after the interrupt wakes up the MCU.
        // Update state immediately to reflect the wake-up trigger.
        mode.update();
        display.wake();
        
    }
}

bool isIntervalElapsed() {
    uint32_t now = millis();

    if (lastReadTime != 0 && (now - lastReadTime) < INTERVAL_BETWEEN_ACTIONS) {
        return false;
    }
    lastReadTime = now;

    return true;
}