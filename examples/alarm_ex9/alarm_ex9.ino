// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2020 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// DS3231/DS3232 Alarm Example Sketch #9
//
// Use Alarm 1 to turn on an LED at a given time, and Alarm 2 to turn
// it off at another time.
// Detect the alarms by polling the alarm flags.
// Assumes the RTC time is already set.
//
// Hardware:
// Arduino Uno, DS3231 RTC.
// Connect RTC SDA to Arduino pin A4.
// Connect RTC SCL to Arduino pin A5.
// Connect a push button from Arduino pin 3 to ground.
// Holding the button down during a reset or power on
// will cause the alarm times to be set. This allows
// us to test for the correct state after a reset or
// power cycle.
//
// Jack Christensen 06Sep2020

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>      // https://github.com/janelia-arduino/Streaming

DS3232RTC myRTC;

// pin definitions
constexpr uint8_t BUTTON_PIN {3};

// current time from the RTC in text format
char timestamp[32];

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial << F( "\n" __FILE__ " " __DATE__ " " __TIME__ "\n" );
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // print the current time
    myRTC.begin();
    time_t t = myRTC.get();
    formatTime(timestamp, t);
    Serial << millis() << F(" Current RTC time ") << timestamp << endl;

    // set the alarms if the button is pushed
    if (!digitalRead(BUTTON_PIN)) {
        // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
        myRTC.setAlarm(DS3232RTC::ALM1_MATCH_DATE, 0, 0, 0, 1);
        myRTC.setAlarm(DS3232RTC::ALM2_MATCH_DATE, 0, 0, 0, 1);
        myRTC.alarm(DS3232RTC::ALARM_1);
        myRTC.alarm(DS3232RTC::ALARM_2);
        myRTC.alarmInterrupt(DS3232RTC::ALARM_1, false);
        myRTC.alarmInterrupt(DS3232RTC::ALARM_2, false);
        myRTC.squareWave(DS3232RTC::SQWAVE_NONE);

        // set alarm 1 to occur in 1-2 minutes
        // set alarm 2 to occur 5 minutes after alarm 1
        tmElements_t tm;
        breakTime(t + 120, tm);
        tm.Second = 0;
        time_t a1 = makeTime(tm);
        time_t a2 = a1 + 300;
        formatTime(timestamp, a1);
        timestamp[8] = 0;                   // keep just hh:mm:ss
        Serial << millis() << F(" Alarm 1 set to ") << timestamp << endl;
        formatTime(timestamp, a2);
        timestamp[5] = 0;                   // keep just hh:mm
        Serial << millis() << F(" Alarm 2 set to ") << timestamp << endl;
        myRTC.setAlarm(DS3232RTC::ALM1_MATCH_HOURS, minute(a1), hour(a1), 1);
        myRTC.alarm(DS3232RTC::ALARM_1);    // ensure alarm flag is cleared
        myRTC.setAlarm(DS3232RTC::ALM2_MATCH_HOURS, minute(a2), hour(a2), 1);
        myRTC.alarm(DS3232RTC::ALARM_2);    // ensure alarm flag is cleared
    }
}

void loop()
{
    enum states_t {LED_OFF, LED_ON};
    static states_t state(LED_OFF);

    switch (state) {
        case LED_OFF:
            if (myRTC.checkAlarm(DS3232RTC::ALARM_1)) { // time to turn on?
                state = LED_ON;
                myRTC.clearAlarm(DS3232RTC::ALARM_2);
                digitalWrite(LED_BUILTIN, HIGH);
                formatTime(timestamp, myRTC.get());     // get current RTC time
                Serial << millis() << F(" Alarm 1 at ") << timestamp << endl;
            }
            break;

        case LED_ON:
            if (myRTC.checkAlarm(DS3232RTC::ALARM_2)) { // time to turn off?
                state = LED_OFF;
                myRTC.clearAlarm(DS3232RTC::ALARM_1);
                digitalWrite(LED_BUILTIN, LOW);
                formatTime(timestamp, myRTC.get());     // get current RTC time
                Serial << millis() << F(" Alarm 2 at ") << timestamp << endl;
            }
            break;
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
