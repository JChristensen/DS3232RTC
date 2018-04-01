// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// DS3231/DS3232 Alarm Example Sketch #2
//
// Set Alarm 1 to occur once a minute at 5 seconds after the minute.
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
    tm.Hour = 00;               // set the RTC to an arbitrary time
    tm.Minute = 00;
    tm.Second = 00;
    tm.Day = 16;
    tm.Month = 9;
    tm.Year = 2017 - 1970;      // tmElements_t.Year is the offset from 1970
    RTC.write(tm);              // set the RTC from the tm structure

    // set Alarm 1 to occur at 5 seconds after every minute
    RTC.setAlarm(ALM1_MATCH_SECONDS, 5, 0, 0, 0);
    // clear the alarm flag
    RTC.alarm(ALARM_1);
    // configure the INT/SQW pin for "interrupt" operation (disable square wave output)
    RTC.squareWave(SQWAVE_NONE);
    // enable interrupt output for Alarm 1
    RTC.alarmInterrupt(ALARM_1, true);

    Serial << millis() << " Start ";
    printDateTime(RTC.get());
    Serial << endl;
}

void loop()
{
    // check to see if the INT/SQW pin is low, i.e. an alarm has occurred
    if ( !digitalRead(alarmInput) )
    {
        RTC.alarm(ALARM_1);    // reset the alarm flag
        Serial << millis() << " ALARM_1 ";
        printDateTime(RTC.get());
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
