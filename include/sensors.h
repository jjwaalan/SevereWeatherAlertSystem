#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_BME280.h>

struct WeatherData
{
    float temperature;
    float humidity;
    float pressure;
};

class SensorManager
{
public:
    bool begin();
    WeatherData readData();

private:
    Adafruit_BME280 bme;
};

#endif
