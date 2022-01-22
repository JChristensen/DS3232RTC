// Arduino DS3232RTC Library
// https://github.com/JChristensen/DS3232RTC
//
// Example sketch illustrating Time library with Real Time Clock.
// This example is similar to the example provided with the Time Library.
// The #include statement has been changed to include the DS3232RTC library,
// a DS3232RTC object has been defined (myRTC) and the begin() method is called.

#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC

DS3232RTC myRTC;

void setup()
{
    Serial.begin(115200);
    myRTC.begin();
    setSyncProvider(myRTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet)
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");
}

void loop()
{
    digitalClockDisplay();
    delay(1000);
}

void digitalClockDisplay()
{
    // digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(' ');
    Serial.print(day());
    Serial.print(' ');
    Serial.print(month());
    Serial.print(' ');
    Serial.print(year());
    Serial.println();
}

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}
