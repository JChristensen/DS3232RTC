// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// DS3231/DS3232 Alarm Example Sketch #8
//
// Set both alarms to occur once per day at different times.
// Configure the RTC INT/SQW pin to be asserted when alarm match occurs.
// Detect the alarms by polling the INT/SQW pin.
// Assumes the RTC time is already set.
//
// Hardware:
// Arduino Uno, DS3231 RTC.
// Connect RTC SDA to Arduino pin A4.
// Connect RTC SCL to Arduino pin A5.
// Connect RTC INT/SQW to Arduino pin 2.
//
// Jack Christensen 17Dec2018

#include <DS3232RTC.h>        // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        // http://arduiniana.org/libraries/streaming/

// pin definitions
const uint8_t RTC_INT_PIN(2);

// current time from the RTC in text format
char timestamp[32];

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial << F( "\n" __FILE__ " " __DATE__ " " __TIME__ "\n" );
    pinMode(RTC_INT_PIN, INPUT_PULLUP);

    // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
    RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
    RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
    RTC.alarm(ALARM_1);
    RTC.alarm(ALARM_2);
    RTC.alarmInterrupt(ALARM_1, false);
    RTC.alarmInterrupt(ALARM_2, false);
    RTC.squareWave(SQWAVE_NONE);

    // print the current time
    time_t t = RTC.get();
    formatTime(timestamp, t);
    Serial << millis() << F(" Current RTC time ") << timestamp << endl;

    // set alarm 1 to occur in 1-2 minutes
    // set alarm 2 to occur 1 minute after alarm 1
    tmElements_t tm;
    breakTime(t + 120, tm);
    tm.Second = 0;
    time_t a1 = makeTime(tm);
    time_t a2 = a1 + 60;
    formatTime(timestamp, a1);
    timestamp[8] = 0;                   // keep just hh:mm:ss
    Serial << millis() << F(" Alarm 1 set to ") << timestamp << endl;
    formatTime(timestamp, a2);
    timestamp[5] = 0;                   // keep just hh:mm
    Serial << millis() << F(" Alarm 2 set to ") << timestamp << endl;
    RTC.setAlarm(ALM1_MATCH_HOURS, minute(a1), hour(a1), 1);
    RTC.alarm(ALARM_1);                 // ensure RTC interrupt flag is cleared
    RTC.alarmInterrupt(ALARM_1, true);
    RTC.setAlarm(ALM2_MATCH_HOURS, minute(a2), hour(a2), 1);
    RTC.alarm(ALARM_2);                 // ensure RTC interrupt flag is cleared
    RTC.alarmInterrupt(ALARM_2, true);
}

void loop()
{
    // check to see if the INT/SQW pin is low, i.e. an alarm has occurred
    if (!digitalRead(RTC_INT_PIN))
    {
        formatTime(timestamp, RTC.get());   // get current RTC time
        if (RTC.alarm(ALARM_1))             // resets the alarm flag if set
        {
            Serial << millis() << F(" Alarm 1 at ") << timestamp << endl;
        }
        else if (RTC.alarm(ALARM_2))
        {
            Serial << millis() << F(" Alarm 2 at ") << timestamp << endl;
        }
    }
}

// format a time_t value, return the formatted string in buf (must be at least 25 bytes)
void formatTime(char *buf, time_t t)
{
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t));
}
