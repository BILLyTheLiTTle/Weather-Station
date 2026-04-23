#include <float.h>
#include "./environment/TemperatureHelper.h"
#include "./power/SleepHelper.h"
#include "./system/SystemHelper.h"

#define INTERVAL_BETWEEN_ACTIONS 10000L

bool isIntervalElapsed(uint32_t interval);

Thermistor therm(
    A0,        // analog pin
    9820.0,    // The real value of series resistor (10k)
    10000.0,   // nominal resistance (10k thermistor) from thermistor datasheet
    25.0,      // nominal temp (25°C) from thermistor datasheet
    ThermistorConstants::BETA_3950   // Beta coefficient (β) of the thermistor
);

Battery battery(A1, 10.0, 15.0);

EEPROM_25LC040A eeprom(10);
TemperatureDailyStats d;
TemperatureLifetimeStats l;

MemoryProfiler ram(2048);

SleepMode sleepSwitch(3);

void setup() {
    Serial.begin(9600);

    battery.begin();

    eeprom.begin();

    sleepSwitch.begin();

    // eeprom.factoryReset();
}

void loop() {
    enterConditionalSleep(sleepSwitch);

    // Only perform measurements if the system is ACTIVE and the interval has elapsed.
    if (isIntervalElapsed(INTERVAL_BETWEEN_ACTIONS)) {
        Serial.println(F("=*=*=*= START =*=*=*="));
        Serial.println(F("-*-*-*- Environment Stats -*-*-*-"));
        printTemperature(therm, eeprom, d, l);
        Serial.println(F("-*-*-*- System Stats -*-*-*-"));
        printSystemStats(battery, ram);
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

