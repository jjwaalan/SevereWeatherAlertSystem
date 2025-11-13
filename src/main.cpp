#include <Arduino.h>
#include "sensors.h"
#include "display.h"
#include "detectStorm.h"

#define BUTTON_PIN 7
#define LED_PIN 8
#define BUZZER_PIN 9

SensorManager sensors;
DisplayManager display;
StormDetector stormDetector;

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.begin(9600);

    if (!sensors.begin())
    {
        Serial.println("BME280 init failed!");
        while (true)
            ;
    }

    display.begin();
}

void loop()
{
    WeatherData data = sensors.readData();

    bool stormDetected = stormDetector.update(data);

    digitalWrite(LED_PIN, stormDetected);
    digitalWrite(BUZZER_PIN, stormDetected);

    display.showReadings(data);
    display.showAlert(stormDetected);

    delay(1000); // 1 Hz sampling
}
