#ifndef DETECT_STORM_H
#define DETECT_STORM_H

#include <Arduino.h>
#include "sensors.h"

struct TrendData
{
    float pressureDrop1min = 0;
    float pressureDrop5min = 0;
    float pressureDrop15min = 0;
    float tempDrop15min = 0;
    float humidityRise15min = 0;
};

// Dew point approximation (Magnus-Tetens formula)
inline float computeDewPoint(float temperature, float humidity)
{
    float a = 17.27;
    float b = 237.7;
    float alpha = ((a * temperature) / (b + temperature)) + log(humidity / 100.0);
    return (b * alpha) / (a - alpha);
}

class StormDetector
{
public:
    StormDetector() : bufferIndex(0), bufferCount(0) {}

    bool update(const WeatherData &data)
    {
        // Store latest data
        history[bufferIndex] = data;
        bufferIndex = (bufferIndex + 1) % MAX_HISTORY;
        if (bufferCount < MAX_HISTORY)
            bufferCount++;

        // Wait until we have at least 15 min of data
        if (bufferCount < MAX_HISTORY)
            return false;

        TrendData trend = computeTrends(data);
        float dewPoint = computeDewPoint(data.temperature, data.humidity);
        float dewDiff = data.temperature - dewPoint;

        // Detection logic
        bool stormDetected =
            (trend.pressureDrop15min >= 3.0) && // ≥3 hPa drop in 15min
            (trend.tempDrop15min >= 1.0) &&     // ≥1°C temp drop
            (trend.humidityRise15min >= 5.0) && // ≥5% humidity rise
            (dewDiff <= 2.0);                   // near saturation

        bool possibleStorm =
            (trend.pressureDrop5min >= 1.5 && data.humidity > 85.0);

        if (stormDetected)
        {
            Serial.println("[ALERT] Severe storm likely!");
        }
        else if (possibleStorm)
        {
            Serial.println("[NOTICE] Possible storm forming...");
        }

        return stormDetected;
    }

private:
    static const int MAX_HISTORY = 15 * 60; // 15 minutes @ 1Hz
    WeatherData history[MAX_HISTORY];
    int bufferIndex;
    int bufferCount;

    int indexOffset(int secondsAgo) const
    {
        int idx = (bufferIndex - secondsAgo - 1 + MAX_HISTORY) % MAX_HISTORY;
        return (idx < 0) ? 0 : idx;
    }

    TrendData computeTrends(const WeatherData &current)
    {
        TrendData t;
        if (bufferCount < 900)
            return t;

        WeatherData d1 = history[indexOffset(60)];
        WeatherData d5 = history[indexOffset(5 * 60)];
        WeatherData d15 = history[indexOffset(15 * 60 - 1)];

        t.pressureDrop1min = d1.pressure - current.pressure;
        t.pressureDrop5min = d5.pressure - current.pressure;
        t.pressureDrop15min = d15.pressure - current.pressure;

        t.tempDrop15min = d15.temperature - current.temperature;
        t.humidityRise15min = current.humidity - d15.humidity;

        return t;
    }
};

#endif