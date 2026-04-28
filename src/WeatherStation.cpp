#include <float.h>
#include "./environment/TemperatureHelper.h"
#include "./power/PowerSaver.h"
#include "./system/SystemHelper.h"
#include "Debugger.h"

Thermistor therm(
    A0,        // analog pin
    9820,      // The real value of series resistor (10k)
    10000,     // nominal resistance (10k thermistor) from thermistor datasheet
    25,        // nominal temp (25°C) from thermistor datasheet
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

    dbg.begin();

    battery.begin();

    eeprom.begin();

    sleepSwitch.begin();

    // eeprom.factoryReset();
}

void loop() {
    enterConditionalSleep(sleepSwitch);

    // Only perform measurements if the system is ACTIVE and the interval has elapsed.
    if ((!isIntervalElapsed() || sleepSwitch.getState() == SystemState::SLEEP)) return;

    Serial.println(F("=*=*=*= START =*=*=*="));
    Serial.println(F("-*-*-*- Environment Stats -*-*-*-"));
    printTemperature(therm, eeprom, d, l);
    Serial.println(F("-*-*-*- System Stats -*-*-*-"));
    printSystemStats(battery, ram);
    Serial.println(F("=*=*=*= END =*=*=*=\n"));
}
