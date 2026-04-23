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

        // storeTemperatureStats(eeprom, maxMeasuredTemp, minMeasuredTemp, d, l);

        eeprom.loadLifetimeTemperature(l);
        Serial.println(F(" Lifetime Stats "));
        Serial.print(F("  Min temperature: "));
        Serial.print(l.minTemp);
        Serial.print(F("°C @ "));
        Serial.print(l.minDay);
        Serial.print(F("/"));
        Serial.print(l.minMonth);
        Serial.print(F("/"));
        Serial.print(l.minYear);
        Serial.print(F(" "));
        Serial.print(l.minHour);
        Serial.print(F(":"));
        Serial.println(l.minMinute);
        Serial.print(F("  Max temperature: "));
        Serial.print(l.maxTemp);
        Serial.print(F("°°C @ "));
        Serial.print(l.maxDay);
        Serial.print(F("/"));
        Serial.print(l.maxMonth);
        Serial.print(F("/"));
        Serial.print(l.maxYear);
        Serial.print(F(" "));
        Serial.print(l.maxHour);
        Serial.print(F(":"));
        Serial.println(l.maxMinute);

        eeprom.loadDailyTemperature(d);
        Serial.println(F(" Daily Stats "));
        Serial.print(F("  Min temperature: "));
        Serial.print(d.minTemp);
        Serial.print(F("°C @ "));
        Serial.print(d.minDay);
        Serial.print(F("/"));
        Serial.print(d.minMonth);
        Serial.print(F("/"));
        Serial.print(d.minYear);
        Serial.print(F(" "));
        Serial.print(d.minHour);
        Serial.print(F(":"));
        Serial.println(d.minMinute);
        Serial.print(F("  Max temperature: "));
        Serial.print(d.maxTemp);
        Serial.print(F("°°C @ "));
        Serial.print(d.maxDay);
        Serial.print(F("/"));
        Serial.print(d.maxMonth);
        Serial.print(F("/"));
        Serial.print(d.maxYear);
        Serial.print(F(" "));
        Serial.print(d.maxHour);
        Serial.print(F(":"));
        Serial.println(d.maxMinute);

    } else {
        Serial.print(F("Error in temperature sensor: "));
        Serial.println(Temperature::getName(temp.status));
    }
}

void storeTemperatureStats(EEPROM_25LC040A &eeprom, float maxTemp, float minTemp, TemperatureDailyStats &day, TemperatureLifetimeStats &life) {
    bool dailyChanged = false;
    bool lifetimeChanged = false;

    // ---------------- DAILY STATS ----------------
    
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
    
    // Save only if a record was broken to protect EEPROM lifespan
    if (dailyChanged) {
        eeprom.saveDailyTemperature(day);
    }
    
    if (lifetimeChanged) {
        eeprom.saveLifetimeTemperature(life);
    }
}