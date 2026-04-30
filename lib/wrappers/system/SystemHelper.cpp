#include "SystemHelper.h"

void printBatteryStats(Battery &battery, ACS712 &acs712) {
    uint16_t voltage = battery.readVoltage();
    uint32_t ma = acs712.getCurrentMA();
    uint32_t tMin = acs712.getRemainingMinutes();

    if (isUsbPowered(voltage)) {
        Serial.println(F("Running on USB power"));
    } else {
        uint8_t voltage_int = voltage / 1000; 
        uint8_t voltage_frac = (voltage % 1000) / 10; // convert fraction to centiVolts to avoid 3 digits and show only 2 digits

        uint8_t p = battery.readPercent();

        Serial.print(F(" Battery: "));
        Serial.print(p);
        Serial.print(F("% | Remaining Time: "));
        if(tMin > 90000) {
            Serial.print(F("Unknown ("));
        } else {
            Serial.print(tMin / 60);
            Serial.print(F("h "));
            Serial.print(tMin % 60);
            Serial.print(F("m ("));
        }
        Serial.print(voltage_int);
        Serial.print(F("."));
        if (voltage_frac < 10)  Serial.print(F("0"));
        Serial.print(voltage_frac);
        Serial.print(F("V | "));
        Serial.print(ma);
        Serial.println(F(" mA)"));

        if(voltage <= Battery::LOWER_BOUND_VOLTAGE) {
            Serial.println(F("  Batteries need to recharge!"));
        }
    }
}

void printCurrentStats(ACS712 &acs712) {
    
}

void printRamStats(MemoryProfiler &ram) {
    Serial.print(F(" Heap used: "));
    Serial.println(ram.getHeapUsed());

    Serial.print(F(" Free RAM: "));
    Serial.println(ram.getFreeRam());

    Serial.print(F(" Used RAM approx: "));
    Serial.println(ram.getUsedRamApprox());
}

void printSystemStats(Battery &battery, ACS712 &acs712, MemoryProfiler &ram) {
    printBatteryStats(battery, acs712);
    printCurrentStats(acs712);
    printRamStats(ram);
}

bool isUsbPowered(uint16_t voltage) {
    return voltage > 4000 && voltage < 5100;
}