#include "sensors.h"
#include <Wire.h>

bool SensorManager::begin() {
    return bme.begin(0x76); // BME280 I2C address
}

WeatherData SensorManager::readData() {
    WeatherData data;
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.pressure = bme.readPressure() / 100.0F;
    return data;
}
