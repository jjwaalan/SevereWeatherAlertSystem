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
unsigned long lastWeatherUpdate = 0;
const unsigned long WEATHER_INTERVAL = 1000;
bool simulationMode = false;
void checkButtonPress()
{
    static bool lastState = HIGH;
    bool currentState = digitalRead(BUTTON_PIN);
    if (lastState == HIGH && currentState == LOW)
    {
        unsigned long now = millis();
        Serial.println("Button pressed");

        if (now - lastPressTime < 700)
        {
            pressCount++;
        }
        else
        {
            pressCount = 1;
        }

        lastPressTime = now;

        if (pressCount == 3)
        {
            if (simulationMode == false){
                Serial.println("Enabling simulation mode");
                stormDetector.clearHistory();
                WeatherData simulationBase = {25.0, 70.0, 1013.0};
                stormDetector.fillHistory(simulationBase);
                sensors.enableSimulation(!sensors.isSimulating());
            }
            else {
                Serial.println("Disabling simulation mode");
                stormDetector.clearHistory();
                stormDetector.fillHistory(sensors.readData());
                sensors.disableSimulation();
            }

            pressCount = 0;
            Serial.println("simulation mode toggled");
        }
    }

    lastState = currentState;
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

    // one sec update interval
    unsigned long now = millis();
    if (now - lastWeatherUpdate >= WEATHER_INTERVAL)
    {
        lastWeatherUpdate = now;
        WeatherData data = sensors.readData();
        StormStatus status = stormDetector.update(data);
        if (status == STORM_DETECTED)
            Serial.println("Storm Detected!");
        else if (status == POSSIBLE_STORM)
            Serial.println("Possible Storm Approaching");
        else
            Serial.println("No Storm");
        digitalWrite(LED_PIN, (status == STORM_DETECTED ? HIGH : LOW));

        display.showReadings(data);
        display.showAlert(status);
    }
    delay(10);
}