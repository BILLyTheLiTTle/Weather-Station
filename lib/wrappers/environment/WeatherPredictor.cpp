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

    if (_historyCount < 2) {
        if (humidity >= 8000) return FORECAST_RAIN;
        return FORECAST_UNKNOWN;
    }

    // =================================================================
    // 1. ΒΡΑΧΥΠΡΟΘΕΣΜΟ ΡΑΝΤΑΡ (Προτεραιότητα στη Βροχή)
    // =================================================================
    uint8_t oldIndex = (_historyCount >= 12) ? 0 : (12 - _historyCount);
    uint32_t oldPressure = _history[oldIndex];
    calculateTrend(currentPressurePascal, oldPressure);
    
    uint16_t oldHumidity = _humidityHistory[oldIndex];
    calculateHumidityTrend(humidity, oldHumidity); 

    int32_t shortPressChange = 0;
    int16_t shortHumChange = 0;

    if (_historyCount >= 3) {
        uint8_t radarIndex = (_historyCount >= 3) ? (_historyCount - 3) : 0;
        shortPressChange = (int32_t)(_history[_historyCount-1] / 100) - (int32_t)(_history[radarIndex] / 100);
        shortHumChange = (int16_t)_humidityHistory[_historyCount-1] - (int16_t)_humidityHistory[radarIndex];
    }

    // ΑΠΟΛΥΤΑ ΚΡΙΤΗΡΙΑ ΒΡΟΧΗΣ (Real-time Overrule)
    uint16_t criticalHumidity = (currentHour >= 21 || currentHour <= 7) ? 8800 : 7800;
    if (humidity >= criticalHumidity) {
        return (temperature <= ICE_TEMPERATURE_THRESHOLD) ? FORECAST_SNOW : FORECAST_RAIN;
    }

    if (shortPressChange <= -SHORT_TERM_PRESSURE_CHANGE_STORM_THRESHOLD) {
        return FORECAST_SUDDEN_STORM;
    }
    if (shortPressChange <= -SHORT_TERM_PRESSURE_CHANGE_RAIN_THRESHOLD && shortHumChange >= SHORT_TERM_HUMIDITY_CHANGE_THRESHOLD) {
        return FORECAST_SUDDEN_RAIN;
    }

    // =================================================================
    // 2. ΜΑΚΡΟΠΡΟΘΕΣΜΗ ΠΡΟΓΝΩΣΗ (ZAMBRETTI)
    // =================================================================
    uint32_t hpa = currentPressurePascal / 100;
    if (!isWinter) hpa += 2; 
    else hpa -= 2; 

    uint8_t zambrettiZ = 32;

    if (_currentTrendVal == 1) { 
        if (hpa >= 1030) zambrettiZ = 1;
        else if (hpa >= 1020) zambrettiZ = 2;
        else if (hpa >= 1010) zambrettiZ = 3;
        else if (hpa >= 1000) zambrettiZ = 4;
        else if (hpa >= 990)  zambrettiZ = 5;
        else zambrettiZ = 6;
    } 
    else if (_currentTrendVal == -1) { 
        if (hpa >= 1030) zambrettiZ = 20;
        else if (hpa >= 1020) zambrettiZ = 21;
        else if (hpa >= 1010) zambrettiZ = 22;
        else if (hpa >= 1000) zambrettiZ = 23;
        else if (hpa >= 990)  zambrettiZ = 24;
        else zambrettiZ = 25;
    } 
    else { 
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
    // LOW PRESSURE GUARD (Ακύρωση Fair/Fine αν είμαστε <= 1008 hPa)
    // =================================================================
    uint32_t rawHpa = currentPressurePascal / 100;
    if (rawHpa <= 1008 && (finalForecast == FORECAST_GOOD || finalForecast == FORECAST_SETTLED || finalForecast == FORECAST_FINE || finalForecast == FORECAST_BECOMING_FINE)) {
        finalForecast = FORECAST_CHANGEABLE;
    }

    if (finalForecast == FORECAST_RAIN && temperature <= ICE_TEMPERATURE_THRESHOLD && humidity >= ICE_HUMIDITY_THRESHOLD) {
        finalForecast = FORECAST_SNOW;
    }

    return finalForecast;
}

ForecastTimeframe WeatherPredictor::getTimeframe(uint16_t humidity) {
    if (_historyCount < 3) return TIME_UNKNOWN;

    int32_t pressTrend = _currentTrendVal;  
    int16_t humTrend = _currentHumTrendVal; 
    uint32_t currentHpa = _history[_historyCount - 1] / 100;

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
    // =================================================================
    // ΜΝΗΜΗ ΕΓΚΑΙΡΗΣ ΠΡΟΕΙΔΟΠΟΙΗΣΗΣ (Radar Alert Memory)
    // =================================================================
    bool radarWarningTriggered = false;

    // Σάρωση όλων των ζευγών στο ιστορικό του 3ώρου
    if (_historyCount >= 2) {
        for (uint8_t i = 0; i < _historyCount - 1; i++) {
            for (uint8_t j = i + 1; j < _historyCount; j++) {
                int32_t pressDiff = (int32_t)(_history[j] / 100) - (int32_t)(_history[i] / 100);
                int16_t humDiff = (int16_t)_humidityHistory[j] - (int16_t)_humidityHistory[i];

                if (pressDiff <= -SHORT_TERM_PRESSURE_CHANGE_STORM_THRESHOLD || 
                   (pressDiff <= -SHORT_TERM_PRESSURE_CHANGE_RAIN_THRESHOLD && humDiff >= SHORT_TERM_HUMIDITY_CHANGE_THRESHOLD)) {
                    radarWarningTriggered = true;
                    break;
                }
            }
            if (radarWarningTriggered) break;
        }
    }

    switch (forecast) {
        case FORECAST_GOOD:          return radarWarningTriggered ? "Mild/Fair *\n[Radar]" : "Mild/Fair\n[Open Sky]";
        case FORECAST_SETTLED:       return radarWarningTriggered ? "Stable Fair *\n[Radar]" : "Stable Fair\n[Clear/Dry Air]";
        case FORECAST_FINE:          return radarWarningTriggered ? "Clear Sky *\n[Radar]" : "Clear Sky\n[High Pressure]";
        case FORECAST_BECOMING_FINE: return radarWarningTriggered ? "Improving *\n[Radar]" : "Improving\n[Clearing Up]";
        case FORECAST_CHANGEABLE:    return radarWarningTriggered ? "Cloudy *\n[Rain Imminent]" : "Cloudy/Stable\n[No Rain]";
        case FORECAST_UNSETTLED:     return radarWarningTriggered ? "Unsettled *\n[Rain Imminent]" : "Unsettled\n[Variable/Changes]";
        
        // Όταν η βροχή/καταιγίδα εκδηλωθεί ενεργά ή είναι σε εξέλιξη:
        case FORECAST_RAIN:          return "Rainy\n[Wet]";
        case FORECAST_STORMY:        return "Stormy/Gale\n[Storm]";
        case FORECAST_SUDDEN_RAIN:   return "Sudden Rain *\n[Radar Alert]";
        case FORECAST_SUDDEN_STORM:  return "Sudden Storm *\n[Squall]";
        case FORECAST_SNOW:          return "Snow\n[Frost]";
        default:                     return "Calculating...";
    }
}

const char* WeatherPredictor::getTimeframeString(ForecastTimeframe timeframe) {
    switch (timeframe) {
        case TIME_IMMINENT: return "1-2 HOURS\n[Fast Change/Sure Prob]";
        case TIME_SOON:     return "3-5 HOURS\n[Almost Change/High Prob]";
        case TIME_LATER:    return "6-12 HOURS\n[Slow Change/Low Prob]";
        case TIME_STABLE:   return "Stable Weather\n[No Change]";
        default:            return "Analyzing...";
    }
}

bool WeatherPredictor::checkIceWarning(int16_t temperature, uint16_t humidity) {
    if (temperature <= 200 && humidity >= 8000) {
        return true;
    }
    return false;
}