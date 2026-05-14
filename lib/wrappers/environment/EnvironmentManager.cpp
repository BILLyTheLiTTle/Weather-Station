#include "EnvironmentManager.h"
#include "Debugger.h"

void EnvironmentManager::printEnvironmentStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc,
    TemperatureDailyStats &td, TemperatureLifetimeStats &tl, 
    HumidityDailyStats &hd, HumidityLifetimeStats &hl) {

    printTemperatureStats(dht, eeprom, rtc, td, tl);
    printHumidityStats(dht, eeprom, rtc, hd, hl);
}

bool EnvironmentManager::saveHumidityLifetimeRecord(EEPROM_25LC040A &eeprom, DS3231 &rtc, uint16_t maxHum, uint16_t minHum, HumidityLifetimeStats &life) {
    bool lifetimeChanged = false;

    // ---------------- LIFETIME STATS ----------------
    
    // Lifetime Max check
    if (isnan(life.maxHum) || maxHum > life.maxHum) {
        life.maxHum = maxHum;
        life.maxYear = rtc.getYear();
        life.maxMonth = rtc.getMonth();
        life.maxDay = rtc.getDay();
        life.maxHour = rtc.getHour();
        life.maxMinute = rtc.getMinute();
        lifetimeChanged = true;
    }

    // Lifetime Min check
    if (isnan(life.minHum) || minHum < life.minHum) {
        life.minHum = minHum;
        life.minYear = rtc.getYear();
        life.minMonth = rtc.getMonth();
        life.minDay = rtc.getDay();
        life.minHour = rtc.getHour();
        life.minMinute = rtc.getMinute();
        lifetimeChanged = true;
    }

    // ---------------- SAVE TO EEPROM ----------------    
    if (lifetimeChanged) {
        eeprom.saveLifetimeHumidity(life);
    }

    return lifetimeChanged;
}

bool EnvironmentManager::saveTemperatureLifetimeRecord(EEPROM_25LC040A &eeprom, DS3231 &rtc, int16_t maxTemp, int16_t minTemp, TemperatureLifetimeStats &life) {
    bool lifetimeChanged = false;

    // ---------------- LIFETIME STATS ----------------
    
    // Lifetime Max check
    if (isnan(life.maxTemp) || maxTemp > life.maxTemp) {
        life.maxTemp = maxTemp;
        life.maxYear = rtc.getYear();
        life.maxMonth = rtc.getMonth();
        life.maxDay = rtc.getDay();
        life.maxHour = rtc.getHour();
        life.maxMinute = rtc.getMinute();
        lifetimeChanged = true;
    }

    // Lifetime Min check
    if (isnan(life.minTemp) || minTemp < life.minTemp) {
        life.minTemp = minTemp;
        life.minYear = rtc.getYear();
        life.minMonth = rtc.getMonth();
        life.minDay = rtc.getDay();
        life.minHour = rtc.getHour();
        life.minMinute = rtc.getMinute();
        lifetimeChanged = true;
    }

    // ---------------- SAVE TO EEPROM ----------------    
    if (lifetimeChanged) {
        eeprom.saveLifetimeTemperature(life);
    }

    return lifetimeChanged;
}

bool EnvironmentManager::rememberTemperatureDailyRecord(DS3231 &rtc, int16_t maxTemp, int16_t minTemp, TemperatureDailyStats &day) {
    bool dailyChanged = false;
    
    // Check for NEW MAX: If current is NaN (after reset) OR new temp is higher
    if (maxTemp > day.maxTemp || shouldResetDailyMetrics(rtc)) {
        day.maxTemp = maxTemp;
        day.maxYear = rtc.getYear();
        day.maxMonth = rtc.getMonth();
        day.maxDay = rtc.getDay();
        day.maxHour = rtc.getHour();
        day.maxMinute = rtc.getMinute();
        dailyChanged = true;
    }

    // Check for NEW MIN: If current is NaN (after reset) OR new temp is lower
    if (minTemp < day.minTemp || shouldResetDailyMetrics(rtc)) {
        day.minTemp = minTemp;
        day.minYear = rtc.getYear();
        day.minMonth = rtc.getMonth();
        day.minDay = rtc.getDay();
        day.minHour = rtc.getHour();
        day.minMinute = rtc.getMinute();
        dailyChanged = true;
    }

    return dailyChanged;
}

bool EnvironmentManager::rememberHumidityDailyRecord(DS3231 &rtc, uint16_t maxHum, uint16_t minHum, HumidityDailyStats &day) {
    bool dailyChanged = false;
    
    // Check for NEW MAX: If current is NaN (after reset) OR new humidity is higher
    if (maxHum > day.maxHum || shouldResetDailyMetrics(rtc)) {
        day.maxHum = maxHum;
        day.maxYear = rtc.getYear();
        day.maxMonth = rtc.getMonth();
        day.maxDay = rtc.getDay();
        day.maxHour = rtc.getHour();
        day.maxMinute = rtc.getMinute();
        dailyChanged = true;
    }

    // Check for NEW MIN: If current is NaN (after reset) OR new humidity is lower
    if (minHum < day.minHum || shouldResetDailyMetrics(rtc)) {
        day.minHum = minHum;
        day.minYear = rtc.getYear();
        day.minMonth = rtc.getMonth();
        day.minDay = rtc.getDay();
        day.minHour = rtc.getHour();
        day.minMinute = rtc.getMinute();
        dailyChanged = true;
    }

    return dailyChanged;
}

bool EnvironmentManager::shouldResetDailyMetrics(DS3231 &rtc) {
    return rtc.getHour() == EnvironmentManager::RESET_HOUR 
    && rtc.getMinute() >= EnvironmentManager::RESET_MINUTE_LOWER_BOUND 
    && rtc.getMinute() <= EnvironmentManager::RESET_MINUTE_UPPER_BOUND;
}

void EnvironmentManager::printTemperature(int16_t temp) {
    formatNumber(bufferedValue, temp);
    DBG(bufferedValue);
    DBG(F("°C"));
}

void EnvironmentManager::printHumidity(uint16_t hum) {
    formatNumber(bufferedValue, hum);
    DBG(bufferedValue);
    DBG(F("%"));
}

void EnvironmentManager::printDate(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute) {
    formatDateTime(bufferedDateTime, day, month, year, hour, minute);
    DBG_LN(bufferedDateTime);
}

void EnvironmentManager::printLine(const __FlashStringHelper* label, int16_t value, bool isTemp, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute) {
    DBG(label);
    if (isTemp) printTemperature(value);
    else printHumidity((uint16_t)value);
    DBG(F(" @ "));
    printDate(day, month, year, hour, minute);
}

void EnvironmentManager::printTemperatureStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc, TemperatureDailyStats &td, TemperatureLifetimeStats &tl) {
    if (shouldResetDailyMetrics(rtc)) {
        _maxMeasuredTemp = INT16_MIN;
        _minMeasuredTemp = INT16_MAX;
    }

    int16_t temp = dht.getTemperature();

    if (temp != dht.INVALID_TEMPERATURE) {
        _currentTemp = ((temp + 25) / 50) * 50;
        DBG_LN(F(" Current Stats "));
        DBG(F("  Temperature: "));
        printTemperature(_currentTemp);
        DBG_LN();

        // Max update
        if (_currentTemp > _maxMeasuredTemp) {
            _maxMeasuredTemp = _currentTemp;
        }

        // Min update (independent!)
        if (_currentTemp < _minMeasuredTemp) {
            _minMeasuredTemp = _currentTemp;
        }

        bool lifetimeTemperatureRecordExists = saveTemperatureLifetimeRecord(eeprom, rtc, _maxMeasuredTemp, _minMeasuredTemp, tl);
        bool dailyTemperatureRecordExists = rememberTemperatureDailyRecord(rtc, _maxMeasuredTemp, _minMeasuredTemp, td);

        eeprom.loadLifetimeTemperature(tl);
        DBG(F("  Lifetime Stats "));
        if (lifetimeTemperatureRecordExists) DBG(F("(*)"));
        DBG_LN();
        printLine(F("   Min temperature: "), tl.minTemp, true, tl.minDay, tl.minMonth, tl.minYear, tl.minHour, tl.minMinute);
        printLine(F("   Max temperature: "), tl.maxTemp, true, tl.maxDay, tl.maxMonth, tl.maxYear, tl.maxHour, tl.maxMinute);

        DBG(F("  Daily Stats "));
        if (dailyTemperatureRecordExists) DBG(F("(*)"));
        DBG_LN();
        printLine(F("   Min temperature: "), td.minTemp, true, td.minDay, td.minMonth, td.minYear, td.minHour, td.minMinute);
        printLine(F("   Max temperature: "), td.maxTemp, true, td.maxDay, td.maxMonth, td.maxYear, td.maxHour, td.maxMinute);
    }
}

void EnvironmentManager::printHumidityStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc, HumidityDailyStats &hd, HumidityLifetimeStats &hl) {
    if (shouldResetDailyMetrics(rtc)) {
        _maxMeasuredHum = 0;
        _minMeasuredHum = UINT16_MAX;
    }

    uint16_t hum = dht.getHumidity();

    if (hum != dht.INVALID_HUMIDITY) {
        _currentHumidity = ((hum + 25) / 50) * 50;
        DBG(F("  Humidity: "));
        printHumidity(_currentHumidity);
        DBG_LN();

        // Max update
        if (_currentHumidity > _maxMeasuredHum) {
            _maxMeasuredHum = _currentHumidity;
        }

        // Min update (independent!)
        if (_currentHumidity < _minMeasuredHum) {
            _minMeasuredHum = _currentHumidity;
        }

        bool lifetimeHumidityRecordExists = saveHumidityLifetimeRecord(eeprom, rtc, _maxMeasuredHum, _minMeasuredHum, hl);
        bool dailyHumidityRecordExists = rememberHumidityDailyRecord(rtc, _maxMeasuredHum, _minMeasuredHum, hd);

        eeprom.loadLifetimeHumidity(hl);
        DBG(F("  Lifetime Stats "));
        if (lifetimeHumidityRecordExists) DBG(F("(*)"));
        DBG_LN();
        printLine(F("   Min humidity: "), hl.minHum, false, hl.minDay, hl.minMonth, hl.minYear, hl.minHour, hl.minMinute);
        printLine(F("   Max humidity: "), hl.maxHum, false, hl.maxDay, hl.maxMonth, hl.maxYear, hl.maxHour, hl.maxMinute);

        DBG(F("  Daily Stats "));
        if (dailyHumidityRecordExists) DBG(F("(*)"));
        DBG_LN();
        printLine(F("   Min humidity: "), hd.minHum, false, hd.minDay, hd.minMonth, hd.minYear, hd.minHour, hd.minMinute);
        printLine(F("   Max humidity: "), hd.maxHum, false, hd.maxDay, hd.maxMonth, hd.maxYear, hd.maxHour, hd.maxMinute);
    }
}

int16_t EnvironmentManager::getCurrentTemp() {return _currentTemp; }
uint16_t EnvironmentManager::getCurrentHum() {return _currentHumidity; }