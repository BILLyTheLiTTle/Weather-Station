#ifndef DS3231_H
#define DS3231_H

#include <Arduino.h>
#include "RTClib.h"

class DS3231 {
private:
    RTC_DS3231 rtc;

public:
    bool begin();
    void updateWithSystemTime();
    
    uint8_t  getDay();
    uint8_t  getMonth();
    uint16_t getYear();
    uint8_t  getHour();
    uint8_t  getMinute();
    uint8_t  getSecond();
};

#endif