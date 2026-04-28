#include "TemperatureHelper.h"

void printTemperatureStats(Thermistor &therm, EEPROM_25LC040A &eeprom, TemperatureDailyStats &d, TemperatureLifetimeStats &l) {
    static int16_t maxMeasuredTemp = INT16_MIN;
    static int16_t minMeasuredTemp = INT16_MAX;
    static int16_t currentTemp = 0;

    Temperature temp = therm.readTemperatureC();

    if (temp.status == Temperature::OK) {
        int16_t roundedTemp = ((temp.value + 25) / 50) * 50;;

        currentTemp = roundedTemp;
        Serial.println(F(" Current Stats "));
        Serial.print(F("  Temperature: "));
        printTemperature(currentTemp);
        Serial.println();

        // Max update
        if (roundedTemp > maxMeasuredTemp) {
            maxMeasuredTemp = roundedTemp;
        }

        // Min update (independent!)
        if (roundedTemp < minMeasuredTemp) {
            minMeasuredTemp = roundedTemp;
        }

        // saveTemperatureLifetimeRecord(eeprom, maxMeasuredTemp, minMeasuredTemp, l);
        rememberTemperatureDailyRecord(maxMeasuredTemp, minMeasuredTemp, d);

        eeprom.loadLifetimeTemperature(l);
        Serial.println(F(" Lifetime Stats "));
        printLine(F("  Min temperature: "), l.minTemp, l.minDay, l.minMonth, l.minYear, l.minHour, l.minMinute);
        printLine(F("  Max temperature: "), l.maxTemp, l.maxDay, l.maxMonth, l.maxYear, l.maxHour, l.maxMinute);

        Serial.println(F(" Daily Stats "));
        printLine(F("  Min temperature: "), d.minTemp, d.minDay, d.minMonth, d.minYear, d.minHour, d.minMinute);
        printLine(F("  Max temperature: "), d.maxTemp, d.maxDay, d.maxMonth, d.maxYear, d.maxHour, d.maxMinute);

    } else {
        Serial.print(F("Error in temperature sensor: "));
        Serial.println(Temperature::getName(temp.status));
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

void printTemperature(int16_t temp) {
    Serial.print(temp/100);

    Serial.print(F("."));

    int8_t decimals = temp % 100;
    if (decimals < 10) Serial.print(F("0"));

    Serial.print(decimals);

    Serial.print(F("°C"));
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