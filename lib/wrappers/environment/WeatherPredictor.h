#ifndef WEATHER_PREDICTOR_H
#define WEATHER_PREDICTOR_H

#include <Arduino.h>
#include <stdint.h>

enum WeatherForecast {
    FORECAST_GOOD = 0,
    FORECAST_SETTLED,
    FORECAST_FINE,
    FORECAST_BECOMING_FINE,
    FORECAST_CHANGEABLE,
    FORECAST_UNSETTLED,
    FORECAST_RAIN,
    FORECAST_STORMY,
    FORECAST_UNKNOWN
};

enum WindForecast {
    GALE_STORMY_WIND = 0,
    STRONG_WINDS,
    MODERATE_BREEZES,
    CALM_LIGHT_WIND,
    WIND_UNKNOWN
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
    uint32_t _lastUpdateTime;
    const uint32_t _updateInterval = 840; // 14 minutes

    int32_t _currentTrendVal; // Αποθήκευση της διαφοράς σε hPa

    int8_t calculateTrend(uint32_t currentPres, uint32_t oldPres);

public:
    WeatherPredictor();
    WeatherForecast addReading(uint32_t currentPressurePascal, uint32_t currentTime, bool isWinter);
    
    ForecastTimeframe getTimeframe();
    WindForecast getWindPrediction();
    
    const char* getForecastString(WeatherForecast forecast);
    const char* getWindString(WindForecast forecast);
    const char* getTimeframeString(ForecastTimeframe timeframe);
};

#endif