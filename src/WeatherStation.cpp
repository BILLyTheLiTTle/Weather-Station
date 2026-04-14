#include "Thermistor.h"

Thermistor therm(
    A0,        // analog pin
    9820.0,    // The real value of series resistor (10k)
    10000.0,   // nominal resistance (10k thermistor) from thermistor datasheet
    25.0,      // nominal temp (25°C) from thermistor datasheet
    ThermistorConstants::BETA_3950   // Beta coefficient (β) of the thermistor
);
void printTemperature();

void setup() {
    Serial.begin(9600);
}

void loop() {
    printTemperature();

    delay(10000);
}

void printTemperature() {
    Temperature temp = therm.readTemperatureC();

    if (temp.status == Temperature::OK) {
        Serial.print(temp.value);
        Serial.println("°C");
    } else {
        Serial.print("Error in temperature sensor: ");
        Serial.println(Temperature::getName(temp.status));
    }
}