#include <Arduino.h>
#include "sensors.h"
#include "display.h"

#define LED_PIN 8
#define BUZZER_PIN 9

SensorManager sensors;
DisplayManager display;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.begin(9600);

    if (!sensors.begin()) {
        Serial.println("BME280 init failed!");
        while (true);
    }

    display.begin();
}

void loop() {
    WeatherData data = sensors.readData();

    bool stormDetected = (data.pressure < 1000 && data.humidity > 80);

    digitalWrite(LED_PIN, stormDetected);
    digitalWrite(BUZZER_PIN, stormDetected);

    display.showReadings(data);
    display.showAlert(stormDetected);

    delay(1000);
}
