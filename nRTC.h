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
 * @file        nRTC.h
 * @summary     Generic Real Time Clock interface
 * @version     3.3
 * @author      nitacku
 * @data        15 July 2018
 */

#ifndef _RTC_H_
#define _RTC_H_

#include <avr/interrupt.h>
#include <inttypes.h>
#include <nI2C.h>

class CRTC
{
    public:
    
    enum status_t : uint8_t
    {
        STATUS_OK = 0,
        STATUS_ERROR,
    };
    
    enum class State : uint8_t
    {
        DISABLE,
        ENABLE,
    };
    
    enum class Unit : uint8_t
    {
        C,
        F,
        K,  
    };
    
    struct RTC
    {
        RTC()
            : second{0}
            , minute{0}
            , hour{0}
            , day{0}
            , month{0}
            , year{0}
            , week_day{0}
            , am{0}
            , twelve_hour{0}
        {
            // empty
        }
        
        uint8_t second;
        uint8_t minute;
        uint8_t hour;
        uint8_t day;
        uint8_t month;
        uint8_t year;
        uint8_t week_day;
        bool am;
        uint8_t twelve_hour;
    };
    
    protected:
    RTC m_rtc;
    CI2C::Handle m_i2c_handle;
    
    public:
    // Default constructor
    CRTC(void);
    
    // Initialize the RTC
    virtual void Initialize(void);
    
    // RTC functions
    virtual void GetRTC(RTC &rtc) = 0;
    virtual status_t SetRTC(const RTC &rtc) = 0;
    
    // Time functions
    uint32_t GetTimeSeconds(void);
    void GetTime(uint8_t &hour, uint8_t &minute, uint8_t &second);
    status_t SetTime(const uint8_t hour, const uint8_t minute, const uint8_t second);
    
    // Date functions
    void GetDate(uint8_t &year, uint8_t &month, uint8_t &day);
    status_t SetDate(const uint8_t year, const uint8_t month, const uint8_t day);
    
    // Temperature functions
    float ConvertTemperature(const float temperature, const Unit input_unit, const Unit output_unit);
    
    // Alarm functions
    virtual status_t SetAlarmRTC(const RTC &rtc) = 0;
    virtual void GetAlarmRTC(RTC &rtc) = 0;
    virtual status_t SetAlarmState(const State state) = 0;
    virtual State GetAlarmState(void) = 0;
    virtual bool IsAlarmTriggered(void) = 0;
    virtual status_t AlarmReset(void) = 0;
    
    uint32_t GetAlarmSeconds(void);
    status_t SetAlarmTime(const uint8_t hour, const uint8_t minute, const uint8_t second);
    void GetAlarmTime(uint8_t &hour, uint8_t &minute, uint8_t &second);
    
    protected:
    virtual uint8_t GetSRAMSize(void);
    virtual uint8_t GetI2CAddress(void) = 0;
    
    uint32_t GetSeconds(const RTC &rtc);
    uint8_t DayOfWeek(uint16_t y, const uint8_t m, const uint8_t d);
    uint8_t DEC_to_BCD(const uint8_t d);
    uint8_t BCD_to_DEC(const uint8_t b);

    uint8_t FitSRAMRange(const uint8_t offset, const uint8_t bytes);
    status_t I2CWrite(const uint8_t address, const uint8_t data[], const uint8_t bytes);
    status_t I2CWriteByte(const uint8_t address, const uint8_t data);
    status_t I2CRead(const uint8_t address, uint8_t data[], const uint8_t bytes);
    uint8_t I2CReadByte(const uint8_t address);
};
    
#endif
