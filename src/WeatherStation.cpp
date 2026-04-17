#include <float.h>
#include "Thermistor.h"
#include "Battery.h"
#include "../../lib/memory/MemoryProfiler.h"
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

MemoryProfiler ram(2048);
void printRamStats();

void setup() {
    Serial.begin(9600);

    battery.begin();

    eeprom.begin();

    eeprom.loadLifetime(l);
}

void loop() {
    if (isIntervalElapsed(10000L)) {
        Serial.println(F("=*=*=*= START =*=*=*="));

        printTemperature();

        printBatteryPercentage();
        
        printRamStats();

        Serial.println(F("=*=*=*= END =*=*=*="));
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
        //if (isnan(currentTemp) || currentTemp != roundedTemp) {
            currentTemp = roundedTemp;
            Serial.print(F("Current temperature: "));
            Serial.print(currentTemp);
            Serial.println(F("°C"));
        //}

        // Max update
        if (roundedTemp > maxMeasuredTemp) {
            maxMeasuredTemp = roundedTemp;
            Serial.print(F("Max temperature: "));
            Serial.print(maxMeasuredTemp);
            Serial.println(F("°C"));
        }

        // Min update (independent!)
        if (roundedTemp < minMeasuredTemp) {
            minMeasuredTemp = roundedTemp;
            Serial.print(F("Min temperature: "));
            Serial.print(minMeasuredTemp);
            Serial.println(F("°C"));
        }


        // storeTemperatureStats(roundedTemp, d, l);

        // old and unprecise printing because we do not store something to save the memory hotspots
        eeprom.loadLifetime(l);
        Serial.print(F("Min temperature lifetime: "));
        Serial.print(l.minTemp);
        Serial.println(F("°C"));
        Serial.print(F("Max temperature lifetime: "));
        Serial.print(l.maxTemp);
        Serial.println(F("°C"));

    } else {
        Serial.print(F("Error in temperature sensor: "));
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

    Serial.print(F("Battery: "));
    Serial.print(p);
    Serial.print(F("% ("));
    Serial.print(v);
    Serial.println(F("V)"));
}

void printRamStats() {
    Serial.print(F("Heap used: "));
    Serial.println(ram.getHeapUsed());

    Serial.print(F("Free RAM: "));
    Serial.println(ram.getFreeRam());

    Serial.print(F("Used RAM approx: "));
    Serial.println(ram.getUsedRamApprox());
}