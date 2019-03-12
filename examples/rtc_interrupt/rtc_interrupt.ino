// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch to use a 1Hz interrupt from the DS3231 to keep time.
// This is an alternative to using the "setSyncProvider" function from
// the Time library to periodically sync the MCU's time with the RTC.
//
// Periodic synchronization will cause discontinuities in the MCU's perceived
// time of day. Depending on the relative frequency of the MCU's clock compared to
// the RTC, when synchronization occurs, a second according to the MCU's time
// of day may appear to be longer or shorter than one second. Alternately, the
// MCU's time of day could jump backwards or forward by more than a second.
//
// While this method requires setting up an interrupt and a few short
// functions to handle the interrupt and to set and read the time,
// it has the following advantages: (1) The MCU time is always exactly
// in step with the RTC time and is not dependent on the MCU's clock
// which will always drift relative to the RTC, and (2) It is more efficient
// as the RTC's time is only read once over the I2C bus and the interrupt
// service routine to increment the time is minimal.
//
// When using the technique of periodically synchronizing the MCU's time
// with the RTC, we have 
//
// Jack Christensen 27Dec2018

#include <DS3232RTC.h>          // https://github.com/JChristensen/DS3232RTC

const uint8_t RTC_1HZ_PIN(2);   // RTC provides a 1Hz interrupt signal on this pin

void setup()
{
    Serial.begin(115200);
    Serial.println(F("\n" __FILE__ " " __DATE__ " " __TIME__));

    pinMode(RTC_1HZ_PIN, INPUT_PULLUP);     // enable pullup on interrupt pin (RTC SQW pin is open drain)
    attachInterrupt(digitalPinToInterrupt(RTC_1HZ_PIN), incrementTime, FALLING);
    RTC.squareWave(SQWAVE_1_HZ);            // 1 Hz square wave

    time_t utc = getUTC();                  // synchronize with RTC
    while ( utc == getUTC() );              // wait for increment to the next second
    utc = RTC.get();                        // get the time from the RTC
    setUTC(utc);                            // set our time to the RTC's time
    Serial.println("Time set from RTC");
}

void loop()
{
    static time_t tLast;
    time_t t = getUTC();

    if (t != tLast)
    {
        tLast = t;
        printTime(t);
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
