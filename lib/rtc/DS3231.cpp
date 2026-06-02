#include "DS3231.h"
#include "Debugger.h"
#include <TimeLib.h> 

DS3231::DS3231() : rtc(false) {}

bool DS3231::begin(bool force) {
    rtc.begin();

    if (force) {
        // In case the clock is not set of mulfunctioning the RTC will block us here. Adjust RTC clock to release the hold.
        updateWithSystemTime(); 
    }

    return (rtc.oscStopped() == false);
}

void DS3231::updateWithSystemTime() {
    uint8_t hh = ((__TIME__[0] - '0') * 10) + (__TIME__[1] - '0');
    uint8_t mm = ((__TIME__[3] - '0') * 10) + (__TIME__[4] - '0');
    uint8_t ss = ((__TIME__[6] - '0') * 10) + (__TIME__[7] - '0');

    uint16_t y = ((__DATE__[7] - '0') * 1000) + ((__DATE__[8] - '0') * 100) + ((__DATE__[9] - '0') * 10) + (__DATE__[10] - '0');
    
    uint8_t d = 0;
    if (__DATE__[4] == ' ') {
        d = __DATE__[5] - '0';
    } else {
        d = ((__DATE__[4] - '0') * 10) + (__DATE__[5] - '0');
    }

    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    uint8_t month_index = 1;
    for (uint8_t i = 0; i < 12; i++) { // uint8_t και για τον μετρητή του loop
        if (__DATE__[0] == months[i*3] && __DATE__[1] == months[i*3+1] && __DATE__[2] == months[i*3+2]) {
            month_index = i + 1;
            break;
        }
    }

    tmElements_t tm;
    tm.Second = ss;
    tm.Minute = mm;
    tm.Hour   = hh;
    tm.Day    = d;
    tm.Month  = month_index;
    tm.Year   = (uint8_t)(y - 1970);

    rtc.write(tm);
    
    DBG(F("RTC Successfully Calibrated: "));
    DBG_LN(__TIME__);
}

void DS3231::reset() {
    tmElements_t tm;
    
    tm.Second = 0;
    tm.Minute = 0;
    tm.Hour   = 0;
    tm.Day    = 1;
    tm.Month  = 1;
    tm.Year   = 2026 - 1970;

    rtc.write(tm);
    
    DBG_LN(F("RTC Reset autonomously without PC!"));
}

void DS3231::setAlarm(uint8_t hour, uint8_t minute, uint8_t second) {
    // Καθαρισμός προηγούμενου alarm flag
    rtc.alarm(DS3232RTC::ALARM_NBR_t::ALARM_1); 
    
    // Ρύθμιση του Alarm 1
    rtc.setAlarm(DS3232RTC::ALARM_TYPES_t::ALM1_MATCH_HOURS, second, minute, hour, 0);
    
    // Ενεργοποίηση του Interrupt pin
    rtc.alarmInterrupt(DS3232RTC::ALARM_NBR_t::ALARM_1, true);
}

void DS3231::setRecurringMinutesAlarm(uint8_t minutesToAdd) {
    time_t now = rtc.get();
    time_t next = now + (minutesToAdd * 60UL);
    
    rtc.alarm(DS3232RTC::ALARM_NBR_t::ALARM_1);
    
    rtc.setAlarm(DS3232RTC::ALARM_TYPES_t::ALM1_MATCH_HOURS, second(next), minute(next), hour(next), 0);
    
    rtc.alarmInterrupt(DS3232RTC::ALARM_NBR_t::ALARM_1, true);
    
    DBG_STAT(F("Next Alarm at min"), minute(next));
}

void DS3231::clearAlarm() {
    rtc.alarm(DS3232RTC::ALARM_NBR_t::ALARM_1);
    rtc.alarmInterrupt(DS3232RTC::ALARM_NBR_t::ALARM_1, false);
}

bool DS3231::alarmFired() {
    // Επιστρέφει true αν το flag είναι ενεργό και το μηδενίζει
    return rtc.alarm(DS3232RTC::ALARM_NBR_t::ALARM_1);
}

uint8_t DS3231::getHour()   { return hour(rtc.get()); }
uint8_t DS3231::getMinute() { return minute(rtc.get()); }
uint8_t DS3231::getSecond() { return second(rtc.get()); }
uint8_t DS3231::getDay()    { return day(rtc.get()); }
uint8_t DS3231::getMonth()  { return month(rtc.get()); }
uint16_t DS3231::getYear()  { return year(rtc.get()); }
time_t DS3231::getTimestamp()  { return rtc.get(); }