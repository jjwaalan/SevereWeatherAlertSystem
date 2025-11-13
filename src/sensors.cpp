#include "sensors.h"

bool SensorManager::begin()
{
    return bme.begin(0x76);
}

WeatherData SensorManager::readData()
{
    if (!simulate)
    {
        WeatherData data;
        data.temperature = bme.readTemperature();
        data.humidity = bme.readHumidity();
        data.pressure = bme.readPressure() / 100.0F; // Pa → hPa
        return data;
    }

    // sim mode
    simStep++;
    WeatherData data;

    static float baseTemp = 25.0;
    static float baseHum = 70.0;
    static float basePres = 1013.0;

    // Simulate storm formation over ~5 minutes (300 steps)
    if (simStep < 300)
    {
        data.temperature = baseTemp - 0.02 * simStep; // slow cooling
        data.humidity = baseHum + 0.05 * simStep;     // rising humidity
        data.pressure = basePres - 0.04 * simStep;    // dropping pressure
    }
    else
    {
        data.temperature = 20.0;
        data.humidity = 95.0;
        data.pressure = 995.0;
    }

    return data;
}