#include <Arduino.h>
#include "sensors.h"
#include "display.h"
#include "detectStorm.h"

#define BUTTON_PIN 0
#define LED_PIN 8


SensorManager sensors;
DisplayManager display;
StormDetector stormDetector;

int pressCount = 0;
unsigned long lastPressTime = 0;

void checkButtonPress()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        Serial.println("Button pressed");
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
    digitalWrite(LED_PIN, HIGH);

    Serial.begin(9600);
    Serial.println("Weather Station Starting...");
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
    StormStatus status = stormDetector.update(data);

    digitalWrite(LED_PIN, status == STORM_DETECTED ? LOW : HIGH);

    display.showReadings(data);
    display.showAlert(status);

    delay(1000);
}