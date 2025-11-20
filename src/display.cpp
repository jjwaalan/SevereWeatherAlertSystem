#include "display.h"
#include "detectStorm.h"

void DisplayManager::begin()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void DisplayManager::showReadings(const WeatherData &data)
{
    lcd.setCursor(0, 0);
    lcd.print("Temperature:");
    lcd.print(data.temperature, 1);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity:");
    lcd.print(data.humidity, 0);
    lcd.print("%");

    lcd.setCursor(0, 2);
    lcd.print("Pressure:");
    lcd.print(data.pressure, 1);
    lcd.print("hPa");
    lcd.print(" ");
}

void DisplayManager::showAlert(StormStatus status)
{
    lcd.setCursor(0, 3);
    if (status == STORM_DETECTED)
        lcd.print("Storm Alert!   ");
    else if (status == POSSIBLE_STORM)
        lcd.print("Possible Storm ");
    else
        lcd.print("Stable Weather ");
}
