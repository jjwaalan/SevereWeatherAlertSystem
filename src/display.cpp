#include "display.h"

void DisplayManager::begin() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void DisplayManager::showReadings(const WeatherData& data) {
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(data.temperature, 1);
    lcd.print("C H:");
    lcd.print(data.humidity, 0);
    lcd.print("%  ");

    lcd.setCursor(0, 1);
    lcd.print("P:");
    lcd.print(data.pressure, 1);
    lcd.print("hPa   ");
}

void DisplayManager::showAlert(bool stormDetected) {
    lcd.setCursor(0, 1);
    if (stormDetected)
        lcd.print("Storm Alert!   ");
    else
        lcd.print("Stable Weather ");
}
