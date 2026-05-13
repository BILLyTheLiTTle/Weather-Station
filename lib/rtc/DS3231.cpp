#include "DS3231.h"

bool DS3231::begin() {
    uint8_t retry = 0;
    while (retry < 3) {
        if (rtc.begin()) return true;
        retry++;
        delay(50);
    }
    return false;
}

void DS3231::updateWithSystemTime() {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println(F(__TIME__));
    delay(20);
}

void DS3231::setAlarm(uint8_t hour, uint8_t minute, uint8_t second, Ds3231Alarm1Mode mode) {
    rtc.clearAlarm(1);
    
    rtc.setAlarm1(
        // Random day, month, year since we only care for minutes!
        DateTime(2026, 1, 1, hour, minute, second), 
        mode
    );
}

void DS3231::setRecurringMinutesAlarm(uint8_t minutes) {
    DateTime now = rtc.now();
    DateTime next = now + TimeSpan(0, 0, minutes, 0);
    setAlarm(next.hour(), next.minute(), next.second(), DS3231_A1_Minute);
}

void DS3231::clearAlarm() {
    rtc.clearAlarm(1);
}


bool DS3231::alarmFired() {
    return rtc.alarmFired(1);
}

uint8_t  DS3231::getDay()    { return rtc.now().day(); }
uint8_t  DS3231::getMonth()  { return rtc.now().month(); }
uint16_t DS3231::getYear()   { return rtc.now().year(); }
uint8_t  DS3231::getHour()   { return rtc.now().hour(); }
uint8_t  DS3231::getMinute() { return rtc.now().minute(); }
uint8_t  DS3231::getSecond() { return rtc.now().second(); }