// Abtract base class for:
//   Arduino DS3232RTC Library https://github.com/JChristensen/DS3232RTC
//   and Arduino MCP79412RTC Library // https://github.com/JChristensen/MCP79412RTC
// Copyright (C) 2025 by Jack Christensen and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Allows a sketch to work with either type of RTC, can be determined at run time.

#ifndef GENERIC_RTC_H_INCLUDED
#define GENERIC_RTC_H_INCLUDED

#include <Arduino.h>
#include <TimeLib.h>    // https://github.com/PaulStoffregen/Time
#include <Wire.h>

class GenericRTC
{
    public:
        GenericRTC(TwoWire& tw=Wire) : wire(tw) {};
        virtual void begin() = 0;
        virtual time_t get() = 0;
        virtual uint8_t set(const time_t t) = 0;
        virtual uint8_t writeRTC(const uint8_t addr, const uint8_t* values, const uint8_t nBytes) =0;
        virtual uint8_t writeRTC(const uint8_t addr, const uint8_t value) = 0;
        virtual uint8_t readRTC(const uint8_t addr, uint8_t* values, const uint8_t nBytes) = 0;
        virtual uint8_t readRTC(const uint8_t addr) = 0;

    protected:
        TwoWire& wire;      // reference to Wire, Wire1, etc.
};
#endif
