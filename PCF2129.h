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
 * @file        PCF2129x.h
 * @summary     Real Time Clock interface for PCF2129
 * @version     1.1
 * @author      nitacku
 * @data        14 July 2018
 */

#ifndef _PCF2129_H_
#define _PCF2129_H_

#include "nRTC.h"

class CPCF2129 : public CRTC
{
    public:
    
    protected:
    enum I2C : uint8_t
    {
        ADDRESS_I2C = 0x51,
    };
    
    enum address_t : uint8_t
    {
        ADDRESS_CONTROL_1       = 0x00,
        ADDRESS_CONTROL_2       = 0x01,
        ADDRESS_CONTROL_3       = 0x02,
        ADDRESS_TIME            = 0x03,
        ADDRESS_DATE            = 0x06,
        ADDRESS_ALARM           = 0x0A,
        ADDRESS_CLOCKOUT        = 0x0F,
        ADDRESS_TIMESTAMP       = 0x12,
    };
    
    enum bitmask_t : uint8_t
    {   
        BITMASK_OSF             = 0x80,
        BITMASK_POR_OVRD        = 0x08,
        BITMASK_ALARM_TOGGLE    = 0x80,
        BITMASK_ALARM_FLAG      = 0x10,
        BITMASK_OTP_REFRESH     = 0x20,
        BITMASK_CLOCK_OUT_F     = 0x07,
        BITMASK_POWER_MNG       = 0xE0,
        BITMASK_TSOFF           = 0x40,
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
    
    protected:
    uint8_t GetI2CAddress(void);
};

#endif
