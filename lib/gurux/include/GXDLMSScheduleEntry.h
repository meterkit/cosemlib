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

#ifndef GXDLMSSCHEDULEENTRY_H
#define GXDLMSSCHEDULEENTRY_H

#include "GXDLMSObject.h"

/**
* Executed scripts.
*/
class CGXDLMSScheduleEntry
{
private:
    /**
    * Schedule entry index.
    */
    unsigned char m_Index;

    /**
     * Is Schedule entry enabled.
     */
    bool m_Enable;

    /**
     * Logical name of the Script table object.
     */
    std::string m_LogicalName;

    /**
     * Script identifier of the script to be executed.
     */
    unsigned char m_ScriptSelector;

    /**
    *
    */
    CGXDateTime m_SwitchTime;

    /**
     * Defines a period in minutes, in which an entry shall be processed after
     * power fail.
     */
    unsigned char m_ValidityWindow;

    /**
     * Days of the week on which the entry is valid.
     */
    std::string m_ExecWeekdays;

    /**
     * Perform the link to the IC “Special days table”, day_id.
     */
    std::string m_ExecSpecDays;

    /**
     * Date starting period in which the entry is valid.
     */
    CGXDateTime m_BeginDate;

    /**
     * Date starting period in which the entry is valid.
     */
    CGXDateTime m_EndDate;

public:
    /**
     * Get schedule entry index.
     *
     * @return Entry index.
     */
    unsigned char GetIndex();

    /**
     * Set schedule entry index.
     *
     * @param value
     *            Entry index.
     */
    void SetIndex(unsigned char value);

    /**
     * Is Schedule entry enabled.
     *
     * @return True, if schedule entry is enabled.
     */
    bool GetEnable();

    /**
     * Enable schedule entry.
     *
     * @param value
     *            Is Schedule entry enabled.
     */
    void SetEnable(bool value);

    /**
     * Returns logical name of the Script table object.
     *
     * @return Logical name of the Script table object.
     */
    std::string GetLogicalName();

    void SetLogicalName(std::string value);

    /**
     * Get script identifier of the script to be executed.
     *
     * @return Script identifier.
     */
    unsigned char GetScriptSelector();

    /**
     * Set script identifier of the script to be executed.
     *
     * @param value
     *            Script identifier.
     */
    void SetScriptSelector(unsigned char value);

    /**
     * @return
     */
    CGXDateTime GetSwitchTime();

    /**
     * @param value
     */
    void SetSwitchTime(CGXDateTime value);

    /**
     * Defines a period in minutes, in which an entry shall be processed after
     * power fail.
     *
     * @return Validity period in minutes.
     */
    unsigned char GetValidityWindow();

    /**
     * Defines a period in minutes, in which an entry shall be processed after
     * power fail.
     *
     * @param value
     *            Validity period in minutes.
     */
    void SetValidityWindow(unsigned char value);

    /**
     * Get days of the week on which the entry is valid.
     *
     * @return Bit array of valid week days.
     */
    std::string GetExecWeekdays();

    /**
     * Set days of the week on which the entry is valid.
     *
     * @param value
     *            Bit array of valid week days.
     */
    void SetExecWeekdays(std::string value);

    /**
     * Perform the link to the IC Special days table.
     *
     * @return day_id.
     */
    std::string GetExecSpecDays();

    /**
     * Perform the link to the IC Special days table.
     *
     * @param value
     *            day_id
     */
    void SetExecSpecDays(std::string value);

    /**
     * Date starting period in which the entry is valid.
     *
     * @return Begin date.
     */
    CGXDateTime GetBeginDate();

    /**
     * Date starting period in which the entry is valid.
     *
     * @param value
     *            Begin date.
     */
    void SetBeginDate(CGXDateTime value);

    /**
     * Get date starting period in which the entry is valid.
     *
     * @return End date.
     */
    CGXDateTime GetEndDate();

    /**
     * Set date starting period in which the entry is valid.
     *
     * @param value
     *            End date.
     */
    void SetEndDate(CGXDateTime value);
};

#endif //GXDLMSSCHEDULEENTRY_H