/*
 * TimeRTC.ino
 * Example code illustrating Time library with DS3232 Real Time Clock.
 * This example is identical to the example provided with the Time Library,
 * only the #include statement has been changed to include the DS3232RTC library.
 */

#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)

/*
 * Setup function.
 */
void setup(void)
{
    Serial.begin(9600); // Start serial at low speed
    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet) // check if we actually got a time from the RTC
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");      
}
/*
 * Update the displayed time every second
 */
void loop(void)
{
    digitalClockDisplay();  
    delay(1000);
}
/*
 * Function to show the time taken from the RTC
 * The time is formatted and printed via serial
 */
void digitalClockDisplay(void)
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

/*
 * Utility function for digital clock display: prints preceding colon and leading 0
 */
void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}
