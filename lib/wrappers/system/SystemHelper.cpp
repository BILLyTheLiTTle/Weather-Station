#include "SystemHelper.h"
#include "Debugger.h"

void printBatteryStats(Battery &battery, ACS712 &acs712) {
    uint16_t voltage = battery.readVoltage();
    uint32_t ma = acs712.getCurrentMA();
    uint32_t tMin = acs712.getRemainingMinutes();

    if (battery.isUsbPowered()) {
        DBG_LN(F("Running on USB power"));
    } else {
        uint8_t voltage_int = voltage / 1000; 
        uint8_t voltage_frac = (voltage % 1000) / 10; // convert fraction to centiVolts to avoid 3 digits and show only 2 digits

        uint8_t p = battery.readPercent();

        DBG(F(" Battery: "));
        DBG(p);
        DBG(F("% | Remaining Time: "));
        if(tMin > 90000) {
            DBG(F("Unknown ("));
        } else {
            DBG(tMin / 60);
            DBG(F("h "));
            DBG(tMin % 60);
            DBG(F("m ("));
        }
        DBG(voltage_int);
        DBG(F("."));
        if (voltage_frac < 10)  DBG(F("0"));
        DBG(voltage_frac);
        DBG(F("V | "));
        DBG(ma);
        DBG_LN(F(" mA)"));

        if(voltage <= Battery::LOWER_BOUND_VOLTAGE) {
            DBG_LN(F("  Batteries need to recharge!"));
        }
    }
}

static void printDecimalNumber(int16_t value, const __FlashStringHelper *symbol) {
    DBG(value/100);

    DBG(F("."));

    int8_t decimals = value % 100;
    if (decimals < 10) DBG(F("0"));

    DBG(decimals);

    DBG(symbol);
}

static void printTemperature(int16_t temp) {
    printDecimalNumber(temp,  F("°C"));
}

void printSystemTemperature(Thermistor &therm) {
    static int16_t currentTemp = 0;
    Temperature temp = therm.readTemperatureC();

    if (temp.status == Temperature::OK) {
        int16_t roundedTemp = ((temp.value + 25) / 50) * 50;

        currentTemp = roundedTemp;
        DBG(F(" Temperature: "));
        printTemperature(currentTemp);
        DBG_LN();

    } else {
        DBG(F("Error in temperature sensor: "));
        DBG_LN(Temperature::getName(temp.status));
    }
}

void printRamStats(MemoryProfiler &ram) {
    DBG_STAT(F(" Heap used"), ram.getHeapUsed());

    DBG_STAT(F(" Free RAM"), ram.getFreeRam());

    DBG_STAT(F(" Used RAM approx"), ram.getUsedRamApprox());
}

void printSystemStats(Battery &battery, ACS712 &acs712, MemoryProfiler &ram, Thermistor &therm) {
    printBatteryStats(battery, acs712);
    printSystemTemperature(therm);
    printRamStats(ram);
}