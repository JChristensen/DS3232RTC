# Arduino DS3232RTC Library
https://github.com/JChristensen/DS3232RTC  
README file  
Jack Christensen  
Mar 2013

## License
Arduino DS3232RTC Library Copyright (C) 2018 Jack Christensen GNU GPL v3.0

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License v3.0 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/gpl.html>

## Important note: Library v2.0.0

The 2.0.0 version of the library has some significant changes and is not completely backwards compatible with earlier versions. These changes provide a more consistent API and reduce the possibility of name collisions. While sketches using this library will likely require changes as a result, these should be mostly straightforward.

 - The library no longer defines a `DS3232RTC` object, therefore each sketch needs to define  one. (Previous versions of the library defined a DS3232RTC object named `RTC`, although only for AVR architecture. Consider using a name other than `RTC` as this can cause a name collision on some architectures.)
 - The constructor no longer has the capability to initialize the I2C bus and no longer accepts an optional parameter. Therefore, the sketch needs to call `DS3232RTC::begin()` in the `setup()` function or elsewhere as appropriate.
 - To reduce the possibility of name collisions, the enumerations as well as register addresses, etc. are now defined in the header file within the DS3232RTC class. Therefore, when using any of these names, it will be necessary to include the DS3232RTC scope, e.g. `myRTC.alarm(DS3232RTC::ALARM_1);`
 - Also to reduce collisions, register address names and bit names have been prefixed with `DS32_` (see the header file.)
 - The example sketches and documentation have been updated to reflect these changes.

## Introduction
**DS3232RTC** is an Arduino library that supports the Maxim Integrated DS3231 and DS3232 Real-Time Clocks. This library is intended to be used with [PJRC's Arduino Time library](https://github.com/PaulStoffregen/Time).

The **DS3232RTC** library also implements functions to support the additional features of the DS3232 and DS3231. The DS3232 has the same features as the DS3231, but the DS3232 also has these extra features:

- Battery-backed SRAM
- Battery-backed 32kHz output (BB32kHz bit in Control/Status register 0x0F)
- Adjustable temperature sensor sample rate (CRATE1:0 bits in the Control/Status register).


## Examples
The following example sketches are included with the **DS3232RTC** library:

- **TimeRTC:** A simple example to display the time and date.
- **SetSerial:** Set the RTC's date and time from the Arduino serial monitor. Displays date, time and temperature.
- **rtcTimeTemp:** Displays time and temperature.
- **rtc_temperature:** Displays time and temperature but initiates a temperature conversion every 10 seconds in addition to the RTC's default conversion period of 64 seconds.
- **rtc_interrupt:** Uses a 1Hz interrupt from the RTC to keep time.
- **mcu_clock_drift:** Demonstrates that MCU time as kept by the **Time** library will drift as compared to RTC time, and that MCU time may not increase by one second every second.
- **tiny3232_KnockBang:** Demonstrates interfacing an ATtiny45/85 to a DS3231 or DS3232 RTC.
- Several examples for using the RTC alarms. See the [alarm primer](https://github.com/JChristensen/DS3232RTC/blob/master/alarm-primer.md) for more information.

## Usage notes

When using the **DS3232RTC** library, the user is responsible for ensuring that reads and writes do not exceed the device's address space (0x00-0x12 for DS3231, 0x00-0xFF for DS3232); no bounds checking is done by the library.            

## Enumerations

### ALARM_TYPES_t
##### Description
Symbolic names used with the `setAlarm()` function (described below).
##### Values for Alarm 1
- ALM1_EVERY_SECOND -- causes an alarm once per second.
- ALM1_MATCH_SECONDS -- causes an alarm when the seconds match (i.e. once per minute).
- ALM1_MATCH_MINUTES -- causes an alarm when the minutes *and* seconds match.
- ALM1_MATCH_HOURS -- causes an alarm when the hours *and* minutes *and* seconds match.
- ALM1_MATCH_DATE -- causes an alarm when the date of the month *and* hours *and* minutes *and* seconds match.
- ALM1_MATCH_DAY -- causes an alarm when the day of the week *and* hours *and* minutes *and* seconds match.

##### Values for Alarm 2
- ALM2_EVERY_MINUTE -- causes an alarm once per minute.
- ALM2_MATCH_MINUTES -- causes an alarm when the minutes match (i.e. once per hour).
- ALM2_MATCH_HOURS -- causes an alarm when the hours *and* minutes match.
- ALM2_MATCH_DATE -- causes an alarm when the date of the month *and* hours *and* minutes match.
- ALM2_MATCH_DAY -- causes an alarm when the day of the week *and* hours *and* minutes match.

### ALARM_NBR_t
##### Description
Symbolic names used with alarm functions.
##### Values
- ALARM_1
- ALARM_2

### SQWAVE_FREQS_t
##### Description
Symbolic names used with the squareWave() function (described below).
##### Values
- SQWAVE_NONE
- SQWAVE_1_HZ
- SQWAVE_1024_HZ
- SQWAVE_4096_HZ
- SQWAVE_8192_HZ

## Constructor
### DS3232RTC()
##### Description
Instantiates a DS3232RTC object.
##### Syntax
`DS3232RTC myRTC;`
##### Parameters
None.
##### Returns
None.
##### Example
```c++
DS3232RTC myRTC;
```

## Initialization function
### begin()
##### Description
Initializes the I2C bus. Calls `Wire.begin()`.
##### Syntax
`begin();`
##### Parameters
None.
##### Returns
None.
##### Example
```c++
DS3232RTC myRTC;
void setup() {
    myRTC.begin();
}
```

## Functions for setting and reading the time
### get()
##### Description
Reads the current date and time from the RTC and returns it as a `time_t` value. Returns zero if an I2C error occurs (RTC not present, etc.).
##### Syntax
`myRTC.get();`
##### Parameters
None.
##### Returns
Current date and time *(time_t)*
##### Example
```c++
time_t myTime = myRTC.get();
```

### set(time_t t)
##### Description
Sets the RTC date and time to the given `time_t` value. Clears the oscillator stop flag (OSF) bit in the control/status register. See the `oscStopped()` function and also the DS323x datasheet for more information on the OSF bit.
##### Syntax
`myRTC.set(t);`
##### Parameters
**t:** The date and time to set the RTC to *(time_t)*
##### Returns
I2C status *(uint8_t)*. Returns zero if successful.
##### Example
```c++
// this example first sets the system time (maintained by the Time library) to
// a hard-coded date and time, and then sets the RTC from the system time.
// the setTime() function is part of the Time library.
setTime(23, 31, 30, 13, 2, 2009);   // set the system time to 23h31m30s on 13Feb2009
myRTC.set(now());                   // set the RTC from the system time
```

### read(tmElements_t &tm)
##### Description
Reads the current date and time from the RTC and returns it as a `tmElements_t` structure. See the Arduino [Time](https://www.arduino.cc/playground/Code/Time) [Library](https://github.com/PaulStoffregen/Time) for details on the `tmElements_t` structure.
##### Syntax
`myRTC.read(tm);`
##### Parameters
**tm:** Address of a `tmElements_t` structure to which the date and time are returned.
##### Returns
I2C status *(uint8_t)*. Returns zero if successful. The date and time read from the RTC are returned to the `tm` parameter.
##### Example
```c++
tmElements_t tm;
myRTC.read(tm);
Serial.print(tm.Hour, DEC);
Serial.print(':');
Serial.print(tm.Minute,DEC);
Serial.print(':');
Serial.println(tm.Second,DEC);
```

### write(tmElements_t &tm)
##### Description
Sets the RTC to the date and time given by a `tmElements_t` structure. Clears the oscillator stop flag (OSF) bit in the control/status register. See the `oscStopped()` function and also the DS323x datasheet for more information on the OSF bit.
##### Syntax
`myRTC.write(tm);`
##### Parameters
**tm:** Address of a `tmElements_t` structure used to set the date and time.
##### Returns
I2C status *(uint8_t)*. Returns zero if successful.
##### Example
```c++
tmElements_t tm;
tm.Hour = 23;             // set the tm structure to 23h31m30s on 13Feb2009
tm.Minute = 31;
tm.Second = 30;
tm.Day = 13;
tm.Month = 2;
tm.Year = 2009 - 1970;    // tmElements_t.Year is the offset from 1970
myRTC.write(tm);          // set the RTC from the tm structure
```

## Alarm functions
The DS3231 and DS3232 have two alarms. Alarm1 can be set to seconds precision; Alarm2 can only be set to minutes precision.

### setAlarm(ALARM_TYPES_t alarmType, uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t daydate)
##### Description
Set an alarm time. Sets the alarm registers only.  To cause the INT pin to be asserted on alarm match, use `alarmInterrupt()`. This function can set either Alarm 1 or Alarm 2, depending on the value of `alarmType` (use the `ALARM_TYPES_t` enumeration above). When setting Alarm 2, the seconds value must be supplied but is ignored, recommend using zero. (Alarm 2 has no seconds register.)

##### Syntax
`myRTC.setAlarm(alarmType, seconds, minutes, hours, dayOrDate);`
##### Parameters
**alarmType:** A value from the `ALARM_TYPES_t` enumeration, above. *(ALARM_TYPES_t)*  
**seconds:** The seconds value to set the alarm to. *(uint8_t)*  
**minutes:** The minutes value to set the alarm to. *(uint8_t)*  
**hours:** The hours value to set the alarm to. *(uint8_t)*  
**dayOrDate:** The day of the week or the date of the month. For day of the week, use a value from the Time library `timeDayOfWeek_t` enumeration, i.e. dowSunday, dowMonday, dowTuesday, dowWednesday, dowThursday, dowFriday, dowSaturday. *(uint8_t)*  
##### Returns
None.
##### Example
```c++
// Set Alarm1 for 12:34:56 on Sunday
myRTC.setAlarm(DS3232RTC::ALM1_MATCH_DAY, 56, 34, 12, dowSunday);
```

### setAlarm(ALARM_TYPES_t alarmType, uint8_t minutes, uint8_t hours, uint8_t daydate)
##### Description
Set an alarm time. Sets the alarm registers only.  To cause the INT pin to be asserted on alarm match, use `alarmInterrupt()`.  This function can set either Alarm 1 or Alarm 2, depending on the value of `alarmTyp`e (use the `ALARM_TYPES_t` enumeration above). However, when using this function to set Alarm 1, the seconds value is set to zero. (Alarm 2 has no seconds register.)

##### Syntax
`myRTC.setAlarm(alarmType, minutes, hours, dayOrDate);`
##### Parameters
**alarmType:** A value from the ALARM_TYPES_t enumeration, above. *(ALARM_TYPES_t)*  
**minutes:** The minutes value to set the alarm to. *(uint8_t)*  
**hours:** The hours value to set the alarm to. *(uint8_t)*  
**dayOrDate:** The day of the week or the date of the month. For day of the week, use a value from the Time library timeDayOfWeek_t enumeration, i.e. dowSunday, dowMonday, dowTuesday, dowWednesday, dowThursday, dowFriday, dowSaturday. *(uint8_t)*  
##### Returns
None.
##### Example
```c++
// Set Alarm2 for 12:34 on the 4th day of the month
myRTC.setAlarm(DS3232RTC::ALM2_MATCH_DATE, 34, 12, 4);
```

### alarmInterrupt(ALARM_NBR_t alarmNumber, bool alarmEnabled)
##### Description
Enable or disable an alarm "interrupt". Note that this "interrupt" just causes the RTC's INT pin to be asserted. To use this signal as an actual interrupt to (for example) a microcontroller, the RTC "interrupt" pin needs to be connected to the microcontroller and the microcontroller application firmware must properly configure the interrupt and also provide for handling it.  
##### Syntax
`myRTC.alarmInterrupt(alarmNumber, enable);`
##### Parameters
**alarmNumber:** The number of the alarm to enable or disable, ALARM_1 or ALARM_2 *(ALARM_NBR_t)*  
**alarmEnabled:** true or false *(bool)*  
##### Returns
None.
##### Example
```c++
myRTC.alarmInterrupt(DS3232RTC::ALARM_1, true);     // assert the INT pin when Alarm1 occurs.
myRTC.alarmInterrupt(DS3232RTC::ALARM_2, false);    // disable Alarm2
```

### alarm(ALARM_NBR_t alarmNumber)
##### Description
Tests whether an alarm has been triggered. If the alarm was triggered, returns true and resets the alarm flag in the RTC, else returns false.
##### Syntax
`myRTC.alarm(alarmNumber);`
##### Parameters
**alarmNumber:** The number of the alarm to test, `ALARM_1` or `ALARM_2` *(ALARM_NBR_t)*  
##### Returns
Value of the alarm flag bit *(bool)*
##### Example
```c++
if ( myRTC.alarm(DS3232RTC::ALARM_1) ) {		// has Alarm1 triggered?
	// yes, act on the alarm
}
else {
	// no alarm
}
```

### checkAlarm(ALARM_NBR_t alarmNumber)
##### Description
Tests whether an alarm has been triggered. If the alarm was triggered, returns true, else returns false. The alarm flag is not reset.
##### Syntax
`myRTC.checkAlarm(alarmNumber);`
##### Parameters
**alarmNumber:** The number of the alarm to test, `ALARM_1` or `ALARM_2` *(ALARM_NBR_t)*  
##### Returns
Value of the alarm flag bit *(bool)*
##### Example
```c++
if ( myRTC.checkAlarm(DS3232RTC::ALARM_1) ) {      // has Alarm1 triggered?
	// yes, act on the alarm
}
else {
	// no alarm
}
```
### clearAlarm(ALARM_NBR_t alarmNumber)
##### Description
Clears the given alarm flag bit.
##### Syntax
`myRTC.clearAlarm(alarmNumber);`
##### Parameters
**alarmNumber:** The number of the alarm to test, ALARM_1 or ALARM_2 *(ALARM_NBR_t)*  
##### Returns
Value of the alarm flag bit, before it was cleared *(bool)*
##### Example
```c++
if ( myRTC.checkAlarm(DS3232RTC::ALARM_1) ) {   // has Alarm1 triggered?
	myRTC.clearAlarm(DS3232RTC::ALARM_1);       // yes, clear the alarm flag
}
else {
	// no alarm
}
```

## Functions for reading and writing RTC registers or static RAM (SRAM) for the DS3232
The DS3232RTC.h file defines symbolic names for the timekeeping, alarm, status and control registers. These can be used for the addr argument in the functions below.

### writeRTC(uint8_t addr, uint8_t* values, uint8_t nBytes)
##### Description
Write one or more bytes to RTC memory.
##### Syntax
`myRTC.writeRTC(addr, values, nbytes);`
##### Parameters
**addr:** First SRAM address to write *(uint8_t)*. The valid address range is 0x00-0x12 for DS3231, 0x00-0xFF for DS3232. The general-purpose SRAM for the DS3232 begins at address 0x14. Address is not checked for validity by the library.  
**values:** An array of values to write _(*uint8_t)_  
**nBytes:** Number of bytes to write *(uint8_t)*. Must be between 1 and 31 (Wire library limitation) but is not checked by the library. 
##### Returns
I2C status *(uint8_t)*. Returns zero if successful.
##### Example
```c++
//write 1, 2, ..., 8 to the first eight DS3232 SRAM locations
uint8_t buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};
myRTC.sramWrite(0x14, buf, 8);
```

### writeRTC(uint8_t addr, uint8_t value)
##### Description
Write a single byte to RTC memory.
##### Syntax
`myRTC.writeRTC(addr, value);`
##### Parameters
**addr:** SRAM address to write *(uint8_t)*. The valid address range is 0x00-0x12 for DS3231, 0x00-0xFF for DS3232. The general-purpose SRAM for the DS3232 begins at address 0x14. Address is not checked for validity by the library.  
**value:** Value to write _(uint8_t)_  
##### Returns
I2C status *(uint8_t)*. Returns zero if successful.
##### Example
```c++
myRTC.writeRTC(3, 14);   // write the value 14 to SRAM address 3
```

### readRTC(uint8_t addr, uint8_t* values, uint8_t nBytes)
##### Description
Read one or more bytes from RTC RAM.
##### Syntax
`myRTC.readRTC(addr, values, nbytes);`
##### Parameters
**addr:** First SRAM address to read *(uint8_t)*. The valid address range is 0x00-0x12 for DS3231, 0x00-0xFF for DS3232. The general-purpose SRAM for the DS3232 begins at address 0x14. Address is not checked for validity by the library.  
**values:** An array to receive the values read _(*uint8_t)_  
**nBytes:** Number of bytes to read *(uint8_t)*. Must be between 1 and 32 (Wire library limitation) but is not checked by the library. 
##### Returns
I2C status *(uint8_t)*. Returns zero if successful.
##### Example
```c++
// read the last eight locations of SRAM into buf
uint8_t buf[8];
myRTC.sramRead(248, buf, 8);
```

### readRTC(uint8_t addr)
##### Description
Reads a single byte from RTC RAM.
##### Syntax
`myRTC.readRTC(addr);`
##### Parameters
**addr:** SRAM address to read *(uint8_t)*. The valid address range is 0x00-0x12 for DS3231, 0x00-0xFF for DS3232. The general-purpose SRAM for the DS3232 begins at address 0x14. Address is not checked for validity by the library.
##### Returns
Value read from the RTC *(uint8_t)*
##### Example
```c++
uint8_t val;
val = myRTC.readRTC(3);  //read the value from SRAM location 3
```

## Other functions
### temperature()
##### Description
Returns the RTC temperature.
##### Syntax
`myRTC.temperature();`
##### Parameters
None.
##### Returns
RTC temperature as degrees Celsius times four. *(int)*
##### Example
```c++
int t = myRTC.temperature();
float celsius = t / 4.0;
float fahrenheit = celsius * 9.0 / 5.0 + 32.0;
```

### squareWave(SQWAVE_FREQS_t freq)
##### Description
Enables or disables the square wave output.
##### Syntax
`myRTC.squareWave(freq);`
##### Parameters
**freq:** a value from the `SQWAVE_FREQS_t` enumeration above. *(SQWAVE_FREQS_t)*  
##### Returns
None.
##### Example
```c++
myRTC.squareWave(DS3232RTC::SQWAVE_1_HZ);   // 1 Hz square wave
myRTC.squareWave(DS3232RTC::SQWAVE_NONE);   // no square wave
```

### oscStopped(bool clearOSF)
##### Description
Returns the value of the oscillator stop flag (OSF) bit in the control/status register which indicates that the oscillator is or was stopped, and that the timekeeping data may be invalid. Optionally clears the OSF bit depending on the argument passed. If the `clearOSF` argument is omitted, the OSF bit is cleared by default. Calls to `set()` and `write()` also clear the OSF bit.

##### Syntax
`myRTC.oscStopped(clearOSF);`
##### Parameters
**clearOSF:** an optional true or false value to indicate whether the OSF bit should be cleared (reset). If not supplied, a default value of true is used, resetting the OSF bit. *(bool)*
##### Returns
True or false *(bool)*
##### Example
```c++
if ( myRTC.oscStopped(false) ) {   // check the oscillator
	// may be trouble
}
else {
	// all is well
}
```
