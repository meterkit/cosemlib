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

#ifndef GXDLMSAUTOCONNECT_H
#define GXDLMSAUTOCONNECT_H

#include "GXDLMSObject.h"
#include "GXDLMSAutoAnswer.h"

class CGXDLMSAutoConnect : public CGXDLMSObject
{
    AUTO_CONNECT_MODE m_Mode;
    std::vector<std::pair< CGXDateTime, CGXDateTime> > m_CallingWindow;
    std::vector< std::string > m_Destinations;
    int m_RepetitionDelay;
    int m_Repetitions;

    void Init();

public:
    //Constructor.
    CGXDLMSAutoConnect();

    //SN Constructor.
    CGXDLMSAutoConnect(unsigned short sn);

    //LN Constructor.
    CGXDLMSAutoConnect(std::string ln);

    AUTO_CONNECT_MODE GetMode();
    void SetMode(AUTO_CONNECT_MODE value);

    int GetRepetitions();

    void SetRepetitions(int value);

    int GetRepetitionDelay();
    void SetRepetitionDelay(int value);

    std::vector<std::pair< CGXDateTime, CGXDateTime> >& GetCallingWindow();
    void SetCallingWindow(std::vector<std::pair< CGXDateTime, CGXDateTime> > value);

    std::vector< std::string >& GetDestinations();

    void SetDestinations(std::vector< std::string >& value);

    // Returns amount of attributes.
    int GetAttributeCount();

    // Returns amount of methods.
    int GetMethodCount();

    //Get attribute values of object.
    void GetValues(std::vector<std::string>& values);

    void GetAttributeIndexToRead(std::vector<int>& attributes);

    int GetDataType(int index, DLMS_DATA_TYPE& type);

    // Returns value of given attribute.
    int GetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e);

    // Set value of given attribute.
    int SetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e);
};
#endif //GXDLMSAUTOCONNECT_H