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
// Connect RTC INT/SQW to Arduino pin 3.
//
// Jack Christensen 16Sep2017

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // https://github.com/janelia-arduino/Streaming

constexpr uint8_t RTC_INT_PIN {3};  // RTC provides an alarm signal on this pin
DS3232RTC myRTC;

void setup()
{
    Serial.begin(115200);
    pinMode(RTC_INT_PIN, INPUT_PULLUP);

    // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    myRTC.begin();
    myRTC.setAlarm(DS3232RTC::ALM1_MATCH_DATE, 0, 0, 0, 1);
    myRTC.setAlarm(DS3232RTC::ALM2_MATCH_DATE, 0, 0, 0, 1);
    myRTC.alarm(DS3232RTC::ALARM_1);
    myRTC.alarm(DS3232RTC::ALARM_2);
    myRTC.alarmInterrupt(DS3232RTC::ALARM_1, false);
    myRTC.alarmInterrupt(DS3232RTC::ALARM_2, false);
    myRTC.squareWave(DS3232RTC::SQWAVE_NONE);

    tmElements_t tm;
    tm.Hour = 06;               // set the RTC time to 06:29:50
    tm.Minute = 29;
    tm.Second = 50;
    tm.Day = 1;
    tm.Month = 1;
    tm.Year = 2022 - 1970;      // tmElements_t.Year is the offset from 1970
    myRTC.write(tm);            // set the RTC from the tm structure

    // set Alarm 2 for 06:30:00
    myRTC.setAlarm(DS3232RTC::ALM2_MATCH_HOURS, 0, 30, 6, 0);
    // clear the alarm flags
    myRTC.alarm(DS3232RTC::ALARM_1);
    myRTC.alarm(DS3232RTC::ALARM_2);
    // configure the INT/SQW pin for "interrupt" operation (i.e. disable square wave output)
    myRTC.squareWave(DS3232RTC::SQWAVE_NONE);
    // enable interrupt output for Alarm 2 only
    myRTC.alarmInterrupt(DS3232RTC::ALARM_1, false);
    myRTC.alarmInterrupt(DS3232RTC::ALARM_2, true);
}

void loop()
{
    // check to see if the INT/SQW pin is low, i.e. an alarm has occurred
    if ( !digitalRead(RTC_INT_PIN) ) {
        myRTC.alarm(DS3232RTC::ALARM_2);    // reset the alarm flag
        Serial << "ALARM_2\n";
    }

    // print the time when it changes
    static time_t tLast;
    time_t t = myRTC.get();
    if (t != tLast) {
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
