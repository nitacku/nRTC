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
 * @file        DS1307.cpp
 * @summary     Real Time Clock interface for DS1307
 * @version     1.2
 * @author      nitacku
 * @data        15 July 2018
 */

#include "DS1307.h"

void CDS1307::Initialize(void)
{
    CRTC::Initialize(); // Setup i2c
}


void CDS1307::GetRTC(CRTC::RTC &rtc)
{
    uint8_t data[7];

    if (CRTC::I2CRead(ADDRESS_TIME, data, 7) == CRTC::STATUS_OK)
    {
        // Clear clock halt bit from read data
        data[0] &= ~(BITMASK_CLOCK_HALT); // clear bit
        
        m_rtc.second        = CRTC::BCD_to_DEC(data[0]);
        m_rtc.minute        = CRTC::BCD_to_DEC(data[1]);
        m_rtc.hour          = CRTC::BCD_to_DEC(data[2]);
        m_rtc.day           = CRTC::BCD_to_DEC(data[4]);
        m_rtc.month         = CRTC::BCD_to_DEC(data[5]); // month 1-12
        m_rtc.year          = CRTC::BCD_to_DEC(data[6]); // year 0-99
        m_rtc.week_day      = CRTC::BCD_to_DEC(data[3]); // week 1-7

        m_rtc.am            = (m_rtc.hour < 12);
        m_rtc.twelve_hour   = (m_rtc.hour % 12);
        m_rtc.twelve_hour  += (m_rtc.twelve_hour == 0) ? 12 : 0;
    }

    rtc = m_rtc;
}


CRTC::status_t CDS1307::SetRTC(const CRTC::RTC &rtc)
{
    uint8_t data[7] =
    {
        CRTC::DEC_to_BCD(rtc.second),
        CRTC::DEC_to_BCD(rtc.minute),
        CRTC::DEC_to_BCD(rtc.hour),
        CRTC::DEC_to_BCD(DayOfWeek(rtc.year, rtc.month, rtc.day)),
        CRTC::DEC_to_BCD(rtc.day),
        CRTC::DEC_to_BCD(rtc.month),
        CRTC::DEC_to_BCD(rtc.year)
    };

    return CRTC::I2CWrite(ADDRESS_TIME, data, 7);
}


CRTC::status_t CDS1307::AlarmReset(void)
{
    uint8_t data[3] =
    {
        (uint8_t)0,
        (uint8_t)0,
        (uint8_t)0
    };

    return CRTC::I2CWrite(ADDRESS_ALARM, data, 3);
}


CRTC::status_t CDS1307::SetAlarmRTC(const CRTC::RTC &rtc)
{
    uint8_t data[3] =
    {
        rtc.second,
        rtc.minute,
        rtc.hour
    };

    if (CRTC::I2CWrite(ADDRESS_ALARM, data, 3) == CRTC::STATUS_OK)
    {
        return SetAlarmState(CRTC::State::ENABLE);
    }

    return CRTC::STATUS_ERROR;
}


CRTC::status_t CDS1307::SetAlarmState(const CRTC::State state)
{
    UNUSED(state);
    return CRTC::STATUS_ERROR;
}


void CDS1307::GetAlarmRTC(CRTC::RTC &rtc)
{
    uint8_t data[3];

    if (CRTC::I2CRead(ADDRESS_ALARM, data, 3) == CRTC::STATUS_OK)
    {
        rtc.second = data[0];
        rtc.minute = data[1];
        rtc.hour = data[2];
    }
}


CRTC::State CDS1307::GetAlarmState(void)
{
    return CRTC::State::ENABLE;
}


bool CDS1307::IsAlarmTriggered(void)
{
    return (CRTC::GetAlarmSeconds() == CRTC::GetTimeSeconds());
}


CRTC::status_t CDS1307::GetSRAM(const uint8_t offset, uint8_t data[], const uint8_t bytes)
{
    return CRTC::I2CRead(ADDRESS_SRAM + offset, data, CRTC::FitSRAMRange(offset, bytes));
}


CRTC::status_t CDS1307::SetSRAM(const uint8_t offset, const uint8_t data[], const uint8_t bytes)
{
    return CRTC::I2CWrite(ADDRESS_SRAM + offset, data, CRTC::FitSRAMRange(offset, bytes));
}


uint8_t CDS1307::GetSRAMSize(void)
{
    return SRAM_SIZE;
}


uint8_t CDS1307::GetI2CAddress(void)
{
    return ADDRESS_I2C;
}
