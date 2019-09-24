// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2019 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Simple example sketch that should work on any architecture.
// For the AVR architecture, instantiating the DS3232RTC object
// is redundant, but this is only for testing purposes.
//
// Jack Christensen 24Sep2019

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
DS3232RTC myRTC;

void setup()
{
    Serial.begin(115200);
    myRTC.begin();
}

void loop()
{
    display();
    delay(10000);
}

// display time, date, temperature
void display()
{
    char buf[40];
    time_t t = myRTC.get();
    float celsius = myRTC.temperature() / 4.0;
    float fahrenheit = celsius * 9.0 / 5.0 + 32.0;
    sprintf(buf, "%.2d:%.2d:%.2d %.2d%s%d ",
        hour(t), minute(t), second(t), day(t), monthShortStr(month(t)), year(t));
    Serial.print(buf);
    Serial.print(celsius);
    Serial.print("C ");
    Serial.print(fahrenheit);
    Serial.println("F");
}
