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

#include "../include/GXDLMSMacAddressSetup.h"
#include "../include/GXDLMSClient.h"

/**
 Constructor.
*/
CGXDLMSMacAddressSetup::CGXDLMSMacAddressSetup() : CGXDLMSObject(DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP, "0.0.25.2.0.255")
{
}

/**
 Constructor.

 @param ln Logical Name of the object.
*/
CGXDLMSMacAddressSetup::CGXDLMSMacAddressSetup(std::string ln) : CGXDLMSObject(DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP, ln)
{
}

/**
 Constructor.

 @param ln Logical Name of the object.
 @param sn Short Name of the object.
*/
CGXDLMSMacAddressSetup::CGXDLMSMacAddressSetup(int sn) : CGXDLMSObject(DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP, sn)
{

}

/**
 Value of COSEM Data object.
*/
std::string CGXDLMSMacAddressSetup::GetMacAddress()
{
    return m_MacAddress;
}
void CGXDLMSMacAddressSetup::SetMacAddress(std::string value)
{
    m_MacAddress = value;
}

// Returns amount of attributes.
int CGXDLMSMacAddressSetup::GetAttributeCount()
{
    return 2;
}

// Returns amount of methods.
int CGXDLMSMacAddressSetup::GetMethodCount()
{
    return 0;
}

void CGXDLMSMacAddressSetup::GetValues(std::vector<std::string>& values)
{
    values.clear();
    std::string ln;
    GetLogicalName(ln);
    values.push_back(ln);
    values.push_back(m_MacAddress);
}

void CGXDLMSMacAddressSetup::GetAttributeIndexToRead(std::vector<int>& attributes)
{
    //LN is static and read only once.
    if (CGXDLMSObject::IsLogicalNameEmpty(m_LN))
    {
        attributes.push_back(1);
    }
    //MacAddress
    if (!IsRead(2))
    {
        attributes.push_back(2);
    }
}

int CGXDLMSMacAddressSetup::GetDataType(int index, DLMS_DATA_TYPE& type)
{
    if (index == 1)
    {
        type = DLMS_DATA_TYPE_OCTET_STRING;
    }
    else if (index == 2)
    {
        type = DLMS_DATA_TYPE_OCTET_STRING;
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}


// Returns value of given attribute.
int CGXDLMSMacAddressSetup::GetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
{
    if (e.GetIndex() == 1)
    {
        int ret;
        CGXDLMSVariant tmp;
        if ((ret = GetLogicalName(this, tmp)) != 0)
        {
            return ret;
        }
        e.SetValue(tmp);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 2)
    {
        e.SetByteArray(true);
        std::string add = GetMacAddress();
        GXHelpers::Replace(add, ":", ".");
        CGXByteBuffer data;
        CGXDLMSVariant tmp = add;
        int ret = GXHelpers::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, tmp);
        e.SetValue(data);
        return ret;
    }
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
}

// Set value of given attribute.
int CGXDLMSMacAddressSetup::SetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
{
    if (e.GetIndex() == 1)
    {
        return SetLogicalName(this, e.GetValue());
    }
    else if (e.GetIndex() == 2)
    {
        CGXDLMSVariant newValue;
        int ret = CGXDLMSClient::ChangeType(e.GetValue(), DLMS_DATA_TYPE_OCTET_STRING, newValue);
        if (ret != DLMS_ERROR_CODE_OK)
        {
            return ret;
        }
        std::string add = e.GetValue().ToString();
        GXHelpers::Replace(add, ".", ":");
        SetMacAddress(add);
        return DLMS_ERROR_CODE_OK;
    }
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
}
