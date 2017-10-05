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

#include "../include/GXDLMSIp4Setup.h"
#include "../include/GXDLMSClient.h"
#include <sstream>

#if defined(_WIN32) || defined(_WIN64)//Windows includes
#include <Winsock.h> //Add support for sockets
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif


//Constructor.
CGXDLMSIp4Setup::CGXDLMSIp4Setup() : CGXDLMSObject(DLMS_OBJECT_TYPE_IP4_SETUP)
{
    m_IPAddress.empty();
    m_SubnetMask = 0;
    m_GatewayIPAddress = 0;
    m_UseDHCP = false;
    m_PrimaryDNSAddress = 0;
    m_SecondaryDNSAddress = 0;
}

//SN Constructor.
CGXDLMSIp4Setup::CGXDLMSIp4Setup(unsigned short sn) : CGXDLMSObject(DLMS_OBJECT_TYPE_IP4_SETUP, sn)
{
    m_IPAddress.empty();
    m_SubnetMask = 0;
    m_GatewayIPAddress = 0;
    m_UseDHCP = false;
    m_PrimaryDNSAddress = 0;
    m_SecondaryDNSAddress = 0;
}

//LN Constructor.
CGXDLMSIp4Setup::CGXDLMSIp4Setup(std::string ln) : CGXDLMSObject(DLMS_OBJECT_TYPE_IP4_SETUP, ln)
{
    m_IPAddress.empty();
    m_SubnetMask = 0;
    m_GatewayIPAddress = 0;
    m_UseDHCP = false;
    m_PrimaryDNSAddress = 0;
    m_SecondaryDNSAddress = 0;
}

std::string& CGXDLMSIp4Setup::GetDataLinkLayerReference()
{
    return m_DataLinkLayerReference;
}
void CGXDLMSIp4Setup::SetDataLinkLayerReference(std::string value)
{
    m_DataLinkLayerReference = value;
}

std::string& CGXDLMSIp4Setup::GetIPAddress()
{
    return m_IPAddress;
}

void CGXDLMSIp4Setup::SetIPAddress(std::string& value)
{
    m_IPAddress = value;
}

std::vector<unsigned long>& CGXDLMSIp4Setup::GetMulticastIPAddress()
{
    return m_MulticastIPAddress;
}

std::vector<CGXDLMSIp4SetupIpOption>& CGXDLMSIp4Setup::GetIPOptions()
{
    return m_IPOptions;
}
void CGXDLMSIp4Setup::SetIPOptions(std::vector<CGXDLMSIp4SetupIpOption>& value)
{
    m_IPOptions.clear();
    for (std::vector<CGXDLMSIp4SetupIpOption>::iterator it = value.begin(); it != value.end(); ++it)
    {
        m_IPOptions.push_back(*it);
    }
}

unsigned long CGXDLMSIp4Setup::GetSubnetMask()
{
    return m_SubnetMask;
}

void CGXDLMSIp4Setup::SetSubnetMask(unsigned long value)
{
    m_SubnetMask = value;
}

unsigned long CGXDLMSIp4Setup::GetGatewayIPAddress()
{
    return m_GatewayIPAddress;
}
void CGXDLMSIp4Setup::SetGatewayIPAddress(unsigned long value)
{
    m_GatewayIPAddress = value;
}

bool CGXDLMSIp4Setup::GetUseDHCP()
{
    return m_UseDHCP;
}
void CGXDLMSIp4Setup::SetUseDHCP(bool value)
{
    m_UseDHCP = value;
}

unsigned long CGXDLMSIp4Setup::GetPrimaryDNSAddress()
{
    return m_PrimaryDNSAddress;
}
void CGXDLMSIp4Setup::SetPrimaryDNSAddress(unsigned long value)
{
    m_PrimaryDNSAddress = value;
}

unsigned long CGXDLMSIp4Setup::GetSecondaryDNSAddress()
{
    return m_SecondaryDNSAddress;
}

void CGXDLMSIp4Setup::SetSecondaryDNSAddress(unsigned long value)
{
    m_SecondaryDNSAddress = value;
}

// Returns amount of attributes.
int CGXDLMSIp4Setup::GetAttributeCount()
{
    return 10;
}

// Returns amount of methods.
int CGXDLMSIp4Setup::GetMethodCount()
{
    return 3;
}

void CGXDLMSIp4Setup::GetValues(std::vector<std::string>& values)
{
    values.clear();
    std::string ln;
    GetLogicalName(ln);
    values.push_back(ln);
    //CGXDLMSVariant().ToString()
    values.push_back(m_DataLinkLayerReference);
    values.push_back(m_IPAddress);
    std::stringstream sb;
    sb << '[';
    bool empty = true;
    for (std::vector<unsigned long>::iterator it = m_MulticastIPAddress.begin(); it != m_MulticastIPAddress.end(); ++it)
    {
        if (!empty)
        {
            sb << ", ";
        }
        empty = false;
        std::string str = CGXDLMSVariant(*it).ToString();
        sb.write(str.c_str(), str.size());
    }
    sb << ']';
    values.push_back(sb.str());

    //Clear str.
    sb.str(std::string());
    sb << '[';
    empty = true;
    for (std::vector<CGXDLMSIp4SetupIpOption>::iterator it = m_IPOptions.begin(); it != m_IPOptions.end(); ++it)
    {
        if (!empty)
        {
            sb << ", ";
        }
        empty = false;
        std::string str = it->ToString();
        sb.write(str.c_str(), str.size());
    }
    sb << ']';
    values.push_back(sb.str());

    values.push_back(CGXDLMSVariant(m_SubnetMask).ToString());
    values.push_back(CGXDLMSVariant(m_GatewayIPAddress).ToString());
    values.push_back(CGXDLMSVariant(m_UseDHCP).ToString());
    values.push_back(CGXDLMSVariant(m_PrimaryDNSAddress).ToString());
    values.push_back(CGXDLMSVariant(m_SecondaryDNSAddress).ToString());
}

void CGXDLMSIp4Setup::GetAttributeIndexToRead(std::vector<int>& attributes)
{
    //LN is static and read only once.
    if (CGXDLMSObject::IsLogicalNameEmpty(m_LN))
    {
        attributes.push_back(1);
    }
    //DataLinkLayerReference
    if (!IsRead(2))
    {
        attributes.push_back(2);
    }
    //IPAddress
    if (CanRead(3))
    {
        attributes.push_back(3);
    }
    //MulticastIPAddress
    if (CanRead(4))
    {
        attributes.push_back(4);
    }
    //IPOptions
    if (CanRead(5))
    {
        attributes.push_back(5);
    }
    //SubnetMask
    if (CanRead(6))
    {
        attributes.push_back(6);
    }
    //GatewayIPAddress
    if (CanRead(7))
    {
        attributes.push_back(7);
    }
    //UseDHCP
    if (!IsRead(8))
    {
        attributes.push_back(8);
    }
    //PrimaryDNSAddress
    if (CanRead(9))
    {
        attributes.push_back(9);
    }
    //SecondaryDNSAddress
    if (CanRead(10))
    {
        attributes.push_back(10);
    }
}

int CGXDLMSIp4Setup::GetDataType(int index, DLMS_DATA_TYPE& type)
{
    if (index == 1)
    {
        type = DLMS_DATA_TYPE_OCTET_STRING;
    }
    else if (index == 2)
    {
        type = DLMS_DATA_TYPE_OCTET_STRING;
    }
    else if (index == 3)
    {
        type = DLMS_DATA_TYPE_UINT32;
    }
    else if (index == 4)
    {
        type = DLMS_DATA_TYPE_ARRAY;
    }
    else if (index == 5)
    {
        type = DLMS_DATA_TYPE_ARRAY;
    }
    else if (index == 6)
    {
        type = DLMS_DATA_TYPE_UINT32;
    }
    else if (index == 7)
    {
        type = DLMS_DATA_TYPE_UINT32;
    }
    else if (index == 8)
    {
        type = DLMS_DATA_TYPE_BOOLEAN;
    }
    else if (index == 9)
    {
        type = DLMS_DATA_TYPE_UINT32;
    }
    else if (index == 10)
    {
        type = DLMS_DATA_TYPE_UINT32;
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

// Returns value of given attribute.
int CGXDLMSIp4Setup::GetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
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
    }
    else if (e.GetIndex() == 2)
    {
        CGXDLMSVariant tmp;
        GXHelpers::SetLogicalName(m_DataLinkLayerReference.c_str(), tmp);
        e.SetValue(tmp);
    }
    else if (e.GetIndex() == 3)
    {
        if (m_IPAddress.size() == 0)
        {
            return 0;
        }
        struct sockaddr_in add;
        add.sin_addr.s_addr = inet_addr(m_IPAddress.c_str());
        //If address is give as name
        if (add.sin_addr.s_addr == INADDR_NONE)
        {
            struct hostent *Hostent = gethostbyname(m_IPAddress.c_str());
            if (Hostent == NULL)
            {
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            };
            add.sin_addr = *(in_addr*)(void*)Hostent->h_addr_list[0];
        };
        e.SetValue((unsigned long)add.sin_addr.s_addr);
    }
    else if (e.GetIndex() == 4)
    {
        e.SetByteArray(true);
        CGXByteBuffer data;
        data.SetUInt8(DLMS_DATA_TYPE_ARRAY);
        GXHelpers::SetObjectCount((unsigned long)m_MulticastIPAddress.size(), data);
        int ret;
        CGXDLMSVariant tmp;
        for (std::vector<unsigned long>::iterator it = m_MulticastIPAddress.begin(); it != m_MulticastIPAddress.end(); ++it)
        {
            tmp = *it;
            if ((ret = GXHelpers::SetData(data, DLMS_DATA_TYPE_UINT32, tmp)) != 0)
            {
                return ret;
            }
        }
        e.SetValue(data);
    }
    else if (e.GetIndex() == 5)
    {
        e.SetByteArray(true);
        CGXByteBuffer bb;
        bb.SetUInt8(DLMS_DATA_TYPE_ARRAY);
        GXHelpers::SetObjectCount((unsigned long)m_IPOptions.size(), bb);
        int ret;
        CGXDLMSVariant type, len, data;
        for (std::vector<CGXDLMSIp4SetupIpOption>::iterator it = m_IPOptions.begin(); it != m_IPOptions.end(); ++it)
        {
            bb.SetUInt8(DLMS_DATA_TYPE_STRUCTURE);
            bb.SetUInt8(3);
            type = it->GetType();
            len = it->GetLength();
            data = it->GetData();
            if ((ret = GXHelpers::SetData(bb, DLMS_DATA_TYPE_UINT8, type)) != 0 ||
                (ret = GXHelpers::SetData(bb, DLMS_DATA_TYPE_UINT8, len)) != 0 ||
                (ret = GXHelpers::SetData(bb, DLMS_DATA_TYPE_OCTET_STRING, data)) != 0)
            {
                return ret;
            }
        }
        e.SetValue(bb);
    }
    else if (e.GetIndex() == 6)
    {
        e.SetValue(m_SubnetMask);
    }
    else if (e.GetIndex() == 7)
    {
        e.SetValue(m_GatewayIPAddress);
    }
    else if (e.GetIndex() == 8)
    {
        e.SetValue(m_UseDHCP);
    }
    else if (e.GetIndex() == 9)
    {
        e.SetValue(m_PrimaryDNSAddress);
    }
    else if (e.GetIndex() == 10)
    {
        e.SetValue(m_SecondaryDNSAddress);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

// Set value of given attribute.
int CGXDLMSIp4Setup::SetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
{
    if (e.GetIndex() == 1)
    {
        return SetLogicalName(this, e.GetValue());
    }
    else if (e.GetIndex() == 2)
    {
        if (e.GetValue().vt == DLMS_DATA_TYPE_STRING)
        {
            m_DataLinkLayerReference = e.GetValue().ToString();
        }
        else
        {
            m_DataLinkLayerReference.clear();
            GXHelpers::GetLogicalName(e.GetValue().byteArr, m_DataLinkLayerReference);
        }
    }
    else if (e.GetIndex() == 3)
    {
        long tmp = e.GetValue().ToInteger();
        CGXByteBuffer bb;
        bb.AddIntAsString(tmp & 0xFF);
        bb.SetInt8('.');
        bb.AddIntAsString((tmp >> 8) & 0xFF);
        bb.SetInt8('.');
        bb.AddIntAsString((tmp >> 16) & 0xFF);
        bb.SetInt8('.');
        bb.AddIntAsString((tmp >> 24) & 0xFF);
        m_IPAddress = bb.ToString();
    }
    else if (e.GetIndex() == 4)
    {
        m_MulticastIPAddress.clear();
        if (e.GetValue().vt == DLMS_DATA_TYPE_ARRAY)
        {
            for (std::vector<CGXDLMSVariant>::iterator it = e.GetValue().Arr.begin(); it != e.GetValue().Arr.end(); ++it)
            {
                m_MulticastIPAddress.push_back((*it).ToInteger());
            }
        }
    }
    else if (e.GetIndex() == 5)
    {
        m_IPOptions.clear();
        if (e.GetValue().vt == DLMS_DATA_TYPE_ARRAY)
        {
            for (std::vector<CGXDLMSVariant>::iterator it = e.GetValue().Arr.begin(); it != e.GetValue().Arr.end(); ++it)
            {
                CGXDLMSIp4SetupIpOption item;
                item.SetType((IP_OPTION_TYPE)it->Arr[0].ToInteger());
                item.SetLength(it->Arr[1].ToInteger());
                CGXByteBuffer tmp;
                tmp.Set(it->Arr[2].byteArr, it->Arr[2].size);
                item.SetData(tmp);
                m_IPOptions.push_back(item);
            }
        }
    }
    else if (e.GetIndex() == 6)
    {
        m_SubnetMask = e.GetValue().ToInteger();
    }
    else if (e.GetIndex() == 7)
    {
        m_GatewayIPAddress = e.GetValue().ToInteger();
    }
    else if (e.GetIndex() == 8)
    {
        m_UseDHCP = e.GetValue().boolVal;
    }
    else if (e.GetIndex() == 9)
    {
        m_PrimaryDNSAddress = e.GetValue().ToInteger();
    }
    else if (e.GetIndex() == 10)
    {
        m_SecondaryDNSAddress = e.GetValue().ToInteger();
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
