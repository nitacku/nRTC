/*
 * Copyright (c) 2018 nitacku
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * @file        nRTC.cpp
 * @summary     Generic Real Time Clock interface
 * @version     3.3
 * @author      nitacku
 * @data        15 July 2018
 */

#include "nRTC.h"

CRTC::CRTC(void)
{
}


void CRTC::Initialize(void)
{
    m_i2c_handle = nI2C->RegisterDevice(GetI2CAddress(), 1, CI2C::Speed::FAST);
}


uint32_t CRTC::GetTimeSeconds(void)
{
    GetRTC(m_rtc); // Populate rtc with current values

    return GetSeconds(m_rtc);
}


void CRTC::GetTime(uint8_t &hour, uint8_t &minute, uint8_t &second)
{
    GetRTC(m_rtc); // Populate rtc with current values

    second = m_rtc.second;
    minute = m_rtc.minute;
    hour = m_rtc.hour;
}


void CRTC::GetDate(uint8_t &year, uint8_t &month, uint8_t &day)
{
    GetRTC(m_rtc); // Populate rtc with current values

    day = m_rtc.day;
    month = m_rtc.month;
    year = m_rtc.year;
}


CRTC::status_t CRTC::SetTime(const uint8_t hour, const uint8_t minute, const uint8_t second)
{
    GetRTC(m_rtc); // Populate rtc with current values

    m_rtc.second = second;
    m_rtc.minute = minute;
    m_rtc.hour = hour;

    return SetRTC(m_rtc);
}


CRTC::status_t CRTC::SetDate(const uint8_t year, const uint8_t month, const uint8_t day)
{
    GetRTC(m_rtc); // Populate rtc with current values

    m_rtc.day = day;
    m_rtc.month = month;
    m_rtc.year = year;

    return SetRTC(m_rtc);
}


float CRTC::ConvertTemperature(const float temperature, const Unit input_unit, const Unit output_unit)
{
    switch (input_unit)
    {
        case Unit::C:
        switch (output_unit)
        {
            case Unit::C:
            return temperature;

            case Unit::F:
            return temperature * (9.0f / 5.0f) + 32;

            case Unit::K:
            return temperature + 273.15f;
        }
        break;

        case Unit::F:
        switch (output_unit)
        {
            case Unit::C:
            return (temperature - 32) * 5.0f / 9.0f;

            case Unit::F:
            return temperature;

            case Unit::K:
            return (temperature + 459.67f) * 5.0f / 9.0f;
        }
        break;

        case Unit::K:
        switch (output_unit)
        {
            case Unit::C:
            return temperature - 237.15f;

            case Unit::F:
            return temperature * (9.0f / 5.0f) - 459.67f;

            case Unit::K:
            return temperature;
        }
        break;
    }

    return temperature;
}


CRTC::status_t CRTC::SetAlarmTime(const uint8_t hour, const uint8_t minute, const uint8_t second)
{
    m_rtc.second = second;
    m_rtc.minute = minute;
    m_rtc.hour = hour;
    
    return SetAlarmRTC(m_rtc);
}


void CRTC::GetAlarmTime(uint8_t &hour, uint8_t &minute, uint8_t &second)
{
    GetAlarmRTC(m_rtc);

    second = m_rtc.second;
    minute = m_rtc.minute;
    hour = m_rtc.hour;
}


uint32_t CRTC::GetAlarmSeconds(void)
{
    GetAlarmRTC(m_rtc);
    return GetSeconds(m_rtc);
}


/// Protected Functions ---------------------------------------

uint32_t CRTC::GetSeconds(const CRTC::RTC &rtc)
{
    return ((3600 * (uint32_t)rtc.hour) + (60 * (uint32_t)rtc.minute) + (uint32_t)rtc.second);
}


// Valid from 00-03-01 to 99-02-28
// Input: y = 00-99, m = 1-12, d = 1-31
// Output: Sunday = 1, Saturday = 7
uint8_t CRTC::DayOfWeek(uint16_t y, const uint8_t m, const uint8_t d)
{
    static const uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y += 2000 - (m < 3);
    return 1 + ((y + y/4 - 15 + t[m-1] + d) % 7);
}


uint8_t CRTC::DEC_to_BCD(const uint8_t d)
{
    return (d + (6 * (d / 10)));
}


uint8_t CRTC::BCD_to_DEC(const uint8_t b)
{
    return (b - (6 * (b >> 4)));
}


uint8_t CRTC::GetSRAMSize(void)
{
    return 0;
}


uint8_t CRTC::FitSRAMRange(const uint8_t offset, const uint8_t bytes)
{
    uint8_t length;

    if (offset > GetSRAMSize())
    {
        return 0;
    }

    if ((offset + bytes) > GetSRAMSize())
    {
        length = (GetSRAMSize() - offset);
    }
    else
    {
        length = bytes;
    }

    return length;
}


CRTC::status_t CRTC::I2CWrite(const uint8_t address, const uint8_t data[], const uint8_t bytes)
{
    return (nI2C->Write(m_i2c_handle, address, data, bytes) == 0) ? STATUS_OK : STATUS_ERROR;
}


CRTC::status_t CRTC::I2CWriteByte(const uint8_t address, const uint8_t data)
{
    uint8_t data_array[] = {data};
    return I2CWrite(address, data_array, 1);
}


CRTC::status_t CRTC::I2CRead(const uint8_t address, uint8_t data[], const uint8_t bytes)
{
    return (nI2C->Read(m_i2c_handle, address, data, bytes) == 0) ? STATUS_OK : STATUS_ERROR;
}


uint8_t CRTC::I2CReadByte(const uint8_t address)
{
    uint8_t data[1] = {0};

    I2CRead(address, data, 1);
    return data[0];
}
