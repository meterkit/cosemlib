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

#include "../include/GXDLMSImageTransfer.h"
#include "../include/GXDLMSClient.h"
#include <sstream>

//Constructor.
CGXDLMSImageTransfer::CGXDLMSImageTransfer() : CGXDLMSObject(DLMS_OBJECT_TYPE_IMAGE_TRANSFER)
{
    m_ImageBlockSize = 0;
    m_ImageFirstNotTransferredBlockNumber = 0;
    m_ImageTransferEnabled = false;
    m_ImageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED;
}

//SN Constructor.
CGXDLMSImageTransfer::CGXDLMSImageTransfer(unsigned short sn) : CGXDLMSObject(DLMS_OBJECT_TYPE_IMAGE_TRANSFER, sn)
{
    m_ImageBlockSize = 0;
    m_ImageFirstNotTransferredBlockNumber = 0;
    m_ImageTransferEnabled = false;
    m_ImageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED;
}

//LN Constructor.
CGXDLMSImageTransfer::CGXDLMSImageTransfer(std::string ln) : CGXDLMSObject(DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln)
{
    m_ImageBlockSize = 0;
    m_ImageFirstNotTransferredBlockNumber = 0;
    m_ImageTransferEnabled = false;
    m_ImageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED;
}
/**
 Holds the ImageBlockSize, expressed in octets,
 * which can be handled by the server
*/
long CGXDLMSImageTransfer::GetImageBlockSize()
{
    return m_ImageBlockSize;
}
void CGXDLMSImageTransfer::SetImageBlockSize(long value)
{
    m_ImageBlockSize = value;
}

/**
 * Provides information about the transfer status of each
 * ImageBlock. Each bit in the bit-std::string provides information about
 * one individual ImageBlock.
*/
std::string& CGXDLMSImageTransfer::GetImageTransferredBlocksStatus()
{
    return m_ImageTransferredBlocksStatus;
}
void CGXDLMSImageTransfer::SetImageTransferredBlocksStatus(std::string value)
{
    m_ImageTransferredBlocksStatus = value;
}

/**
 Provides the ImageBlockNumber of the first ImageBlock not transferred.
 * NOTE If the Image is complete, the value returned should be above the
 * number of blocks calculated from the Image size and the ImageBlockSize
*/
long CGXDLMSImageTransfer::GetImageFirstNotTransferredBlockNumber()
{
    return m_ImageFirstNotTransferredBlockNumber;
}
void CGXDLMSImageTransfer::SetImageFirstNotTransferredBlockNumber(long value)
{
    m_ImageFirstNotTransferredBlockNumber = value;
}

/**
 * Controls enabling the Image transfer process. The method can
 * be invoked successfully only if the value of this attribute is true.
 */
bool CGXDLMSImageTransfer::GetImageTransferEnabled()
{
    return m_ImageTransferEnabled;
}
void CGXDLMSImageTransfer::SetImageTransferEnabled(bool value)
{
    m_ImageTransferEnabled = value;
}

/**
 * Holds the status of the Image transfer process.
 */
DLMS_IMAGE_TRANSFER_STATUS CGXDLMSImageTransfer::GetImageTransferStatus()
{
    return m_ImageTransferStatus;
}
void CGXDLMSImageTransfer::SetImageTransferStatus(DLMS_IMAGE_TRANSFER_STATUS value)
{
    m_ImageTransferStatus = value;
}

std::vector<CGXDLMSImageActivateInfo>& CGXDLMSImageTransfer::GetImageActivateInfo()
{
    return m_ImageActivateInfo;
}

// Returns amount of attributes.
int CGXDLMSImageTransfer::GetAttributeCount()
{
    return 7;
}

// Returns amount of methods.
int CGXDLMSImageTransfer::GetMethodCount()
{
    return 4;
}

void CGXDLMSImageTransfer::GetValues(std::vector<std::string>& values)
{
    values.clear();
    std::string ln;
    GetLogicalName(ln);
    values.push_back(ln);
    values.push_back(CGXDLMSVariant(m_ImageBlockSize).ToString());
    values.push_back(CGXDLMSVariant(m_ImageTransferredBlocksStatus).ToString());
    values.push_back(CGXDLMSVariant(m_ImageFirstNotTransferredBlockNumber).ToString());
    values.push_back(CGXDLMSVariant(m_ImageTransferEnabled).ToString());
    values.push_back(CGXDLMSVariant(m_ImageTransferStatus).ToString());
    std::stringstream sb;
    sb << '[';
    bool empty = true;
    for (std::vector<CGXDLMSImageActivateInfo>::iterator it = m_ImageActivateInfo.begin(); it != m_ImageActivateInfo.end(); ++it)
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
}

void CGXDLMSImageTransfer::GetAttributeIndexToRead(std::vector<int>& attributes)
{
    //LN is static and read only once.
    if (CGXDLMSObject::IsLogicalNameEmpty(m_LN))
    {
        attributes.push_back(1);
    }
    //ImageBlockSize
    if (!IsRead(2))
    {
        attributes.push_back(2);
    }
    //ImageTransferredBlocksStatus
    if (!IsRead(3))
    {
        attributes.push_back(3);
    }
    //ImageFirstNotTransferredBlockNumber
    if (!IsRead(4))
    {
        attributes.push_back(4);
    }
    //ImageTransferEnabled
    if (!IsRead(5))
    {
        attributes.push_back(5);
    }
    //ImageTransferStatus
    if (!IsRead(6))
    {
        attributes.push_back(6);
    }
    //ImageActivateInfo
    if (!IsRead(7))
    {
        attributes.push_back(7);
    }
}

int CGXDLMSImageTransfer::GetDataType(int index, DLMS_DATA_TYPE& type)
{
    if (index == 1)
    {
        type = DLMS_DATA_TYPE_OCTET_STRING;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 2)
    {
        type = DLMS_DATA_TYPE_UINT32;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 3)
    {
        type = DLMS_DATA_TYPE_BIT_STRING;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 4)
    {
        type = DLMS_DATA_TYPE_UINT32;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 5)
    {
        type = DLMS_DATA_TYPE_BOOLEAN;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 6)
    {
        type = DLMS_DATA_TYPE_ENUM;
        return DLMS_ERROR_CODE_OK;
    }
    if (index == 7)
    {
        type = DLMS_DATA_TYPE_ARRAY;
        return DLMS_ERROR_CODE_OK;
    }
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
}

// Returns value of given attribute.
int CGXDLMSImageTransfer::GetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
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
        e.SetValue(GetImageBlockSize());
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 3)
    {
        e.SetValue(m_ImageTransferredBlocksStatus);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 4)
    {
        e.SetValue(m_ImageFirstNotTransferredBlockNumber);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 5)
    {
        e.SetValue(m_ImageTransferEnabled);
        return DLMS_ERROR_CODE_OK;

    }
    if (e.GetIndex() == 6)
    {
        e.SetValue(m_ImageTransferStatus);
        return DLMS_ERROR_CODE_OK;
    }
    if (e.GetIndex() == 7)
    {
        e.SetByteArray(true);
        CGXByteBuffer data;
        data.SetUInt8(DLMS_DATA_TYPE_ARRAY);
        GXHelpers::SetObjectCount((unsigned long)m_ImageActivateInfo.size(), data); //Count
        int ret;
        CGXDLMSVariant size, id, signature;
        for (std::vector<CGXDLMSImageActivateInfo>::iterator it = m_ImageActivateInfo.begin(); it != m_ImageActivateInfo.end(); ++it)
        {
            data.SetUInt8(DLMS_DATA_TYPE_STRUCTURE);
            data.SetUInt8(3);//Item count.
            size = it->GetSize();
            id = it->GetIdentification();
            signature = (*it).GetSignature();
            if ((ret = GXHelpers::SetData(data, DLMS_DATA_TYPE_UINT32, size)) != 0 ||
                (ret = GXHelpers::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, id)) != 0 ||
                (ret = GXHelpers::SetData(data, DLMS_DATA_TYPE_OCTET_STRING, signature)) != 0)
            {
                return ret;
            }
        }
        e.SetValue(data);
        return DLMS_ERROR_CODE_OK;
    }
    return DLMS_ERROR_CODE_INVALID_PARAMETER;
}

// Set value of given attribute.
int CGXDLMSImageTransfer::SetValue(CGXDLMSSettings& settings, CGXDLMSValueEventArg& e)
{
    if (e.GetIndex() == 1)
    {
        return SetLogicalName(this, e.GetValue());
    }
    else if (e.GetIndex() == 2)
    {
        m_ImageBlockSize = e.GetValue().ToInteger();
    }
    else if (e.GetIndex() == 3)
    {
        m_ImageTransferredBlocksStatus = e.GetValue().ToString();
    }
    else if (e.GetIndex() == 4)
    {
        m_ImageFirstNotTransferredBlockNumber = e.GetValue().ToInteger();
    }
    else if (e.GetIndex() == 5)
    {
        m_ImageTransferEnabled = e.GetValue().boolVal;
    }
    else if (e.GetIndex() == 6)
    {
        m_ImageTransferStatus = (DLMS_IMAGE_TRANSFER_STATUS)e.GetValue().ToInteger();
    }
    else if (e.GetIndex() == 7)
    {
        m_ImageActivateInfo.clear();
        if (e.GetValue().vt == DLMS_DATA_TYPE_ARRAY)
        {
            CGXDLMSVariant tmp;
            for (std::vector<CGXDLMSVariant>::iterator it = e.GetValue().Arr.begin(); it != e.GetValue().Arr.end(); ++it)
            {
                CGXDLMSImageActivateInfo item;
                item.SetSize((*it).Arr[0].ToInteger());
                CGXDLMSClient::ChangeType((*it).Arr[1], DLMS_DATA_TYPE_STRING, tmp);
                item.SetIdentification(tmp.ToString());
                CGXDLMSClient::ChangeType((*it).Arr[2], DLMS_DATA_TYPE_STRING, tmp);
                item.SetSignature(tmp.ToString());
                m_ImageActivateInfo.push_back(item);
            }
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}
