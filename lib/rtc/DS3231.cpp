#include "DS3231.h"
#include "Debugger.h"
#include <TimeLib.h> 

DS3231::DS3231() : rtc(false) {}

bool DS3231::begin() {
    rtc.begin();
    // Στην v2.x η oscStopped δεν δέχεται πλέον παραμέτρους
    return (rtc.oscStopped() == false);
}

void DS3231::updateWithSystemTime() {
    const char *monthNames = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char m[4];
    int d, y, hh, mm, ss;
    
    sscanf(__DATE__, "%s %d %d", m, &d, &y);
    sscanf(__TIME__, "%d:%d:%d", &hh, &mm, &ss);
    
    tmElements_t tm;
    tm.Second = ss;
    tm.Minute = mm;
    tm.Hour = hh;
    tm.Day = d;
    tm.Month = (strstr(monthNames, m) - monthNames) / 3 + 1;
    tm.Year = y - 1970;
    
    rtc.write(tm);
    DBG_LN(__TIME__);
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