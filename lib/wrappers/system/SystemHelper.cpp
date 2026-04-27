#include "SystemHelper.h"

void printBatteryPercentage(Battery &battery) {
    uint16_t voltage = battery.readVoltage();

    if (isUsbPowered(voltage)) {
        Serial.println(F("Running on USB power"));
    } else {
        float v = voltage / 1000.0f;
        uint8_t p = battery.readPercent();

        Serial.print(F(" Battery: "));
        Serial.print(p);
        Serial.print(F("% ("));
        Serial.print(v);
        Serial.println(F("V)"));

        if(v <= 6.0f) {
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