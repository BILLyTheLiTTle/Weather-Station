#include "./environment/TemperatureHelper.h"
#include "./power/PowerSaver.h"
#include "./system/SystemHelper.h"
#include "Debugger.h"
#include "ACS712.h"

Thermistor therm(
    A0,        // analog pin
    9820,      // The real value of series resistor (10k)
    10000,     // nominal resistance (10k thermistor) from thermistor datasheet
    25,        // nominal temp (25°C) from thermistor datasheet
    ThermistorConstants::BETA_4250   // Beta coefficient (β) of the thermistor
);

Battery battery(A1, 9810, 14830);

EEPROM_25LC040A eeprom(10);
TemperatureDailyStats d;
TemperatureLifetimeStats l;

MemoryProfiler ram(2048);

SleepMode sleepSwitch(3);

ACS712 acs712(A2, ACS712_05B, 2200);

void setup() {
    Serial.begin(9600);

    // For calibration we need to remove all power sources (Vin, batteries) apart from USB. After calibration comment it out again
    // acs712.calibrate();

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
    printTemperatureStats(therm, eeprom, d, l);
    Serial.println(F("-*-*-*- System Stats -*-*-*-"));
    printSystemStats(battery, acs712, ram);
    Serial.println(F("=*=*=*= END =*=*=*=\n"));
}

void calculate() {
    uint32_t ma = acs712.getCurrentMA();
    uint32_t tMin = acs712.getRemainingMinutes();

    Serial.print("Current: ");
    Serial.print(ma);
    Serial.print(" mA | Time: ");
    
    if(tMin > 90000) {
        Serial.println("Unknown");
    } else {
        Serial.print(tMin / 60);
        Serial.print("h ");
        Serial.print(tMin % 60);
        Serial.println("m");
    }
}
