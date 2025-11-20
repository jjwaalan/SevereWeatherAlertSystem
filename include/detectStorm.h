#ifndef DETECT_STORM_H
#define DETECT_STORM_H

#include <Arduino.h>
#include "sensors.h"

//------------------------
// Trend Data Structure
//------------------------
struct TrendData
{
    float pressureDrop1min = 0;
    float pressureDrop5min = 0;
    float pressureDrop15min = 0;
    float tempDrop15min = 0;
    float humidityRise15min = 0;
};
enum StormStatus
{
    NO_STORM,
    POSSIBLE_STORM,
    STORM_DETECTED
};

//------------------------
// Dew Point Function
//------------------------
inline float computeDewPoint(float t, float h)
{
    float a = 17.27, b = 237.7;
    float alpha = ((a * t) / (b + t)) + log(h / 100.0);
    return (b * alpha) / (a - alpha);
}

class StormDetector
{
public:
    StormDetector() {}
    
    void clearHistory() {
        for (int i = 0; i < 60; i++)
            raw60[i] = {0, 0, 0};

        for (int i = 0; i < 15; i++)
            avg15[i] = {0, 0, 0};

        rawIndex = 0;
        avgIndex = 0;
        minutesCollected = 0;
    }

    void fillHistory(const WeatherData &data)
    {
        for (int i = 0; i < 60; i++)
            raw60[i] = data;

        for (int i = 0; i < 15; i++)
            avg15[i] = data;

        rawIndex = 0;
        avgIndex = 0;
        minutesCollected = 15;  // fully primed
    }

    StormStatus update(const WeatherData &data)
    {
        // Store raw sample (1Hz, only 60 sec kept)
        raw60[rawIndex] = data;
        rawIndex++;

        // If we filled 60 seconds, compute 1-minute avg
        if (rawIndex == 60)
        {
            WeatherData avg = compute1MinAverage();
            avg15[avgIndex] = avg;

            if (minutesCollected < 15)
                minutesCollected++;

            avgIndex = (avgIndex + 1) % 15;
            rawIndex = 0; // restart collection
        }

        if (minutesCollected < 15)
            return NO_STORM; // not enough history yet

        TrendData trend = computeTrends(data);

        float dewPoint = computeDewPoint(data.temperature, data.humidity);
        float dewDiff = data.temperature - dewPoint;

        bool stormDetected =
            trend.pressureDrop15min >= 3.0 &&
            trend.tempDrop15min >= 1.0 &&
            trend.humidityRise15min >= 5.0 &&
            dewDiff <= 2.0;

        bool possibleStorm =
            (trend.pressureDrop5min >= 1.5 && data.humidity > 85.0);

        if (stormDetected)
            return STORM_DETECTED;
        else if (possibleStorm)
            return POSSIBLE_STORM;

        return NO_STORM;
    }

private:
    // RAM-optimized structure
    WeatherData raw60[60];   // 60 seconds of raw data
    WeatherData avg15[15];   // 15 × 1-minute averages
    int rawIndex = 0;
    int avgIndex = 0;
    int minutesCollected = 0;

    //-----------------------------------
    // Compute 1-minute average
    //-----------------------------------
    WeatherData compute1MinAverage()
    {
        WeatherData out = {0, 0, 0};
        for (int i = 0; i < 60; i++)
        {
            out.temperature += raw60[i].temperature;
            out.humidity += raw60[i].humidity;
            out.pressure += raw60[i].pressure;
        }
        out.temperature /= 60.0;
        out.humidity /= 60.0;
        out.pressure /= 60.0;
        return out;
    }

    //-----------------------------------
    // Compute trends from averages only
    //-----------------------------------
    TrendData computeTrends(const WeatherData &current)
    {
        TrendData t;

        int idx1 = (avgIndex - 1 + 15) % 15;
        int idx5 = (avgIndex - 5 + 15) % 15;
        int idx15 = (avgIndex - 15 + 15) % 15;

        WeatherData d1 = avg15[idx1];
        WeatherData d5 = avg15[idx5];
        WeatherData d15 = avg15[idx15];

        t.pressureDrop1min = d1.pressure - current.pressure;
        t.pressureDrop5min = d5.pressure - current.pressure;
        t.pressureDrop15min = d15.pressure - current.pressure;

        t.tempDrop15min = d15.temperature - current.temperature;
        t.humidityRise15min = current.humidity - d15.humidity;

        return t;
    }
};

#endif