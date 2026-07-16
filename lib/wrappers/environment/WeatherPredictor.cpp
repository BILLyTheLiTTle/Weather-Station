#include "WeatherPredictor.h"
#include "Debugger.h"

WeatherPredictor::WeatherPredictor() : _historyCount(0), _lastUpdateTime(0), _currentTrendVal(0), _currentHumTrendVal(0) {
    for(int i = 0; i < 12; i++) {
        _history[i] = 0;
        _humidityHistory[i] = 0;
    }
}

int8_t WeatherPredictor::calculateTrend(uint32_t currentPres, uint32_t oldPres) {
    int32_t curHpa = currentPres / 100;
    int32_t oldHpa = oldPres / 100;
    
    if (_historyCount == 12) {
        _currentTrendVal = (int32_t)(_history[11] / 100) - (int32_t)(_history[0] / 100);
    } else {
        _currentTrendVal = curHpa - oldHpa; 
    }

    if (_currentTrendVal >= 1)  return 1;  
    if (_currentTrendVal <= -1) return -1; 
    return 0;                             
}

int8_t WeatherPredictor::calculateHumidityTrend(uint16_t currentHum, uint16_t oldHum) {
    if (_historyCount == 12) {
        _currentHumTrendVal = (int16_t)_humidityHistory[11] - (int16_t)_humidityHistory[0];
    } else {
        _currentHumTrendVal = (int16_t)currentHum - (int16_t)oldHum;
    }

    // Κρατάμε το 100 γιατί ο DHT επιστρέφει scaled τιμή (π.χ. 4150 για 41.5% υγρασία)
    if (_currentHumTrendVal >= 100)  return 1;  
    if (_currentHumTrendVal <= -100) return -1; 
    return 0;                                   
}

WeatherForecast WeatherPredictor::addReading(uint32_t currentPressurePascal, uint32_t currentTime, bool isWinter, int16_t temperature, uint32_t humidity, uint8_t currentHour) {
    
    if (_historyCount == 0 || (currentTime - _lastUpdateTime >= _updateInterval)) {
        for (uint8_t i = 0; i < 11; i++) {
            _history[i] = _history[i+1];
            _humidityHistory[i] = _humidityHistory[i+1];
        }
        _history[11] = currentPressurePascal; 
        _humidityHistory[11] = humidity;      
        
        if (_historyCount < 12) _historyCount++;
        _lastUpdateTime = currentTime;
    }

    if (_historyCount < 3) return FORECAST_UNKNOWN;

    // =================================================================
    // 1. ΜΑΚΡΟΠΡΟΘΕΣΜΗ ΠΡΟΓΝΩΣΗ (ZAMBRETTI)
    // =================================================================
    uint8_t oldIndex = (_historyCount >= 12) ? 0 : (12 - _historyCount);
    uint32_t oldPressure = _history[oldIndex];
    int8_t trend = calculateTrend(currentPressurePascal, oldPressure);
    
    uint16_t oldHumidity = _humidityHistory[oldIndex];
    calculateHumidityTrend(humidity, oldHumidity); 
    
    uint32_t hpa = currentPressurePascal / 100;

    // ΕΠΙΣΤΗΜΟΝΙΚΗ ΔΙΟΡΘΩΣΗ ΕΠΟΧΗΣ (Zambretti Baseline Adjustment)
    // Αν είναι Καλοκαίρι (!isWinter), προσθέτουμε offset γιατί η ζέστη ρίχνει την πίεση χωρίς βροχή.
    // Αν είναι Χειμώνας (isWinter), αφαιρούμε offset.
    if (!isWinter) {
        hpa += 2; 
    } else {
        hpa -= 2; 
    }

    uint8_t zambrettiZ = 32;

    if (trend == 1) { // Πίεση Ανεβαίνει
        if (hpa >= 1030) zambrettiZ = 1;
        else if (hpa >= 1020) zambrettiZ = 2;
        else if (hpa >= 1010) zambrettiZ = 3;
        else if (hpa >= 1000) zambrettiZ = 4;
        else if (hpa >= 990)  zambrettiZ = 5;
        else zambrettiZ = 6;
    } 
    else if (trend == -1) { // Πίεση Πέφτει
        if (hpa >= 1030) zambrettiZ = 20;
        else if (hpa >= 1020) zambrettiZ = 21;
        else if (hpa >= 1010) zambrettiZ = 22;
        else if (hpa >= 1000) zambrettiZ = 23;
        else if (hpa >= 990)  zambrettiZ = 24;
        else zambrettiZ = 25;
    } 
    else { // Σταθερή Πίεση
        if (hpa >= 1016)      zambrettiZ = 10; 
        else if (hpa >= 1010) zambrettiZ = 11;
        else if (hpa >= 1002) zambrettiZ = 12;
        else zambrettiZ = 13;
    }

    WeatherForecast finalForecast;
    if (zambrettiZ <= 2)       finalForecast = FORECAST_GOOD;
    else if (zambrettiZ <= 4)  finalForecast = FORECAST_SETTLED;
    else if (zambrettiZ <= 10) finalForecast = FORECAST_FINE;
    else if (zambrettiZ <= 12) finalForecast = FORECAST_BECOMING_FINE;
    else if (zambrettiZ <= 19) finalForecast = FORECAST_CHANGEABLE;
    else if (zambrettiZ <= 22) finalForecast = FORECAST_UNSETTLED;
    else if (zambrettiZ <= 24) finalForecast = FORECAST_RAIN;
    else                       finalForecast = FORECAST_STORMY;

    // =================================================================
    // 2. ΒΡΑΧΥΠΡΟΘΕΣΜΟ ΡΑΝΤΑΡ (Short-term Override - 30 λεπτά / Θέση 8)
    // =================================================================
    if (_historyCount == 12) {
        int32_t shortPressChange = (int32_t)(_history[11] / 100) - (int32_t)(_history[8] / 100);
        int16_t shortHumChange = (int16_t)_humidityHistory[11] - (int16_t)_humidityHistory[8];

        // Εφαρμογή των σωστών προσήμων (-) και των δικών σου καθορισμένων define thresholds
        if (shortPressChange <= -SHORT_TERM_PRESSURE_CHANGE_RAIN_THRESHOLD && shortHumChange >= SHORT_TERM_HUMIDITY_CHANGE_THRESHOLD) { 
            finalForecast = FORECAST_SUDDEN_RAIN; 
        }
        else if (shortPressChange <= -SHORT_TERM_PRESSURE_CHANGE_STORM_THRESHOLD) {
            finalForecast = FORECAST_SUDDEN_STORM; 
        }
    }

    // =================================================================
    // 3. ΔΥΝΑΜΙΚΑ ΟΡΙΑ ΒΑΣΕΙ ΩΡΑΣ
    // =================================================================
    uint16_t rainHumidityThreshold = 7500; // 75% την ημέρα (αντί για 80%)
    if (currentHour >= 21 || currentHour <= 7) {
        rainHumidityThreshold = 8500;      // 85% τη νύχτα (αντί για 90%)
    }

    if (finalForecast <= FORECAST_BECOMING_FINE) {
        if (humidity >= rainHumidityThreshold) { 
            finalForecast = FORECAST_RAIN; 
        }
        else if (humidity >= (rainHumidityThreshold - 1000) && _currentHumTrendVal > 0) {
            finalForecast = FORECAST_UNSETTLED; 
        }
    }

    if (finalForecast == FORECAST_RAIN || finalForecast == FORECAST_STORMY || 
        finalForecast == FORECAST_SUDDEN_RAIN || finalForecast == FORECAST_SUDDEN_STORM) {
        if (temperature <= ICE_TEMPERATURE_THRESHOLD && humidity >= ICE_HUMIDITY_THRESHOLD) {
            finalForecast = FORECAST_SNOW;
        }
    }

    return finalForecast;
}

ForecastTimeframe WeatherPredictor::getTimeframe(uint16_t humidity) {
    if (_historyCount < 12) return TIME_UNKNOWN;

    int32_t pressTrend = _currentTrendVal;  
    int16_t humTrend = _currentHumTrendVal; 
    uint32_t currentHpa = _history[11] / 100;

    if (pressTrend <= -1) { 
        if (humTrend >= 100) return TIME_IMMINENT; 
        else return TIME_LATER;    
    }
    if (pressTrend >= 1) {
        if (humTrend <= -100) return TIME_IMMINENT; 
        else return TIME_LATER;    
    }
    if (currentHpa < 1005) {
        if (humTrend >= 100 || humidity >= 6500) return TIME_SOON;     
    }
    return TIME_STABLE; 
}

const char* WeatherPredictor::getForecastString(WeatherForecast forecast) {
    // --- ΠΑΤΕΝΤΑ ΜΝΗΜΗΣ ΠΡΟΣΦΑΤΗΣ ΜΠΟΡΑΣ (ΔΙΟΡΘΩΜΕΝΗ) ---
    // Ελέγχουμε αν την τελευταία 1.5 ώρα υπήρξε πτώση >= 2 hPa σε παράθυρο 1 ώρας (6 θέσεις)
    bool recentStormAlert = false;
    if (_historyCount == 12) {
        for(uint8_t i = 0; i < 6; i++) {
            if ((int32_t)(_history[i+6]/100) - (int32_t)(_history[i]/100) <= -2) {
                recentStormAlert = true;
                break;
            }
        }
    }

    // Προσθήκη του χαρακτήρα '*' στα switch cases για να ξέρεις ότι υπήρξε πρόσφατο alert
    switch (forecast) {
        // ΚΑΛΟΣ ΚΑΙΡΟΣ: Μια χαρά μέρα, ήπιος ουρανός, φυσιολογική πίεση
        case FORECAST_GOOD:          return recentStormAlert ? "Mild/Fair *\n[Open Sky]" : "Mild/Fair\n[Open Sky]";
        // ΣΤΑΘΕΡΟΣ ΚΑΙΡΟΣ: Κλειδωμένη καλοκαιρία μεγάλης διάρκειας, ακλόνητος
        case FORECAST_SETTLED:       return recentStormAlert ? "Stable Fair *\n[Clear/Dry Air]" : "Stable Fair\n[Clear/Dry Air]";
        // Ο ΠΙΟ ΚΑΘΑΡΟΣ ΚΑΙΡΟΣ: Απόλυτη, κρυστάλλινη ηλιοφάνεια & υψηλή πίεση
        case FORECAST_FINE:          return recentStormAlert ? "Clear Sky *\n[High Pressure]" : "Clear Sky\n[High Pressure]";
        // Βελτίωση: Ο καιρός καθαρίζει και πηγαίνει προς το Fine (Διορθώθηκε το typo)
        case FORECAST_BECOMING_FINE: return recentStormAlert ? "Improving *\n[Clearing Up]" : "Improving\n[Clearing Up]";
        // Μεταβατικός: Άστατος καιρός, μισός ήλιος / μισή συννεφιά
        case FORECAST_CHANGEABLE:    return "Cloudy/Stable\n[No Rain Expected]";
        // Άστατος: Κακός καιρός, αλλά με διαστήματα βελτίωσης
        case FORECAST_UNSETTLED:     return "Unsettled\n[Variable/Changes]";
        // Βροχή: Κλειστός, γκρίζος και βρεγμένος καιρός
        case FORECAST_RAIN:          return "Rainy\n[Wet/Overcast]";
        // Καταιγίδα: Έντονα φαινόμενα, θύελλα
        case FORECAST_STORMY:        return "Stormy/Gale\n[Severe Storm]";
        // Ξαφνική Βροχή: Ραγδαία πτώση πίεσης, βροχή πιθανον αμεσα
        case FORECAST_SUDDEN_RAIN:    return "Sudden Rain\n[Radar Alert!]";
        // Ξαφνική Καταιγίδα: Ραγδαία πτώση πίεσης, καταιγίδα πιθανον αμεσα
        case FORECAST_SUDDEN_STORM:   return "Sudden Storm\n[Severe Squall!]";
        // ΧΙΟΝΙ: Υετός με θερμοκρασία <= 1.5°C
        case FORECAST_SNOW:          return "Snow\n[Heavy/Frost]";
        default:                     return "Calculating...";
    }
}

const char* WeatherPredictor::getTimeframeString(ForecastTimeframe timeframe) {
    switch (timeframe) {
        case TIME_IMMINENT: return "In 1-2 HOURS!\n[Fast Shift/Sure]";          // Η ΠΙΟ ΚΟΝΤΙΝΗ/ΣΙΓΟΥΡΗ ΑΛΛΑΓΗ: Ραγδαία πτώση, έρχεται άμεσα
        case TIME_SOON:     return "In 3-5 HOURS\n[Almost ready/High Prob]";    // ΥΨΗΛΗ ΕΤΟΙΜΟΤΗΤΑ: Η υγρασία ανέβηκε, ο καιρός κλειδώνει για βροχή
        case TIME_LATER:    return "In 6-12 HOURS\n[Slow Move/Low Prob]";       // ΜΑΚΡΙΝΗ ΕΚΤΙΜΗΣΗ: Μικρή πιθανότητα για τώρα, ο καιρός θέλει χρόνο ακόμα
        case TIME_STABLE:   return "Stable Weather\n[No Change Ahead]";         // Σταθερότητα: Καμία αλλαγή στον ορίζοντα
        default:            return "Analyzing trend...";                        // Μέχρι να γεμίσει το 3ωρο
    }
}

bool WeatherPredictor::checkIceWarning(int16_t temperature, uint16_t humidity) {
    if (temperature <= 200 && humidity >= 8000) {
        return true;
    }
    return false;
}