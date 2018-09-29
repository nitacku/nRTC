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
 * @file        PCF2129x.cpp
 * @summary     Real Time Clock interface for PCF2129
 * @version     1.1
 * @author      nitacku
 * @data        14 July 2018
 */

#include "PCF2129.h"
#include <Arduino.h>

void CPCF2129::Initialize(void)
{
    uint8_t byte;
    
    CRTC::Initialize();                     // Setup i2c
    delay(250);                             // Wait for i2c
    CRTC::I2CReadByte(ADDRESS_TIME);        // Send stop bit
    byte = CRTC::I2CReadByte(ADDRESS_TIME); // Read OSF
    
    // Check if Oscillator Stop Flag is set
    if (byte & BITMASK_OSF)
    {
        //I2CWriteByte(ADDRESS_CONTROL_3, 0xA0);                    // Adjust power management
        CRTC::I2CWriteByte(ADDRESS_TIMESTAMP, BITMASK_TSOFF);       // Disable timestamp
        CRTC::I2CWriteByte(ADDRESS_CONTROL_1, 0x00);                // Clear Power-On-Reset Override
        CRTC::I2CWriteByte(ADDRESS_CLOCKOUT, BITMASK_CLOCK_OUT_F);  // Disable Clock-out & clear OTPR
        CRTC::SetTime(0, 0, 0);                                     // Set default time
        CRTC::SetDate(0, 1, 1);                                     // Set default date
        CRTC::SetAlarmTime(0, 0, 0);                                // Set default alarm
        delay(1750);                                                // Wait for oscillator to stabilize
        CRTC::I2CWriteByte(ADDRESS_CLOCKOUT, BITMASK_OTP_REFRESH | BITMASK_CLOCK_OUT_F); // Perform OTP refresh
        delay(100); // Wait for OTP refresh to complete
    }
}


void CPCF2129::GetRTC(CRTC::RTC &rtc)
{
    uint8_t data[7];

    if (CRTC::I2CRead(ADDRESS_TIME, data, 7) == CRTC::STATUS_OK)
    {
        // Clear clock halt bit from read data
        data[0] &= ~(BITMASK_OSF); // clear OSF bit
        
        m_rtc.second        = CRTC::BCD_to_DEC(data[0]);
        m_rtc.minute        = CRTC::BCD_to_DEC(data[1]);
        m_rtc.hour          = CRTC::BCD_to_DEC(data[2]);
        m_rtc.day           = CRTC::BCD_to_DEC(data[3]);
        m_rtc.week_day      = CRTC::BCD_to_DEC(data[4]) + 1; // week 0-6
        m_rtc.month         = CRTC::BCD_to_DEC(data[5]); // month 1-12
        m_rtc.year          = CRTC::BCD_to_DEC(data[6]); // year 0-99

        m_rtc.am            = (m_rtc.hour < 12);
        m_rtc.twelve_hour   = (m_rtc.hour % 12);
        m_rtc.twelve_hour  += (m_rtc.twelve_hour == 0) ? 12 : 0;
    }

    rtc = m_rtc;
}


CRTC::status_t CPCF2129::SetRTC(const CRTC::RTC &rtc)
{
    uint8_t data[7] =
    {
        CRTC::DEC_to_BCD(rtc.second),
        CRTC::DEC_to_BCD(rtc.minute),
        CRTC::DEC_to_BCD(rtc.hour),
        CRTC::DEC_to_BCD(rtc.day),
        CRTC::DEC_to_BCD(DayOfWeek(rtc.year, rtc.month, rtc.day) - 1),
        CRTC::DEC_to_BCD(rtc.month),
        CRTC::DEC_to_BCD(rtc.year)
    };

    return CRTC::I2CWrite(ADDRESS_TIME, data, 7);
}


CRTC::status_t CPCF2129::AlarmReset(void)
{
    // Clear alarm flag
    return CRTC::I2CWriteByte(ADDRESS_CONTROL_2, 0x00);
}


CRTC::status_t CPCF2129::SetAlarmRTC(const CRTC::RTC &rtc)
{
    uint8_t data[3] =
    {
        CRTC::DEC_to_BCD(rtc.second),
        CRTC::DEC_to_BCD(rtc.minute),
        CRTC::DEC_to_BCD(rtc.hour),
    };
    
    return CRTC::I2CWrite(ADDRESS_ALARM, data, 3);
}


CRTC::status_t CPCF2129::SetAlarmState(const CRTC::State state)
{
    uint8_t data[3];

    if (CRTC::I2CRead(ADDRESS_ALARM, data, 3) == CRTC::STATUS_OK)
    {
        data[0] ^= (-(state == CRTC::State::DISABLE) ^ data[0]) & (BITMASK_ALARM_TOGGLE);
        data[1] ^= (-(state == CRTC::State::DISABLE) ^ data[1]) & (BITMASK_ALARM_TOGGLE);
        data[2] ^= (-(state == CRTC::State::DISABLE) ^ data[2]) & (BITMASK_ALARM_TOGGLE);
        
        // Alarm when hour, minute and second match
        if (CRTC::I2CWrite(ADDRESS_ALARM, data, 3) == CRTC::STATUS_OK)
        {
            return AlarmReset();
        }
    }

    return CRTC::STATUS_ERROR;
}


void CPCF2129::GetAlarmRTC(CRTC::RTC &rtc)
{
    uint8_t data[3];

    if (CRTC::I2CRead(ADDRESS_ALARM, data, 3) == CRTC::STATUS_OK)
    {
        rtc.second  = CRTC::BCD_to_DEC(data[0] & ~BITMASK_ALARM_TOGGLE);
        rtc.minute  = CRTC::BCD_to_DEC(data[1] & ~BITMASK_ALARM_TOGGLE);
        rtc.hour    = CRTC::BCD_to_DEC(data[2] & ~BITMASK_ALARM_TOGGLE);
    }
}


CRTC::State CPCF2129::GetAlarmState(void)
{
    uint8_t b;

    if ((b = CRTC::I2CReadByte(ADDRESS_ALARM)))
    {
        return (b & BITMASK_ALARM_TOGGLE) ? CRTC::State::DISABLE : CRTC::State::ENABLE;
    }

    return CRTC::State::DISABLE;
}


bool CPCF2129::IsAlarmTriggered(void)
{
    uint8_t b;

    if ((b = CRTC::I2CReadByte(ADDRESS_CONTROL_2)))
    {
        return !!(b & BITMASK_ALARM_FLAG);
    }

    return false;
}


uint8_t CPCF2129::GetI2CAddress(void)
{
    return ADDRESS_I2C;
}
