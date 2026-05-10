#include "EnvironmentManager.h"

void EnvironmentManager::printEnvironmentStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc,
    TemperatureDailyStats &td, TemperatureLifetimeStats &tl, 
    HumidityDailyStats &hd, HumidityLifetimeStats &hl) {

    printTemperatureStats(dht, eeprom, rtc, td, tl);
    printHumidityStats(dht, eeprom, rtc, hd, hl);
}

bool EnvironmentManager::saveHumidityLifetimeRecord(EEPROM_25LC040A &eeprom, DS3231 &rtc, int16_t maxHum, int16_t minHum, HumidityLifetimeStats &life) {
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
    bool forceUpdate = false;
    
    forceUpdate = rtc.getHour() == EnvironmentManager::RESET_HOUR 
    && rtc.getMinute() >= EnvironmentManager::RESET_MINUTE_LOWER_BOUND 
    && rtc.getMinute() <= EnvironmentManager::RESET_MINUTE_UPPER_BOUND;

    // Check for NEW MAX: If current is NaN (after reset) OR new temp is higher
    if (isnan(day.maxTemp) || maxTemp > day.maxTemp || forceUpdate) {
        day.maxTemp = maxTemp;
        day.maxYear = rtc.getYear();
        day.maxMonth = rtc.getMonth();
        day.maxDay = rtc.getDay();
        day.maxHour = rtc.getHour();
        day.maxMinute = rtc.getMinute();
        dailyChanged = true;
    }

    // Check for NEW MIN: If current is NaN (after reset) OR new temp is lower
    if (isnan(day.minTemp) || minTemp < day.minTemp || forceUpdate) {
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
    bool forceUpdate = false;
    
    forceUpdate = rtc.getHour() == EnvironmentManager::RESET_HOUR 
    && rtc.getMinute() >= EnvironmentManager::RESET_MINUTE_LOWER_BOUND 
    && rtc.getMinute() <= EnvironmentManager::RESET_MINUTE_UPPER_BOUND;

    // Check for NEW MAX: If current is NaN (after reset) OR new humidity is higher
    if (isnan(day.maxHum) || maxHum > day.maxHum || forceUpdate) {
        day.maxHum = maxHum;
        day.maxYear = rtc.getYear();
        day.maxMonth = rtc.getMonth();
        day.maxDay = rtc.getDay();
        day.maxHour = rtc.getHour();
        day.maxMinute = rtc.getMinute();
        dailyChanged = true;
    }

    // Check for NEW MIN: If current is NaN (after reset) OR new humidity is lower
    if (isnan(day.minHum) || minHum < day.minHum || forceUpdate) {
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

void EnvironmentManager::printDecimalNumber(int16_t value, const __FlashStringHelper *symbol) {
    Serial.print(value/100);

    Serial.print(F("."));

    int8_t decimals = value % 100;
    if (decimals < 10) Serial.print(F("0"));

    Serial.print(decimals);

    Serial.print(symbol);
}

void EnvironmentManager::printTemperature(int16_t temp) {
    printDecimalNumber(temp,  F("°C"));
}

void EnvironmentManager::printHumidity(uint16_t temp) {
    printDecimalNumber(temp,  F("%"));
}

void EnvironmentManager::printDate(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute) {
        if (day < 10) Serial.print('0');
    Serial.print(day);
    Serial.print(F("/"));
    if (month < 10) Serial.print('0');
    Serial.print(month);
    Serial.print(F("/"));
    Serial.print(year);
    Serial.print(F(" "));
    if (hour < 10) Serial.print('0');
    Serial.print(hour);
    Serial.print(F(":"));
    if (minute < 10) Serial.print('0');
    Serial.println(minute);
}

void EnvironmentManager::printLine(const __FlashStringHelper* label, int16_t value, bool isTemp, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute) {
        Serial.print(label);
        if (isTemp) printTemperature(value);
    else printHumidity((uint16_t)value);
        Serial.print(F(" @ "));
        printDate(day, month, year, hour, minute);
}

void EnvironmentManager::printTemperatureStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc, TemperatureDailyStats &td, TemperatureLifetimeStats &tl) {
    static int16_t maxMeasuredTemp = INT16_MIN;
    static int16_t minMeasuredTemp = INT16_MAX;
    static int16_t currentTemp = 0;

    int16_t temp = dht.getTemperature();

    if (temp != dht.INVALID_TEMPERATURE) {
        currentTemp = ((temp + 25) / 50) * 50;
        Serial.println(F(" Current Stats "));
        Serial.print(F("  Temperature: "));
        printTemperature(currentTemp);
        Serial.println();

        // Max update
        if (currentTemp > maxMeasuredTemp) {
            maxMeasuredTemp = currentTemp;
        }

        // Min update (independent!)
        if (currentTemp < minMeasuredTemp) {
            minMeasuredTemp = currentTemp;
        }

        bool lifetimeTemperatureRecordExists = saveTemperatureLifetimeRecord(eeprom, rtc, maxMeasuredTemp, minMeasuredTemp, tl);
        bool dailyTemperatureRecordExists = rememberTemperatureDailyRecord(rtc, maxMeasuredTemp, minMeasuredTemp, td);

        eeprom.loadLifetimeTemperature(tl);
        Serial.print(F("  Lifetime Stats "));
        if (lifetimeTemperatureRecordExists) Serial.print(F("(*)"));
        Serial.println();
        printLine(F("   Min temperature: "), tl.minTemp, true, tl.minDay, tl.minMonth, tl.minYear, tl.minHour, tl.minMinute);
        printLine(F("   Max temperature: "), tl.maxTemp, true, tl.maxDay, tl.maxMonth, tl.maxYear, tl.maxHour, tl.maxMinute);

        Serial.print(F("  Daily Stats "));
        if (dailyTemperatureRecordExists) Serial.print(F("(*)"));
        Serial.println();
        printLine(F("   Min temperature: "), td.minTemp, true, td.minDay, td.minMonth, td.minYear, td.minHour, td.minMinute);
        printLine(F("   Max temperature: "), td.maxTemp, true, td.maxDay, td.maxMonth, td.maxYear, td.maxHour, td.maxMinute);
    }
}

void EnvironmentManager::printHumidityStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, DS3231 &rtc, HumidityDailyStats &hd, HumidityLifetimeStats &hl) {
    static uint16_t maxMeasuredHum = 0;
    static uint16_t minMeasuredHum = UINT16_MAX;
    static uint16_t currentHum = 0;

    uint16_t hum = dht.getHumidity();

    if (hum != dht.INVALID_HUMIDITY) {
        currentHum = ((hum + 25) / 50) * 50;
        Serial.print(F("  Humidity: "));
        printHumidity(currentHum);
        Serial.println();

        // Max update
        if (currentHum > maxMeasuredHum) {
            maxMeasuredHum = currentHum;
        }

        // Min update (independent!)
        if (currentHum < minMeasuredHum) {
            minMeasuredHum = currentHum;
        }

        bool lifetimeHumidityRecordExists = saveHumidityLifetimeRecord(eeprom, rtc, maxMeasuredHum, minMeasuredHum, hl);
        bool dailyHumidityRecordExists = rememberHumidityDailyRecord(rtc, maxMeasuredHum, minMeasuredHum, hd);

        eeprom.loadLifetimeHumidity(hl);
        Serial.print(F("  Lifetime Stats "));
        if (lifetimeHumidityRecordExists) Serial.print(F("(*)"));
        Serial.println();
        printLine(F("   Min humidity: "), hl.minHum, false, hl.minDay, hl.minMonth, hl.minYear, hl.minHour, hl.minMinute);
        printLine(F("   Max humidity: "), hl.maxHum, false, hl.maxDay, hl.maxMonth, hl.maxYear, hl.maxHour, hl.maxMinute);

        Serial.print(F("  Daily Stats "));
        if (dailyHumidityRecordExists) Serial.print(F("(*)"));
        Serial.println();
        printLine(F("   Min humidity: "), hd.minHum, false, hd.minDay, hd.minMonth, hd.minYear, hd.minHour, hd.minMinute);
        printLine(F("   Max humidity: "), hd.maxHum, false, hd.maxDay, hd.maxMonth, hd.maxYear, hd.maxHour, hd.maxMinute);
    }
}