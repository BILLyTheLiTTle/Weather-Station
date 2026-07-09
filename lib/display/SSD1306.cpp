#include "SSD1306.h"
#include "../../lib/wrappers/system/SystemHelper.h"

SSD1306 display(8);

SSD1306::SSD1306(uint8_t navigationControl)
    : navigationButton(navigationControl){}

void SSD1306::begin() {
    _oled.begin(&Adafruit128x64, 0x3C);
    _oled.setFont(Adafruit5x7);
    _oled.setContrast(1);
}

void SSD1306::showBootMessage() {
    _oled.clear();
    _oled.set2X();
    _oled.println(F("SYSTEM"));
    _oled.println(F("STARTING..."));
    _oled.set1X();
}

void SSD1306::updateTime(int hours, int minutes, int seconds) {
    _oled.setCursor(0, 4);
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

void SSD1306::showCurrentStats(int16_t temp, uint16_t hum, uint32_t pres, bool isIcy) {
    _oled.setCursor(20, 0);
    _oled.println(F("CURRENT STATS"));

    if (isIcy) {
        _oled.setCursor(25, 1);
        _oled.println(F("!!! ICE !!!"));
    } else {
        _oled.println();
    }

    _oled.println(F("Temperature: "));
    formatNumber(bufferedValue, temp);
    _oled.print(F(" "));
    _oled.print(bufferedValue);
    _oled.println(F(" C"));

    _oled.println(F("Humidity: "));
    formatNumber(bufferedValue, hum);
    _oled.print(F(" "));
    _oled.print(bufferedValue);
    _oled.println(F(" %"));

    _oled.println(F("Pressure: "));
    formatNumber(bufferedValue, pres);
    _oled.print(F(" "));
    _oled.print(bufferedValue);
    _oled.println(F(" hPa"));
}

void SSD1306::showWeatherPrediction(const char* forecast, const char* timeframe) {
    _oled.setCursor(10, 0);
    _oled.println(F("WEATHER PREDICTION"));
    // _oled.println();

    _oled.println(F("Forecast: "));
    _oled.println(forecast);
    // _oled.println();

    _oled.println(F("Timeframe: "));
    _oled.print(timeframe);
}

void SSD1306::showDailyTemperatureStats(TemperatureDailyStats &stats) {
    _oled.setCursor(0, 0);
    _oled.print(F("DAY TEMPERATURE STATS"));
    _oled.println();

    _oled.println(F("Minimum: "));
    formatNumber(bufferedValue, stats.minTemp);
    _oled.print(bufferedValue);
    _oled.println(F(" C "));
    formatDateTime(bufferedDateTime, stats.minDay, stats.minMonth, stats.minYear, stats.minHour, stats.minMinute);
    _oled.println(bufferedDateTime);
    _oled.println();

    _oled.println(F("Maximum: "));
    formatNumber(bufferedValue, stats.maxTemp);
    _oled.print(bufferedValue);
    _oled.println(F(" C "));
    formatDateTime(bufferedDateTime, stats.maxDay, stats.maxMonth, stats.maxYear, stats.maxHour, stats.maxMinute);
    _oled.println(bufferedDateTime);
}

void SSD1306::showDailyHumidityStats(HumidityDailyStats &stats) {
    _oled.setCursor(8, 0);
    _oled.print(F("DAY HUMIDITY STATS"));
    _oled.println();

    _oled.println(F("Minimum: "));
    formatNumber(bufferedValue, stats.minHum);
    _oled.print(bufferedValue);
    _oled.println(F(" % "));
    formatDateTime(bufferedDateTime, stats.minDay, stats.minMonth, stats.minYear, stats.minHour, stats.minMinute);
    _oled.println(bufferedDateTime);
    _oled.println();

    _oled.println(F("Maximum: "));
    formatNumber(bufferedValue, stats.maxHum);
    _oled.print(bufferedValue);
    _oled.println(F(" % "));
    formatDateTime(bufferedDateTime, stats.maxDay, stats.maxMonth, stats.maxYear, stats.maxHour, stats.maxMinute);
    _oled.println(bufferedDateTime);
}

void SSD1306::showLifetimeTemperatureStats(TemperatureLifetimeStats &stats) {
    _oled.setCursor(0, 0);
    _oled.print(F("ALL TEMPERATURE STATS"));
    _oled.println();

    _oled.println(F("Minimum: "));
    formatNumber(bufferedValue, stats.minTemp);
    _oled.print(bufferedValue);
    _oled.println(F(" C "));
    formatDateTime(bufferedDateTime, stats.minDay, stats.minMonth, stats.minYear, stats.minHour, stats.minMinute);
    _oled.println(bufferedDateTime);
    _oled.println();

    _oled.println(F("Maximum: "));
    formatNumber(bufferedValue, stats.maxTemp);
    _oled.print(bufferedValue);
    _oled.println(F(" C "));
    formatDateTime(bufferedDateTime, stats.maxDay, stats.maxMonth, stats.maxYear, stats.maxHour, stats.maxMinute);
    _oled.println(bufferedDateTime);
}

void SSD1306::showLifetimeHumidityStats(HumidityLifetimeStats &stats) {
    _oled.setCursor(8, 0);
    _oled.print(F("ALL HUMIDITY STATS"));
    _oled.println();

    _oled.println(F("Minimum: "));
    formatNumber(bufferedValue, stats.minHum);
    _oled.print(bufferedValue);
    _oled.println(F(" % "));
    formatDateTime(bufferedDateTime, stats.minDay, stats.minMonth, stats.minYear, stats.minHour, stats.minMinute);
    _oled.println(bufferedDateTime);
    _oled.println();

    _oled.println(F("Maximum: "));
    formatNumber(bufferedValue, stats.maxHum);
    _oled.print(bufferedValue);
    _oled.println(F(" % "));
    formatDateTime(bufferedDateTime, stats.maxDay, stats.maxMonth, stats.maxYear, stats.maxHour, stats.maxMinute);
    _oled.println(bufferedDateTime);
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
    uint32_t tMin = getBatteryRemainingMinutes(battery, acs712);
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
    _oled.println(F(" C"));

    _oled.println(F("Version: 1.1"));
}

void SSD1306::clear() {
    _oled.clear();
}

void SSD1306::sleep() {
    clear();
    _oled.ssd1306WriteCmd(SSD1306_DISPLAYOFF);
}

void SSD1306::wake() {
    _oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
    clear();
}

Page SSD1306::readControls() {
    navigationButton.update();

    ButtonEvent e = navigationButton.getEvent();

    if (e != BUTTON_NONE) {

        switch (e) {
            case BUTTON_CLICK:
                currentPage = static_cast<Page> ((currentPage + 1) % PAGE_COUNT);
                break;

            case BUTTON_DOUBLE_CLICK:
                if (currentPage == 0) {
                    currentPage = static_cast<Page>(PAGE_COUNT - 1);
                } else {
                    currentPage = static_cast<Page>(currentPage - 1);
                }
                break;

            case BUTTON_LONG_CLICK:
                currentPage = PAGE_CURRENT_STATS;
                break;

            default:
                break;
        }

        navigationButton.clearEvent();
    }
    
    return currentPage;
}
