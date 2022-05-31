// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2022 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch to create an interrupt with Alarm 1
// at regular intervals.
// Assumes the RTC is already set and running.
//
// Notes:
// Using the INT/SQW pin for alarms is mutually exclusive with using
// it to output a square wave. However, alarms can still be set when
// a square wave is output, but then the alarm() function will need
// to be used to determine if an alarm has triggered. Even though
// the DS3231 power-on default for the INT/SQW pin is as an alarm
// output, it's good practice to call DS3232RTC::squareWave(SQWAVE_NONE)
// before setting alarms.
//
// I recommend calling DS3232RTC::alarm() before DS3232RTC::alarmInterrupt()
// to ensure the RTC's alarm flag is cleared.
//
// Tested with Arduino 1.8.19, Arduino Uno, DS3231.
// Connect RTC SDA to Arduino pin A4.
// Connect RTC SCL to Arduino pin A5.
// Connect RTC INT/SQW to Arduino pin 3.
//
// Jack Christensen 01Apr2022

#include <DS3232RTC.h>              // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>              // https://github.com/janelia-arduino/Streaming

constexpr uint8_t sqwPin{3};        // RTC provides an interrupt signal on this pin
                                    // (Can use Pin 2 (INT0) or Pin 3 (INT1) with Arduino Uno)
constexpr time_t alarmInterval{60}; // alarm interval in seconds
DS3232RTC myRTC;

void setup()
{
    Serial.begin(115200);
    Serial << F( "\n" __FILE__ "\n" __DATE__ " " __TIME__ "\n\n" );
    
    // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    myRTC.begin();
    myRTC.setAlarm(DS3232RTC::ALM1_MATCH_DATE, 0, 0, 0, 1);
    myRTC.setAlarm(DS3232RTC::ALM2_MATCH_DATE, 0, 0, 0, 1);
    myRTC.alarm(DS3232RTC::ALARM_1);
    myRTC.alarm(DS3232RTC::ALARM_2);
    myRTC.alarmInterrupt(DS3232RTC::ALARM_1, false);
    myRTC.alarmInterrupt(DS3232RTC::ALARM_2, false);
    myRTC.squareWave(DS3232RTC::SQWAVE_NONE);

    // configure an interrupt on the falling edge from the SQW pin
    pinMode(sqwPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(sqwPin), alarmIsr, FALLING);

    Serial << "Alarm interval is " << alarmInterval << " seconds\n";
    time_t t = myRTC.get();
    time_t a = t + alarmInterval - t % alarmInterval;
    if (a <= t) a += alarmInterval;
    // set the alarm
    myRTC.setAlarm(DS3232RTC::ALM1_MATCH_HOURS, second(a), minute(a), hour(a), 0);
    myRTC.alarm(DS3232RTC::ALARM_1);    // clear the alarm flag
    myRTC.alarmInterrupt(DS3232RTC::ALARM_1, true);
    printDateTime(t);
    Serial << "  Current RTC time\n";
    printDateTime(a);
    Serial << "  Alarm1 set time\n";
}

volatile bool alarmFlag{false};

void alarmIsr()
{
    alarmFlag = true;
}

void loop()
{
    if (alarmFlag) {
        alarmFlag = false;
        time_t t = myRTC.get();
        time_t a = t + alarmInterval;
        // set the alarm
        myRTC.setAlarm(DS3232RTC::ALM1_MATCH_HOURS, second(a), minute(a), hour(a), 0);
        myRTC.alarm(DS3232RTC::ALARM_1);    // clear the alarm flag
        //myRTC.alarmInterrupt(DS3232RTC::ALARM_1, true);
        printDateTime(t);
        Serial << "  Alarm1 interrupt occurred\n";
        printDateTime(a);
        Serial << "  Alarm1 set time\n";
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
