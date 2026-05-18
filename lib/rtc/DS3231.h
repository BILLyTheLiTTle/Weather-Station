#pragma once

#include <Arduino.h>
#include <DS3232RTC.h>

class DS3231 {
private:
    DS3232RTC rtc;

public:
    DS3231();
    bool begin(bool force);
    void updateWithSystemTime();
    void reset();

    void setAlarm(uint8_t hour, uint8_t minute, uint8_t second);
    void setRecurringMinutesAlarm(uint8_t minutes);
    void clearAlarm();
    bool alarmFired();
    
    uint8_t  getDay();
    uint8_t  getMonth();
    uint16_t getYear();
    uint8_t  getHour();
    uint8_t  getMinute();
    uint8_t  getSecond();
};