#ifndef WEATHER_PREDICTOR_H
#define WEATHER_PREDICTOR_H

#include <Arduino.h>
#include <stdint.h>

#define ICE_TEMPERATURE_THRESHOLD 150
#define ICE_HUMIDITY_THRESHOLD 7500
#define SHORT_TERM_PRESSURE_CHANGE_RAIN_THRESHOLD 2
#define SHORT_TERM_PRESSURE_CHANGE_STORM_THRESHOLD 3
#define SHORT_TERM_HUMIDITY_CHANGE_THRESHOLD 500

enum WeatherForecast {
    FORECAST_GOOD = 0,
    FORECAST_SETTLED,
    FORECAST_FINE,
    FORECAST_BECOMING_FINE,
    FORECAST_CHANGEABLE,
    FORECAST_UNSETTLED,
    FORECAST_RAIN,
    FORECAST_STORMY,
    FORECAST_SUDDEN_RAIN,
    FORECAST_SUDDEN_STORM,
    FORECAST_SNOW,
    FORECAST_UNKNOWN
};

// Εμπλουτισμένο Enum για σαφείς χρόνους αλλαγής
enum ForecastTimeframe {
    TIME_IMMINENT = 0, // Άμεσα (Επόμενες 1-2 ώρες)
    TIME_SOON,         // Σύντομα (Επόμενες 3-5 ώρες)
    TIME_LATER,        // Αργότερα (Επόμενες 6-12 ώρες)
    TIME_STABLE,       // Δεν αναμένεται αλλαγή / Σταθερός καιρός
    TIME_UNKNOWN       // Αναμονή ιστορικού (Γέμισμα πίνακα)
};

class WeatherPredictor {
private:
    uint32_t _history[12];
    uint8_t  _historyCount;
    uint16_t _humidityHistory[12];

    uint32_t _lastUpdateTime;
    const uint32_t _updateInterval = 600; // 10 minutes

    int32_t _currentTrendVal;
    int16_t _currentHumTrendVal;

    int8_t calculateTrend(uint32_t currentPres, uint32_t oldPres);
    int8_t calculateHumidityTrend(uint16_t currentHum, uint16_t oldHum);

public:
    WeatherPredictor();
    WeatherForecast addReading(uint32_t currentPressurePascal, uint32_t currentTime, bool isWinter, int16_t temperature, uint32_t humidity, uint8_t currentHour);
    
    ForecastTimeframe getTimeframe(uint16_t humidity);
    bool checkIceWarning(int16_t temperature, uint16_t humidity);
    
    const char* getForecastString(WeatherForecast forecast);
    const char* getTimeframeString(ForecastTimeframe timeframe);
};

#endif