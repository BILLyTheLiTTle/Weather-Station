#include "SystemHelper.h"

void printBatteryPercentage(Battery &battery) {
    uint16_t voltage = battery.readVoltage();

    if (isUsbPowered(voltage)) {
        Serial.println(F("Running on USB power"));
    } else {
        uint8_t voltage_int = voltage / 1000; 
        uint8_t voltage_frac = (voltage % 1000) / 10; // convert fraction to centiVolts to avoid 3 digits and show only 2 digits

        uint8_t p = battery.readPercent();

        Serial.print(F(" Battery: "));
        Serial.print(p);
        Serial.print(F("% ("));
        Serial.print(voltage_int);
        Serial.print(F("."));
        if (voltage_frac < 10)  Serial.print(F("0"));
        Serial.print(voltage_frac);
        Serial.println(F("V)"));

        if(voltage <= Battery::LOWER_BOUND_VOLTAGE) {
            Serial.println(F("  Batteries need to recharge!"));
        }
    }
}

void printRamStats(MemoryProfiler &ram) {
    Serial.print(F(" Heap used: "));
    Serial.println(ram.getHeapUsed());

    Serial.print(F(" Free RAM: "));
    Serial.println(ram.getFreeRam());

    Serial.print(F(" Used RAM approx: "));
    Serial.println(ram.getUsedRamApprox());
}

void printSystemStats(Battery &battery, MemoryProfiler &ram) {
    printBatteryPercentage(battery);
    printRamStats(ram);
}

bool isUsbPowered(uint16_t voltage) {
    return voltage > 4000 && voltage < 5100;
}