// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// DS3231/DS3232 Alarm Example Sketch #5
//
// Set Alarm 2 to occur once per minute.
// Detect the alarm by polling the RTC alarm flag.
//
// Hardware:
// Arduino Uno, DS3231 RTC.
// Connect RTC SDA to Arduino pin A4.
// Connect RTC SCL to Arduino pin A5.
//
// Jack Christensen 16Sep2017

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // https://github.com/janelia-arduino/Streaming

DS3232RTC myRTC;

void setup()
{
    Serial.begin(115200);

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
    tm.Hour = 00;           // set the RTC to an arbitrary time
    tm.Minute = 00;
    tm.Second = 00;
    tm.Day = 1;
    tm.Month = 1;
    tm.Year = 2022 - 1970;  // tmElements_t.Year is the offset from 1970
    myRTC.write(tm);        // set the RTC from the tm structure

    // set Alarm 2 to occur once per minute
    myRTC.setAlarm(DS3232RTC::ALM2_EVERY_MINUTE, 0, 0, 0, 0);
    // clear the alarm flag
    myRTC.alarm(DS3232RTC::ALARM_2);

    Serial << millis() << " Start ";
    printDateTime(myRTC.get());
    Serial << endl;
}

void loop()
{
    // check to see if the alarm flag is set (also resets the flag if set)
    if ( myRTC.alarm(DS3232RTC::ALARM_2) ) {
        Serial << millis() << " ALARM_2 ";
        printDateTime(myRTC.get());
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
