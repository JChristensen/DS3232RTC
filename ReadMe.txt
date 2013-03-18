ReadMe file for Arduino DS3232RTC Library Alpha Version*
*Use at your own risk. Not all functionality has been tested.
http://github.com/JChristensen/DS3232RTC
Jack Christensen Mar 2013

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0
Unported License. To view a copy of this license, visit
http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative
Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.

================================================================================
Arduino library to support the Maxim Integrated DS3232 and DS3231
Real-Time Clocks. This library is intended for use with the Arduino Time.h
library, http://www.arduino.cc/playground/Code/Time.

The DS3232RTC library is a drop-in replacement for the DS1307RTC.h library by
Michael Margolis that is supplied with the Arduino Time library above. To change
from using a DS1307 RTC to a DS3232 RTC, it is only necessary to change the
#include statement to include DS3232RTC.h instead of DS1307RTC.h.

This library also implements functions to support the additional
features of the DS3232.

--------------------------------------------------------------------------------
To use the library:
(1) Go to http://github.com/JChristensen/DS3232RTC and click the ZIP button to
    download the repository as a ZIP file to a convenient location on your PC.
(2) Uncompress the downloaded file. This will result in a folder containing all
    the files for the library, that has a name that includes the branch name,
    for example "DS3232RTC-master".
(3) Rename the folder to just "DS3232RTC".
(4) Copy the renamed folder to the Arduino sketchbook\libraries folder.

--------------------------------------------------------------------------------
The following example sketches are included with the Timezone library:

(No examples yet.)

--------------------------------------------------------------------------------
Similar to the DS1307RTC library, the DS3232RTC library instantiates an RTC
object; the user does not need to do this.

To use the DS3232RTC library, the Time and Wire libraries must also be included.
For brevity, these includes are not repeated in the examples below:
#include <DS3232RTC.h>   	      //http://github.com/JChristensen/DS3232RTC
#include <Time.h>                 //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>                 //http://arduino.cc/en/Reference/Wire
                                  //(Wire.h is included with Arduino IDE)

================================================================================
SETTING AND READING THE TIME

--------------------------------------------------------------------------------
The get() method reads the current time from the RTC and returns it as a time_t
value.

    time_t myTime;
    myTime = RTC.get();

--------------------------------------------------------------------------------
The set(time_t t) method sets the RTC to the given time_t value.
The example below first sets the system time (maintained by the Time library) to
a hard-coded date and time, then sets the RTC from the system time. The
setTime(hr, min, sec, day, month, year) function comes from the Time library.

    setTime(23, 31, 30, 13, 2, 2009);   //set the system time to
                                        //23h31m30s on 3Feb2009
    RTC.set(now());                     //set the RTC from the system time

--------------------------------------------------------------------------------
The read(tmElements_t &tm) method reads the current time from the RTC and
returns it as a tmElements_t structure.
(See the Arduino Time library for details on the tmElements_t structure:
http://www.arduino.cc/playground/Code/Time.)

    tmElements_t tm;
    RTC.read(tm);
    Serial.print(tm.Hour, DEC);
    Serial.print(':');
    Serial.print(tm.Minute,DEC);
    Serial.print(':');
    Serial.println(tm.Second,DEC);

--------------------------------------------------------------------------------
The write(tmElements_t &tm) method sets the RTC to the date and time
as represented in a tmElements_t structure.

    tmElements_t tm;
    tm.Hour = 23;             //set the tm structure to 23h31m30s on 13Feb2009
    tm.Minute = 31;
    tm.Minute = 30;
    tm.Day = 13;
    tm.Month = 2;
    tm.Year = 2009 - 1970;    //tmElements_t.Year is the offset from 1970.
    RTC.write(tm);            //set the RTC from the tm structure

--------------------------------------------------------------------------------
