#include <float.h>
#include "Thermistor.h"
#include "Battery.h"
#include "../../lib/storage/eeprom/EEPROM_25LC040A.h"

bool isIntervalElapsed(uint32_t interval);

Thermistor therm(
    A0,        // analog pin
    9820.0,    // The real value of series resistor (10k)
    10000.0,   // nominal resistance (10k thermistor) from thermistor datasheet
    25.0,      // nominal temp (25°C) from thermistor datasheet
    ThermistorConstants::BETA_3950   // Beta coefficient (β) of the thermistor
);
void printTemperature();

Battery battery(A1, 10.0, 15.0);
void printBatteryPercentage();

EEPROM_25LC040A eeprom(10);
DailyStats d;
LifetimeStats l;
void storeTemperatureStats(float roundedTemp, DailyStats &day, LifetimeStats &life);

void setup() {
    Serial.begin(9600);

    battery.begin();

    eeprom.begin();

    eeprom.loadLifetime(l);
}

void loop() {
    if (isIntervalElapsed(10000L)) {
        printTemperature();

        printBatteryPercentage();
    }

    // delay(10000);
}

bool isIntervalElapsed(uint32_t interval) {
    static uint32_t lastReadTime = 0;

    uint32_t now = millis();

    if (lastReadTime != 0 && (now - lastReadTime) < interval) {
        return false;
    }
    lastReadTime = now;

    return true;
}

void printTemperature() {
    static float maxMeasuredTemp = -FLT_MAX;
    static float minMeasuredTemp = FLT_MAX;
    static float currentTemp = NAN;

    Temperature temp = therm.readTemperatureC();

    if (temp.status == Temperature::OK) {
        float roundedTemp = floorf(temp.value * 2.0f + 0.5f) / 2.0f;

        // Print only if changed
        if (isnan(currentTemp) || currentTemp != roundedTemp) {
            currentTemp = roundedTemp;
            Serial.print("Current temperature: ");
            Serial.print(currentTemp);
            Serial.println("°C");
        }

        // Max update
        if (roundedTemp > maxMeasuredTemp) {
            maxMeasuredTemp = roundedTemp;
            Serial.print("Max temperature: ");
            Serial.print(maxMeasuredTemp);
            Serial.println("°C");
        }

        // Min update (independent!)
        if (roundedTemp < minMeasuredTemp) {
            minMeasuredTemp = roundedTemp;
            Serial.print("Min temperature: ");
            Serial.print(minMeasuredTemp);
            Serial.println("°C");
        }


        // storeTemperatureStats(roundedTemp, d, l);

        // old and unprecise printing because we do not store something to save the memory hotspots
        eeprom.loadLifetime(l);
        Serial.print("Min temperature lifetime: ");
        Serial.print(l.minTemp);
        Serial.println("°C");
        Serial.print("Max temperature lifetime: ");
        Serial.print(l.maxTemp);
        Serial.println("°C");

    } else {
        Serial.print("Error in temperature sensor: ");
        Serial.println(Temperature::getName(temp.status));
    }
}

void storeTemperatureStats(float roundedTemp, DailyStats &day, LifetimeStats &life) {
    // ---------------- DAILY ----------------
    bool dailyChanged = false; // to be used with RTC
    if (roundedTemp > day.maxTemp) {
        day.maxTemp = roundedTemp;
        dailyChanged = true;
    }

    if (roundedTemp < day.minTemp) {
        day.minTemp = roundedTemp;
        dailyChanged = true;
    }

    // ---------------- LIFETIME ----------------
    bool lifetimeChanged = false; // TO be used with RTC
    if (roundedTemp > life.maxTemp) {
        life.maxTemp = roundedTemp;
        lifetimeChanged = true;
    }

    if (roundedTemp < life.minTemp) {
        life.minTemp = roundedTemp;
        lifetimeChanged = true;
    }

    if (dailyChanged) eeprom.saveDaily(day);
    if (lifetimeChanged) eeprom.saveLifetime(life);
}

void printBatteryPercentage() {
    float v = battery.readFilteredVoltage();
    int p = battery.readPercent();

    Serial.print("Battery: ");
    Serial.print(p);
    Serial.print("% (");
    Serial.print(v);
    Serial.println("V)");
}