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
}

uint8_t  DS3231::getDay()    { return rtc.now().day(); }
uint8_t  DS3231::getMonth()  { return rtc.now().month(); }
uint16_t DS3231::getYear()   { return rtc.now().year(); }
uint8_t  DS3231::getHour()   { return rtc.now().hour(); }
uint8_t  DS3231::getMinute() { return rtc.now().minute(); }
uint8_t  DS3231::getSecond() { return rtc.now().second(); }