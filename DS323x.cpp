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
 * @file        DS323x.cpp
 * @summary     Real Time Clock interface for DS3231 & DS3232
 * @version     1.2
 * @author      nitacku
 * @data        15 July 2018
 */

#include "DS323x.h"

void CDS3231::Initialize(void)
{
    CRTC::Initialize(); // Setup i2c
    uint8_t b = CRTC::I2CReadByte(ADDRESS_STATUS);
    b &= ~(BITMASK_32KHZ_OUTPUT); // disable 32KHZ output
    CRTC::I2CWriteByte(ADDRESS_STATUS, b);
}


void CDS3231::GetRTC(CRTC::RTC &rtc)
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


CRTC::status_t CDS3231::SetRTC(const CRTC::RTC &rtc)
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


CRTC::status_t CDS3231::AlarmReset(void)
{
    // Clear alarm flag
    uint8_t b = CRTC::I2CReadByte(ADDRESS_STATUS);
    b &= ~(BITMASK_ALARM_FLAG);

    return CRTC::I2CWriteByte(ADDRESS_STATUS, b);
}


CRTC::status_t CDS3231::SetAlarmRTC(const CRTC::RTC &rtc)
{
    uint8_t data[4] =
    {
        CRTC::DEC_to_BCD(rtc.second),
        CRTC::DEC_to_BCD(rtc.minute),
        CRTC::DEC_to_BCD(rtc.hour),
        BITMASK_ALARM_TOGGLE
    };

    if (CRTC::I2CWrite(ADDRESS_ALARM, data, 4) == CRTC::STATUS_OK)
    {
        return SetAlarmState(CRTC::State::ENABLE);
    }

    return CRTC::STATUS_ERROR;
}


CRTC::status_t CDS3231::SetAlarmState(const CRTC::State state)
{
    // Read alarm flag
    uint8_t b = CRTC::I2CReadByte(ADDRESS_CTRL);

    // Set bit
    b ^= (-(state == CRTC::State::ENABLE) ^ b) & (BITMASK_ALARM_FLAG);

    // Alarm when hour, minute and second match
    if (CRTC::I2CWriteByte(ADDRESS_CTRL, b) == CRTC::STATUS_OK)
    {
        return AlarmReset();
    }

    return CRTC::STATUS_ERROR;
}


void CDS3231::GetAlarmRTC(CRTC::RTC &rtc)
{
    uint8_t data[3];

    if (CRTC::I2CRead(ADDRESS_ALARM, data, 3) == CRTC::STATUS_OK)
    {
        rtc.second  = CRTC::BCD_to_DEC(data[0] & ~BITMASK_ALARM_TOGGLE);
        rtc.minute  = CRTC::BCD_to_DEC(data[1] & ~BITMASK_ALARM_TOGGLE);
        rtc.hour    = CRTC::BCD_to_DEC(data[2] & ~BITMASK_ALARM_TOGGLE);
    }
}


CRTC::State CDS3231::GetAlarmState(void)
{
    uint8_t b;

    if ((b = CRTC::I2CReadByte(ADDRESS_CTRL)))
    {
        return (b & BITMASK_ALARM_FLAG) ? CRTC::State::ENABLE : CRTC::State::DISABLE;
    }

    return CRTC::State::DISABLE;
}


bool CDS3231::IsAlarmTriggered(void)
{
    uint8_t b;

    if ((b = CRTC::I2CReadByte(ADDRESS_STATUS)))
    {
        return !!(b & BITMASK_ALARM_FLAG);
    }

    return false;
}


float CDS3231::GetTemperature(void)
{
    uint16_t msb, lsb;

    msb = CRTC::I2CReadByte(ADDRESS_TEMPERATURE);
    lsb = CRTC::I2CReadByte(ADDRESS_TEMPERATURE + 1);

    return (((msb << 8) | lsb) >> 6) / 4.0f;
}


CRTC::status_t CDS3231::SetSquareWave(const bool state, const uint8_t frequency)
{
    // Read control register
    uint8_t b = CRTC::I2CReadByte(ADDRESS_CTRL);

    if (state)
    {
        b &= ~(BITMASK_SQUARE_WAVE); // active low
    }
    else
    {
        b |= BITMASK_SQUARE_WAVE;
    }

    b &= ~(BITMASK_FREQUENCY); // clear frequency bits
    b |= (BITMASK_FREQUENCY & (frequency << 3)); // set frequency bits

    return CRTC::I2CWriteByte(ADDRESS_CTRL, b);
}


void CDS3231::GetSquareWave(bool &state, uint8_t &frequency)
{
    // Read control register
    uint8_t b = CRTC::I2CReadByte(ADDRESS_CTRL);

    state = ((b & BITMASK_SQUARE_WAVE) >> 2);
    frequency = ((b & BITMASK_FREQUENCY) >> 3);
}


CRTC::status_t CDS3232::GetSRAM(const uint8_t offset, uint8_t data[], const uint8_t bytes)
{
    return CRTC::I2CRead(ADDRESS_SRAM + offset, data, CRTC::FitSRAMRange(offset, bytes));
}


CRTC::status_t CDS3232::SetSRAM(const uint8_t offset, const uint8_t data[], const uint8_t bytes)
{
    return CRTC::I2CWrite(ADDRESS_SRAM + offset, data, CRTC::FitSRAMRange(offset, bytes));
}


uint8_t CDS3232::GetSRAMSize(void)
{
    return SRAM_SIZE;
}


uint8_t CDS3231::GetI2CAddress(void)
{
    return ADDRESS_I2C;
}
