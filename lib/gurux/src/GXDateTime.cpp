//
// --------------------------------------------------------------------------
//  Gurux Ltd
//
//
//
// Filename:        $HeadURL$
//
// Version:         $Revision$,
//                  $Date$
//                  $Author$
//
// Copyright (c) Gurux Ltd
//
//---------------------------------------------------------------------------
//
//  DESCRIPTION
//
// This file is a part of Gurux Device Framework.
//
// Gurux Device Framework is Open Source software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; version 2 of the License.
// Gurux Device Framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// More information of Gurux products: http://www.gurux.org
//
// This code is licensed under the GNU General Public License v2.
// Full text may be retrieved at http://www.gnu.org/licenses/gpl-2.0.txt
//---------------------------------------------------------------------------

#include "../include/GXDateTime.h"
#include "../include/GXHelpers.h"

#include <string.h>
#include <locale>
#include <vector>
#include <assert.h>

//Get UTC offset in minutes.
void GetUtcOffset(int& hours, int& minutes)
{
    time_t zero = 24 * 60 * 60L;
    struct tm tm;

    // local time for Jan 2, 1900 00:00 UTC
#if _MSC_VER > 1000
    localtime_s(&tm, &zero);
#else
    tm = *localtime(&zero);
#endif
    hours = tm.tm_hour;

    //If the local time is the "day before" the UTC, subtract 24 hours from the hours to get the UTC offset
    if (tm.tm_mday < 2)
    {
        hours -= 24;
    }
    minutes = tm.tm_min;
}

static time_t GetUtcTime(struct tm * timeptr)
{
    /* gets the epoch time relative to the local time zone,
    and then adds the appropriate number of seconds to make it UTC */
    int hours, minutes;
    GetUtcOffset(hours, minutes);
    return mktime(timeptr) + (hours * 3600) + (minutes * 60);
}


// Constructor.
CGXDateTime::CGXDateTime()
{
    int hours, minutes;
    GetUtcOffset(hours, minutes);
    m_Deviation = -(hours * 60 + minutes);
    m_Skip = DATETIME_SKIPS_NONE;
    memset(&m_Value, 0xFF, sizeof(m_Value));
    m_DaylightSavingsBegin = m_DaylightSavingsEnd = false;
    m_Status = DLMS_CLOCK_STATUS_OK;
}

// Constructor.
CGXDateTime::CGXDateTime(struct tm value)
{
    int hours, minutes;
    GetUtcOffset(hours, minutes);
    m_Deviation = -(hours * 60 + minutes);
    m_Value = value;
    m_Skip = DATETIME_SKIPS_NONE;
    m_DaylightSavingsBegin = m_DaylightSavingsEnd = false;
    m_Status = DLMS_CLOCK_STATUS_OK;
}

CGXDateTime::CGXDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
{
    int hours, minutes;
    GetUtcOffset(hours, minutes);
    Init(year, month, day, hour, minute, second, millisecond, -(hours * 60 + minutes));
}

// Constructor.
CGXDateTime::CGXDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int devitation)
{
    Init(year, month, day, hour, minute, second, millisecond, devitation);
}

// Constructor.
void CGXDateTime::Init(int year, int month, int day, int hour, int minute, int second, int millisecond, int devitation)
{
    memset(&m_Value, 0, sizeof(m_Value));
    m_DaylightSavingsBegin = m_DaylightSavingsEnd = false;
    m_Status = DLMS_CLOCK_STATUS_OK;
    int skip = DATETIME_SKIPS_NONE;
    if (year < 1 || year == 0xFFFF)
    {
        skip |= DATETIME_SKIPS_YEAR;
        year = 1900 + Now().m_Value.tm_year;
    }
    if (month < 1 || month == 0xFF)
    {
        skip |= DATETIME_SKIPS_MONTH;
        month = 0;
    }
    else
    {
        --month;
    }
    if (day < 1 || day == 0xFF)
    {
        skip |= DATETIME_SKIPS_DAY;
        day = 1;
    }
    else if (day == 0xFD)
    {
        day = DaysInMonth(year, month) - 1;
    }
    else if (day == 0xFE)
    {
        day = DaysInMonth(year, month);
    }
    if (hour == -1 || hour == 0xFF)
    {
        skip |= DATETIME_SKIPS_HOUR;
        hour = 1;
    }
    if (minute == -1 || minute == 0xFF)
    {
        skip |= DATETIME_SKIPS_MINUTE;
        minute = 0;
    }
    if (second == -1 || second == 0xFF)
    {
        skip |= DATETIME_SKIPS_SECOND;
        second = 0;
    }
    if (millisecond < 1 || millisecond == 0xFF)
    {
        skip |= DATETIME_SKIPS_MS;
        millisecond = 0;
    }
    m_Skip = (DATETIME_SKIPS)skip;
    if (year != 0)
    {
        m_Value.tm_year = year - 1900;
    }
    m_Value.tm_mon = month;
    m_Value.tm_mday = day;
    m_Value.tm_hour = hour;
    m_Value.tm_min = minute;
    m_Value.tm_sec = second;
    m_Deviation = devitation;
}

// Used date time value.
struct tm& CGXDateTime::GetValue()
{
    return m_Value;
}

unsigned char CGXDateTime::DaysInMonth(int year, short month)
{
    if (month == 0 || month == 2 || month == 4 ||
        month == 6 || month == 7 || month == 9 || month == 11)
    {
        return 31;
    }
    else if (month == 3 || month == 5 || month == 8 || month == 10)
    {
        return 30;
    }
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0)
            {
                return 29;
            }
            return 28;
        }
        return 29;
    }
    return 28;
}

void CGXDateTime::SetValue(const struct tm& value)
{
    m_Value = value;
}

// Skip selected date time fields.
DATETIME_SKIPS CGXDateTime::GetSkip()
{
    return m_Skip;
}

void CGXDateTime::SetSkip(DATETIME_SKIPS value)
{
    m_Skip = value;
}

/**
 Daylight savings begin.
*/
bool CGXDateTime::GetDaylightSavingsBegin()
{
    return m_DaylightSavingsBegin;
}
void CGXDateTime::SetDaylightSavingsBegin(bool value)
{
    m_DaylightSavingsBegin = value;
}

/**
 Daylight savings end.
*/
bool CGXDateTime::GetDaylightSavingsEnd()
{
    return m_DaylightSavingsEnd;
}
void CGXDateTime::SetDaylightSavingsEnd(bool value)
{
    m_DaylightSavingsEnd = value;
}

int CGXDateTime::GetDeviation()
{
    return m_Deviation;
}

void CGXDateTime::SetDeviation(int value)
{
    m_Deviation = value;
}


//Constants for different orders of date components.
typedef enum
{
    GXDLMS_DATE_FORMAT_INVALID = -1,
    GXDLMS_DATE_FORMAT_DMY = 0,
    GXDLMS_DATE_FORMAT_MDY = 1,
    GXDLMS_DATE_FORMAT_YMD = 2,
    GXDLMS_DATE_FORMAT_YDM = 3
} GXDLMS_DATE_FORMAT;

int GetDateFormat(GXDLMS_DATE_FORMAT& format, char& separator)
{
    int ret = 0, value, lastPos = 0, pos;
    char buff[11];
    struct tm order = { 0 };
    order.tm_year = 0;
    order.tm_mday = 1;
    order.tm_mon = 1;//Month is zero based.
    ret = (int)strftime(buff, 11, "%x", &order);
    if (ret > 0)
    {
        for (pos = 0; pos != ret; ++pos)
        {
            //If numeric value
            if (buff[pos] >= '0' && buff[pos] <= '9')
            {

            }
            else //If date time separator.
            {
                separator = buff[pos];
#if _MSC_VER > 1000
                if (sscanf_s(buff + lastPos, "%d", &value) == 1)
#else
                if (sscanf(buff + lastPos, "%d", &value) == 1)
#endif
                {
                    if (value == 1)
                    {
                        format = lastPos == 0 ? GXDLMS_DATE_FORMAT_DMY : GXDLMS_DATE_FORMAT_YDM;
                        break;
                    }
                    else if (value == 2)
                    {
                        format = lastPos == 0 ? GXDLMS_DATE_FORMAT_MDY : GXDLMS_DATE_FORMAT_YMD;
                        break;
                    }
                }
                lastPos = pos + 1;
            }
        }
    }
    return ret;
}


std::string CGXDateTime::ToString()
{
    char buff[50];
    int ret;
    if (m_Skip != DATETIME_SKIPS_NONE)
    {
        CGXByteBuffer ba;
        GXDLMS_DATE_FORMAT format;
        char separator;
        //Add year, month and date if used.
        if ((m_Skip & (DATETIME_SKIPS_YEAR | DATETIME_SKIPS_MONTH | DATETIME_SKIPS_DAY)) != (DATETIME_SKIPS_YEAR | DATETIME_SKIPS_MONTH | DATETIME_SKIPS_DAY))
        {
            ret = GetDateFormat(format, separator);
            switch (format)
            {
            case GXDLMS_DATE_FORMAT_DMY:
            {
                if (m_Value.tm_mday != -1 && (m_Skip & DATETIME_SKIPS_DAY) == 0)
                {
                    ba.AddIntAsString(m_Value.tm_mday);
                }
                if (m_Value.tm_mon != -1 && (m_Skip & DATETIME_SKIPS_MONTH) == 0)
                {
                    if (ba.GetSize() != 0)
                    {
                        ba.SetUInt8(separator);
                    }
                    ba.AddIntAsString(1 + m_Value.tm_mon);
                }
                if (m_Value.tm_year != -1 && (m_Skip & DATETIME_SKIPS_YEAR) == 0)
                {
                    if (ba.GetSize() != 0)
                    {
                        ba.SetUInt8(separator);
                    }
                    ba.AddIntAsString(1900 + m_Value.tm_year);
                }
            }
            break;
            case GXDLMS_DATE_FORMAT_MDY:
            {
                if (m_Value.tm_mon != -1 && (m_Skip & DATETIME_SKIPS_MONTH) == 0)
                {
                    ba.AddIntAsString(1 + m_Value.tm_mon);
                }
                if (m_Value.tm_mday != -1 && (m_Skip & DATETIME_SKIPS_DAY) == 0)
                {
                    if (ba.GetSize() != 0)
                    {
                        ba.SetUInt8(separator);
                    }
                    ba.AddIntAsString(m_Value.tm_mday);
                }
                if (m_Value.tm_year != -1 && (m_Skip & DATETIME_SKIPS_YEAR) == 0)
                {
                    if (ba.GetSize() != 0)
                    {
                        ba.SetUInt8(separator);
                    }
                    ba.AddIntAsString(1900 + m_Value.tm_year);
                }
            }
            break;
            case GXDLMS_DATE_FORMAT_YMD:
            {
                if (m_Value.tm_year != -1 && (m_Skip & DATETIME_SKIPS_YEAR) == 0)
                {
                    ba.AddIntAsString(1900 + m_Value.tm_year);
                }
                if (m_Value.tm_mon != -1 && (m_Skip & DATETIME_SKIPS_MONTH) == 0)
                {
                    if (ba.GetSize() != 0)
                    {
                        ba.SetUInt8(separator);
                    }
                    ba.AddIntAsString(1 + m_Value.tm_mon);
                }
                if (m_Value.tm_mday != -1 && (m_Skip & DATETIME_SKIPS_DAY) == 0)
                {
                    if (ba.GetSize() != 0)
                    {
                        ba.SetUInt8(separator);
                    }
                    ba.AddIntAsString(m_Value.tm_mday);
                }
            }
            break;
            case GXDLMS_DATE_FORMAT_YDM:
            {
                if (m_Value.tm_year != -1 && (m_Skip & DATETIME_SKIPS_YEAR) == 0)
                {
                    ba.AddIntAsString(1900 + m_Value.tm_year);
                }
                if (m_Value.tm_mday != -1 && (m_Skip & DATETIME_SKIPS_DAY) == 0)
                {
                    if (ba.GetSize() != 0)
                    {
                        ba.SetUInt8(separator);
                    }
                    ba.AddIntAsString(m_Value.tm_mday);
                }
                if (m_Value.tm_mon != -1 && (m_Skip & DATETIME_SKIPS_MONTH) == 0)
                {
                    if (ba.GetSize() != 0)
                    {
                        ba.SetUInt8(separator);
                    }
                    ba.AddIntAsString(1 + m_Value.tm_mon);
                }
            }
            break;
            default:
            {
                ret = (int)strftime(buff, 50, "%X", &m_Value);
                ba.SetUInt8(' ');
                ba.Set(buff, ret);
                return 0;
            }
            }
        }

        //Add hours.
        if (m_Value.tm_hour != -1 && (m_Skip & DATETIME_SKIPS_HOUR) == 0)
        {
            if (ba.GetSize() != 0)
            {
                ba.SetUInt8(' ');
            }
#if _MSC_VER > 1000
            sprintf_s(buff, 50, "%.2d", m_Value.tm_hour);
#else
            sprintf(buff, "%.2d", m_Value.tm_hour);
#endif
            ba.AddString(buff);
        }
        //Add minutes.
        if (m_Value.tm_min != -1 && (m_Skip & DATETIME_SKIPS_MINUTE) == 0)
        {
            if (ba.GetSize() != 0)
            {
                ba.SetUInt8(':');
            }
#if _MSC_VER > 1000
            sprintf_s(buff, 50, "%.2d", m_Value.tm_min);
#else
            sprintf(buff, "%.2d", m_Value.tm_min);
#endif
            ba.AddString(buff);
        }
        //Add seconds.
        if (m_Value.tm_sec != -1 && (m_Skip & DATETIME_SKIPS_SECOND) == 0)
        {
            if (ba.GetSize() != 0)
            {
                ba.SetUInt8(':');
            }
#if _MSC_VER > 1000
            sprintf_s(buff, 50, "%.2d", m_Value.tm_sec);
#else
            sprintf(buff, "%.2d", m_Value.tm_sec);
#endif
            ba.AddString(buff);
        }
        return ba.ToString();
    }
    //If value is not set return empty std::string.
    if (m_Value.tm_year == -1)
    {
        return "";
    }
    std::string str;
    ret = (int)strftime(buff, 50, "%x %X", &m_Value);
    str.append(buff, ret);
    return str;
}

CGXDateTime CGXDateTime::Now()
{
    time_t tm1 = time(NULL);
#if _MSC_VER > 1000
    struct tm dt;
    localtime_s(&dt, &tm1);
#else
    struct tm dt = *localtime(&tm1);
#endif
    CGXDateTime now(dt);
    return now;
}

// Status of the clock.
DLMS_CLOCK_STATUS CGXDateTime::GetStatus()
{
    return m_Status;
}

void CGXDateTime::SetStatus(DLMS_CLOCK_STATUS value)
{
    m_Status = value;
}

void CGXDateTime::ResetTime()
{
    m_Value.tm_hour = m_Value.tm_min = m_Value.tm_sec = 0;
}

int CGXDateTime::AddDays(int days)
{
    m_Value.tm_mday += days;
    if ((int)mktime(&m_Value) == -1)
    {
        return DLMS_ERROR_CODE_INVALID_DATE_TIME;
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDateTime::AddHours(int hours)
{
    m_Value.tm_hour += hours;
    if ((int)mktime(&m_Value) == -1)
    {
        return DLMS_ERROR_CODE_INVALID_DATE_TIME;
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDateTime::AddMinutes(int minutes)
{
    m_Value.tm_min += minutes;
    if ((int)mktime(&m_Value) == -1)
    {
        return DLMS_ERROR_CODE_INVALID_DATE_TIME;
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDateTime::AddSeconds(int seconds)
{
    m_Value.tm_sec += seconds;
    if ((int)mktime(&m_Value) == -1)
    {
        return DLMS_ERROR_CODE_INVALID_DATE_TIME;
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDateTime::CompareTo(CGXDateTime& antherDate)
{
    time_t time1 = mktime(&m_Value);
    time_t time2 = mktime(&antherDate.GetValue());
    if (time1 < time2)
    {
        return -1;
    }
    if (time1 > time2)
    {
        return 1;
    }
    return 0;
}

int CGXDateTime::ToLocalTime(struct tm& localTime)
{
    localTime = m_Value;
    if (m_Deviation != -32768)//0x8000
    {
        localTime.tm_min += m_Deviation;
        time_t t = GetUtcTime(&localTime);
        if (t == -1)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
#if _MSC_VER > 1000
        localtime_s(&localTime, &t);
#else
        localTime = *localtime(&t);
#endif
    }
    return 0;
}

long CGXDateTime::GetDifference(struct tm& start, CGXDateTime& to)
{
    long diff = 0;
    // Compare seconds.
    if ((to.GetSkip() & DATETIME_SKIPS_SECOND) != 0)
    {
        if (start.tm_sec < to.m_Value.tm_sec)
        {
            diff += (to.m_Value.tm_sec - start.tm_sec) * 1000L;
        }
        else
        {
            diff -= (start.tm_sec - to.m_Value.tm_sec) * 1000L;
        }
    }
    else if (diff < 0)
    {
        diff = 60000 + diff;
    }
    // Compare minutes.
    if ((to.GetSkip() & DATETIME_SKIPS_MINUTE) != 0)
    {
        if (start.tm_min < to.m_Value.tm_min)
        {
            diff += (to.m_Value.tm_min - start.tm_min) * 60000L;
        }
        else
        {
            diff -= (start.tm_min - to.m_Value.tm_min) * 60000L;
        }
    }
    else if (diff < 0)
    {
        diff = 60 * 60000 + diff;
    }
    // Compare hours.
    if ((to.GetSkip() & DATETIME_SKIPS_HOUR) != 0)
    {
        if (start.tm_hour < to.m_Value.tm_hour)
        {
            diff += (to.m_Value.tm_hour - start.tm_hour) * 60 * 60000L;
        }
        else
        {
            diff -= (start.tm_hour - to.m_Value.tm_hour) * 60 * 60000L;
        }
    }
    else if (diff < 0)
    {
        diff = 60 * 60000 + diff;
    }
    // Compare days.
    if ((to.GetSkip() & DATETIME_SKIPS_DAY) != 0)
    {
        if (start.tm_mday < to.m_Value.tm_mday)
        {
            diff += (to.m_Value.tm_mday - start.tm_mday) * 24 * 60 * 60000;
        }
        else if (start.tm_mday != to.m_Value.tm_mday)
        {
            if ((to.GetSkip() & DATETIME_SKIPS_DAY) == 0)
            {
                diff += (to.m_Value.tm_mday - start.tm_mday) * 24 * 60 * 60000L;
            }
            else {
                diff = ((DaysInMonth(start.tm_year,
                    start.tm_mon)
                    - start.tm_mday
                    + to.m_Value.tm_mday) * 24
                    * 60 * 60000L) + diff;
            }
        }
    }
    else if (diff < 0)
    {
        diff = 24 * 60 * 60000 + diff;
    }
    // Compare months.
    if ((to.GetSkip() & DATETIME_SKIPS_MONTH) != 0)
    {
        if (start.tm_mon < to.m_Value.tm_mon)
        {
            for (int m = start.tm_mon; m != to.m_Value.tm_mon; ++m)
            {
                diff += DaysInMonth(start.tm_year, m) * 24 * 60 * 60000L;
            }
        }
        else
        {
            for (int m = to.m_Value.tm_mon; m != start.tm_mon; ++m)
            {
                diff += -DaysInMonth(start.tm_year, m) * 24 * 60 * 60000L;
            }
        }
    }
    else if (diff < 0)
    {
        diff = DaysInMonth(start.tm_year, start.tm_mon) * 24 * 60 * 60000L + diff;
    }
    return diff;
}
