#include <Arduino.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);          // Start serial communication
  Serial.println("Started");   // Initial log
}

void loop() {
  Serial.println("LED ON");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000); 

  Serial.println("LED OFF");
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}