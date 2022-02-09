// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2019 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch that initiates an RTC temperature conversion every
// 10 seconds and prints temperature, time and date if the temperature has
// changed since the last conversion. If the temperature does not change,
// it is still printed once per minute.
//
// The default temperature conversion rate for the DS3231/2 is every 64 seconds
// so this allows quicker observation of temperature changes. Note that the
// automatic conversions still occur every 64 seconds in addition to the
// forced conversions.
//
// This sketch uses a 1Hz interrupt from the RTC to keep time instead of
// using the "setSyncProvider" function from the Time library. See the
// rtc_interrupt example sketch for more information.
//
// Jack Christensen 09Sep2019

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // https://github.com/janelia-arduino/Streaming

constexpr uint8_t RTC_1HZ_PIN {3};  // RTC provides a 1Hz interrupt signal on this pin
                                    // Can use Pin 2 (INT0) or Pin 3 (INT1) with Arduino Uno
DS3232RTC myRTC;

void setup()
{
    Serial.begin(115200);
    Serial << F("\n" __FILE__ " " __DATE__ " " __TIME__ "\n");

    pinMode(RTC_1HZ_PIN, INPUT_PULLUP);     // enable pullup on interrupt pin (RTC SQW pin is open drain)
    attachInterrupt(digitalPinToInterrupt(RTC_1HZ_PIN), incrementTime, FALLING);
    myRTC.begin();
    myRTC.squareWave(DS3232RTC::SQWAVE_1_HZ);   // 1 Hz square wave

    time_t utc = getUTC();                  // synchronize with RTC
    while (utc == getUTC());                // wait for increment to the next second
    utc = myRTC.get();                      // get the time from the RTC
    setUTC(utc);                            // set our time to the RTC's time
    Serial << F("Time set from RTC\n");
}

void loop()
{
    static time_t tLast;
    static int16_t lastTemp;
    time_t t = getUTC();

    if (t != tLast) {
        tLast = t;
        if (second(t) % 10 == 0) convertTemperature();
        int16_t rtcTemp = myRTC.temperature();
        if (rtcTemp != lastTemp || second(t) == 0) {
            lastTemp = rtcTemp;
            float c = rtcTemp / 4.;
            float f = c * 9. / 5. + 32.;
            Serial << c << F(" C ") << f << F(" F  ");
            printTime(t);
        }
    }
}

// force the RTC to do a temperature conversion
void convertTemperature()
{
    // get the status register
    uint8_t s = myRTC.readRTC(DS3232RTC::DS32_STATUS);
    // start a conversion, unless one is already in progress
    if (!(s & _BV(DS3232RTC::DS32_BSY))) {
        // get the control register and set the CONV bit
        uint8_t c = myRTC.readRTC(DS3232RTC::DS32_CONTROL);
        myRTC.writeRTC(DS3232RTC::DS32_CONTROL, c | _BV(DS3232RTC::DS32_CONV));
        // wait for the CONV bit to turn off
        bool busy = true;
        while (busy) {
            Serial << F("Wait CONV\n");
            delay(200);
            busy = myRTC.readRTC(DS3232RTC::DS32_CONTROL) & _BV(DS3232RTC::DS32_CONV);
        }
    }
    else {
        bool busy = true;
        while (busy) {
            Serial << F("Wait BSY\n");
            delay(200);
            busy = myRTC.readRTC(DS3232RTC::DS32_STATUS) & _BV(DS3232RTC::DS32_BSY);
        }
    }
}

volatile time_t isrUTC;         // ISR's copy of current time in UTC

// return current time
time_t getUTC()
{
    noInterrupts();
    time_t utc = isrUTC;
    interrupts();
    return utc;
}

// set the current time
void setUTC(time_t utc)
{
    noInterrupts();
    isrUTC = utc;
    interrupts();
}

// 1Hz RTC interrupt handler increments the current time
void incrementTime()
{
    ++isrUTC;
}

// format and print a time_t value
void printTime(time_t t)
{
    char buf[25];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t));
    Serial.println(buf);
}
