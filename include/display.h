#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include "sensors.h"

class DisplayManager
{
public:
    void begin();
    void showReadings(const WeatherData &data);
    void showAlert(bool stormDetected);

private:
    LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
};

#endif
