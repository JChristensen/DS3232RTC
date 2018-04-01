// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch: Digital clock display using a DS3231/32 Real-Time
// Clock and an ATtiny45/85 with a 1MHz system clock.
// Also seems to work with a DS1307 which is fairly similar but the
// DS3232RTC library doesn't officially support it.
//
// Run TinyISP on an ATmega microcontroller that does not have an LED
// connected to pin 13 (SCK). The LED causes problems because the SPI
// pins are also the I2C pins on the ATtiny. Connect MISO, MOSI, SCK
// on the ATmega to the corresponding pins on the ATtiny through 220Ω
// resistors for safety. Use 4.7K pullup resistors on the ATtiny
// I2C bus.
//
// Jack Christensen 21Aug2013

#include <DS3232RTC.h>              // https://github.com/JChristensen/DS3232RTC
#include <TinyDebugKnockBang.h>     // https://github.com/Coding-Badly/TinyDebugKnockBang

void setup()
{
    Debug.begin(250000);

    // setSyncProvider() causes the Time library to synchronize with the
    // external RTC by calling RTC.get() every five minutes by default.
    setSyncProvider(RTC.get);
    Debug.print(F("RTC Sync"));
    if (timeStatus() != timeSet) Debug.print(F(" FAIL!"));
    Debug.println();
}

void loop()
{
    static time_t tLast;

    time_t t = now();
    if (t != tLast) {
        tLast = t;
        printDateTime(t);
        Debug.println();
    }
}

// print date and time to Serial
void printDateTime(time_t t)
{
    printDate(t);
    Debug.print(' ');
    printTime(t);
}

// print time to Serial
void printTime(time_t t)
{
    printI00(hour(t), ':');
    printI00(minute(t), ':');
    printI00(second(t), ' ');
}

// print date to Serial
void printDate(time_t t)
{
    printI00(day(t), 0);
    Debug.print(monthShortStr(month(t)));
    Debug.print(year(t), DEC);
}

// Print an integer in "00" format (with leading zero),
// followed by a delimiter character to Serial.
// Input value assumed to be between 0 and 99.
void printI00(int val, char delim)
{
    if (val < 10) Debug.print('0');
    Debug.print(val, DEC);;
    if (delim > 0) Debug.print(delim);
    return;
}
