#include "SSD1306.h"

SSD1306 display;

void SSD1306::begin() {
    _oled.begin(&Adafruit128x64, 0x3C); // 0x3C είναι η standard διεύθυνση
    _oled.setFont(Adafruit5x7);         // Ελαφριά γραμματοσειρά
    _oled.setContrast(1);
}

void SSD1306::showBootMessage() {
    _oled.clear();
    _oled.set2X();                      // Διπλάσιο μέγεθος γραμμάτων
    _oled.println(F("SYSTEM"));
    _oled.println(F("STARTING..."));
    _oled.set1X();                      // Επαναφορά σε κανονικό μέγεθος
}

void SSD1306::updateTime(int hours, int minutes, int seconds) {
    _oled.setCursor(0, 4);              // Πήγαινε στη μέση της οθόνης
    if(hours < 10) _oled.print('0');
    _oled.print(hours);
    _oled.print(':');
    if(minutes < 10) _oled.print('0');
    _oled.print(minutes);
    _oled.print(':');
    if(seconds < 10) _oled.print('0');
    _oled.print(seconds);
}

void SSD1306::showError(const __FlashStringHelper* msg) {
    _oled.clear();
    _oled.println(F("ERROR:"));
    _oled.println(msg);
}

void SSD1306::showCurrentStats(int16_t temp, uint16_t hum) {
    _oled.setCursor(20, 0);
    _oled.println(F("CURRENT STATS"));
    _oled.println();

    _oled.println(F("Temperature: "));
    _oled.print(temp/100);
    _oled.print(F("."));
    int8_t decimals = temp % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" C"));
    _oled.println();

    _oled.println(F("Humidity: "));
    _oled.print(hum/100);
    _oled.print(F("."));
    decimals = hum % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" %"));
}

void SSD1306::showDailyTemperatureStats(TemperatureDailyStats &stats) {
    _oled.setCursor(0, 0);
    _oled.print(F("DAY TEMPERATURE STATS"));
    _oled.println();

    _oled.println(F("Minimum: "));
    _oled.print(stats.minTemp/100);
    _oled.print(F("."));
    int8_t decimals = stats.minTemp % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" C "));
    if (stats.minDay < 10) _oled.print('0');
    _oled.print(stats.minDay);
    _oled.print(F("/"));
    if (stats.minMonth < 10) _oled.print('0');
    _oled.print(stats.minMonth);
    _oled.print(F("/"));
    _oled.print(stats.minYear);
    _oled.print(F(" "));
    if (stats.minHour < 10) _oled.print('0');
    _oled.print(stats.minHour);
    _oled.print(F(":"));
    if (stats.minMinute < 10) _oled.print('0');
    _oled.println(stats.minMinute);
    _oled.println();

    _oled.println(F("Maximum: "));
    _oled.print(stats.maxTemp/100);
    _oled.print(F("."));
    decimals = stats.maxTemp % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" C "));
    if (stats.maxDay < 10) _oled.print('0');
    _oled.print(stats.maxDay);
    _oled.print(F("/"));
    if (stats.maxMonth < 10) _oled.print('0');
    _oled.print(stats.maxMonth);
    _oled.print(F("/"));
    _oled.print(stats.maxYear);
    _oled.print(F(" "));
    if (stats.maxHour < 10) _oled.print('0');
    _oled.print(stats.maxHour);
    _oled.print(F(":"));
    if (stats.maxMinute < 10) _oled.print('0');
    _oled.println(stats.maxMinute);
}

void SSD1306::showDailyHumidityStats(HumidityDailyStats &stats) {
    _oled.setCursor(8, 0);
    _oled.print(F("DAY HUMIDITY STATS"));
    _oled.println();

    _oled.println(F("Minimum: "));
    _oled.print(stats.minHum/100);
    _oled.print(F("."));
    int8_t decimals = stats.minHum % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" % "));
    if (stats.minDay < 10) _oled.print('0');
    _oled.print(stats.minDay);
    _oled.print(F("/"));
    if (stats.minMonth < 10) _oled.print('0');
    _oled.print(stats.minMonth);
    _oled.print(F("/"));
    _oled.print(stats.minYear);
    _oled.print(F(" "));
    if (stats.minHour < 10) _oled.print('0');
    _oled.print(stats.minHour);
    _oled.print(F(":"));
    if (stats.minMinute < 10) _oled.print('0');
    _oled.println(stats.minMinute);
    _oled.println();

    _oled.println(F("Maximum: "));
    _oled.print(stats.maxHum/100);
    _oled.print(F("."));
    decimals = stats.maxHum % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" % "));
    if (stats.maxDay < 10) _oled.print('0');
    _oled.print(stats.maxDay);
    _oled.print(F("/"));
    if (stats.maxMonth < 10) _oled.print('0');
    _oled.print(stats.maxMonth);
    _oled.print(F("/"));
    _oled.print(stats.maxYear);
    _oled.print(F(" "));
    if (stats.maxHour < 10) _oled.print('0');
    _oled.print(stats.maxHour);
    _oled.print(F(":"));
    if (stats.maxMinute < 10) _oled.print('0');
    _oled.println(stats.maxMinute);
}

void SSD1306::showLifetimeTemperatureStats(TemperatureLifetimeStats &stats) {
    _oled.setCursor(0, 0);
    _oled.print(F("ALL TEMPERATURE STATS"));
    _oled.println();

    _oled.println(F("Minimum: "));
    _oled.print(stats.minTemp/100);
    _oled.print(F("."));
    int8_t decimals = stats.minTemp % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" C "));
    if (stats.minDay < 10) _oled.print('0');
    _oled.print(stats.minDay);
    _oled.print(F("/"));
    if (stats.minMonth < 10) _oled.print('0');
    _oled.print(stats.minMonth);
    _oled.print(F("/"));
    _oled.print(stats.minYear);
    _oled.print(F(" "));
    if (stats.minHour < 10) _oled.print('0');
    _oled.print(stats.minHour);
    _oled.print(F(":"));
    if (stats.minMinute < 10) _oled.print('0');
    _oled.println(stats.minMinute);
    _oled.println();

    _oled.println(F("Maximum: "));
    _oled.print(stats.maxTemp/100);
    _oled.print(F("."));
    decimals = stats.maxTemp % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" C "));
    if (stats.maxDay < 10) _oled.print('0');
    _oled.print(stats.maxDay);
    _oled.print(F("/"));
    if (stats.maxMonth < 10) _oled.print('0');
    _oled.print(stats.maxMonth);
    _oled.print(F("/"));
    _oled.print(stats.maxYear);
    _oled.print(F(" "));
    if (stats.maxHour < 10) _oled.print('0');
    _oled.print(stats.maxHour);
    _oled.print(F(":"));
    if (stats.maxMinute < 10) _oled.print('0');
    _oled.println(stats.maxMinute);
}

void SSD1306::showLifetimeHumidityStats(HumidityLifetimeStats &stats) {
    _oled.setCursor(8, 0);
    _oled.print(F("ALL HUMIDITY STATS"));
    _oled.println();

    _oled.println(F("Minimum: "));
    _oled.print(stats.minHum/100);
    _oled.print(F("."));
    int8_t decimals = stats.minHum % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" % "));
    if (stats.minDay < 10) _oled.print('0');
    _oled.print(stats.minDay);
    _oled.print(F("/"));
    if (stats.minMonth < 10) _oled.print('0');
    _oled.print(stats.minMonth);
    _oled.print(F("/"));
    _oled.print(stats.minYear);
    _oled.print(F(" "));
    if (stats.minHour < 10) _oled.print('0');
    _oled.print(stats.minHour);
    _oled.print(F(":"));
    if (stats.minMinute < 10) _oled.print('0');
    _oled.println(stats.minMinute);
    _oled.println();

    _oled.println(F("Maximum: "));
    _oled.print(stats.maxHum/100);
    _oled.print(F("."));
    decimals = stats.maxHum % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.println(F(" % "));
    if (stats.maxDay < 10) _oled.print('0');
    _oled.print(stats.maxDay);
    _oled.print(F("/"));
    if (stats.maxMonth < 10) _oled.print('0');
    _oled.print(stats.maxMonth);
    _oled.print(F("/"));
    _oled.print(stats.maxYear);
    _oled.print(F(" "));
    if (stats.maxHour < 10) _oled.print('0');
    _oled.print(stats.maxHour);
    _oled.print(F(":"));
    if (stats.maxMinute < 10) _oled.print('0');
    _oled.println(stats.maxMinute);
}

void SSD1306::showSystemStats(Battery &battery, ACS712 &acs712, MemoryProfiler &ram, Thermistor &therm) {
    _oled.setCursor(24, 0);
    _oled.print(F("SYSTEM STATS"));
    _oled.println();

    uint16_t voltage = battery.readVoltage();
    bool isUsbPowered = battery.isUsbPowered();
    _oled.print(F("Battery "));
    if(voltage <= Battery::LOWER_BOUND_VOLTAGE && !isUsbPowered) {
        _oled.print(F(" (Recharge)"));
    }
    _oled.println();
    if (isUsbPowered) {
        _oled.print(F("USB powered"));
    } else {
        _oled.print(battery.readPercent());
        _oled.print(F(" % ("));
        uint8_t voltage_int = voltage / 1000; 
        uint8_t voltage_frac = (voltage % 1000) / 10;
        _oled.print(voltage_int);
        _oled.print(F("."));
        if (voltage_frac < 10)  _oled.print(F("0"));
        _oled.print(voltage_frac);
        _oled.print(F("V | "));
        _oled.print(acs712.getCurrentMA());
        _oled.print(F("mA)"));
    }
    _oled.println();

    _oled.println(F("Remaining Time"));
    uint32_t tMin = acs712.getRemainingMinutes();
    if(tMin > 90000 || isUsbPowered) {
        _oled.print(F("Unknown"));
    } else {
        _oled.print(tMin / 60);
        _oled.print(F("h "));
        _oled.print(tMin % 60);
        _oled.print(F("m"));
    }
    _oled.println();

    _oled.println(F("Temperature"));
    Temperature temp = therm.readTemperatureC();
    int16_t roundedTemp = ((temp.value + 25) / 50) * 50;
    _oled.print(roundedTemp/100);
    _oled.print(F("."));
    int8_t decimals = roundedTemp % 100;
    if (decimals < 10) _oled.print(F("0"));
    _oled.print(decimals);
    _oled.print(F(" C"));
}

void SSD1306::clear() {
    _oled.clear();
}

// void SSD1306::print(const __FlashStringHelper* msg) {
//     _oled.print(msg);
// }

// void SSD1306::println(const __FlashStringHelper* msg) {
//     _oled.println(msg);
// }

// void SSD1306::print(int16_t value) {
//     _oled.print(value);
// }

// void SSD1306::println(int16_t value) {
//     _oled.println(value);
// }

// void SSD1306::print(uint16_t value) {
//     _oled.print(value);
// }

// void SSD1306::println(uint16_t value) {
//     _oled.println(value);
// }

// void SSD1306::print(int8_t value) {
//     _oled.print(value);
// }

// void SSD1306::println(int8_t value) {
//     _oled.println(value);
// }

// void SSD1306::print(uint8_t value) {
//     _oled.print(value);
// }

// void SSD1306::println(uint8_t value) {
//     _oled.println(value);
// }