#pragma once

#include "SleepMode.h"
#include "DS3231.h"

#define INTERVAL_BETWEEN_ACTIONS 10000UL
#define INTERVAL_BETWEEN_RTC_WAKEUPS 15 // minutes

void enterConditionalSleep(DS3231 &rtc, SleepMode &flag);

bool isIntervalElapsed();