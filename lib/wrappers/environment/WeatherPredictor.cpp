#include "WeatherPredictor.h"
#include "Debugger.h"

WeatherPredictor::WeatherPredictor() : _historyCount(0), _lastUpdateTime(0), _currentTrendVal(0) {
    for(int i = 0; i < 12; i++) _history[i] = 0; // 👈 Αρχικοποίηση 12 θέσεων
}

int8_t WeatherPredictor::calculateTrend(uint32_t currentPres, uint32_t oldPres) {
    int32_t curHpa = currentPres / 100;
    int32_t oldHpa = oldPres / 100;
    
    // Όταν γεμίσει ο πίνακας (12 θέσεις), η πραγματική τάση χρόνου υπολογίζεται 
    // ανάμεσα στο Τώρα (_history[11]) και πριν 2.8 ώρες (_history[0])
    if (_historyCount == 12) {
        _currentTrendVal = (int32_t)(_history[11] / 100) - (int32_t)(_history[0] / 100);
    } else {
        _currentTrendVal = curHpa - oldHpa; 
    }

    if (curHpa - oldHpa >= 1)  return 1;  // Rising
    if (curHpa - oldHpa <= -1) return -1; // Falling
    return 0;                              // Steady
}

WeatherForecast WeatherPredictor::addReading(uint32_t currentPressurePascal, uint32_t currentTime, bool isWinter) {
    
    if (_historyCount == 0 || (currentTime - _lastUpdateTime >= _updateInterval)) {
        DBG(F("CURRENT TIME: ")); DBG_LN(currentTime);
        DBG(F("LAST TIME: "));    DBG_LN(_lastUpdateTime);
        DBG(F("WINTER: "));       DBG_LN(isWinter);
        
        // Ολίσθηση (Shift) για τον νέο πίνακα 12 θέσεων (0 έως 11)
        for (uint8_t i = 0; i < 11; i++) _history[i] = _history[i+1];
        _history[11] = currentPressurePascal; // Το "τώρα" μπαίνει στην τελευταία θέση
        
        if (_historyCount < 12) _historyCount++;
        _lastUpdateTime = currentTime;
    }

    // Χρειαζόμαστε τουλάχιστον 3 μετρήσεις (42 λεπτά) για μια πρώτη πρόχειρη πρόβλεψη
    if (_historyCount < 3) return FORECAST_UNKNOWN;

    // Παίρνουμε την παλαιότερη διαθέσιμη μέτρηση ανάλογα με το πόσες έχουμε μαζέψει
    uint32_t oldPressure = _history[12 - _historyCount];
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

ForecastTimeframe WeatherPredictor::getTimeframe() {
    // Περιμένουμε να γεμίσει πλήρως ο 12άρης πίνακας για ακρίβεια χρόνου (2.8 ώρες)
    if (_historyCount < 12) return TIME_UNKNOWN;

    int32_t absDiff = abs(_currentTrendVal);

    // Επίσημα μετεωρολογικά κριτήρια τάσης για παράθυρο ~3 ωρών
    if (absDiff <= 1) return TIME_STABLE;   // Μεταβολή 0-1 hPa -> Σταθερός καιρός
    if (absDiff >= 4) return TIME_IMMINENT; // Μεταβολή >= 4 hPa -> Ραγδαία αλλαγή (Άμεσα)
    if (absDiff >= 2) return TIME_SOON;     // Μεταβολή 2-3 hPa -> Κανονική αλλαγή (Σύντομα)

    return TIME_LATER;                      // Μεταβολή 1.5 hPa (Αργότερα)
}

WindForecast WeatherPredictor::getWindPrediction() {
    if (_historyCount < 12) {
        return WIND_UNKNOWN;
    }

    // Υπολογισμός άνεμου στο πλήρες τρίωρο παράθυρο πλέον
    float currentHpa = _history[11] / 100.0;
    float oldHpa = _history[0] / 100.0;
    float pressureChange = abs(currentHpa - oldHpa);

    if (pressureChange >= 5.0)      return GALE_STORMY_WIND;
    else if (pressureChange >= 3.0) return STRONG_WINDS;
    else if (pressureChange >= 1.5) return MODERATE_BREEZES;
    else                            return CALM_LIGHT_WIND;     
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
        case GALE_STORMY_WIND:  return "Gale / Stormy Winds";
        case STRONG_WINDS:      return "Strong Winds";
        case MODERATE_BREEZES:  return "Moderate Breezes";
        case CALM_LIGHT_WIND:   return "Calm / Light Wind";
        default:                return "Calculating...";
    }
}

const char* WeatherPredictor::getTimeframeString(ForecastTimeframe timeframe) {
    switch (timeframe) {
        case TIME_IMMINENT: return "In 1 to 2 HOURS!\n(Rapid Change)";
        case TIME_SOON:     return "In 3 to 5 HOURS\n(Normal Change)";
        case TIME_LATER:    return "In 6 to 12 HOURS\n(Slow Change)";
        case TIME_STABLE:   return "Stable weather\n(No change)";
        default:            return "Analyzing trend..."; // Μέχρι να γεμίσει το 3ωρο
    }
}