// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2017-2022 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino library for the Maxim Integrated DS3232
// and DS3231 Real-Time Clocks.

#ifndef DS3232RTC_H_INCLUDED
#define DS3232RTC_H_INCLUDED

#include <Arduino.h>
#include <TimeLib.h>        // https://github.com/PaulStoffregen/Time

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#ifndef BUFFER_LENGTH       // a horrible and limiting kludge for samd (arduino zero)
#define BUFFER_LENGTH 32
#endif

class DS3232RTC
{
    public:
        // Alarm masks
        enum ALARM_TYPES_t {
            ALM1_EVERY_SECOND = 0x0F,       // trigger alarm every second
            ALM1_MATCH_SECONDS = 0x0E,      // match seconds
            ALM1_MATCH_MINUTES = 0x0C,      // match minutes *and* seconds
            ALM1_MATCH_HOURS = 0x08,        // match hours *and* minutes, seconds
            ALM1_MATCH_DATE = 0x00,         // match date *and* hours, minutes, seconds
            ALM1_MATCH_DAY = 0x10,          // match day *and* hours, minutes, seconds
            ALM2_EVERY_MINUTE = 0x8E,       // trigger alarm every minute
            ALM2_MATCH_MINUTES = 0x8C,      // match minutes
            ALM2_MATCH_HOURS = 0x88,        // match hours *and* minutes
            ALM2_MATCH_DATE = 0x80,         // match date *and* hours, minutes
            ALM2_MATCH_DAY = 0x90           // match day *and* hours, minutes
        };

        // Square-wave output frequency (TS2, RS1 bits)
        enum SQWAVE_FREQS_t {
            SQWAVE_1_HZ,
            SQWAVE_1024_HZ,
            SQWAVE_4096_HZ,
            SQWAVE_8192_HZ,
            SQWAVE_NONE
        };

        // Alarm numbers for alarm functions
        enum ALARM_NBR_t {
            ALARM_1 = 1,
            ALARM_2 = 2
        };

        static constexpr uint8_t
            RTC_ADDR        {0x68},     // I2C device address
            RTC_SECONDS     {0x00},     // register addresses
            RTC_MINUTES     {0x01},
            RTC_HOURS       {0x02},
            RTC_DAY         {0x03},
            RTC_DATE        {0x04},
            RTC_MONTH       {0x05},
            RTC_YEAR        {0x06},
            ALM1_SECONDS    {0x07},
            ALM1_MINUTES    {0x08},
            ALM1_HOURS      {0x09},
            ALM1_DAYDATE    {0x0A},
            ALM2_MINUTES    {0x0B},
            ALM2_HOURS      {0x0C},
            ALM2_DAYDATE    {0x0D},
            RTC_CONTROL     {0x0E},
            RTC_STATUS      {0x0F},
            RTC_AGING       {0x10},
            RTC_TEMP_MSB    {0x11},
            RTC_TEMP_LSB    {0x12},
            SRAM_START_ADDR {0x14},     // first SRAM address
            SRAM_SIZE       {236},      // number of bytes of SRAM
            A1M1            {7},        // alarm mask bits
            A1M2            {7},
            A1M3            {7},
            A1M4            {7},
            A2M2            {7},
            A2M3            {7},
            A2M4            {7},
            EOSC            {7},        // control register bits
            BBSQW           {6},
            CONV            {5},
            RS2             {4},
            RS1             {3},
            INTCN           {2},
            A2IE            {1},
            A1IE            {0},
            OSF             {7},        // status register bits
            BB32KHZ         {6},
            CRATE1          {5},
            CRATE0          {4},
            EN32KHZ         {3},
            BSY             {2},
            A2F             {1},
            A1F             {0},
            DS1307_CH       {7},        // for DS1307 compatibility, Clock Halt bit in Seconds register
            HR1224          {6},        // Hours register 12 or 24 hour mode (24 hour mode==0)
            CENTURY         {7},        // Century bit in Month register
            DYDT            {6};        // Day/Date flag bit in alarm Day/Date registers

        DS3232RTC() {};
        void begin();
        static time_t get();       // static needed to work with setSyncProvider() in the Time library
        byte set(time_t t);
        static byte read(tmElements_t &tm);
        byte write(tmElements_t &tm);
        byte writeRTC(byte addr, byte* values, byte nBytes);
        byte writeRTC(byte addr, byte value);
        byte readRTC(byte addr, byte* values, byte nBytes);
        byte readRTC(byte addr);
        void setAlarm(ALARM_TYPES_t alarmType, byte seconds, byte minutes, byte hours, byte daydate);
        void setAlarm(ALARM_TYPES_t alarmType, byte minutes, byte hours, byte daydate);
        void alarmInterrupt(ALARM_NBR_t alarmNumber, bool alarmEnabled);
        bool alarm(ALARM_NBR_t alarmNumber);
        bool checkAlarm(ALARM_NBR_t alarmNumber);
        bool clearAlarm(ALARM_NBR_t alarmNumber);
        void squareWave(SQWAVE_FREQS_t freq);
        bool oscStopped(bool clearOSF = false);
        int16_t temperature();
        static byte errCode;

    private:
        uint8_t dec2bcd(uint8_t n);
        static uint8_t bcd2dec(uint8_t n);
};

#endif
