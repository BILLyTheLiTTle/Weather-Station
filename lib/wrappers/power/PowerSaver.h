#pragma once

#include "SleepMode.h"

#define INTERVAL_BETWEEN_ACTIONS 10000UL

void enterConditionalSleep(SleepMode &flag);

bool isIntervalElapsed();