#include "WeatherPredictor.h"
#include "Debugger.h"

WeatherPredictor::WeatherPredictor() : _historyCount(0), _lastUpdateTime(0), _currentTrendVal(0), _currentHumTrendVal(0) {
    for(int i = 0; i < 12; i++) {
        _history[i] = 0;
        _humidityHistory[i] = 0; // Αρχικοποίηση ιστορικού υγρασίας 12 θέσεων
    }
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

int8_t WeatherPredictor::calculateHumidityTrend(uint16_t currentHum, uint16_t oldHum) {
    // Όταν γεμίσει ο πίνακας, η τάση υπολογίζεται μεταξύ του Τώρα και πριν 2.8 ώρες
    if (_historyCount == 12) {
        _currentHumTrendVal = (int16_t)_humidityHistory[11] - (int16_t)_humidityHistory[0];
    } else {
        _currentHumTrendVal = (int16_t)currentHum - (int16_t)oldHum;
    }

    // Αν η υγρασία άλλαξε πάνω από 1.00% (δηλαδή 100 μονάδες στην κλίμακα x100)
    if (_currentHumTrendVal >= 100)  return 1;  // Rising (Αυξάνεται)
    if (_currentHumTrendVal <= -100) return -1; // Falling (Μειώνεται)
    return 0;                                   // Steady (Σταθερή)
}

WeatherForecast WeatherPredictor::addReading(uint32_t currentPressurePascal, uint32_t currentTime, bool isWinter, int16_t temperature, uint32_t humidity) {
    
    if (_historyCount == 0 || (currentTime - _lastUpdateTime >= _updateInterval)) {
        DBG(F("CURRENT TIME: ")); DBG_LN(currentTime);
        DBG(F("LAST TIME: "));    DBG_LN(_lastUpdateTime);
        DBG(F("WINTER: "));       DBG_LN(isWinter);
        
        // Ολίσθηση (Shift) για τον νέο πίνακα 12 θέσεων (0 έως 11) για πίεση και υγρασία
        for (uint8_t i = 0; i < 11; i++) {
            _history[i] = _history[i+1];
            _humidityHistory[i] = _humidityHistory[i+1]; // Shift υγρασίας
        }
        _history[11] = currentPressurePascal; // Το "τώρα" της πίεσης στην τελευταία θέση
        _humidityHistory[11] = humidity;      // Το "τώρα" της υγρασίας στην τελευταία θέση
        
        if (_historyCount < 12) _historyCount++;
        _lastUpdateTime = currentTime;
    }

    // Χρειαζόμαστε τουλάχιστον 3 μετρήσεις (42 λεπτά) για μια πρώτη πρόχειρη πρόβλεψη
    if (_historyCount < 3) return FORECAST_UNKNOWN;

    // Παίρνουμε την παλαιότερη διαθέσιμη μέτρηση ανάλογα με το πόσες έχουμε μαζέψει
    uint32_t oldPressure = _history[12 - _historyCount];
    int8_t trend = calculateTrend(currentPressurePascal, oldPressure);
    
    // Υπολογισμός τάσης της υγρασίας
    uint16_t oldHumidity = _humidityHistory[12 - _historyCount];
    calculateHumidityTrend(humidity, oldHumidity);
    
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

    WeatherForecast finalForecast;
    if (zambrettiZ <= 2)       finalForecast = FORECAST_GOOD;
    else if (zambrettiZ <= 4)  finalForecast = FORECAST_SETTLED;
    else if (zambrettiZ <= 10) finalForecast = FORECAST_FINE;
    else if (zambrettiZ <= 12) finalForecast = FORECAST_BECOMING_FINE;
    else if (zambrettiZ <= 19) finalForecast = FORECAST_CHANGEABLE;
    else if (zambrettiZ <= 22) finalForecast = FORECAST_UNSETTLED;
    else if (zambrettiZ <= 24) finalForecast = FORECAST_RAIN;
    else                       finalForecast = FORECAST_STORMY;

    if (finalForecast == FORECAST_RAIN || finalForecast == FORECAST_STORMY) {
        if (temperature <= 150 && humidity >= 7500) {
            finalForecast = FORECAST_SNOW;
        }
    }

    return finalForecast;
}

ForecastTimeframe WeatherPredictor::getTimeframe(uint16_t humidity) {
    if (_historyCount < 12) return TIME_UNKNOWN;

    int32_t pressTrend = _currentTrendVal;  // Τάση πίεσης (hPa)
    int16_t humTrend = _currentHumTrendVal; // Τάση υγρασίας (x100)
    uint32_t currentHpa = _history[11] / 100;

    // =================================================================
    // ΣΥΝΔΥΑΣΜΟΣ 1: ΠΙΕΣΗ ΠΕΦΤΕΙ + ΥΓΡΑΣΙΑ ΑΝΕΒΑΙΝΕΙ -> ΕΡΧΕΤΑΙ ΚΑΚΟΚΑΙΡΙΑ
    // =================================================================
    if (pressTrend <= -1) { 
        if (humTrend >= 100) {
            return TIME_IMMINENT; // 1-2 ώρες (Κλειδώνει η βροχή άμεσα)
        } else {
            return TIME_LATER;    // 6-12 ώρες (Ξερή πτώση, θα αργήσει)
        }
    }

    // =================================================================
    // ΣΥΝΔΥΑΣΜΟΣ 2: ΠΙΕΣΗ ΑΝΕΒΑΙΝΕΙ + ΥΓΡΑΣΙΑ ΠΕΦΤΕΙ -> ΕΡΧΕΤΑΙ ΒΕΛΤΙΩΣΗ
    // =================================================================
    if (pressTrend >= 1) {
        if (humTrend <= -100) {
            return TIME_IMMINENT; // 1-2 ώρες (Καθαρίζει γρήγορα ο τόπος)
        } else {
            return TIME_LATER;    // 6-12 ώρες (Ανεβαίνει η πίεση αλλά έχει μουντάδα)
        }
    }

    // =================================================================
    // ΣΥΝΔΥΑΣΜΟΣ 3: Η ΠΙΕΣΗ ΕΙΝΑΙ ΚΟΛΛΗΜΕΝΗ ΧΑΜΗΛΑ
    // =================================================================
    if (currentHpa < 1005) {
        if (humTrend >= 100 || humidity >= 6500) {
            return TIME_SOON;     // 3-5 ώρες (Το χαμηλό είναι εδώ, η υγρασία πιέζει)
        }
    }

    return TIME_STABLE; // Σταθερός καιρός
}

WindForecast WeatherPredictor::getWindPrediction() {
    if (_historyCount < 12) {
        return WIND_UNKNOWN;
    }

    uint32_t currentPresPascal = _history[11];
    uint32_t oldPresPascal = _history[0];
    
    uint32_t pressureChangePascal = (currentPresPascal > oldPresPascal) ? 
                                    (currentPresPascal - oldPresPascal) : 
                                    (oldPresPascal - currentPresPascal);

    if (pressureChangePascal >= 500)      return GALE_STORMY_WIND;
    else if (pressureChangePascal >= 300) return STRONG_WINDS;
    else if (pressureChangePascal >= 150) return MODERATE_BREEZES;
    else                                  return CALM_LIGHT_WIND;     
}

const char* WeatherPredictor::getForecastString(WeatherForecast forecast) {
    switch (forecast) {
        case FORECAST_GOOD:          return "Good \n(Fair/Mild Sky)";       // ΚΑΛΟΣ ΚΑΙΡΟΣ: Μια χαρά μέρα, ήπιος ουρανός, φυσιολογική πίεση
        case FORECAST_SETTLED:       return "Settled\n(Stable/Durable)";    // ΣΤΑΘΕΡΟΣ ΚΑΙΡΟΣ: Κλειδωμένη καλοκαιρία μεγάλης διάρκειας, ακλόνητος
        case FORECAST_FINE:          return "Fine/Clear\n(Totally Clear)";  // Ο ΠΙΟ ΚΑΘΑΡΟΣ ΚΑΙΡΟΣ: Απόλυτη, κρυστάλλινη ηλιοφάνεια & υψηλή πίεση
        case FORECAST_BECOMING_FINE: return "Becoming Fine\n(Improving)";   // Βελτίωση: Ο καιρός καθαρίζει και πηγαίνει προς το Fine
        case FORECAST_CHANGEABLE:    return "Changeable\n(Partly Cloudy)";  // Μεταβατικός: Άστατος καιρός, μισός ήλιος / μισή συννεφιά
        case FORECAST_UNSETTLED:     return "Unsettled\n(Variable/Spells)"; // Άστατος: Κακός καιρός, αλλά με διαστήματα βελτίωσης
        case FORECAST_RAIN:          return "Rainy\n(Wet/Overcast)";        // Βροχή: Κλειστός, γκρίζος και βρεγμένος καιρός
        case FORECAST_STORMY:        return "Stormy/Gale\n(Severe Storm)";  // Καταιγίδα: Έντονα φαινόμενα, θύελλα
        case FORECAST_SNOW:          return "Snow\n(Heavy/White)";          // ΧΙΟΝΙ: Υετός με θερμοκρασία <= 1.5°C
        default:                     return "Calculating...";
    }
}

const char* WeatherPredictor::getWindString(WindForecast forecast) {
    switch (forecast) {
        case GALE_STORMY_WIND:  return "Stormy (8+)";       // ΘΥΕΛΛΩΔΗΣ ΑΝΕΜΟΣ: Πολύ επικίνδυνος άνεμος, καταιγίδα/μπουρίνι (8+ Bft)
        case STRONG_WINDS:      return "Strong (6-7)";      // ΙΣΧΥΡΟΣ ΑΝΕΜΟΣ: Δυνατό αγιάζι, χρειάζεται προσοχή (6-7 Bft)
        case MODERATE_BREEZES:  return "Moderate (4-5)";    // ΜΕΤΡΙΟΣ ΑΝΕΜΟΣ: Σχετικά αισθητό αεράκι, φυσιολογικές συνθήκες (4-5 Bft)
        case CALM_LIGHT_WIND:   return "Calm (0-3)";        // Ο ΠΙΟ ΗΣΥΧΟΣ ΑΝΕΜΟΣ: Άπνοια ή ελαφρύ αεράκι, ιδανικές συνθήκες (0-3 Bft)
        default:                return "Calculating...";
    }
}

const char* WeatherPredictor::getTimeframeString(ForecastTimeframe timeframe) {
    switch (timeframe) {
        case TIME_IMMINENT: return "In 1 to 2 HOURS!\n(Prob: IMMINENT)";    // Η ΠΙΟ ΚΟΝΤΙΝΗ/ΣΙΓΟΥΡΗ ΑΛΛΑΓΗ: Ραγδαία πτώση, έρχεται άμεσα
        case TIME_SOON:     return "In 3 to 5 HOURS\n(Prob: HIGH)";         // ΥΨΗΛΗ ΕΤΟΙΜΟΤΗΤΑ: Η υγρασία ανέβηκε, ο καιρός κλειδώνει για βροχή
        case TIME_LATER:    return "In 6 to 12 HOURS\n(Prob: LOW)";         // ΜΑΚΡΙΝΗ ΕΚΤΙΜΗΣΗ: Μικρή πιθανότητα για τώρα, ο καιρός θέλει χρόνο ακόμα
        case TIME_STABLE:   return "Stable weather\n(No change)";           // Σταθερότητα: Καμία αλλαγή στον ορίζοντα
        default:            return "Analyzing trend...";                    // Μέχρι να γεμίσει το 3ωρο
    }
}

bool WeatherPredictor::checkIceWarning(int16_t temperature, uint16_t humidity) {
    if (temperature <= 200 && humidity >= 8000) {
        return true;
    }
    return false;
}