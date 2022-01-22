// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch to demonstrate microcontroller clock drift vs. RTC.
// The MCU's time of day is synchronized with the RTC every five minutes (by default).
// Between syncs, the MCU's time of day is goverened by its system clock (which
// also is used for millis). Because the MCU system clock and the RTC cannot be
// expected to run at identical rates, discontinuities in the MCU's time of day
// will occur when its time is synchronized with the RTC.
//
// We can see the drift and discontinuities by printing both millis and
// the time of day together. The sketch may have to run for some time before
// discrepancies are detected.
// Here is sample output using an Arduino Pro Mini. Note that occasionally
// the MCU time moves forward by one second, but two seconds actual
// time have elapsed according to millis(). YMMV depending on the
// speed of the MCU clock vs. the RTC.
//
// mcu_clock_drift.ino Jan 22 2022 10:44:21
// RTC sync OK
// 0 5 Start 15:44:30 2022-01-22
// 899007 901008 15:59:29 2022-01-22 15:59:30 2022-01-22
// 2099010 2101012 16:19:28 2022-01-22 16:19:29 2022-01-22
// 2999014 3001015 16:34:27 2022-01-22 16:34:28 2022-01-22
//
// Jack Christensen 27Dec2018

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC

DS3232RTC myRTC;

void setup()
{
    Serial.begin(115200);
    Serial.println(F("\n" __FILE__ " " __DATE__ " " __TIME__));
    myRTC.begin();
    setSyncProvider(myRTC.get);     // the function to get the time from the RTC
    Serial.print(F("RTC sync "));
    if (timeStatus() == timeSet)
        Serial.println(F("OK"));
    else
        Serial.println(F("FAIL!"));
}

// when the MCU's time of day changes, if it did not move forward
// by one second, or if millis changed by something other than about
// one second, then print the millis and time values before and after.
void loop()
{
    static uint32_t msLast;
    static time_t tLast;
    uint32_t ms = millis();
    time_t t = now();
    if (t != tLast) {
        if (t - tLast != 1 || ms - msLast > 1010 || ms - msLast < 990) {
            Serial.print(msLast);
            Serial.print(' ');
            Serial.print(ms);
            Serial.print(' ');
            printTime(tLast);
            Serial.print(' ');
            printTime(t);
            Serial.println();
        }
        tLast = t;
        msLast = ms;
    }
}

// format and print a time_t value
void printTime(time_t t)
{
    if (t > 0) {
        char buf[25];
        char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
        strcpy(m, monthShortStr(month(t)));
        sprintf(buf, "%.2d:%.2d:%.2d %.4d-%.2d-%.2d",
            hour(t), minute(t), second(t), year(t), month(t), day(t));
        Serial.print(buf);
    }
    else {
        Serial.print(F("Start"));
    }
}
