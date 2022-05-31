// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Example sketch to demonstrate usage of the alarm interrupt
// for alarm 1 and alarm 2.
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
// The RTC's time is updated on the falling edge of the 1Hz square
// wave (whether it is output or not). However, the Arduino Time
// library has no knowledge of this, as its time is set asynchronously
// with the RTC via I2C. So on average, it will be 500ms slow
// immediately after setting its time from the RTC. This is seen
// in the sketch output as a one-second difference because the
// time returned by now() has not yet rolled to the next second.
//
// Tested with Arduino 1.8.5, Arduino Uno, DS3231.
// Connect RTC SDA to Arduino pin A4.
// Connect RTC SCL to Arduino pin A5.
// Connect RTC INT/SQW to Arduino pin 3.
//
// Jack Christensen 01Jan2018

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // https://github.com/janelia-arduino/Streaming

constexpr uint8_t SQW_PIN {3};  // RTC provides an interrupt signal on this pin
                                // Can use Pin 2 (INT0) or Pin 3 (INT1) with Arduino Uno
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

    // setSyncProvider() causes the Time library to synchronize with the
    // external RTC by calling myRTC.get() every five minutes by default.
    setSyncProvider(myRTC.get);
    Serial << "RTC Sync";
    if (timeStatus() != timeSet) {
        Serial << " FAIL!";
    }
    Serial << endl;

    printDateTime(myRTC.get());
    Serial << "  Current RTC time\n";

    // configure an interrupt on the falling edge from the SQW pin
    pinMode(SQW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SQW_PIN), alarmIsr, FALLING);

    // set alarm 1 for 20 seconds after every minute
    myRTC.setAlarm(DS3232RTC::ALM1_MATCH_SECONDS, 20, 0, 0, 1); // daydate parameter should be between 1 and 7
    myRTC.alarm(DS3232RTC::ALARM_1);                            // ensure RTC interrupt flag is cleared
    myRTC.alarmInterrupt(DS3232RTC::ALARM_1, true);

    // set alarm 2 for every minute
    myRTC.setAlarm(DS3232RTC::ALM2_EVERY_MINUTE, 0, 0, 0, 1);   // daydate parameter should be between 1 and 7
    myRTC.alarm(DS3232RTC::ALARM_2);                            // ensure RTC interrupt flag is cleared
    myRTC.alarmInterrupt(DS3232RTC::ALARM_2, true);
}

volatile bool alarmIsrWasCalled = false;

void alarmIsr()
{
    alarmIsrWasCalled = true;
}

void loop()
{
    if (alarmIsrWasCalled) {
        if (myRTC.alarm(DS3232RTC::ALARM_1)) {
            printDateTime( myRTC.get() );
            Serial << "  Alarm 1\n";
        }
        if (myRTC.alarm(DS3232RTC::ALARM_2)) {
            printDateTime( myRTC.get() );
            Serial << "  Alarm 2\n";
        }
        alarmIsrWasCalled = false;
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
