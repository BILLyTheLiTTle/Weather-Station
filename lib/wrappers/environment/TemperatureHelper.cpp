#include "TemperatureHelper.h"

void printTemperature(Thermistor &therm, EEPROM_25LC040A &eeprom, TemperatureDailyStats d, TemperatureLifetimeStats l) {
    static float maxMeasuredTemp = FLT_MIN;
    static float minMeasuredTemp = FLT_MAX;
    static float currentTemp = NAN;

    Temperature temp = therm.readTemperatureC();

    if (temp.status == Temperature::OK) {
        float roundedTemp = floorf(temp.value * 2.0f + 0.5f) / 2.0f;

        // Print only if changed
        //if (isnan(currentTemp) || currentTemp != roundedTemp) {
            currentTemp = roundedTemp;
            Serial.println(F(" Current Stats "));
            Serial.print(F("  Temperature: "));
            Serial.print(currentTemp);
            Serial.println(F("°C"));
        //}

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

void printLine(const __FlashStringHelper* label, float value, uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute) {
        Serial.print(label);
        Serial.print(value);
        Serial.print(F("°C @ "));
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

void saveTemperatureLifetimeRecord(EEPROM_25LC040A &eeprom, float maxTemp, float minTemp, TemperatureLifetimeStats &life) {
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

void rememberTemperatureDailyRecord(float maxTemp, float minTemp, TemperatureDailyStats &day) {
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