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
#ifndef GXDLMSREGISTER_H
#define GXDLMSREGISTER_H

#include "GXDLMSObject.h"

class CGXDLMSRegister : public CGXDLMSObject
{
    void Init();
protected:
    CGXDLMSVariant m_Value;
    signed char m_Scaler;
    unsigned char m_Unit;

    //SN Constructor.
    CGXDLMSRegister(DLMS_OBJECT_TYPE type, unsigned short sn);

    //LN Constructor.
    CGXDLMSRegister(DLMS_OBJECT_TYPE type, std::string ln);

    bool IsRead(int index);

public:
    //Constructor.
    CGXDLMSRegister(void);

    //SN Constructor.
    CGXDLMSRegister(unsigned short sn);

    //SN Constructor.
    CGXDLMSRegister(unsigned short sn, double scaler, int unit, CGXDLMSVariant value);

    //LN Constructor.
    CGXDLMSRegister(std::string ln);

    //LN Constructor.
    CGXDLMSRegister(std::string ln, double scaler, int unit, CGXDLMSVariant value);

    /// <summary>
    /// Get value of COSEM Data object.
    /// </summary>
    CGXDLMSVariant GetValue();

    /// <summary>
    /// Set value of COSEM Data object.
    /// </summary>
    void SetValue(CGXDLMSVariant& value);

    // Scaler of COSEM Register object.
    double GetScaler();

    void SetScaler(double value);

    // Unit of COSEM Register object.
    int GetUnit();

    void SetUnit(unsigned char value);

    /*
     * Reset value.
     */
    void Reset();

    // Returns amount of attributes.
    int GetAttributeCount();

    // Returns amount of methods.
    int GetMethodCount();

    int Invoke(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e);

    //Get attribute values of object.
    void GetValues(std::vector<std::string>& values);

    void GetAttributeIndexToRead(std::vector<int>& attributes);

    int GetDataType(int index, DLMS_DATA_TYPE& type);

    int GetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e);

    int SetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e);
};
#endif //GXDLMSREGISTER_H