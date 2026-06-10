#include "./power/PowerSaver.h"
#include "./system/SystemHelper.h"
#include "Debugger.h"
#include "ACS712.h"
#include "environment/EnvironmentManager.h"
#include "environment/WeatherPredictor.h"
#include "DS3231.h"
#include "SSD1306.h"

Thermistor therm(
    A0,        // analog pin
    9820,      // The real value of series resistor (10k)
    10000,     // nominal resistance (10k thermistor) from thermistor datasheet
    25,        // nominal temp (25°C) from thermistor datasheet
    ThermistorConstants::BETA_4250   // Beta coefficient (β) of the thermistor
);

SleepMode sleepSwitch(3, 2);
Battery battery(A1, 9810, 14830);
ACS712 acs712(A2, ACS712_05B, 2200);
bool shouldStickToSystemStats = false;

EnvironmentManager envMan;
EEPROM_25LC040A eeprom(10);
TemperatureDailyStats td;
TemperatureLifetimeStats tl;
HumidityDailyStats hd;
HumidityLifetimeStats hl;

MemoryProfiler ram(2048);

DHT_Sensor environmentSensor(9, DHT_Sensor::DHT22);
BME280Sensor bmp;
WeatherPredictor predictor;
WeatherForecast forecast;
WindForecast wind;
ForecastTimeframe timeframe;
bool isIcy;

DS3231 rtc;

void navigate(Page page, bool forceRender);

void setup() {
    DBG_BEGIN(9600);

    // For calibration we need to remove all power sources (Vin, batteries) apart from USB. After calibration comment it again
    // acs712.calibrate();

    battery.begin();

    eeprom.begin();
    // To reset the stored data uncomment the factory reset and comment the load lifetime data. Don't forget to undo the commenting for a regular run
    // eeprom.factoryReset();
    eeprom.loadLifetimeTemperature(tl);
    eeprom.loadLifetimeHumidity(hl);

    sleepSwitch.begin();

    Wire.setWireTimeout(25000, true);
    if (!rtc.begin(false)) { // When RTC is out of sync or on hold then use `begin(true)`.
        DBG_LN(F("RTC not found! Retrying in 2 seconds..."));
        delay(2000);
    }

    display.begin();
    display.showBootMessage();

    while (!bmp.begin(0x76)) {
        DBG_LN(F("Pressure sensor error!"));
        delay(1000);
    }
}

void loop() {
    // ==========================================
    // 1. FAILSAFE PART
    // ==========================================
    if (Wire.getWireTimeoutFlag()) {
        DBG_LN(F("I2C Timeout!"));
        Wire.clearWireTimeoutFlag();

        if (battery.isUsbPowered()) {
            rtc.updateWithSystemTime();
        } else {
            rtc.reset();
        }         
    }

    // ==========================================
    // 2. IMMEDIATE EXECUTION
    // ==========================================
    Page screen = display.readControls();
    if (shouldStickToSystemStats) {
        screen = PAGE_SYSTEM_STATS;
    }
    navigate(screen, false);

    if (rtc.alarmFired()) {
        rtc.clearAlarm();
    }

    enterConditionalSleep(rtc, sleepSwitch);

    // ==========================================
    // 3. DELAYED EXECUTION
    // ==========================================
    if (isIntervalElapsed() && sleepSwitch.getState() != SystemState::SLEEP) {

        bmp.update();

        shouldStickToSystemStats = !battery.isUsbPowered() && battery.readVoltage() <= Battery::LOWER_BOUND_VOLTAGE;

        uint32_t currentPres = envMan.getCurrentPres();
        int16_t currentTemp = envMan.getCurrentTemp();
        uint16_t currentHum = envMan.getCurrentHum();

        uint8_t currentMonth = rtc.getMonth();
        bool winterFlag = (currentMonth >= 10 || currentMonth <= 3);
        
        forecast = predictor.addReading(currentPres, rtc.getTimestamp(), winterFlag, currentTemp, currentHum);
        wind = predictor.getWindPrediction();
        timeframe = predictor.getTimeframe(currentHum);

        // --- 3. ΠΡΟΣΘΗΚΗ ΓΙΑ ΤΟΝ ΠΑΓΟ (ICE WARNING) ---
        isIcy = predictor.checkIceWarning(currentTemp, currentHum);
        if (isIcy) {
            DBG_LN(F("!!!! WARNING: ICY ROADS !!!!"));
        }

        if (shouldStickToSystemStats) {
            screen = PAGE_SYSTEM_STATS;
        }
        navigate(screen, true); 

        DBG_LN(F("=*=*=*= START =*=*=*="));
        DBG_LN(F("-*-*-*- Environment Stats -*-*-*-"));
        envMan.printEnvironmentStats(bmp, environmentSensor, eeprom, rtc, td, tl, hd, hl);
        DBG_LN(F("-*-*-*- Weather Prediction -*-*-*-"));
        DBG("Forecast: ");
        DBG_LN(predictor.getForecastString(forecast));
        DBG("Wind: ");
        DBG_LN(predictor.getWindString(wind));
        DBG("Timeframe: ");
        DBG_LN(predictor.getTimeframeString(timeframe));
        
        // Εκτύπωση αν έχει πάγο στο Serial Monitor
        DBG("Road Ice Warning: ");
        DBG_LN(isIcy ? "YES" : "NO");

        DBG_LN(F("-*-*-*- System Stats -*-*-*-"));
        printSystemStats(battery, acs712, ram, therm);
        DBG(F("=*=*=*= END =*=*=*=\n"));
    }
}

void navigate(Page page, bool forceRender) {
    static Page currentPage = PAGE_COUNT;

    if (page == currentPage && forceRender == false) return;

    currentPage = page;
    display.clear();
    switch (page) {
        case PAGE_CURRENT_STATS:
            display.showCurrentStats(envMan.getCurrentTemp(), envMan.getCurrentHum(), envMan.getCurrentPres(), isIcy);
            break;

        case PAGE_WEATHER_PREDICTION:
            display.showWeatherPrediction(
                predictor.getForecastString(forecast),
                predictor.getWindString(wind),
                predictor.getTimeframeString(timeframe)
            );
            break;

        case PAGE_DAILY_TEMPERATURE_STATS:
            display.showDailyTemperatureStats(td);
            break;

        case PAGE_DAILY_HUMIDITY_STATS:
            display.showDailyHumidityStats(hd);
            break;

        case PAGE_LIFETIME_TEMPERATURE_STATS:
            display.showLifetimeTemperatureStats(tl);
            break;

        case PAGE_LIFETIME_HUMIDITY_STATS:
            display.showLifetimeHumidityStats(hl);
            break;
        
        case PAGE_SYSTEM_STATS:
            display.showSystemStats(battery, acs712, ram, therm);
            break;

        default:
            break;
    }
}