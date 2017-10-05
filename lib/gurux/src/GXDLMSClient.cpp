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

#include "../include/enums.h"
#include "../include/GXDLMSClient.h"
#include "../include/GXHelpers.h"
#include "../include/GXAPDU.h"
#include "../include/GXDLMSObjectFactory.h"
#include "../include/GXSecure.h"
#include "../include/GXSerialNumberCounter.h"
#include "../include/GXDLMSLNParameters.h"
#include "../include/GXDLMSSNParameters.h"


CGXDLMSClient::CGXDLMSClient(bool UseLogicalNameReferencing,
    int clientAddress,
    int serverAddress,
    //Authentication type.
    DLMS_AUTHENTICATION authentication,
    //Password if authentication is used.
    const char* password,
    DLMS_INTERFACE_TYPE intefaceType) : m_Settings(false)
{
    m_IsAuthenticationRequired = false;
    m_Settings.SetUseLogicalNameReferencing(UseLogicalNameReferencing);
    m_Settings.SetClientAddress(clientAddress);
    m_Settings.SetServerAddress(serverAddress);
    m_Settings.SetInterfaceType(intefaceType);
    m_Settings.SetAuthentication(authentication);
    m_Settings.GetPassword().AddString(password);
    if (UseLogicalNameReferencing)
    {
        SetProposedConformance((DLMS_CONFORMANCE)(DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_ACTION |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_SET_OR_WRITE |
            DLMS_CONFORMANCE_BLOCK_TRANSFER_WITH_GET_OR_READ |
            DLMS_CONFORMANCE_SET | DLMS_CONFORMANCE_SELECTIVE_ACCESS |
            DLMS_CONFORMANCE_ACTION | DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
            DLMS_CONFORMANCE_GET | DLMS_CONFORMANCE_GENERAL_PROTECTION));
    }
    else
    {
        SetProposedConformance((DLMS_CONFORMANCE)(DLMS_CONFORMANCE_INFORMATION_REPORT |
            DLMS_CONFORMANCE_READ | DLMS_CONFORMANCE_UN_CONFIRMED_WRITE |
            DLMS_CONFORMANCE_WRITE | DLMS_CONFORMANCE_PARAMETERIZED_ACCESS |
            DLMS_CONFORMANCE_MULTIPLE_REFERENCES |
            DLMS_CONFORMANCE_GENERAL_PROTECTION));
    }
}

CGXDLMSClient::~CGXDLMSClient()
{
}

DLMS_CONFORMANCE CGXDLMSClient::GetNegotiatedConformance()
{
    return (DLMS_CONFORMANCE)m_Settings.GetNegotiatedConformance();
}

void CGXDLMSClient::SetNegotiatedConformance(DLMS_CONFORMANCE value)
{
    m_Settings.SetNegotiatedConformance(value);
}


DLMS_CONFORMANCE CGXDLMSClient::GetProposedConformance()
{
    return (DLMS_CONFORMANCE)m_Settings.GetProposedConformance();
}

void CGXDLMSClient::SetProposedConformance(DLMS_CONFORMANCE value)
{
    m_Settings.SetProposedConformance(value);
}

bool CGXDLMSClient::GetUseLogicalNameReferencing()
{
    return m_Settings.GetUseLogicalNameReferencing();
}

DLMS_INTERFACE_TYPE CGXDLMSClient::GetInterfaceType()
{
    return m_Settings.GetInterfaceType();
}

DLMS_PRIORITY CGXDLMSClient::GetPriority()
{
    return m_Settings.GetPriority();
}

void CGXDLMSClient::SetPriority(DLMS_PRIORITY value)
{
    m_Settings.SetPriority(value);
}

DLMS_SERVICE_CLASS CGXDLMSClient::GetServiceClass()
{
    return m_Settings.GetServiceClass();
}

void CGXDLMSClient::SetServiceClass(DLMS_SERVICE_CLASS value)
{
    m_Settings.SetServiceClass(value);
}

CGXDLMSLimits& CGXDLMSClient::GetLimits()
{
    return m_Settings.GetLimits();
}

// Collection of the objects.
CGXDLMSObjectCollection& CGXDLMSClient::GetObjects()
{
    return m_Settings.GetObjects();
}

int CGXDLMSClient::SNRMRequest(std::vector<CGXByteBuffer>& packets)
{
    int ret;
    m_Settings.SetConnected(false);
    packets.clear();
    m_IsAuthenticationRequired = false;
    // SNRM request is not used in network connections.
    if (m_Settings.GetInterfaceType() == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        return 0;
    }
    CGXByteBuffer data(25);
    // FromatID
    data.SetUInt8(0x81);
    // GroupID
    data.SetUInt8(0x80);
    // Length is updated later.
    data.SetUInt8(0);
    // If custom HDLC parameters are used.
 //   if (CGXDLMSLimits::DEFAULT_MAX_INFO_TX != GetLimits().GetMaxInfoTX().ToInteger())
    {
        data.SetUInt8(HDLC_INFO_MAX_INFO_TX);
        data.SetUInt8(GetLimits().GetMaxInfoTX().GetSize());
        if ((ret = GetLimits().GetMaxInfoTX().GetBytes(data)) != 0)
        {
            return ret;
        }
    }
//   if (CGXDLMSLimits::DEFAULT_MAX_INFO_RX != GetLimits().GetMaxInfoRX().ToInteger())
    {
        data.SetUInt8(HDLC_INFO_MAX_INFO_RX);
        data.SetUInt8(GetLimits().GetMaxInfoRX().GetSize());
        if ((ret = GetLimits().GetMaxInfoRX().GetBytes(data)) != 0)
        {
            return ret;
        }
    }
  //  if (CGXDLMSLimits::DEFAULT_WINDOWS_SIZE_TX != GetLimits().GetWindowSizeTX().ToInteger())
    {
        data.SetUInt8(HDLC_INFO_WINDOW_SIZE_TX);
        data.SetUInt8(GetLimits().GetWindowSizeTX().GetSize());
        if ((ret = GetLimits().GetWindowSizeTX().GetBytes(data)) != 0)
        {
            return ret;
        }
    }
 //   if (CGXDLMSLimits::DEFAULT_WINDOWS_SIZE_RX != GetLimits().GetWindowSizeRX().ToInteger())
    {
        data.SetUInt8(HDLC_INFO_WINDOW_SIZE_RX);
        data.SetUInt8(GetLimits().GetWindowSizeRX().ToInteger());
        if ((ret = GetLimits().GetWindowSizeRX().GetBytes(data)) != 0)
        {
            return ret;
        }
    }
    // If default HDLC parameters are not used.
    if (data.GetSize() != 3)
    {
        // Length.
        data.SetUInt8(2, (unsigned char)(data.GetSize() - 3));
    }
    else
    {
        data.Clear();
    }
    m_Settings.ResetFrameSequence();
    CGXByteBuffer reply;
    ret = CGXDLMS::GetHdlcFrame(m_Settings, DLMS_COMMAND_SNRM, &data, reply);
    packets.push_back(reply);
    return ret;
}

// SN referencing
int CGXDLMSClient::ParseSNObjects(CGXByteBuffer& buff, bool onlyKnownObjects)
{
    int ret;
    CGXDataInfo info;
    //Get array tag.
    unsigned char ch;
    unsigned long cnt;
    //Check that data is in the array
    // Get array tag.
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 1)
    {
        return DLMS_ERROR_CODE_INVALID_RESPONSE;
    }
    //get object count
    CGXDLMSVariant value;
    if ((ret = GXHelpers::GetObjectCount(buff, cnt)) != 0)
    {
        return ret;
    }
    for (unsigned long objPos = 0; objPos != cnt; ++objPos)
    {
        info.Clear();
        if ((ret = GXHelpers::GetData(buff, info, value)) != 0)
        {
            return ret;
        }
        if (value.vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr.size() != 4)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        if (value.Arr[0].vt != DLMS_DATA_TYPE_INT16 ||
            value.Arr[1].vt != DLMS_DATA_TYPE_UINT16 ||
            value.Arr[2].vt != DLMS_DATA_TYPE_UINT8 ||
            value.Arr[3].vt != DLMS_DATA_TYPE_OCTET_STRING)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        short sn = value.Arr[0].ToInteger();
        unsigned short class_id = (unsigned short)value.Arr[1].ToInteger();
        unsigned char version = (unsigned char)value.Arr[2].ToInteger();
        CGXDLMSVariant ln = value.Arr[3];
        CGXDLMSObject* pObj = CGXDLMSObjectFactory::CreateObject((DLMS_OBJECT_TYPE)class_id);
        if (pObj != NULL)
        {
            pObj->SetShortName(sn);
            pObj->SetVersion(version);
            int cnt = ln.GetSize();
            assert(cnt == 6);
            CGXDLMSObject::SetLogicalName(pObj, ln);
            m_Settings.GetObjects().push_back(pObj);
        }
    }
    return 0;
}

int CGXDLMSClient::ParseLNObjects(CGXByteBuffer& buff, bool onlyKnownObjects)
{
    int ret;
    unsigned long cnt;
    unsigned char ch;
    CGXDataInfo info;
    // Get array tag.
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    // Check that data is in the array
    if (ch != 0x01)
    {
        //Invalid response.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    CGXDLMSVariant value;
    if ((ret = GXHelpers::GetObjectCount(buff, cnt)) != 0)
    {
        return ret;
    }
    for (unsigned long pos = 0; pos != cnt; ++pos)
    {
        // Some meters give wrong item count.
        // This fix Iskraemeco (MT-880) bug.
        if (buff.GetPosition() == buff.GetSize())
        {
            break;
        }
        info.SetType(DLMS_DATA_TYPE_NONE);
        info.SetIndex(0);
        info.SetCount(0);

        if ((ret = GXHelpers::GetData(buff, info, value)) != 0)
        {
            return ret;
        }
        if (value.Arr.size() != 4)
        {
            //Invalid structure format.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        int classID = value.Arr[0].ToInteger() & 0xFFFF;
        if (classID > 0)
        {
            CGXDLMSObject* pObj = CGXDLMSObjectFactory::CreateObject((DLMS_OBJECT_TYPE)classID);
            if (pObj != NULL)
            {
                if (value.vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr.size() != 4)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                if (value.Arr[0].vt != DLMS_DATA_TYPE_UINT16)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                if (value.Arr[1].vt != DLMS_DATA_TYPE_UINT8)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                unsigned char version = value.Arr[1].ToInteger();
                if (value.Arr[2].vt != DLMS_DATA_TYPE_OCTET_STRING)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                CGXDLMSVariant ln = value.Arr[2];
                //Get Access rights...
                if (value.Arr[3].vt != DLMS_DATA_TYPE_STRUCTURE || value.Arr[3].Arr.size() != 2)
                {
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                pObj->SetVersion(version);
                int cnt;
                // attribute_access_descriptor Start
                if (value.Arr[3].Arr[0].vt != DLMS_DATA_TYPE_ARRAY)
                {
                    delete pObj;
                    return DLMS_ERROR_CODE_INVALID_PARAMETER;
                }
                if (value.Arr[3].Arr.size() == 2)
                {
                    for (unsigned int pos = 0; pos != value.Arr[3].Arr[0].Arr.size(); ++pos)
                    {
                        if (value.Arr[3].Arr[0].Arr[pos].vt != DLMS_DATA_TYPE_STRUCTURE ||
                            value.Arr[3].Arr[0].Arr[pos].Arr.size() != 3)
                        {
                            delete pObj;
                            return DLMS_ERROR_CODE_INVALID_PARAMETER;
                        }
                        int id = value.Arr[3].Arr[0].Arr[pos].Arr[0].ToInteger();
                        //Get access_mode
                        DLMS_DATA_TYPE tp = value.Arr[3].Arr[0].Arr[pos].Arr[1].vt;
                        if (tp != DLMS_DATA_TYPE_ENUM)
                        {
                            delete pObj;
                            return DLMS_ERROR_CODE_INVALID_PARAMETER;
                        }
                        pObj->SetAccess(id, (DLMS_ACCESS_MODE)value.Arr[3].Arr[0].Arr[pos].Arr[1].ToInteger());
                        //Get access_selectors
                        if (value.Arr[3].Arr[0].Arr[pos].Arr[2].vt == DLMS_DATA_TYPE_ARRAY)
                        {
                            int cnt2 = (unsigned long)value.Arr[3].Arr[0].Arr[pos].Arr[2].Arr.size();
                            for (int pos2 = 0; pos2 != cnt2; ++pos2)
                            {
                                //Get access_mode
                            }
                        }
                        else if (value.Arr[3].Arr[0].Arr[pos].Arr[2].vt != DLMS_DATA_TYPE_NONE)
                        {
                            delete pObj;
                            return DLMS_ERROR_CODE_INVALID_PARAMETER;
                        }
                    }
                    // attribute_access_descriptor End
                    // method_access_item Start
                    if (value.Arr[3].Arr[1].vt != DLMS_DATA_TYPE_ARRAY)
                    {
                        delete pObj;
                        return DLMS_ERROR_CODE_INVALID_PARAMETER;
                    }
                    for (unsigned int pos = 0; pos != value.Arr[3].Arr[1].Arr.size(); ++pos)
                    {
                        CGXDLMSVariant tmp = value.Arr[3].Arr[1].Arr[pos];
                        if (tmp.vt != DLMS_DATA_TYPE_STRUCTURE ||
                            tmp.Arr.size() != 2)
                        {
                            delete pObj;
                            return DLMS_ERROR_CODE_INVALID_PARAMETER;
                        }
                        int id = tmp.Arr[0].ToInteger();
                        //Get access_mode
                        //In version 0 data type is boolean.
                        if (tmp.Arr[1].vt != DLMS_DATA_TYPE_ENUM && tmp.Arr[1].vt != DLMS_DATA_TYPE_BOOLEAN)
                        {
                            delete pObj;
                            return DLMS_ERROR_CODE_INVALID_PARAMETER;
                        }
                        pObj->SetMethodAccess(id, (DLMS_METHOD_ACCESS_MODE)tmp.Arr[1].ToInteger());
                    }
                }
                // method_access_item End
                cnt = ln.GetSize();
                assert(cnt == 6);
                if ((ret = CGXDLMSObject::SetLogicalName(pObj, ln)) != 0)
                {
                    return ret;
                }
                m_Settings.GetObjects().push_back(pObj);
            }
        }
    }
    return 0;
}

int CGXDLMSClient::ParseObjects(CGXByteBuffer& data, bool onlyKnownObjects)
{
    int ret;
    m_Settings.GetObjects().Free();
    if (GetUseLogicalNameReferencing())
    {
        if ((ret = ParseLNObjects(data, onlyKnownObjects)) != 0)
        {
            return ret;
        }
    }
    else
    {
        if ((ret = ParseSNObjects(data, onlyKnownObjects)) != 0)
        {
            return ret;
        }
    }
    return 0;
}

int CGXDLMSClient::UpdateValue(CGXDLMSObject& target, int attributeIndex, CGXDLMSVariant& value)
{
    int ret;
    if (value.vt == DLMS_DATA_TYPE_OCTET_STRING)
    {
        DLMS_DATA_TYPE type;
        if ((ret = target.GetUIDataType(attributeIndex, type)) != 0)
        {
            return ret;
        }
        if (type == DLMS_DATA_TYPE_DATETIME && value.GetSize() == 5)
        {
            type = DLMS_DATA_TYPE_DATE;
            target.SetUIDataType(attributeIndex, type);
        }
        if (type != DLMS_DATA_TYPE_NONE)
        {
            if ((ret = ChangeType(value, type, value)) != 0)
            {
                return ret;
            }
        }
    }
    CGXDLMSValueEventArg e(NULL, &target, attributeIndex);
    e.SetValue(value);
    return target.SetValue(m_Settings, e);
}

int CGXDLMSClient::GetValue(CGXByteBuffer& data, CGXDLMSVariant& value)
{
    CGXDataInfo info;
    return GXHelpers::GetData(data, info, value);
}


int CGXDLMSClient::ChangeType(CGXDLMSVariant& value, DLMS_DATA_TYPE type, CGXDLMSVariant& newValue)
{
    if (value.vt == DLMS_DATA_TYPE_OCTET_STRING)
    {
        CGXByteBuffer tmp;
        tmp.Set(value.byteArr, value.GetSize());
        return ChangeType(tmp, type, newValue);
    }
    else
    {
        newValue = value;
    }
    return 0;
}

int CGXDLMSClient::ChangeType(CGXByteBuffer& value, DLMS_DATA_TYPE type, CGXDLMSVariant& newValue)
{
    int ret;
    CGXDataInfo info;
    newValue.Clear();
    if (value.GetSize() == 0)
    {
        return 0;
    }
    if (type == DLMS_DATA_TYPE_NONE)
    {
        newValue = value.ToHexString();
    }
    if (value.GetSize() == 0 && (type == DLMS_DATA_TYPE_STRING || type == DLMS_DATA_TYPE_OCTET_STRING))
    {
        newValue = "";
        return 0;
    }
    info.SetType(type);
    if ((ret = GXHelpers::GetData(value, info, newValue)) != 0)
    {
        return ret;
    }
    if (!info.IsCompleate())
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    if (type == DLMS_DATA_TYPE_OCTET_STRING && newValue.vt == DLMS_DATA_TYPE_OCTET_STRING)
    {
        int size = newValue.size;
        std::string str;
        if (size == 0)
        {
            str = "";
        }
        else
        {
            CGXByteBuffer bcd(size * 4);
            for (int pos = 0; pos != size; ++pos)
            {
                bcd.AddIntAsString(newValue.byteArr[pos]);
                bcd.SetUInt8('.');
            }
            //Remove last dot.
            if (bcd.GetSize() != 0)
            {
                bcd.SetSize(bcd.GetSize() - 1);
            }
            str = bcd.ToString();
        }
        newValue = str;
    }
    return ret;
}

int CGXDLMSClient::ParseUAResponse(CGXByteBuffer& data)
{
    /*
    unsigned char ch, id, len;
    unsigned short ui;
    unsigned long ul;
    int ret;
    if (data.GetSize() == 0)
    {
        return 0;
    }
    // Skip FromatID
    if ((ret = data.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    // Skip Group ID.
    if ((ret = data.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    // Skip Group len
    if ((ret = data.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    CGXDLMSVariant value;
    while (data.GetPosition() < data.GetSize())
    {
        if ((ret = data.GetUInt8(&id)) != 0)
        {
            return ret;
        }
        if ((ret = data.GetUInt8(&len)) != 0)
        {
            return ret;
        }
        switch (len)
        {
        case 1:
            if ((ret = data.GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            value = ch;
            break;
        case 2:
            if ((ret = data.GetUInt16(&ui)) != 0)
            {
                return ret;
            }
            value = ui;
            break;
        case 4:
            if ((ret = data.GetUInt32(&ul)) != 0)
            {
                return ret;
            }
            value = ul;
            break;
        default:
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        // RX / TX are delivered from the partner's point of view =>
        // reversed to ours
        switch (id)
        {
        case HDLC_INFO_MAX_INFO_TX:
            m_Settings.GetLimits().SetMaxInfoRX(value);
            break;
        case HDLC_INFO_MAX_INFO_RX:
            m_Settings.GetLimits().SetMaxInfoTX(value);
            break;
        case HDLC_INFO_WINDOW_SIZE_TX:
            m_Settings.GetLimits().SetWindowSizeRX(value);
            break;
        case HDLC_INFO_WINDOW_SIZE_RX:
            m_Settings.GetLimits().SetWindowSizeTX(value);
            break;
        default:
            ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
            break;
        }
    }
    return ret;
    */
    m_Settings.GetLimits().SetMaxInfoRX(128);
    m_Settings.GetLimits().SetMaxInfoTX(128);
    m_Settings.GetLimits().SetWindowSizeRX(7);
    m_Settings.GetLimits().SetWindowSizeTX(7);
    return 0;
}

int CGXDLMSClient::AARQRequest(std::vector<CGXByteBuffer>& packets)
{
    CGXByteBuffer buff(20);
    m_Settings.ResetBlockIndex();
    m_Settings.SetNegotiatedConformance((DLMS_CONFORMANCE)0);
    packets.clear();
    int ret = CGXDLMS::CheckInit(m_Settings);
    if (ret != 0)
    {
        return ret;
    }
    m_Settings.GetStoCChallenge().Clear();
    // If authentication or ciphering is used.
    if (m_Settings.GetAuthentication() > DLMS_AUTHENTICATION_LOW)
    {
        CGXByteBuffer challenge;
        if ((ret = CGXSecure::GenerateChallenge(m_Settings.GetAuthentication(), challenge)) != 0)
        {
            return ret;
        }
        m_Settings.SetCtoSChallenge(challenge);
    }
    else
    {
        m_Settings.GetCtoSChallenge().Clear();
    }
    if ((ret = CGXAPDU::GenerateAarq(m_Settings, m_Settings.GetCipher(), NULL, buff)) != 0)
    {
        return ret;
    }
    if (GetUseLogicalNameReferencing())
    {
        CGXDLMSLNParameters p(&m_Settings, DLMS_COMMAND_AARQ, 0, &buff, NULL, 0xff);
        ret = CGXDLMS::GetLnMessages(p, packets);
    }
    else
    {
        CGXDLMSSNParameters p(&m_Settings, DLMS_COMMAND_AARQ, 0, 0, NULL, &buff);
        ret = CGXDLMS::GetSnMessages(p, packets);
    }
    return ret;
}

int CGXDLMSClient::ParseAAREResponse(CGXByteBuffer& reply)
{
    int ret;
    DLMS_SOURCE_DIAGNOSTIC sd;
    if ((ret = CGXAPDU::ParsePDU(m_Settings, m_Settings.GetCipher(),
        reply, sd)) != 0)
    {
        return ret;
    }
    m_IsAuthenticationRequired = (DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_REQUIRED == sd);
    if (m_Settings.GetDLMSVersion() != 6)
    {
        //Invalid DLMS version number.
        return DLMS_ERROR_CODE_INVALID_VERSION_NUMBER;
    }
    m_Settings.SetConnected(true);
    return 0;
}

bool CGXDLMSClient::IsAuthenticationRequired()
{
    return m_IsAuthenticationRequired;
}

int CGXDLMSClient::GetApplicationAssociationRequest(
    std::vector<CGXByteBuffer>& packets)
{
    int ret;
    packets.clear();
    if (m_Settings.GetAuthentication() != DLMS_AUTHENTICATION_HIGH_ECDSA &&
        m_Settings.GetAuthentication() != DLMS_AUTHENTICATION_HIGH_GMAC &&
        m_Settings.GetPassword().GetSize() == 0)
    {
        //Password is invalid.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    m_Settings.ResetBlockIndex();
    CGXByteBuffer pw, challenge;
    if (m_Settings.GetAuthentication() == DLMS_AUTHENTICATION_HIGH_GMAC)
    {
        pw = m_Settings.GetCipher()->GetSystemTitle();
    }
    else
    {
        pw = m_Settings.GetPassword();
    }
    long ic = 0;
    if (m_Settings.GetCipher() != NULL)
    {
        ic = m_Settings.GetCipher()->GetFrameCounter();
    }
    if ((ret = CGXSecure::Secure(m_Settings, m_Settings.GetCipher(), ic,
        m_Settings.GetStoCChallenge(), pw, challenge)) != 0)
    {
        return ret;
    }
    CGXByteBuffer bb;
    bb.SetUInt8(DLMS_DATA_TYPE_OCTET_STRING);
    GXHelpers::SetObjectCount(challenge.GetSize(), bb);
    bb.Set(&challenge);
    CGXDLMSVariant name, data = bb;
    if (GetUseLogicalNameReferencing())
    {
        name = "0.0.40.0.0.255";
        return Method(name, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME,
            1, data, packets);
    }
    name = 0xFA00;
    return Method(name, DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME, 8, data,
        packets);
}

int CGXDLMSClient::ParseApplicationAssociationResponse(
    CGXByteBuffer& reply)
{
    CGXDataInfo info;
    unsigned char ch;
    bool equals = false;
    CGXByteBuffer secret;
    int ret;
    unsigned long ic = 0;
    CGXDLMSVariant value;
    if ((ret = GXHelpers::GetData(reply, info, value)) != 0)
    {
        return ret;
    }
    if (value.vt != DLMS_DATA_TYPE_NONE)
    {
        if (m_Settings.GetAuthentication() == DLMS_AUTHENTICATION_HIGH_GMAC)
        {
            secret = m_Settings.GetSourceSystemTitle();
            CGXByteBuffer bb;
            bb.Set(value.byteArr, value.GetSize());
            if ((ret = bb.GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            if ((ret = bb.GetUInt32(&ic)) != 0)
            {
                return ret;
            }
        }
        else
        {
            secret = m_Settings.GetPassword();
        }
        CGXByteBuffer challenge, cToS = m_Settings.GetCtoSChallenge();
        if ((ret = CGXSecure::Secure(
            m_Settings,
            m_Settings.GetCipher(),
            ic,
            cToS,
            secret,
            challenge)) != 0)
        {
            return ret;
        }
        equals = challenge.Compare(value.byteArr, value.GetSize());
    }
    else
    {
        // Server did not accept CtoS.
    }

    if (!equals)
    {
        //ParseApplicationAssociationResponse failed. Server to Client do not match.
        return DLMS_ERROR_CODE_AUTHENTICATION_FAILURE;
    }
    return 0;
}

int CGXDLMSClient::ReceiverReady(DLMS_DATA_REQUEST_TYPES type, CGXByteBuffer& packets)
{
    return CGXDLMS::ReceiverReady(m_Settings, type, NULL, packets);
}

std::string CGXDLMSClient::ObjectTypeToString(DLMS_OBJECT_TYPE type)
{
    return CGXDLMSObjectFactory::ObjectTypeToString(type);
}

void CGXDLMSClient::UpdateOBISCodes(CGXDLMSObjectCollection& objects)
{
    /*
    stream sb;
    sb << OBIS_CODES1;
    sb << OBIS_CODES2;
    sb << OBIS_CODES3;
    sb << OBIS_CODES4;
    sb << OBIS_CODES5;
    sb << OBIS_CODES6;
    sb << OBIS_CODES7;
    sb << OBIS_CODES8;
    sb << OBIS_CODES9;
    sb << OBIS_CODES10;
    sb << OBIS_CODES11;
    CGXStandardObisCodeCollection codes;
    std::string str = sb.str();
    std::vector< std::string > rows = GXHelpers::Split(str, "\r\n", true);
    int row = 0;
    std::string last;
    for(std::vector< std::string >::iterator it = rows.begin(); it != rows.end(); ++it)
    {
        std::vector< std::string > items = GXHelpers::Split(*it, ";\r\n", false);
        if (items.size() != 8)
        {
            items = GXHelpers::Split(*it, ";\r\n", false);
        }
        assert(items.size() == 8);
        std::vector< std::string > obis = GXHelpers::Split(items[0], ".\r\n", false);
        if (obis.size() != 6)
        {
            obis = GXHelpers::Split(items[0], ".\r\n", false);
        }
        std::string str = items[3] + "; " + items[4] + "; " + items[5] + "; " + items[6] + "; " + items[7];
        codes.push_back(CGXStandardObisCode(obis, str, items[1], items[2]));
        ++row;
        last = *it;
    }
    for(std::vector<CGXDLMSObject*>::iterator it = objects.begin(); it != objects.end(); ++it)
    {
        std::string ln;
        (*it)->GetLogicalName(ln);
        CGXStandardObisCode code;
        if (codes.Find(ln, (*it)->GetObjectType(), code))
        {
            (*it)->SetDescription(code.GetDescription());
            //If std::string is used
            if (code.GetDataType().find("10") != std::string::npos)
            {
                code.SetDataType("10");
            }
            //If date time is used.
            else if (code.GetDataType().find("25") != std::string::npos ||
                     code.GetDataType().find("26") != std::string::npos)
            {
                code.SetDataType("25");
            }
            else if (code.GetDataType().find("9"))
            {
                //Time stamps of the billing periods objects (first scheme if there are two)
                if ((CGXStandardObisCodeCollection::EqualsMask("0.0-64.96.7.10-14.255", ln) ||
                        //Time stamps of the billing periods objects (second scheme)
                        CGXStandardObisCodeCollection::EqualsMask("0.0-64.0.1.5.0-99,255", ln) ||
                        //Time of power failure
                        CGXStandardObisCodeCollection::EqualsMask("0.0-64.0.1.2.0-99,255", ln) ||
                        //Time stamps of the billing periods objects (first scheme if there are two)
                        CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.1.2.0-99,255", ln) ||
                        //Time stamps of the billing periods objects (second scheme)
                        CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.1.5.0-99,255", ln) ||
                        //Time expired since last end of billing period
                        CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.0.255", ln) ||
                        //Time of last reset
                        CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.6.255", ln) ||
                        //Date of last reset
                        CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.7.255", ln) ||
                        //Time expired since last end of billing period (Second billing period scheme)
                        CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.13.255", ln) ||
                        //Time of last reset (Second billing period scheme)
                        CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.14.255", ln) ||
                        //Date of last reset (Second billing period scheme)
                        CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.15.255", ln)))
                {
                    code.SetDataType("25");
                }
                //Local time
                else if (CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.1.255", ln))
                {
                    code.SetDataType("27");
                }
                //Local date
                else if (CGXStandardObisCodeCollection::EqualsMask("1.0-64.0.9.2.255", ln))
                {
                    code.SetDataType("26");
                }
                //Active firmware identifier
                else if (CGXStandardObisCodeCollection::EqualsMask("1.0.0.2.0.255", ln))
                {
                    code.SetDataType("10");
                }
            }
            if (code.GetDataType() != "*" &&
                    code.GetDataType() != "" &&
                    code.GetDataType().find(",") == std::string::npos)
            {
                int value;
    #if _MSC_VER > 1000
                sscanf_s(code.GetDataType().c_str(), "%d", &value);
    #else
                sscanf(code.GetDataType().c_str(), "%d", &value);
    #endif
                DLMS_DATA_TYPE type = (DLMS_DATA_TYPE) value;
                switch ((*it)->GetObjectType())
                {
                case DLMS_OBJECT_TYPE_DATA:
                case DLMS_OBJECT_TYPE_REGISTER:
                case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
                case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
                    (*it)->SetUIDataType(2, type);
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            System.out.println("Unknown OBIS Code: " + it.getLogicalName() +
                               " Type: " + it.getObjectType());
        }
    }
    */
}

int CGXDLMSClient::ReleaseRequest(std::vector<CGXByteBuffer>& packets)
{
    int ret;
    CGXByteBuffer buff;
    packets.clear();
    // If connection is not established, there is no need to send DisconnectRequest.
    if (!m_Settings.IsConnected())
    {
        return 0;
    }
    //Length.
    buff.SetUInt8(0);
    buff.SetUInt8(0x80);
    buff.SetUInt8(01);
    buff.SetUInt8(00);
    CGXAPDU::GenerateUserInformation(m_Settings, m_Settings.GetCipher(), NULL, buff);
    buff.SetUInt8(0, (unsigned char)(buff.GetSize() - 1));
    if (GetUseLogicalNameReferencing())
    {
        CGXDLMSLNParameters p(&m_Settings, DLMS_COMMAND_RELEASE_REQUEST, 0, NULL, &buff, 0xff);
        ret = CGXDLMS::GetLnMessages(p, packets);
    }
    else
    {
        CGXDLMSSNParameters p(&m_Settings, DLMS_COMMAND_RELEASE_REQUEST, 0xFF, 0xFF, NULL, &buff);
        ret = CGXDLMS::GetSnMessages(p, packets);
    }
    if (GetInterfaceType() == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        m_Settings.SetConnected(false);
    }
    return ret;
}

int CGXDLMSClient::DisconnectRequest(std::vector<CGXByteBuffer>& packets)
{
    int ret;
    CGXByteBuffer reply;
    packets.clear();
    m_Settings.SetMaxReceivePDUSize(0xFFFF);
    // If connection is not established, there is no need to send DisconnectRequest.
    if (!m_Settings.IsConnected())
    {
        return 0;
    }
    if (GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC)
    {
        ret = CGXDLMS::GetHdlcFrame(m_Settings, DLMS_COMMAND_DISC, NULL, reply);
        packets.push_back(reply);
        return ret;
    }
    CGXByteBuffer bb(2);
    bb.SetUInt8(DLMS_COMMAND_RELEASE_REQUEST);
    bb.SetUInt8(0x0);
    ret = CGXDLMS::GetWrapperFrame(m_Settings, bb, reply);
    packets.push_back(reply);
    return ret;
}

int CGXDLMSClient::GetData(CGXByteBuffer& reply, CGXReplyData& data)
{
    return CGXDLMS::GetData(m_Settings, reply, data);
}

int CGXDLMSClient::GetObjectsRequest(std::vector<CGXByteBuffer>& reply)
{
    CGXDLMSVariant name;
    if (GetUseLogicalNameReferencing())
    {
        name = "0.0.40.0.0.255";
    }
    else
    {
        name = (short)0xFA00;
    }
    return Read(name, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, 2, reply);
}

int CGXDLMSClient::Read(CGXDLMSObject* pObject, int attributeOrdinal, std::vector<CGXByteBuffer>& reply)
{
    CGXDLMSVariant name = pObject->GetName();
    return Read(name, pObject->GetObjectType(), attributeOrdinal, NULL, reply);
}

int CGXDLMSClient::Read(CGXDLMSVariant& name, DLMS_OBJECT_TYPE objectType, int attributeOrdinal, std::vector<CGXByteBuffer>& reply)
{
    return Read(name, objectType, attributeOrdinal, NULL, reply);
}

int CGXDLMSClient::Read(CGXDLMSVariant& name, DLMS_OBJECT_TYPE objectType, int attributeOrdinal, CGXByteBuffer* data,
    std::vector<CGXByteBuffer>& reply)
{
    int ret;
    if ((attributeOrdinal < 1))
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    m_Settings.ResetBlockIndex();
    CGXByteBuffer attributeDescriptor;
    if (GetUseLogicalNameReferencing())
    {
        // CI
        attributeDescriptor.SetUInt16(objectType);
        // Add LN
        unsigned char ln[6];
        GXHelpers::SetLogicalName(name.strVal.c_str(), ln);
        attributeDescriptor.Set(ln, 6);
        // Attribute ID.
        attributeDescriptor.SetUInt8(attributeOrdinal);
        if (data == NULL || data->GetSize() == 0)
        {
            // Access selection is not used.
            attributeDescriptor.SetUInt8(0);
        }
        else
        {
            // Access selection is used.
            attributeDescriptor.SetUInt8(1);
            // Add data.
            attributeDescriptor.Set(data, 0, data->GetSize());
        }
        CGXDLMSLNParameters p(&m_Settings,
            DLMS_COMMAND_GET_REQUEST, DLMS_GET_COMMAND_TYPE_NORMAL,
            &attributeDescriptor, data, 0xFF);
        ret = CGXDLMS::GetLnMessages(p, reply);
    }
    else
    {
        DLMS_VARIABLE_ACCESS_SPECIFICATION requestType;
        int sn = name.ToInteger();
        sn += (attributeOrdinal - 1) * 8;
        attributeDescriptor.SetUInt16(sn);
        // Add Selector.
        if (data != NULL && data->GetSize() != 0)
        {
            requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_PARAMETERISED_ACCESS;
        }
        else
        {
            requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME;
        }
        CGXDLMSSNParameters p(&m_Settings, DLMS_COMMAND_READ_REQUEST, 1,
            requestType, &attributeDescriptor, data);
        ret = CGXDLMS::GetSnMessages(p, reply);
    }
    return ret;
}

/**
* Read list of COSEM objects.
*
* @param list
*            DLMS objects to read.
* @return Read request as byte array.
*/
int CGXDLMSClient::ReadList(
    std::vector<std::pair<CGXDLMSObject*, unsigned char> >& list,
    std::vector<CGXByteBuffer>& reply)
{
    if (list.size() == 0)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    int ret;
    m_Settings.ResetBlockIndex();
    CGXByteBuffer bb;
    if (GetUseLogicalNameReferencing())
    {
        CGXDLMSLNParameters p(&m_Settings,
            DLMS_COMMAND_GET_REQUEST, DLMS_GET_COMMAND_TYPE_WITH_LIST,
            &bb, NULL, 0xff);
        //Request service primitive shall always fit in a single APDU.
        unsigned short pos = 0, count = (m_Settings.GetMaxPduSize() - 12) / 10;
        if (list.size() < count)
        {
            count = (unsigned short)list.size();
        }
        //All meters can handle 10 items.
        if (count > 10)
        {
            count = 10;
        }
        // Add length.
        GXHelpers::SetObjectCount(count, bb);
        for (std::vector<std::pair<CGXDLMSObject*, unsigned char> >::iterator it = list.begin(); it != list.end(); ++it)
        {
            // CI.
            bb.SetUInt16(it->first->GetObjectType());
            bb.Set(it->first->m_LN, 6);
            // Attribute ID.
            bb.SetUInt8(it->second);
            // Attribute selector is not used.
            bb.SetUInt8(0);
            ++pos;
            if (pos % count == 0 && list.size() != pos)
            {
                if ((ret = CGXDLMS::GetLnMessages(p, reply)) != 0)
                {
                    return ret;
                }
                bb.Clear();
                if (list.size() - pos < count)
                {
                    GXHelpers::SetObjectCount((unsigned long)(list.size() - pos), bb);
                }
                else
                {
                    GXHelpers::SetObjectCount(count, bb);
                }
            }
        }
        if ((ret = CGXDLMS::GetLnMessages(p, reply)) != 0)
        {
            return ret;
        }
    }
    else
    {
        int sn;
        for (std::vector<std::pair<CGXDLMSObject*, unsigned char> >::iterator it = list.begin(); it != list.end(); ++it)
        {
            // Add variable type.
            bb.SetUInt8(DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME);
            sn = it->first->GetShortName();
            sn += (it->second - 1) * 8;
            bb.SetUInt16(sn);
        }
        CGXDLMSSNParameters p(&m_Settings, DLMS_COMMAND_READ_REQUEST,
            (unsigned long)list.size(), 0xFF, &bb, NULL);
        ret = CGXDLMS::GetSnMessages(p, reply);
    }
    return ret;
}

/**
     * Update list of values.
     *
     * @param list
     *            read objects.
     * @param values
     *            Received values.
     */
int CGXDLMSClient::UpdateValues(
    std::vector<std::pair<CGXDLMSObject*, unsigned char> >& list,
    std::vector<CGXDLMSVariant>& values)
{
    int ret, pos = 0;
    for (std::vector<std::pair<CGXDLMSObject*, unsigned char> >::iterator it = list.begin(); it != list.end(); ++it)
    {
        CGXDLMSValueEventArg e(NULL, it->first, it->second);
        e.SetValue(values.at(pos));
        if ((ret = it->first->SetValue(m_Settings, e)) != 0)
        {
            return ret;
        }
        ++pos;
    }
    return 0;
}

int CGXDLMSClient::Write(
    CGXDLMSObject* pObject,
    int index,
    std::vector<CGXByteBuffer>& reply)
{
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    CGXDLMSValueEventArg e(pObject, index);
    int ret = pObject->GetValue(m_Settings, e);
    if (ret == 0)
    {
        CGXDLMSVariant name = pObject->GetName();
        CGXDLMSVariant value = e.GetValue();
        int ret;
        m_Settings.ResetBlockIndex();
        CGXByteBuffer bb, data;
        if (e.IsByteArray())
        {
            data.Set(value.byteArr, value.GetSize());
        }
        else
        {
            DLMS_DATA_TYPE type = DLMS_DATA_TYPE_NONE;
            if ((ret = pObject->GetDataType(index, type)) != 0)
            {
                return ret;
            }
            /*
            if (value.vt != DLMS_DATA_TYPE_NONE)
            {
                if ((ret = pObject->GetDataType(index, type)) != 0)
                {
                    return ret;
                }
                if (value.vt != type)
                {
                    if ((ret = value.ChangeType(type)) != 0)
                    {
                        return ret;
                    }
                }
            }*/
            if ((ret = GXHelpers::SetData(data, type, value)) != 0)
            {
                return ret;
            }
        }
        if (GetUseLogicalNameReferencing())
        {
            // Add CI.
            bb.SetUInt16(pObject->GetObjectType());
            // Add LN.
            unsigned char ln[6];
            GXHelpers::SetLogicalName(name.strVal.c_str(), ln);
            bb.Set(ln, 6);
            // Attribute ID.
            bb.SetUInt8(index);
            // Access selection is not used.
            bb.SetUInt8(0);
            CGXDLMSLNParameters p(&m_Settings,
                DLMS_COMMAND_SET_REQUEST, DLMS_SET_COMMAND_TYPE_NORMAL,
                &bb, &data, 0xff);
            ret = CGXDLMS::GetLnMessages(p, reply);
        }
        else
        {
            // Add name.
            int sn = name.ToInteger();
            sn += (index - 1) * 8;
            bb.SetUInt16(sn);
            // Add data count.
            bb.SetUInt8(1);
            CGXDLMSSNParameters p(&m_Settings,
                DLMS_COMMAND_WRITE_REQUEST, 1,
                DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME,
                &bb, &data);
            ret = CGXDLMS::GetSnMessages(p, reply);
        }
        return ret;
    }
    return ret;
}


int CGXDLMSClient::Write(CGXDLMSObject* pObject,
    int index,
    CGXDLMSVariant& data,
    std::vector<CGXByteBuffer>& reply)
{
    CGXDLMSVariant name = pObject->GetName();
    return Write(name, pObject->GetObjectType(), index, data, reply);
}

int CGXDLMSClient::Write(CGXDLMSVariant& name, DLMS_OBJECT_TYPE objectType,
    int index, CGXDLMSVariant& value, std::vector<CGXByteBuffer>& reply)
{
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    int ret;
    m_Settings.ResetBlockIndex();
    CGXByteBuffer bb, data;
    if ((ret = GXHelpers::SetData(data, value.vt, value)) != 0)
    {
        return ret;
    }

    if (GetUseLogicalNameReferencing())
    {
        // Add CI.
        bb.SetUInt16(objectType);
        // Add LN.
        unsigned char ln[6];
        GXHelpers::SetLogicalName(name.strVal.c_str(), ln);
        bb.Set(ln, 6);
        // Attribute ID.
        bb.SetUInt8(index);
        // Access selection is not used.
        bb.SetUInt8(0);
        CGXDLMSLNParameters p(&m_Settings,
            DLMS_COMMAND_SET_REQUEST, DLMS_SET_COMMAND_TYPE_NORMAL,
            &bb, &data, 0xff);
        ret = CGXDLMS::GetLnMessages(p, reply);
    }
    else
    {
        // Add name.
        int sn = name.ToInteger();
        sn += (index - 1) * 8;
        bb.SetUInt16(sn);
        // Add data count.
        bb.SetUInt8(1);
        CGXDLMSSNParameters p(&m_Settings,
            DLMS_COMMAND_WRITE_REQUEST, 1,
            DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME,
            &bb, &data);
        ret = CGXDLMS::GetSnMessages(p, reply);
    }
    return ret;
}

/**
    * Generate Method (Action) request.
    *
    * @param item
    *            Method object short name or Logical Name.
    * @param index
    *            Method index.
    * @param data
    *            Method data.
    * @param type
    *            Data type.
    * @return DLMS action message.
    */
int CGXDLMSClient::Method(CGXDLMSObject* item, int index,
    CGXDLMSVariant& data, std::vector<CGXByteBuffer>& reply)
{
    CGXDLMSVariant name = item->GetName();
    return Method(name, item->GetObjectType(), index, data, reply);
}

/**
    * Generate Method (Action) request..
    *
    * @param name
    *            Method object short name or Logical Name.
    * @param objectType
    *            Object type.
    * @param index
    *            Method index.
    * @param value
    *            Method data.
    * @param dataType
    *            Data type.
    * @return DLMS action message.
    */
int CGXDLMSClient::Method(CGXDLMSVariant name, DLMS_OBJECT_TYPE objectType,
    int index, CGXDLMSVariant& value, std::vector<CGXByteBuffer>& reply)
{
    int ret;
    if (index < 1)
    {
        //Invalid parameter
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    CGXByteBuffer bb, data;
    m_Settings.ResetBlockIndex();
    if (value.vt != DLMS_DATA_TYPE_NONE)
    {
        if (value.vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            data.Set(value.byteArr, value.size);
        }
        else
        {
            if ((ret = GXHelpers::SetData(data, value.vt, value)) != 0)
            {
                return ret;
            }
        }
    }
    if (GetUseLogicalNameReferencing())
    {
        // CI
        bb.SetUInt16(objectType);
        // Add LN.
        unsigned char ln[6];
        GXHelpers::SetLogicalName(name.strVal.c_str(), ln);
        bb.Set(ln, 6);
        // Attribute ID.
        bb.SetUInt8(index);
        // Method Invocation Parameters is not used.
        if (value.vt == DLMS_DATA_TYPE_NONE)
        {
            bb.SetUInt8(0);
        }
        else
        {
            bb.SetUInt8(1);
        }
        CGXDLMSLNParameters p(&m_Settings,
            DLMS_COMMAND_METHOD_REQUEST, DLMS_ACTION_COMMAND_TYPE_NORMAL,
            &bb, &data, 0xff);
        ret = CGXDLMS::GetLnMessages(p, reply);
    }
    else
    {
        int requestType;
        if (value.vt == DLMS_DATA_TYPE_NONE)
        {
            requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_VARIABLE_NAME;
        }
        else
        {
            requestType = DLMS_VARIABLE_ACCESS_SPECIFICATION_PARAMETERISED_ACCESS;
        }
        unsigned char ind, count;
        if ((ret = CGXDLMS::GetActionInfo(objectType, ind, count)) != 0)
        {
            return ret;
        }

        if (index > count)
        {
            //Invalid parameter
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        int sn = name.ToInteger();
        index = (ind + (index - 1) * 0x8);
        sn += index;
        // Add SN count.
        bb.SetUInt8(1);
        // Add name length.
        bb.SetUInt8(4);
        // Add name.
        bb.SetUInt16(sn);
        // Method Invocation Parameters is not used.
        if (value.vt == DLMS_DATA_TYPE_NONE)
        {
            bb.SetUInt8(0);
        }
        else
        {
            bb.SetUInt8(1);
        }
        CGXDLMSSNParameters p(&m_Settings, DLMS_COMMAND_READ_REQUEST, 1,
            requestType, &bb, &data);
        ret = CGXDLMS::GetSnMessages(p, reply);
    }

    return ret;
}

int CGXDLMSClient::ReadRowsByEntry(
    CGXDLMSProfileGeneric* pg,
    int index,
    int count,
    std::vector<CGXByteBuffer>& reply)
{
    std::vector<std::pair<CGXDLMSObject*, CGXDLMSCaptureObject*> > cols;
    return ReadRowsByEntry(pg, index, count, cols, reply);
}

int CGXDLMSClient::ReadRowsByEntry(
    CGXDLMSProfileGeneric* pg,
    int index,
    int count,
    std::vector<std::pair<CGXDLMSObject*, CGXDLMSCaptureObject*> >& columns,
    std::vector<CGXByteBuffer>& reply)
{
    CGXByteBuffer buff(19);
    if (pg == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pg->Reset();
    // Add AccessSelector value
    buff.SetUInt8(0x02);
    // Add enum tag.
    buff.SetUInt8(DLMS_DATA_TYPE_STRUCTURE);
    // Add item count
    buff.SetUInt8(0x04);
    // Add start index
    CGXDLMSVariant tmp = index;
    GXHelpers::SetData(buff, DLMS_DATA_TYPE_UINT32, tmp);
    // Add Count
    tmp = count;
    GXHelpers::SetData(buff, DLMS_DATA_TYPE_UINT32, tmp);
    // Read all columns.
    if (GetUseLogicalNameReferencing())
    {
        tmp = 1;
        GXHelpers::SetData(buff, DLMS_DATA_TYPE_UINT16, tmp);
    }
    else
    {
        tmp = 0;
        GXHelpers::SetData(buff, DLMS_DATA_TYPE_UINT16, tmp);
    }
    tmp = 0;
    GXHelpers::SetData(buff, DLMS_DATA_TYPE_UINT16, tmp);
    CGXDLMSVariant name = pg->GetName();
    return Read(name, DLMS_OBJECT_TYPE_PROFILE_GENERIC, 2, &buff, reply);
}

int CGXDLMSClient::ReadRowsByRange(
    CGXDLMSProfileGeneric* pObject,
    CGXDateTime& start,
    CGXDateTime& end,
    std::vector<CGXByteBuffer>& reply)
{
    return ReadRowsByRange(pObject, &start.GetValue(), &end.GetValue(), reply);
}


int CGXDLMSClient::ReadRowsByRange(
    CGXDLMSProfileGeneric* pg,
    struct tm* start,
    struct tm* end,
    std::vector<CGXByteBuffer>& reply)
{
    std::vector<std::pair<CGXDLMSObject*, CGXDLMSCaptureObject*> > columns;
    return ReadRowsByRange(pg, start, end, columns, reply);
}

int CGXDLMSClient::ReadRowsByRange(
    CGXDLMSProfileGeneric* pg,
    struct tm* start,
    struct tm* end,
    std::vector<std::pair<CGXDLMSObject*, CGXDLMSCaptureObject*> >& columns,
    std::vector<CGXByteBuffer>& reply)
{
    int ret;
    unsigned char LN[] = { 0, 0, 1, 0, 0, 255 };
    CGXByteBuffer buff(51);
    CGXDLMSVariant name = pg->GetName();
    if (pg == NULL || start == NULL || end == NULL)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    pg->Reset();
    m_Settings.ResetBlockIndex();
    // Add AccessSelector value.
    buff.SetUInt8(0x01);
    // Add enum tag.
    buff.SetUInt8(DLMS_DATA_TYPE_STRUCTURE);
    // Add item count
    buff.SetUInt8(0x04);
    // Add enum tag.
    buff.SetUInt8(0x02);
    // Add item count
    buff.SetUInt8(0x04);
    // CI
    CGXDLMSVariant tmp = DLMS_OBJECT_TYPE_CLOCK;
    GXHelpers::SetData(buff, DLMS_DATA_TYPE_UINT16, tmp);
    // LN
    CGXDLMSVariant ln(LN, 6, DLMS_DATA_TYPE_OCTET_STRING);
    GXHelpers::SetData(buff, DLMS_DATA_TYPE_OCTET_STRING, ln);
    // Add attribute index.
    tmp = 2;
    GXHelpers::SetData(buff, DLMS_DATA_TYPE_INT8, tmp);
    // Add version
    tmp = 0;
    if ((ret = GXHelpers::SetData(buff, DLMS_DATA_TYPE_UINT16, tmp)) != 0)
    {
        return ret;
    }
    // Add start time
    tmp = *start;
    tmp.dateTime.SetSkip((DATETIME_SKIPS)(tmp.dateTime.GetSkip() | DATETIME_SKIPS_MS));
    if ((ret = GXHelpers::SetData(buff, DLMS_DATA_TYPE_OCTET_STRING, tmp)) != 0)
    {
        return ret;
    }
    // Add end time
    tmp = *end;
    tmp.dateTime.SetSkip((DATETIME_SKIPS)(tmp.dateTime.GetSkip() | DATETIME_SKIPS_MS));
    if ((ret = GXHelpers::SetData(buff, DLMS_DATA_TYPE_OCTET_STRING, tmp)) != 0)
    {
        return ret;
    }
    // Add array of read columns. Read All...
    buff.SetUInt8(0x01);
    // Add item count
    buff.SetUInt8(0x00);
    return Read(name, DLMS_OBJECT_TYPE_PROFILE_GENERIC, 2, &buff, reply);
}

int CGXDLMSClient::GetServerAddress(unsigned long serialNumber,
    const char* formula)
{
    // If formula is not given use default formula.
    // This formula is defined in DLMS specification.
    if (formula == NULL || strlen(formula) == 0)
    {
        return 0x4000 | CGXSerialNumberCounter::Count(serialNumber, "SN % 10000 + 1000");
    }
    return 0x4000 | CGXSerialNumberCounter::Count(serialNumber, formula);
}

int  CGXDLMSClient::GetServerAddress(unsigned long logicalAddress,
    unsigned long physicalAddress, unsigned char addressSize)
{
    if (addressSize < 4 && physicalAddress < 0x80
        && logicalAddress < 0x80)
    {
        return logicalAddress << 7 | physicalAddress;
    }
    if (physicalAddress < 0x4000 && logicalAddress < 0x4000)
    {
        return logicalAddress << 14 | physicalAddress;
    }
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
}
