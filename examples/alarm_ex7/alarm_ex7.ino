// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// DS3231/DS3232 Alarm Example Sketch #7
//
// Set Alarm 2 to occur at a given time hh:mm.
// Configure the RTC INT/SQW pin to be asserted when alarm match occurs.
// Detect the alarm by polling the INT/SQW pin.
//
// Hardware:
// Arduino Uno, DS3231 RTC.
// Connect RTC SDA to Arduino pin A4.
// Connect RTC SCL to Arduino pin A5.
// Connect RTC INT/SQW to Arduino pin 2.
//
// Jack Christensen 16Sep2017

#include <DS3232RTC.h>        // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        // http://arduiniana.org/libraries/streaming/

// pin definitions
const uint8_t alarmInput(2);

void setup()
{
    Serial.begin(115200);
    pinMode(2, INPUT_PULLUP);

    // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);

    tmElements_t tm;
    tm.Hour = 06;               // set the RTC time to 06:29:50
    tm.Minute = 29;
    tm.Second = 50;
    tm.Day = 16;
    tm.Month = 9;
    tm.Year = 2017 - 1970;      // tmElements_t.Year is the offset from 1970
    RTC.write(tm);              // set the RTC from the tm structure

    // set Alarm 2 for 06:30:00
    RTC.setAlarm(ALM2_MATCH_HOURS, 0, 30, 6, 0);
    // clear the alarm flags
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
    RTC.squareWave(SQWAVE_NONE);
    // enable interrupt output for Alarm 2 only
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, true);
}

void loop()
{
    // check to see if the INT/SQW pin is low, i.e. an alarm has occurred
    if ( !digitalRead(alarmInput) )
    {
        RTC.alarm(ALARM_2);    // reset the alarm flag
        Serial << "ALARM_2\n";
    }

    // print the time when it changes
    static time_t tLast;
    time_t t = RTC.get();
    if (t != tLast)
    {
        tLast = t;
        printDateTime(t);
        Serial << endl;
    }
}

void printDateTime(time_t t)
{
    Serial << ((day(t)<10) ? "0" : "") << _DEC(day(t));
    Serial << monthShortStr(month(t)) << _DEC(year(t)) << ' ';
    Serial << ((hour(t)<10) ? "0" : "") << _DEC(hour(t)) << ':';
    Serial << ((minute(t)<10) ? "0" : "") << _DEC(minute(t)) << ':';
    Serial << ((second(t)<10) ? "0" : "") << _DEC(second(t));
}
