#ifndef WEATHER_PREDICTOR_H
#define WEATHER_PREDICTOR_H

#include <Arduino.h>
#include <stdint.h>

enum WeatherForecast {
    FORECAST_GOOD = 0, FORECAST_SETTLED, FORECAST_FINE,
    FORECAST_BECOMING_FINE, FORECAST_CHANGEABLE,
    FORECAST_UNSETTLED, FORECAST_RAIN, FORECAST_STORMY, FORECAST_UNKNOWN
};

// Νέο Enum για το πότε θα γίνει η αλλαγή
enum ForecastTimeframe {
    TIME_IMMINENT = 0, // Άμεσα (Επόμενες 1-3 ώρες)
    TIME_SOON,         // Σύντομα (Επόμενες 3-6 ώρες)
    TIME_LATER,        // Αργότερα (Επόμενες 6-12 ώρες)
    TIME_STABLE,       // Δεν αναμένεται αλλαγή / Σταθερός καιρός
    TIME_UNKNOWN       // Αναμονή ιστορικού
};

class WeatherPredictor {
private:
    uint32_t _history[6];
    uint8_t  _historyCount;
    uint32_t _lastUpdateTime;
    const uint32_t _updateInterval = 840000; // 14 minutes

    int32_t _currentTrendVal; // Αποθήκευση της διαφοράς σε hPa

    int8_t calculateTrend(uint32_t currentPres, uint32_t oldPres);

public:
    WeatherPredictor();
    WeatherForecast addReading(uint32_t currentPressurePascal, uint32_t currentTime, bool isWinter);
    
    // Νέα συνάρτηση που σου επιστρέφει το πότε
    ForecastTimeframe getTimeframe(); 
    
    const char* getForecastString(WeatherForecast forecast);
    const char* getTimeframeString(ForecastTimeframe timeframe); // Text για το χρόνο
};

#endif