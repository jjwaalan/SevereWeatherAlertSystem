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
    
    void enableSimulation(bool enable) { simulate = enable; }
    void disableSimulation() { simulate = false; }
    bool isSimulating() const { return simulate; }

private:
    Adafruit_BME280 bme;
    bool simulate = false;
    unsigned long simStep = 0;
};

#endif
