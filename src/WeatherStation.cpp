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
void storeTemperatureStats(float maxTemp, float minTemp, DailyStats &day, LifetimeStats &life);
void printDataVersion() {
    uint8_t version = eeprom.loadDataVersion();
    Serial.print(F(" Data version: "));
    Serial.println(version);
}

MemoryProfiler ram(2048);
void printRamStats();

SleepMode sleepSwitch(3);
void enterSleep();

void printSystemStats() {
    printBatteryPercentage();
    printRamStats();
    printDataVersion();
}

void setup() {
    Serial.begin(9600);

    battery.begin();

    eeprom.begin();

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
        Serial.println(F("-*-*-*- Environment Stats -*-*-*-"));
        printTemperature();
        Serial.println(F("-*-*-*- System Stats -*-*-*-"));
        printSystemStats();
        Serial.println(F("=*=*=*= END =*=*=*=\n"));
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

        // storeTemperatureStats(maxMeasuredTemp, minMeasuredTemp, d, l);

        eeprom.loadLifetime(l);
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

        eeprom.loadDaily(d);
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

void storeTemperatureStats(float maxTemp, float minTemp, DailyStats &day, LifetimeStats &life) {
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
        eeprom.saveDaily(day);
    }
    
    if (lifetimeChanged) {
        eeprom.saveLifetime(life);
    }
}

void printBatteryPercentage() {
    float v = battery.readFilteredVoltage();
    int p = battery.readPercent();

    Serial.print(F(" Battery: "));
    Serial.print(p);
    Serial.print(F("% ("));
    Serial.print(v);
    Serial.println(F("V)"));
    if(v <= 6.0f) Serial.println(F("  Batteries need to recharge!"));
}

void printRamStats() {
    Serial.print(F(" Heap used: "));
    Serial.println(ram.getHeapUsed());

    Serial.print(F(" Free RAM: "));
    Serial.println(ram.getFreeRam());

    Serial.print(F(" Used RAM approx: "));
    Serial.println(ram.getUsedRamApprox());
}