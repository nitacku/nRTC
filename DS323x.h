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
 * @file        DS323x.h
 * @summary     Real Time Clock interface for DS3231 & DS3232
 * @version     1.2
 * @author      nitacku
 * @data        15 July 2018
 */

#ifndef _DS323X_H_
#define _DS323X_H_

#include "nRTC.h"

class CDS3231 : public CRTC
{
    public:
    enum class Frequency : uint8_t
    {
        F1HZ,
        F1KHZ,
        F4KHZ,  
        F8KHZ,
    };
    
    protected:
    enum I2C : uint8_t
    {
        ADDRESS_I2C             = 0x68,
    };
        
    enum address_t : uint8_t
    {
        ADDRESS_TIME            = 0x00,
        ADDRESS_DAY             = 0x03,
        ADDRESS_DATE            = 0x04,
        ADDRESS_ALARM           = 0x07,
        ADDRESS_CTRL            = 0x0E,
        ADDRESS_STATUS          = 0x0F,
        ADDRESS_TEMPERATURE     = 0x11,
    };
    
    enum bitmask_t : uint8_t
    {   
        BITMASK_32KHZ_OUTPUT    = 0x48,
        BITMASK_CLOCK_HALT      = 0x80,
        BITMASK_ALARM_TOGGLE    = 0x80,
        BITMASK_ALARM_FLAG      = 0x01,
        BITMASK_SQUARE_WAVE     = 0x04,
        BITMASK_FREQUENCY       = 0x18,
    };

    public:
    
    void Initialize(void);
    void GetRTC(CRTC::RTC &rtc);
    CRTC::status_t SetRTC(const CRTC::RTC &rtc);
    
    CRTC::status_t AlarmReset(void);
    CRTC::status_t SetAlarmRTC(const CRTC::RTC &rtc);
    CRTC::status_t SetAlarmState(const CRTC::State state);
    void GetAlarmRTC(CRTC::RTC &rtc);
    CRTC::State GetAlarmState(void);
    bool IsAlarmTriggered(void);
    
    float GetTemperature(void);
    CRTC::status_t SetSquareWave(const bool state, const uint8_t frequency);
    void GetSquareWave(bool &state, uint8_t &frequency);
    
    protected:
    uint8_t GetI2CAddress(void);
};


class CDS3232 : public CDS3231
{
    protected:
    enum address_t : uint8_t
    {
        ADDRESS_SRAM    = 0x14,
    };
    
    enum sram_t : uint8_t
    {
        SRAM_SIZE       = (0xFF - ADDRESS_SRAM),
    };
    
    public:
    
    CRTC::status_t GetSRAM(const uint8_t offset, uint8_t data[], const uint8_t bytes);
    CRTC::status_t SetSRAM(const uint8_t offset, const uint8_t data[], const uint8_t bytes);
    
    protected:
    uint8_t GetSRAMSize(void);
};

#endif
