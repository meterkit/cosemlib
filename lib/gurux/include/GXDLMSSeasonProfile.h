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

#ifndef GXDLMSSEASONPROFILE_H
#define GXDLMSSEASONPROFILE_H

#include "GXDateTime.h"

class CGXDLMSSeasonProfile
{
    std::string m_Name;
    CGXDateTime m_Start;
    std::string m_WeekName;

public:
    /**
     Constructor.
    */
    CGXDLMSSeasonProfile();

    /**
     Constructor.
    */
    CGXDLMSSeasonProfile(std::string name, CGXDateTime start, std::string weekName);

    /**
     Name of season profile.
    */
    std::string GetName();
    void SetName(std::string value);

    /**
     Season Profile start time.
    */
    CGXDateTime& GetStart();
    void SetStart(CGXDateTime value);

    /**
     Week name of season profile.
    */
    std::string GetWeekName();
    void SetWeekName(std::string value);

    std::string ToString();
};

#endif //GXDLMSSEASONPROFILE_H
