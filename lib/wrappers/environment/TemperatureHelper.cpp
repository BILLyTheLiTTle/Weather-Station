#include "TemperatureHelper.h"

void printEnvironmentStats(DHT_Sensor &dht, EEPROM_25LC040A &eeprom, 
    TemperatureDailyStats &td, TemperatureLifetimeStats &tl, 
    HumidityDailyStats &hd, HumidityLifetimeStats &hl) {

    static int16_t maxMeasuredTemp = INT16_MIN;
    static int16_t minMeasuredTemp = INT16_MAX;
    static int16_t currentTemp = 0;
    static uint16_t maxMeasuredHum = 0;
    static uint16_t minMeasuredHum = UINT16_MAX;
    static uint16_t currentHum = 0;

    int16_t temp = dht.getTemperature();
    uint16_t hum = dht.getHumidity();

    if (temp != dht.INVALID_VALUE) {
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

        // saveTemperatureLifetimeRecord(eeprom, maxMeasuredTemp, minMeasuredTemp, tl);
        rememberTemperatureDailyRecord(maxMeasuredTemp, minMeasuredTemp, td);

        eeprom.loadLifetimeTemperature(tl);
        Serial.println(F("  Lifetime Stats "));
        printLine(F("   Min temperature: "), tl.minTemp, tl.minDay, tl.minMonth, tl.minYear, tl.minHour, tl.minMinute);
        printLine(F("   Max temperature: "), tl.maxTemp, tl.maxDay, tl.maxMonth, tl.maxYear, tl.maxHour, tl.maxMinute);

        Serial.println(F("  Daily Stats "));
        printLine(F("   Min temperature: "), td.minTemp, td.minDay, td.minMonth, td.minYear, td.minHour, td.minMinute);
        printLine(F("   Max temperature: "), td.maxTemp, td.maxDay, td.maxMonth, td.maxYear, td.maxHour, td.maxMinute);
    }

    if (hum != dht.INVALID_VALUE) {
        currentHum = ((hum + 25) / 50) * 50;
        Serial.print(F("  Humidity: "));
        printDecimalNumber(currentHum, F("%"));
        Serial.println();

        // Max update
        if (currentHum > maxMeasuredHum) {
            maxMeasuredHum = currentHum;
        }

        // Min update (independent!)
        if (currentHum < minMeasuredHum) {
            minMeasuredHum = currentHum;
        }

        // saveTemperatureLifetimeRecord(eeprom, maxMeasuredHum, minMeasuredHum, hl);
        rememberHumidityDailyRecord(maxMeasuredHum, minMeasuredHum, hd);

        // eeprom.loadLifetimeTemperature(hl);
        Serial.println(F("  Lifetime Stats "));
        printLine(F("   Min humidity: "), hl.minHum, hl.minDay, hl.minMonth, hl.minYear, hl.minHour, hl.minMinute);
        printLine(F("   Max humidity: "), hl.maxHum, hl.maxDay, hl.maxMonth, hl.maxYear, hl.maxHour, hl.maxMinute);

        Serial.println(F("  Daily Stats "));
        printLine(F("   Min humidity: "), hd.minHum, hd.minDay, hd.minMonth, hd.minYear, hd.minHour, hd.minMinute);
        printLine(F("   Max humidity: "), hd.maxHum, hd.maxDay, hd.maxMonth, hd.maxYear, hd.maxHour, hd.maxMinute);
    }
}

void printLine(const __FlashStringHelper* label, int16_t value, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute) {
        Serial.print(label);
        printTemperature(value);
        Serial.print(F(" @ "));
        Serial.print(day);
        Serial.print(F("/"));
        Serial.print(month);
        Serial.print(F("/"));
        Serial.print(year);
        Serial.print(F(" "));
        Serial.print(hour);
        Serial.print(F(":"));
        Serial.println(minute);
}

void printLine(const __FlashStringHelper* label, uint16_t value, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute) {
        Serial.print(label);
        printTemperature(value);
        Serial.print(F(" @ "));
        Serial.print(day);
        Serial.print(F("/"));
        Serial.print(month);
        Serial.print(F("/"));
        Serial.print(year);
        Serial.print(F(" "));
        Serial.print(hour);
        Serial.print(F(":"));
        Serial.println(minute);
}

void printTemperature(int16_t temp) {
    printDecimalNumber(temp,  F("°C"));
}

void printDecimalNumber(int16_t temp, const __FlashStringHelper *symbol) {
    Serial.print(temp/100);

    Serial.print(F("."));

    int8_t decimals = temp % 100;
    if (decimals < 10) Serial.print(F("0"));

    Serial.print(decimals);

    Serial.print(symbol);
}

void saveTemperatureLifetimeRecord(EEPROM_25LC040A &eeprom, int16_t maxTemp, int16_t minTemp, TemperatureLifetimeStats &life) {
    bool lifetimeChanged = false;

    // ---------------- LIFETIME STATS ----------------
    
    // Lifetime Max check
    if (isnan(life.maxTemp) || maxTemp > life.maxTemp) {
        life.maxTemp = maxTemp;
        life.maxYear = 2026;
        life.maxMonth = 12;
        life.maxDay = 31;
        life.maxHour = 23;
        life.maxMinute = 59;
        lifetimeChanged = true;
    }

    // Lifetime Min check
    if (isnan(life.minTemp) || minTemp < life.minTemp) {
        life.minTemp = minTemp;
        life.minYear = 2026;
        life.minMonth = 12;
        life.minDay = 31;
        life.minHour = 23;
        life.minMinute = 59;
        lifetimeChanged = true;
    }

    // ---------------- SAVE TO EEPROM ----------------    
    if (lifetimeChanged) {
        eeprom.saveLifetimeTemperature(life);
    }
}

void rememberTemperatureDailyRecord(int16_t maxTemp, int16_t minTemp, TemperatureDailyStats &day) {
    bool dailyChanged = false;
    
    // TODO if current time is between 00:05 - 00:25 force write the daily stats

    // Check for NEW MAX: If current is NaN (after reset) OR new temp is higher
    if (isnan(day.maxTemp) || maxTemp > day.maxTemp) {
        day.maxTemp = maxTemp;
        day.maxYear = 2026;
        day.maxMonth = 12;
        day.maxDay = 31;
        day.maxHour = 23;
        day.maxMinute = 59;
        dailyChanged = true;
    }

    // Check for NEW MIN: If current is NaN (after reset) OR new temp is lower
    if (isnan(day.minTemp) || minTemp < day.minTemp) {
        day.minTemp = minTemp;
        day.minYear = 2026;
        day.minMonth = 12;
        day.minDay = 31;
        day.minHour = 23;
        day.minMinute = 59;
        dailyChanged = true;
    }
}

void rememberHumidityDailyRecord(uint16_t maxHum, uint16_t minHum, HumidityDailyStats &day) {
    bool dailyChanged = false;
    
    // TODO if current time is between 00:05 - 00:25 force write the daily stats

    // Check for NEW MAX: If current is NaN (after reset) OR new humidity is higher
    if (isnan(day.maxHum) || maxHum > day.maxHum) {
        day.maxHum = maxHum;
        day.maxYear = 2026;
        day.maxMonth = 12;
        day.maxDay = 31;
        day.maxHour = 23;
        day.maxMinute = 59;
        dailyChanged = true;
    }

    // Check for NEW MIN: If current is NaN (after reset) OR new humidity is lower
    if (isnan(day.minHum) || minHum < day.minHum) {
        day.minHum = minHum;
        day.minYear = 2026;
        day.minMonth = 12;
        day.minDay = 31;
        day.minHour = 23;
        day.minMinute = 59;
        dailyChanged = true;
    }
}