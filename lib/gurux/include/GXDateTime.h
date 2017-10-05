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

#ifndef GXDATETIME_H
#define GXDATETIME_H

#include "enums.h"

// DataType enumerates skipped fields from date time.
enum DATETIME_SKIPS
{
    // Nothing is skipped from date time.
    DATETIME_SKIPS_NONE = 0x0,
    // Year part of date time is skipped.
    DATETIME_SKIPS_YEAR = 0x1,
    // Month part of date time is skipped.
    DATETIME_SKIPS_MONTH = 0x2,
    // Day part is skipped.
    DATETIME_SKIPS_DAY = 0x4,
    // Day of week part of date time is skipped.
    DATETIME_SKIPS_DAYOFWEEK = 0x8,
    // Hours part of date time is skipped.
    DATETIME_SKIPS_HOUR = 0x10,
    // Minute part of date time is skipped.
    DATETIME_SKIPS_MINUTE = 0x20,
    // Seconds part of date time is skipped.
    DATETIME_SKIPS_SECOND = 0x40,
    // Hundreds of seconds part of date time is skipped.
    DATETIME_SKIPS_MS = 0x80,
    //Devitation is skipped on write.
    DATETIME_SKIPS_DEVITATION = 0x100
};

// This class is used because in COSEM object model some fields from date time can be ignored.
// Default behavior of DateTime do not allow this.
class CGXDateTime
{
    friend class CGXTime;
    friend class CGXDate;
    short m_Deviation;
    DATETIME_SKIPS m_Skip;
    struct tm m_Value;
    bool m_DaylightSavingsBegin;
    bool m_DaylightSavingsEnd;
    DLMS_CLOCK_STATUS m_Status;
    void Init(int year, int month, int day, int hour, int minute, int second, int millisecond, int devitation);
public:
    // Constructor.
    CGXDateTime();
    // Constructor.
    CGXDateTime(struct tm value);

    // Constructor.
    CGXDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);

    // Constructor.
    CGXDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int devitation);

    // Used date time value.
    struct tm& GetValue();
    void SetValue(const struct tm& value);

    // Skip selected date time fields.
    DATETIME_SKIPS GetSkip();
    void SetSkip(DATETIME_SKIPS value);
    std::string ToString();

    //Get currect time.
    static CGXDateTime Now();

    // Daylight savings begin.
    bool GetDaylightSavingsBegin();
    void SetDaylightSavingsBegin(bool value);

    // Daylight savings end.
    bool GetDaylightSavingsEnd();
    void SetDaylightSavingsEnd(bool value);

    // Get deviation.
    int GetDeviation();

    // Set deviation.
    void SetDeviation(int value);

    // Status of the clock.
    DLMS_CLOCK_STATUS GetStatus();
    void SetStatus(DLMS_CLOCK_STATUS value);

    /**
    * Set time to midnight.
    */
    void ResetTime();
    /**
    * Add hours to current time.
    */
    int AddHours(int hours);

    /**
    * Add minutes to current time.
    */
    int AddMinutes(int minutes);

    /**
    * Add seconds to current time.
    */
    int AddSeconds(int seconds);
    /**
    * Add days to current time.
    */
    int AddDays(int hours);

    /**
    * Compare current time to another time.
    */
    int CompareTo(CGXDateTime& antherDate);

    /**
    * Get amount of days in given month.
    */
    static unsigned char DaysInMonth(int year, short month);


    /**
    * Get difference between given time and run time in ms.
    *
    * @param start
    *            Start date time.
    * @param to
    *            Compared time.
    * @return Difference in milliseconds.
    */
    static long GetDifference(struct tm& start, CGXDateTime& to);

    //Convert value to local time.
    int ToLocalTime(struct tm& localTime);

};
#endif //GXDATETIME_H