#include <float.h>
#include "Thermistor.h"

Thermistor therm(
    A0,        // analog pin
    9820.0,    // The real value of series resistor (10k)
    10000.0,   // nominal resistance (10k thermistor) from thermistor datasheet
    25.0,      // nominal temp (25°C) from thermistor datasheet
    ThermistorConstants::BETA_3950   // Beta coefficient (β) of the thermistor
);
void updateTemperature(uint32_t interval);

void setup() {
    Serial.begin(9600);
}

void loop() {
    updateTemperature(10000L);

    //delay(10000);
}

void updateTemperature(uint32_t interval) {
    static float maxMeasuredTemp = -FLT_MAX;
    static float minMeasuredTemp = FLT_MAX;
    static float currentTemp = NAN;

    static uint32_t lastReadTime = 0;

    uint32_t now = millis();

    if (lastReadTime != 0 && (now - lastReadTime) < interval) {
        return;
    }
    lastReadTime = now;

    Temperature temp = therm.readTemperatureC();

    if (temp.status == Temperature::OK) {
        float roundedTemp = floorf(temp.value * 2.0f + 0.5f) / 2.0f;

        // Print only if changed
        if (isnan(currentTemp) || currentTemp != roundedTemp) {
            currentTemp = roundedTemp;
            Serial.print("Current temperature: ");
            Serial.print(currentTemp);
            Serial.println("°C");
        }

        // Max update
        if (roundedTemp > maxMeasuredTemp) {
            maxMeasuredTemp = roundedTemp;
            Serial.print("Max temperature: ");
            Serial.print(maxMeasuredTemp);
            Serial.println("°C");
        }

        // Min update (independent!)
        if (roundedTemp < minMeasuredTemp) {
            minMeasuredTemp = roundedTemp;
            Serial.print("Min temperature: ");
            Serial.print(minMeasuredTemp);
            Serial.println("°C");
        }

    } else {
        Serial.print("Error in temperature sensor: ");
        Serial.println(Temperature::getName(temp.status));
    }
}