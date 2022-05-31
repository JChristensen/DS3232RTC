// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
// Copyright (C) 2017-2022 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino library for the Maxim Integrated DS3232
// and DS3231 Real-Time Clocks.

#include <DS3232RTC.h>

uint8_t DS3232RTC::errCode;

// Initialize the I2C bus.
void DS3232RTC::begin()
{
    i2cBegin();
}

// Read the current time from the RTC and return it as a time_t
// value. Returns a zero value if an I2C error occurred (e.g. RTC
// not present).
time_t DS3232RTC::get()
{
    tmElements_t tm;

    if ( read(tm) ) return 0;
    return( makeTime(tm) );
}

// Set the RTC to the given time_t value and clear the
// oscillator stop flag (OSF) in the Control/Status register.
// Returns the I2C status (zero if successful).
uint8_t DS3232RTC::set(time_t t)
{
    tmElements_t tm;

    breakTime(t, tm);
    return ( write(tm) );
}

// Read the current time from the RTC and return it in a tmElements_t
// structure. Returns the I2C status (zero if successful).
uint8_t DS3232RTC::read(tmElements_t &tm)
{
    i2cBeginTransmission(DS32_ADDR);
    i2cWrite(DS32_SECONDS);
    if ( uint8_t e = i2cEndTransmission() ) { errCode = e; return e; }
    // request 7 bytes (secs, min, hr, dow, date, mth, yr)
    i2cRequestFrom(DS32_ADDR, static_cast<uint8_t>(tmNbrFields));
    tm.Second = bcd2dec(i2cRead() & ~_BV(DS1307_CH));
    tm.Minute = bcd2dec(i2cRead());
    tm.Hour = bcd2dec(i2cRead() & ~_BV(DS32_HR1224));   // assumes 24hr clock
    tm.Wday = i2cRead();
    tm.Day = bcd2dec(i2cRead());
    tm.Month = bcd2dec(i2cRead() & ~_BV(DS32_CENTURY)); // don't use the Century bit
    tm.Year = y2kYearToTm(bcd2dec(i2cRead()));
    return 0;
}

// Set the RTC time from a tmElements_t structure and clear the
// oscillator stop flag (OSF) in the Control/Status register.
// Returns the I2C status (zero if successful).
uint8_t DS3232RTC::write(tmElements_t &tm)
{
    i2cBeginTransmission(DS32_ADDR);
    i2cWrite(DS32_SECONDS);
    i2cWrite(dec2bcd(tm.Second));
    i2cWrite(dec2bcd(tm.Minute));
    i2cWrite(dec2bcd(tm.Hour));         // sets 24 hour format (Bit 6 == 0)
    i2cWrite(tm.Wday);
    i2cWrite(dec2bcd(tm.Day));
    i2cWrite(dec2bcd(tm.Month));
    i2cWrite(dec2bcd(tmYearToY2k(tm.Year)));
    uint8_t ret = i2cEndTransmission();
    uint8_t s = readRTC(DS32_STATUS);               // read the status register
    writeRTC( DS32_STATUS, s & ~_BV(DS32_OSF) );    // clear the Oscillator Stop Flag
    return ret;
}

// Write multiple bytes to RTC RAM.
// Valid address range is 0x00 - 0xFF, no checking.
// Number of bytes (nBytes) must be between 1 and 31 (Wire library
// limitation).
// Returns the I2C status (zero if successful).
uint8_t DS3232RTC::writeRTC(uint8_t addr, uint8_t* values, uint8_t nBytes)
{
    i2cBeginTransmission(DS32_ADDR);
    i2cWrite(addr);
    for (uint8_t i=0; i<nBytes; i++) i2cWrite(values[i]);
    return i2cEndTransmission();
}

// Write a single byte to RTC RAM.
// Valid address range is 0x00 - 0xFF, no checking.
// Returns the I2C status (zero if successful).
uint8_t DS3232RTC::writeRTC(uint8_t addr, uint8_t value)
{
    return ( writeRTC(addr, &value, 1) );
}

// Read multiple bytes from RTC RAM.
// Valid address range is 0x00 - 0xFF, no checking.
// Number of bytes (nBytes) must be between 1 and 32 (Wire library
// limitation).
// Returns the I2C status (zero if successful).
uint8_t DS3232RTC::readRTC(uint8_t addr, uint8_t* values, uint8_t nBytes)
{
    i2cBeginTransmission(DS32_ADDR);
    i2cWrite(addr);
    if ( uint8_t e = i2cEndTransmission() ) return e;
    i2cRequestFrom( (uint8_t)DS32_ADDR, nBytes );
    for (uint8_t i=0; i<nBytes; i++) values[i] = i2cRead();
    return 0;
}

// Read a single byte from RTC RAM.
// Valid address range is 0x00 - 0xFF, no checking.
uint8_t DS3232RTC::readRTC(uint8_t addr)
{
    uint8_t b {0};

    readRTC(addr, &b, 1);
    return b;
}

// Set an alarm time. Sets the alarm registers only.  To cause the
// INT pin to be asserted on alarm match, use alarmInterrupt().
// This method can set either Alarm 1 or Alarm 2, depending on the
// value of alarmType (use a value from the ALARM_TYPES_t enumeration).
// When setting Alarm 2, the seconds value must be supplied but is
// ignored, recommend using zero. (Alarm 2 has no seconds register.)
void DS3232RTC::setAlarm(ALARM_TYPES_t alarmType, uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t daydate)
{
    seconds = dec2bcd(seconds);
    minutes = dec2bcd(minutes);
    hours = dec2bcd(hours);
    daydate = dec2bcd(daydate);
    if (alarmType & 0x01) seconds |= _BV(DS32_A1M1);
    if (alarmType & 0x02) minutes |= _BV(DS32_A1M2);
    if (alarmType & 0x04) hours |= _BV(DS32_A1M3);
    if (alarmType & 0x10) daydate |= _BV(DS32_DYDT);
    if (alarmType & 0x08) daydate |= _BV(DS32_A1M4);

    uint8_t addr;
    if ( !(alarmType & 0x80) ) {    // alarm 1
        addr = DS32_ALM1_SEC;
        writeRTC(addr++, seconds);
    }
    else {
        addr = DS32_ALM2_MIN;
    }
    writeRTC(addr++, minutes);
    writeRTC(addr++, hours);
    writeRTC(addr++, daydate);
}

// Set an alarm time. Sets the alarm registers only. To cause the
// INT pin to be asserted on alarm match, use alarmInterrupt().
// This method can set either Alarm 1 or Alarm 2, depending on the
// value of alarmType (use a value from the ALARM_TYPES_t enumeration).
// However, when using this method to set Alarm 1, the seconds value
// is set to zero. (Alarm 2 has no seconds register.)
void DS3232RTC::setAlarm(ALARM_TYPES_t alarmType, uint8_t minutes, uint8_t hours, uint8_t daydate)
{
    setAlarm(alarmType, 0, minutes, hours, daydate);
}

// Enable or disable an alarm "interrupt" which asserts the INT pin
// on the RTC.
void DS3232RTC::alarmInterrupt(ALARM_NBR_t alarmNumber, bool interruptEnabled)
{
    uint8_t controlReg = readRTC(DS32_CONTROL);
    uint8_t mask = _BV(DS32_A1IE) << (alarmNumber - 1);
    if (interruptEnabled) {
        controlReg |= mask;
    }
    else {
        controlReg &= ~mask;
    }
    writeRTC(DS32_CONTROL, controlReg);
}

// Returns true or false depending on whether the given alarm has been
// triggered, and resets the alarm flag bit.
bool DS3232RTC::alarm(ALARM_NBR_t alarmNumber)
{
    uint8_t statusReg = readRTC(DS32_STATUS);
    uint8_t mask = _BV(DS32_A1F) << (alarmNumber - 1);
    if (statusReg & mask) {
        statusReg &= ~mask;
        writeRTC(DS32_STATUS, statusReg);
        return true;
    }
    else {
        return false;
    }
}

// Returns true or false depending on whether the given alarm has been
// triggered, without resetting the alarm flag bit.
bool DS3232RTC::checkAlarm(ALARM_NBR_t alarmNumber)
{
    uint8_t statusReg = readRTC(DS32_STATUS);
    uint8_t mask = _BV(DS32_A1F) << (alarmNumber - 1);
    return (statusReg & mask);
}

// Clears the given alarm flag bit if it is set.
// Returns the value of the flag bit before if was cleared.
bool DS3232RTC::clearAlarm(ALARM_NBR_t alarmNumber)
{
    uint8_t statusReg = readRTC(DS32_STATUS);
    uint8_t mask = _BV(DS32_A1F) << (alarmNumber - 1);
    bool retVal = statusReg & mask;
    if (retVal) {
        statusReg &= ~mask;
        writeRTC(DS32_STATUS, statusReg);
    }
    return retVal;
}

// Enable or disable the square wave output.
// Use a value from the SQWAVE_FREQS_t enumeration for the parameter.
void DS3232RTC::squareWave(SQWAVE_FREQS_t freq)
{
    uint8_t controlReg = readRTC(DS32_CONTROL);
    if (freq >= SQWAVE_NONE) {
        controlReg |= _BV(DS32_INTCN);
    }
    else {
        controlReg = (controlReg & 0xE3) | (freq << DS32_RS1);
    }
    writeRTC(DS32_CONTROL, controlReg);
}

// Returns the value of the oscillator stop flag (OSF) bit in the
// control/status register which indicates that the oscillator is or
// was stopped, and that the timekeeping data may be invalid.
// Optionally clears the OSF bit depending on the argument passed.
bool DS3232RTC::oscStopped(bool clearOSF)
{
    uint8_t s = readRTC(DS32_STATUS);   // read the status register
    bool ret = s & _BV(DS32_OSF);       // isolate the osc stop flag to return to caller
    if (ret && clearOSF) {              // clear OSF if it's set and the caller wants to clear it
        writeRTC( DS32_STATUS, s & ~_BV(DS32_OSF) );
    }
    return ret;
}

// Returns the temperature in Celsius times four.
int16_t DS3232RTC::temperature()
{
    union int16_byte {
        int16_t i;
        uint8_t b[2];
    } rtcTemp;

    rtcTemp.b[0] = readRTC(DS32_TEMP_LSB);
    rtcTemp.b[1] = readRTC(DS32_TEMP_MSB);
    return rtcTemp.i / 64;
}

// Decimal-to-BCD conversion
uint8_t DS3232RTC::dec2bcd(uint8_t n)
{
    return n + 6 * (n / 10);
}

// BCD-to-Decimal conversion
uint8_t __attribute__ ((noinline)) DS3232RTC::bcd2dec(uint8_t n)
{
    return n - 6 * (n >> 4);
}

