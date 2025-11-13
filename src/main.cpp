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

int pressCount = 0;
unsigned long lastPressTime = 0;

void checkButtonPress()
{
    if (digitalRead(BUTTON_PIN) == HIGH)
    {
        unsigned long now = millis();
        if (now - lastPressTime < 700)
        { // double/triple press window
            pressCount++;
        }
        else
        {
            pressCount = 1; // reset if too slow
        }
        lastPressTime = now;

        if (pressCount == 3)
        {
            sensors.enableSimulation(!sensors.isSimulating());
            pressCount = 0;
            Serial.println("simulation mode toggled");
        }
        delay(200); // debounce
    }
}

void setup()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Serial.begin(9600);

    if (!sensors.begin())
    {
        Serial.println("BME280 init failed!");
        Serial.println("Continuing in simulation mode");
        sensors.enableSimulation(true);
    }

    display.begin();
}

void loop()
{
    checkButtonPress();

    WeatherData data = sensors.readData();
    bool stormDetected = stormDetector.update(data);

    digitalWrite(LED_PIN, stormDetected);
    digitalWrite(BUZZER_PIN, stormDetected);

    display.showReadings(data);
    display.showAlert(stormDetected);

    delay(1000);
}