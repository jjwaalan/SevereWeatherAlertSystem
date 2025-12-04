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

    // Simulate storm formation over ~2 minutes (100 steps)
    if (simStep < 100)
    {
        data.temperature = baseTemp - 0.05 * simStep; // slow cooling
        data.humidity = baseHum + 0.25 * simStep;     // rising humidity
        data.pressure = basePres - 0.18 * simStep;    // dropping pressure
    }
    // Peak storm conditions at step 100
    else if (simStep == 100)
    {
        data.temperature = 20.0;
        data.humidity = 95.0;
        data.pressure = 995.0;
    }
    // Recovery phase: return to normal conditions (~100 steps)
    // Humidity decreases, pressure increases back to baseline
    else if (simStep < 200)
    {
        int recoveryStep = simStep - 100;
        data.humidity = 95.0 - 0.25 * recoveryStep;
        data.pressure = 995.0 + 0.18 * recoveryStep;
        data.temperature = 20.0 + 0.05 * recoveryStep;
    }

    else
    {
        data.temperature = 25.0;
        data.humidity = 70.0;
        data.pressure = 1013.0;
        if (simStep >= 250)
            simStep = 0;
    }

    return data;
}