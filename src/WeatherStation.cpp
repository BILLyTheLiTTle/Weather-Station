#include "./power/PowerSaver.h"
#include "./system/SystemHelper.h"
#include "Debugger.h"
#include "ACS712.h"
#include "environment/EnvironmentManager.h"
#include "DS3231.h"

Thermistor therm(
    A0,        // analog pin
    9820,      // The real value of series resistor (10k)
    10000,     // nominal resistance (10k thermistor) from thermistor datasheet
    25,        // nominal temp (25°C) from thermistor datasheet
    ThermistorConstants::BETA_4250   // Beta coefficient (β) of the thermistor
);

Battery battery(A1, 9810, 14830);

EnvironmentManager envMan;
EEPROM_25LC040A eeprom(10);
TemperatureDailyStats td;
TemperatureLifetimeStats tl;
HumidityDailyStats hd;
HumidityLifetimeStats hl;

MemoryProfiler ram(2048);

SleepMode sleepSwitch(3, 2);

ACS712 acs712(A2, ACS712_05B, 2200);

DHT_Sensor environmentSensor(9, DHT_Sensor::DHT22);

DS3231 rtc;

void setup() {
    Serial.begin(9600);

    // For calibration we need to remove all power sources (Vin, batteries) apart from USB. After calibration comment it again
    // acs712.calibrate();

    // dbg.begin();

    battery.begin();

    eeprom.begin();
    // To reset the stored data uncomment the factory reset and comment the load lifetime data. Don't forget to undo the commenting for a regular run
    // eeprom.factoryReset();
    eeprom.loadLifetimeTemperature(tl);
    eeprom.loadLifetimeHumidity(hl);

    sleepSwitch.begin();

    if (!rtc.begin()) {
        Serial.println(F("RTC Error!"));
        while (1);
    }

    // You call it when the RTC get cut off from power. Then you comment it again
    // rtc.updateWithSystemTime();
}

void loop() {
    if (rtc.alarmFired()) {
        rtc.clearAlarm();
    }

    enterConditionalSleep(rtc, sleepSwitch);

    // Only perform measurements if the system is ACTIVE and the interval has elapsed.
    if ((!isIntervalElapsed() || sleepSwitch.getState() == SystemState::SLEEP)) return;

    Serial.println(F("=*=*=*= START =*=*=*="));
    Serial.println(F("-*-*-*- Environment Stats -*-*-*-"));
    envMan.printEnvironmentStats(environmentSensor, eeprom, rtc, td, tl, hd, hl);
    Serial.println(F("-*-*-*- System Stats -*-*-*-"));
    printSystemStats(battery, acs712, ram, therm);
    Serial.println(F("=*=*=*= END =*=*=*=\n"));
}
