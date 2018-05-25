// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2018 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino library for the Maxim Integrated DS3232 and DS3231
// Real-Time Clocks.
// Requires PJRC's improved version of the Arduino Time Library,
// https://playground.arduino.cc/Code/Time
// https://github.com/PaulStoffregen/Time
//
// For AVR architecture, a DS3232RTC object named RTC is instantiated
// by the library and I2C initialization occurs in the constructor;
// this is for backwards compatibility.
// For other architectures, the user needs to instantiate a DS3232RTC
// object and optionally initialize the I2C bus by calling
// DS3232RTC::begin(). The constructor has an optional bool parameter
// to indicate whether I2C initialization should occur in the
// constructor; this parameter defaults to true if not given.

#ifndef DS3232RTC_H_INCLUDED
#define DS3232RTC_H_INCLUDED

#include <Arduino.h>
#include <TimeLib.h>        // https://github.com/PaulStoffregen/Time

// Alarm masks
enum ALARM_TYPES_t {
    ALM1_EVERY_SECOND = 0x0F,
    ALM1_MATCH_SECONDS = 0x0E,
    ALM1_MATCH_MINUTES = 0x0C,     // match minutes *and* seconds
    ALM1_MATCH_HOURS = 0x08,       // match hours *and* minutes, seconds
    ALM1_MATCH_DATE = 0x00,        // match date *and* hours, minutes, seconds
    ALM1_MATCH_DAY = 0x10,         // match day *and* hours, minutes, seconds
    ALM2_EVERY_MINUTE = 0x8E,
    ALM2_MATCH_MINUTES = 0x8C,     // match minutes
    ALM2_MATCH_HOURS = 0x88,       // match hours *and* minutes
    ALM2_MATCH_DATE = 0x80,        // match date *and* hours, minutes
    ALM2_MATCH_DAY = 0x90,         // match day *and* hours, minutes
};

// Square-wave output frequency (TS2, RS1 bits)
enum SQWAVE_FREQS_t {
    SQWAVE_1_HZ,
    SQWAVE_1024_HZ,
    SQWAVE_4096_HZ,
    SQWAVE_8192_HZ,
    SQWAVE_NONE
};

#define ALARM_1 1                  // constants for alarm functions
#define ALARM_2 2

class DS3232RTC
{
    public:
        DS3232RTC(bool initI2C = true);
        void begin();
        static time_t get();       // static needed to work with setSyncProvider() in the Time library
        byte set(time_t t);
        static byte read(tmElements_t &tm);
        byte write(tmElements_t &tm);
        byte writeRTC(byte addr, byte *values, byte nBytes);
        byte writeRTC(byte addr, byte value);
        byte readRTC(byte addr, byte *values, byte nBytes);
        byte readRTC(byte addr);
        void setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate);
        void setAlarm(ALARM_TYPES_t alarmType, byte minutes, byte hours, byte daydate);
        void alarmInterrupt(byte alarmNumber, bool alarmEnabled);
        bool alarm(byte alarmNumber);
        void squareWave(SQWAVE_FREQS_t freq);
        bool oscStopped(bool clearOSF = false);
        int temperature();
        static byte errCode;

    private:
        uint8_t dec2bcd(uint8_t n);
        static uint8_t bcd2dec(uint8_t n);
};

#ifdef ARDUINO_ARCH_AVR
extern DS3232RTC RTC;
#endif

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#ifndef BUFFER_LENGTH       // a horrible and limiting kludge for samd (arduino zero)
#define BUFFER_LENGTH 32
#endif

#endif
