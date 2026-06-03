#include "WeatherPredictor.h"
#include "Debugger.h"

WeatherPredictor::WeatherPredictor() : _historyCount(0), _lastUpdateTime(0), _currentTrendVal(0) {
    for(int i = 0; i < 6; i++) _history[i] = 0;
}

int8_t WeatherPredictor::calculateTrend(uint32_t currentPres, uint32_t oldPres) {
    int32_t curHpa = currentPres / 100;
    int32_t oldHpa = oldPres / 100;
    
    // Κρατάμε την ακριβή διαφορά hPa στο ιστορικό (συνήθως εύρος 3 ωρών)
    _currentTrendVal = curHpa - oldHpa; 

    if (_currentTrendVal >= 1)  return 1;  // Rising
    if (_currentTrendVal <= -1) return -1; // Falling
    return 0;                              // Steady
}

WeatherForecast WeatherPredictor::addReading(uint32_t currentPressurePascal, uint32_t currentTime, bool isWinter) {
    
    if (_historyCount == 0 || (currentTime - _lastUpdateTime >= _updateInterval)) {
        DBG(F("CURRENT TIME: "));
        DBG_LN(currentTime);
        DBG(F("LAST TIME: "));
        DBG_LN(_lastUpdateTime);
        DBG(F("WINTER: "));
        DBG_LN(isWinter);
        for (uint8_t i = 0; i < 5; i++) _history[i] = _history[i+1];
        _history[5] = currentPressurePascal;
        if (_historyCount < 6) _historyCount++;
        _lastUpdateTime = currentTime;
    }

    if (_historyCount < 3) return FORECAST_UNKNOWN;

    uint32_t oldPressure = _history[6 - _historyCount];
    int8_t trend = calculateTrend(currentPressurePascal, oldPressure);
    
    uint32_t hpa = currentPressurePascal / 100;
    uint8_t zambrettiZ = 32;

    if (trend == 1) {
        if (isWinter) hpa += 2;
        if (hpa >= 1030) zambrettiZ = 1;
        else if (hpa >= 1020) zambrettiZ = 2;
        else if (hpa >= 1010) zambrettiZ = 3;
        else if (hpa >= 1000) zambrettiZ = 4;
        else if (hpa >= 990)  zambrettiZ = 5;
        else zambrettiZ = 6;
    } 
    else if (trend == -1) {
        if (isWinter) hpa -= 2;
        if (hpa >= 1030) zambrettiZ = 20;
        else if (hpa >= 1020) zambrettiZ = 21;
        else if (hpa >= 1010) zambrettiZ = 22;
        else if (hpa >= 1000) zambrettiZ = 23;
        else if (hpa >= 990)  zambrettiZ = 24;
        else zambrettiZ = 25;
    } 
    else {
        if (hpa >= 1020) zambrettiZ = 10;
        else if (hpa >= 1010) zambrettiZ = 11;
        else if (hpa >= 1000) zambrettiZ = 12;
        else zambrettiZ = 13;
    }

    if (zambrettiZ <= 2)  return FORECAST_GOOD;
    if (zambrettiZ <= 4)  return FORECAST_SETTLED;
    if (zambrettiZ <= 10) return FORECAST_FINE;
    if (zambrettiZ <= 12) return FORECAST_BECOMING_FINE;
    if (zambrettiZ <= 19) return FORECAST_CHANGEABLE;
    if (zambrettiZ <= 22) return FORECAST_UNSETTLED;
    if (zambrettiZ <= 24) return FORECAST_RAIN;
    return FORECAST_STORMY;
}

// Υπολογισμός του ΠΟΤΕ
ForecastTimeframe WeatherPredictor::getTimeframe() {
    if (_historyCount < 3) return TIME_UNKNOWN;

    int32_t absDiff = abs(_currentTrendVal);

    // Αν η πίεση είναι σταθερή (μεταβολή μικρότερη από 1 hPa στο ιστορικό)
    if (absDiff <= 1) {
        return TIME_STABLE;
    }
    
    // Ραγδαία μεταβολή (πάνω από 3 hPa διαφορά) -> Έρχεται άμεσα!
    if (absDiff >= 3) {
        return TIME_IMMINENT;
    }
    
    // Μέτρια μεταβολή (2 hPa διαφορά) -> Έρχεται σύντομα
    if (absDiff == 2) {
        return TIME_SOON;
    }

    return TIME_LATER;
}

WindForecast WeatherPredictor::getWindPrediction() {
    if (_historyCount < 12) {
        return WIND_UNKNOWN;
    }

    // Παίρνουμε την απόλυτη διαφορά πίεσης σε hPa ανάμεσα στο τώρα και πριν 3 ώρες
    float currentHpa = _history[11] / 100.0;
    float oldHpa = _history[0] / 100.0;
    float pressureChange = abs(currentHpa - oldHpa);

    // Έλεγχος της μεταβολής για την πρόβλεψη ανέμου
    if (pressureChange >= 6.0) {
        return GALE_STORMY_WIND;
    } 
    else if (pressureChange >= 3.5) {
        return STRONG_WINDS;
    } 
    else if (pressureChange >= 1.5) {
        return MODERATE_BREEZES;
    } 
    else {
        return CALM_LIGHT_WIND;//;     
    }
}

const char* WeatherPredictor::getForecastString(WeatherForecast forecast) {
    switch (forecast) {
        case FORECAST_GOOD:          return "Good Weather\n(Stable)";
        case FORECAST_SETTLED:       return "Settled Weather";
        case FORECAST_FINE:          return "Fine / Clear";
        case FORECAST_BECOMING_FINE: return "Becoming Fine";
        case FORECAST_CHANGEABLE:    return "Changeable\nPartly Cloudy";
        case FORECAST_UNSETTLED:     return "Unsettled Weather";
        case FORECAST_RAIN:          return "Rainy";
        case FORECAST_STORMY:        return "Stormy / Gale";
        default:                     return "Calculating...";
    }
}

const char* WeatherPredictor::getWindString(WindForecast forecast) {
    switch (forecast) {
        case GALE_STORMY_WIND:  return "Gale / Stormy Winds!";  // Πολύ ισχυροί άνεμοι / Θύελλα
        case STRONG_WINDS:      return "Strong Winds Expected"; // Ισχυροί άνεμοι
        case MODERATE_BREEZES:  return "Moderate Breezes";      // Μέτριος αέρας / Αγέρας
        case CALM_LIGHT_WIND:   return "Calm / Light Wind";     // Άπνοια / Ασθενής άνεμος
        default:                return "Calculating...";
    }
}

const char* WeatherPredictor::getTimeframeString(ForecastTimeframe timeframe) {
    switch (timeframe) {
        case TIME_IMMINENT: return "IMMINENT!\n(Next 1-3 hours)";
        case TIME_SOON:     return "Soon\n(Next 3-6 hours)";
        case TIME_LATER:    return "Later\n(Next 6-12 hours)";
        case TIME_STABLE:   return "Stable weather\n(No change expected)";
        default:            return "Waiting for data...";
    }
}