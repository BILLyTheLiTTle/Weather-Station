#include <float.h>
#include "Thermistor.h"
#include "Battery.h"
#include "SleepMode.h"
#include "../../lib/memory/MemoryProfiler.h"
#include "../../lib/storage/eeprom/EEPROM_25LC040A.h"

#define INTERVAL_BETWEEN_ACTIONS 10000L

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

SleepMode sleepSwitch(3);
void enterSleep();

void setup() {
    Serial.begin(9600);

    battery.begin();

    eeprom.begin();

    eeprom.loadLifetime(l);

    sleepSwitch.begin();

    // eeprom.factoryReset();
}

void loop() {
    // 1. Constantly update the switch state to catch real-time changes
    sleepSwitch.update();

    // If the state is SLEEP, enter low-power mode immediately.
    // We don't wait for the interval timer to trigger sleep.
    if (sleepSwitch.getState() == SystemState::SLEEP) {
        Serial.println(F("System going to sleep..."));
        sleepSwitch.enable(); 
        
        // Execution resumes here after the interrupt wakes up the MCU.
        // Update state immediately to reflect the wake-up trigger.
        sleepSwitch.update();
    }

    // Only perform measurements if the system is ACTIVE and the interval has elapsed.
    if (isIntervalElapsed(INTERVAL_BETWEEN_ACTIONS)) {
        Serial.println(F("=*=*=*= START =*=*=*="));
        printTemperature();
        printBatteryPercentage();
        printRamStats();
        Serial.println(F("=*=*=*= END =*=*=*="));
    }
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
    static float maxMeasuredTemp = FLT_MIN;
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
        Serial.print(F("Max temperature lifetime: "));
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

    } else {
        Serial.print(F("Error in temperature sensor: "));
        Serial.println(Temperature::getName(temp.status));
    }
}

void storeTemperatureStats(float roundedTemp, DailyStats &day, LifetimeStats &life) {
    bool dailyChanged = false;
    bool lifetimeChanged = false;

    // ---------------- DAILY STATS ----------------
    
    // Check for NEW MAX: If current is NaN (after reset) OR new temp is higher
    if (isnan(day.maxTemp) || roundedTemp > day.maxTemp) {
        day.maxTemp = roundedTemp;
        day.maxYear = 2026;
        day.maxMonth = 12;
        day.maxDay = 31;
        day.maxHour = 23;
        day.maxMinute = 59;
        dailyChanged = true;
    }

    // Check for NEW MIN: If current is NaN (after reset) OR new temp is lower
    if (isnan(day.minTemp) || roundedTemp < day.minTemp) {
        day.minTemp = roundedTemp;
        day.minYear = 2026;
        day.minMonth = 12;
        day.minDay = 31;
        day.minHour = 23;
        day.minMinute = 59;
        dailyChanged = true;
    }

    // ---------------- LIFETIME STATS ----------------
    
    // Lifetime Max check
    if (isnan(life.maxTemp) || roundedTemp > life.maxTemp) {
        life.maxTemp = roundedTemp;
        life.maxYear = 2026;
        life.maxMonth = 12;
        life.maxDay = 31;
        life.maxHour = 23;
        life.maxMinute = 59;
        lifetimeChanged = true;
    }

    // Lifetime Min check
    if (isnan(life.minTemp) || roundedTemp < life.minTemp) {
        life.minTemp = roundedTemp;
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
        eeprom.saveDaily(day);
    }
    
    if (lifetimeChanged) {
        eeprom.saveLifetime(life);
    }
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