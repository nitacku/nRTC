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
 * @file        DS1307.h
 * @summary     Real Time Clock interface for DS1307
 * @version     1.2
 * @author      nitacku
 * @data        15 July 2018
 */

#ifndef _DS1307_H_
#define _DS1307_H_

#include "nRTC.h"
#define UNUSED(x) (void)(x)

class CDS1307 : public CRTC
{
    private:
    enum I2C : uint8_t
    {
        ADDRESS_I2C         = 0x68,
    };
    
    enum address_t : uint8_t
    {
        ADDRESS_TIME        = 0x00,
        ADDRESS_DAY         = 0x03,
        ADDRESS_DATE        = 0x04,
        ADDRESS_ALARM       = 0x08,
        ADDRESS_SRAM        = 0x0B, // Reserve 0x8-0xA for Alarm
    };
    
    enum bitmask_t : uint8_t
    {
        BITMASK_CLOCK_HALT  = 0x80,
    };
    
    enum sram_t : uint8_t
    {
        SRAM_SIZE           = (0x3F - ADDRESS_SRAM),
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

    CRTC::status_t GetSRAM(const uint8_t offset, uint8_t data[], const uint8_t bytes);
    CRTC::status_t SetSRAM(const uint8_t offset, const uint8_t data[], const uint8_t bytes);
    
    private:
    uint8_t GetSRAMSize(void);
    uint8_t GetI2CAddress(void);
};

#endif
