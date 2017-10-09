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

#include "../include/GXDLMS.h"
#include "../include/GXAPDU.h"
#include "../include/GXDLMSClient.h"
#include "../include/GXDLMSObjectFactory.h"
#include "../include/GXBytebuffer.h"

static unsigned char CIPHERING_HEADER_SIZE = 7 + 12 + 3;
//CRC table.
static unsigned short FCS16Table[256] =
{
    0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
    0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
    0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
    0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
    0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
    0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
    0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
    0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
    0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
    0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
    0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
    0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
    0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
    0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
    0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
    0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
    0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
    0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
    0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
    0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
    0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
    0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
    0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
    0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
    0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
    0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
    0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
    0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
    0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
    0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
    0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
    0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

bool CGXDLMS::IsReplyMessage(DLMS_COMMAND cmd)
{
    return cmd == DLMS_COMMAND_GET_RESPONSE ||
        cmd == DLMS_COMMAND_SET_RESPONSE ||
        cmd == DLMS_COMMAND_METHOD_RESPONSE;
}

int CGXDLMS::GetAddress(long value, unsigned long& address, int& size)
{
    if (value < 0x80)
    {
        address = (unsigned char)(value << 1);
        size = 1;
        return 0;
    }
    else if (value < 0x4000)
    {
        address = (unsigned short)((value & 0x3F80) << 2 | (value & 0x7F) << 1 | 1);
        size = 2;
    }
    else if (value < 0x10000000)
    {
        address = (unsigned long)((value & 0xFE00000) << 4 | (value & 0x1FC000) << 3
            | (value & 0x3F80) << 2 | (value & 0x7F) << 1 | 1);
        size = 4;
    }
    else
    {
        //Invalid address
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMS::CheckInit(CGXDLMSSettings& settings)
{
    if (settings.GetClientAddress() == 0)
    {
        return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
    }
    if (settings.GetServerAddress() == 0)
    {
        return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
    }
    return DLMS_ERROR_CODE_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Get data from Block.
/////////////////////////////////////////////////////////////////////////////
// data : Stored data block.
// index : Position where data starts.
// Returns : Amount of removed bytes.
/////////////////////////////////////////////////////////////////////////////
int GetDataFromBlock(CGXByteBuffer& data, int index)
{
    if (data.GetSize() == data.GetPosition())
    {
        data.Clear();
        return 0;
    }
    int len = data.GetPosition() - index;
    if (len < 0)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    unsigned long pos = data.GetPosition();
    data.SetPosition(pos - len);
    data.Move(pos, pos - len, data.GetSize() - pos);
    return 0;
}

int CGXDLMS::ReceiverReady(
    CGXDLMSSettings& settings,
    DLMS_DATA_REQUEST_TYPES type,
    CGXCipher* cipher,
    CGXByteBuffer& reply)
{
    int ret;
    reply.Clear();
    if (type == DLMS_DATA_REQUEST_TYPES_NONE)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    // Get next frame.
    if ((type & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
    {
        if ((ret = CGXDLMS::GetHdlcFrame(settings, settings.GetReceiverReady(), NULL, reply)) != 0)
        {
            return ret;
        }
        return ret;
    }
    // Get next block.
    DLMS_COMMAND cmd;
    CGXByteBuffer bb(6);
    std::vector<CGXByteBuffer> tmp;
    if (settings.GetUseLogicalNameReferencing())
    {
        if (settings.IsServer())
        {
            cmd = DLMS_COMMAND_GET_RESPONSE;
        }
        else
        {
            cmd = DLMS_COMMAND_GET_REQUEST;
        }
    }
    else
    {
        if (settings.IsServer())
        {
            cmd = DLMS_COMMAND_READ_RESPONSE;
        }
        else
        {
            cmd = DLMS_COMMAND_READ_REQUEST;
        }
    }
    if (settings.GetUseLogicalNameReferencing())
    {
        bb.SetUInt32(settings.GetBlockIndex());
    }
    else
    {
        bb.SetUInt16((unsigned short)settings.GetBlockIndex());
    }
    settings.IncreaseBlockIndex();
    if (settings.GetUseLogicalNameReferencing())
    {
        CGXDLMSLNParameters p(&settings, cmd,
            DLMS_GET_COMMAND_TYPE_NEXT_DATA_BLOCK, &bb, NULL, 0xff);
        ret = GetLnMessages(p, tmp);
    }
    else
    {
        CGXDLMSSNParameters p(&settings, cmd, 1,
            DLMS_VARIABLE_ACCESS_SPECIFICATION_BLOCK_NUMBER_ACCESS,
            &bb, NULL);
        ret = GetSnMessages(p, tmp);
    }
    reply.Set(&tmp.at(0), 0, -1);
    return ret;
}

/**
     * Split DLMS PDU to wrapper frames.
     *
     * @param settings
     *            DLMS settings.
     * @param data
     *            Wrapped data.
     * @return Wrapper frames.
*/
int CGXDLMS::GetWrapperFrame(
    CGXDLMSSettings& settings,
    CGXByteBuffer& data,
    CGXByteBuffer& reply)
{
    reply.Clear();
    // Add version.
    reply.SetUInt16(1);
    if (settings.IsServer())
    {
        reply.SetUInt16((unsigned short)settings.GetServerAddress());
        reply.SetUInt16((unsigned short)settings.GetClientAddress());
    }
    else
    {
        reply.SetUInt16((unsigned short)settings.GetClientAddress());
        reply.SetUInt16((unsigned short)settings.GetServerAddress());
    }
    // Data length.
    reply.SetUInt16((unsigned short)data.GetSize());
    // Data
    reply.Set(&data, data.GetPosition(), -1);

    // Remove sent data in server side.
    if (settings.IsServer())
    {
        if (data.GetSize() == data.GetPosition())
        {
            data.Clear();
        }
        else
        {
            data.Move(data.GetPosition(), 0, data.GetSize() - data.GetPosition());
            data.SetPosition(0);
        }
    }
    return DLMS_ERROR_CODE_OK;
}

/**
 * Get HDLC frame for data.
 *
 * @param settings
 *            DLMS settings.
 * @param frame
 *            Frame ID. If zero new is generated.
 * @param data
 *            Data to add.
 * @return HDLC frame.
 */
int CGXDLMS::GetHdlcFrame(
    CGXDLMSSettings& settings,
    unsigned char frame,
    CGXByteBuffer* data,
    CGXByteBuffer& reply)
{
    reply.Clear();
    unsigned short frameSize;
    int ret, len = 0;
    CGXByteBuffer primaryAddress, secondaryAddress;
    if (settings.IsServer())
    {
        if ((ret = GetAddressBytes(settings.GetClientAddress(), primaryAddress)) != 0)
        {
            return ret;
        }
        if ((ret = GetAddressBytes(settings.GetServerAddress(), secondaryAddress)) != 0)
        {
            return ret;
        }
    }
    else
    {
        if ((ret = GetAddressBytes(settings.GetServerAddress(), primaryAddress)) != 0)
        {
            return ret;
        }
        if ((ret = GetAddressBytes(settings.GetClientAddress(), secondaryAddress)) != 0)
        {
            return ret;
        }
    }

    // Add BOP
    reply.SetUInt8(HDLC_FRAME_START_END);
    frameSize = settings.GetLimits().GetMaxInfoTX().ToInteger();
    // If no data
    if (data == NULL || data->GetSize() == 0)
    {
        reply.SetUInt8(0xA0);
    }
    else if (data->GetSize() - data->GetPosition() <= frameSize)
    {
        len = data->GetSize() - data->GetPosition();
        // Is last packet.
        reply.SetUInt8(0xA0 | ((len >> 8) & 0x7));
    }
    else
    {
        len = frameSize;
        // More data to left.
        reply.SetUInt8(0xA8 | ((len >> 8) & 0x7));
    }
    // Frame len.
    if (len == 0)
    {
        reply.SetUInt8((unsigned char)(5 + primaryAddress.GetSize() +
            secondaryAddress.GetSize() + len));
    }
    else
    {
        reply.SetUInt8((unsigned char)(7 + primaryAddress.GetSize() +
            secondaryAddress.GetSize() + len + 1));
    }
    /*
    // Add primary address.
    reply.Set(&primaryAddress);
    // Add secondary address.
    secondaryAddress.GetData()[1] = secondaryAddress.GetData()[1] | 1;
    reply.Set(&secondaryAddress);
    */

    reply.SetUInt8(0U);
    reply.SetUInt8(2U);
    reply.SetUInt8(0U);
    reply.SetUInt8(0x23U);
    reply.SetUInt8(3U);

    // Add frame ID.
    if (frame == 0)
    {
        reply.SetUInt8(settings.GetNextSend(1));
    }
    else
    {
        reply.SetUInt8(frame);
    }
    // Add header CRC.
    int crc = CountFCS16(reply, 1, reply.GetSize() - 1);
    reply.SetUInt16(crc);
    if (len != 0)
    {
        // Add data.
        reply.Set(data, data->GetPosition(), len);
        // Add data CRC.
        crc = CountFCS16(reply, 1, reply.GetSize() - 1);
        reply.SetUInt16(crc);
    }
    // Add EOP
    reply.SetUInt8(HDLC_FRAME_START_END);
    // Remove sent data in server side.
    if (settings.IsServer())
    {
        if (data != NULL)
        {
            if (data->GetSize() == data->GetPosition())
            {
                data->Clear();
            }
            else
            {
                data->Move(data->GetPosition(), 0, data->GetSize() - data->GetPosition());
                data->SetPosition(0);
            }
        }
    }
    return DLMS_ERROR_CODE_OK;
}

/**
* Get used glo message.
*
* @param command
*            Executed DLMS_COMMAND_
* @return Integer value of glo message.
*/
unsigned char GetGloMessage(DLMS_COMMAND command)
{
    unsigned char cmd;
    switch (command)
    {
    case DLMS_COMMAND_READ_REQUEST:
        cmd = DLMS_COMMAND_GLO_READ_REQUEST;
        break;
    case DLMS_COMMAND_GET_REQUEST:
        cmd = DLMS_COMMAND_GLO_GET_REQUEST;
        break;
    case DLMS_COMMAND_WRITE_REQUEST:
        cmd = DLMS_COMMAND_GLO_WRITE_REQUEST;
        break;
    case DLMS_COMMAND_SET_REQUEST:
        cmd = DLMS_COMMAND_GLO_SET_REQUEST;
        break;
    case DLMS_COMMAND_METHOD_REQUEST:
        cmd = DLMS_COMMAND_GLO_METHOD_REQUEST;
        break;
    case DLMS_COMMAND_READ_RESPONSE:
        cmd = DLMS_COMMAND_GLO_READ_RESPONSE;
        break;
    case DLMS_COMMAND_GET_RESPONSE:
        cmd = DLMS_COMMAND_GLO_GET_RESPONSE;
        break;
    case DLMS_COMMAND_WRITE_RESPONSE:
        cmd = DLMS_COMMAND_GLO_WRITE_RESPONSE;
        break;
    case DLMS_COMMAND_SET_RESPONSE:
        cmd = DLMS_COMMAND_GLO_SET_RESPONSE;
        break;
    case DLMS_COMMAND_METHOD_RESPONSE:
        cmd = DLMS_COMMAND_GLO_METHOD_RESPONSE;
        break;
    default:
        cmd = DLMS_COMMAND_NONE;
    }
    return cmd;
}

unsigned char GetInvokeIDPriority(CGXDLMSSettings& settings)
{
    unsigned char value = 0;
    if (settings.GetPriority() == DLMS_PRIORITY_HIGH)
    {
        value |= 0x80;
    }
    if (settings.GetServiceClass() == DLMS_SERVICE_CLASS_CONFIRMED)
    {
        value |= 0x40;
    }
    value |= settings.GetInvokeID();
    return value;
}

/**
     * Generates Invoke ID and priority.
     *
     * @param settings
     *            DLMS settings.
     * @return Invoke ID and priority.
     */
long GetLongInvokeIDPriority(CGXDLMSSettings& settings)
{
    long value = 0;
    if (settings.GetPriority() == DLMS_PRIORITY_HIGH)
    {
        value = 0x80000000;
    }
    if (settings.GetServiceClass() == DLMS_SERVICE_CLASS_CONFIRMED)
    {
        value |= 0x40000000;
    }
    value |= (settings.GetLongInvokeID() & 0xFFFFFF);
    settings.SetLongInvokeID(settings.GetLongInvokeID() + 1);
    return value;
}

/**
     * Add LLC bytes to generated message.
     *
     * @param settings
     *            DLMS settings.
     * @param data
     *            Data where bytes are added.
     */
void AddLLCBytes(CGXDLMSSettings* settings, CGXByteBuffer& data)
{
    if (settings->IsServer())
    {
        data.Set(LLC_REPLY_BYTES, 3);
    }
    else
    {
        data.Set(LLC_SEND_BYTES, 3);
    }
}

/**
     * Check is all data fit to one data block.
     *
     * @param p
     *            LN parameters.
     * @param reply
     *            Generated reply.
     */
void MultipleBlocks(
    CGXDLMSLNParameters& p,
    CGXByteBuffer& reply,
    unsigned char ciphering)
{
    // Check is all data fit to one message if data is given.
    int len = p.GetData()->GetSize() - p.GetData()->GetPosition();
    if (p.GetAttributeDescriptor() != NULL)
    {
        len += p.GetAttributeDescriptor()->GetSize();
    }
    if (ciphering)
    {
        len += CIPHERING_HEADER_SIZE;
    }
    if (!p.IsMultipleBlocks())
    {
        // Add command type and invoke and priority.
        p.SetMultipleBlocks(2 + reply.GetSize() + len > p.GetSettings()->GetMaxPduSize());
    }
    if (p.IsMultipleBlocks())
    {
        // Add command type and invoke and priority.
        p.SetLastBlock(!(8 + reply.GetSize() + len > p.GetSettings()->GetMaxPduSize()));
    }
    if (p.IsLastBlock())
    {
        // Add command type and invoke and priority.
        p.SetLastBlock(!(8 + reply.GetSize() + len > p.GetSettings()->GetMaxPduSize()));
    }
}

int CGXDLMS::GetLNPdu(
    CGXDLMSLNParameters& p,
    CGXByteBuffer& reply)
{
    int ret;
    unsigned char ciphering = p.GetSettings()->GetCipher() != NULL
        && p.GetSettings()->GetCipher()->GetSecurity() != DLMS_SECURITY_NONE;
    int len = 0;
    if (!ciphering && p.GetSettings()->GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC)
    {
        AddLLCBytes(p.GetSettings(), reply);
    }
    if (p.GetCommand() == DLMS_COMMAND_AARQ)
    {
        reply.Set(p.GetAttributeDescriptor());
    }
    else
    {
        if ((p.GetSettings()->GetNegotiatedConformance() & DLMS_CONFORMANCE_GENERAL_BLOCK_TRANSFER) != 0)
        {
            reply.SetUInt8(DLMS_COMMAND_GENERAL_BLOCK_TRANSFER);
            MultipleBlocks(p, reply, ciphering);
            // Is last block
            if (!p.IsLastBlock())
            {
                reply.SetUInt8(0);
            }
            else
            {
                reply.SetUInt8(0x80);
            }
            // Set block number sent.
            reply.SetUInt8(0);
            // Set block number acknowledged
            reply.SetUInt8((unsigned char)p.GetBlockIndex());
            p.SetBlockIndex(p.GetBlockIndex() + 1);
            // Add APU tag.
            reply.SetUInt8(0);
            // Add Addl fields
            reply.SetUInt8(0);
        }
        // Add command.
        reply.SetUInt8((unsigned char)p.GetCommand());

        if (p.GetCommand() == DLMS_COMMAND_EVENT_NOTIFICATION ||
            p.GetCommand() == DLMS_COMMAND_DATA_NOTIFICATION ||
            p.GetCommand() == DLMS_COMMAND_ACCESS_REQUEST ||
            p.GetCommand() == DLMS_COMMAND_ACCESS_RESPONSE)
        {
            // Add Long-Invoke-Id-And-Priority
            if (p.GetCommand() != DLMS_COMMAND_EVENT_NOTIFICATION)
            {
                if (p.GetInvokeId() != 0)
                {
                    reply.SetUInt32(p.GetInvokeId());
                }
                else
                {
                    reply.SetUInt32(GetLongInvokeIDPriority(*p.GetSettings()));
                }
            }

            // Add date time.
            if (p.GetTime() == NULL)
            {
                reply.SetUInt8(DLMS_DATA_TYPE_NONE);
            }
            else
            {
                // Data is send in octet string. Remove data type.
                int pos = reply.GetSize();
                CGXDLMSVariant tmp = *p.GetTime();
                if ((ret = GXHelpers::SetData(reply, DLMS_DATA_TYPE_OCTET_STRING, tmp)) != 0)
                {
                    return ret;
                }
                reply.Move(pos + 1, pos, reply.GetSize() - pos - 1);
            }
        }
        else if (p.GetCommand() != DLMS_COMMAND_RELEASE_REQUEST)
        {
            // Get request size can be bigger than PDU size.
            if (p.GetCommand() != DLMS_COMMAND_GET_REQUEST && p.GetData() != NULL
                && p.GetData()->GetSize() != 0)
            {
                MultipleBlocks(p, reply, ciphering);
            }
            // Change Request type if Set request and multiple blocks is
            // needed.
            if (p.GetCommand() == DLMS_COMMAND_SET_REQUEST)
            {
                if (p.IsMultipleBlocks())
                {
                    if (p.GetRequestType() == 1)
                    {
                        p.SetRequestType(2);
                    }
                    else if (p.GetRequestType() == 2)
                    {
                        p.SetRequestType(3);
                    }
                }
            }
            // Change request type If get response and multiple blocks is
            // needed.
            if (p.GetCommand() == DLMS_COMMAND_GET_RESPONSE)
            {
                if (p.IsMultipleBlocks())
                {
                    if (p.GetRequestType() == 1)
                    {
                        p.SetRequestType(2);
                    }
                }
            }
            reply.SetUInt8(p.GetRequestType());
            // Add Invoke Id And Priority.
            if (p.GetInvokeId() != 0)
            {
                reply.SetUInt8((unsigned char)p.GetInvokeId());
            }
            else
            {
                reply.SetUInt8(GetInvokeIDPriority(*p.GetSettings()));
            }
        }

        // Add attribute descriptor.
        reply.Set(p.GetAttributeDescriptor());
        if (p.GetCommand() != DLMS_COMMAND_EVENT_NOTIFICATION &&
            p.GetCommand() != DLMS_COMMAND_DATA_NOTIFICATION &&
            (p.GetSettings()->GetNegotiatedConformance() & DLMS_CONFORMANCE_GENERAL_BLOCK_TRANSFER) == 0)
        {
            // If multiple blocks.
            if (p.IsMultipleBlocks())
            {
                // Is last block.
                if (p.IsLastBlock())
                {
                    reply.SetUInt8(1);
                    p.GetSettings()->SetCount(0);
                    p.GetSettings()->SetIndex(0);
                }
                else
                {
                    reply.SetUInt8(0);
                }
                // Block index.
                reply.SetUInt32(p.GetBlockIndex());
                p.SetBlockIndex(p.GetBlockIndex() + 1);
                // Add status if reply.
                if (p.GetStatus() != 0xFF)
                {
                    if (p.GetStatus() != 0 && p.GetCommand() == DLMS_COMMAND_GET_RESPONSE)
                    {
                        reply.SetUInt8(1);
                    }
                    reply.SetUInt8(p.GetStatus());
                }
                // Block size.
                if (p.GetData() != NULL)
                {
                    len = p.GetData()->GetSize() - p.GetData()->GetPosition();
                }
                else
                {
                    len = 0;
                }
                int totalLength = len + reply.GetSize();
                if (ciphering)
                {
                    totalLength += CIPHERING_HEADER_SIZE;
                }

                if (totalLength > p.GetSettings()->GetMaxPduSize())
                {
                    len = p.GetSettings()->GetMaxPduSize() - reply.GetSize();
                    if (ciphering)
                    {
                        len -= CIPHERING_HEADER_SIZE;
                    }
                    len -= GXHelpers::GetObjectCountSizeInBytes(len);
                }
                GXHelpers::SetObjectCount(len, reply);
                reply.Set(p.GetData(), 0, len);
            }
        }
        // Add data that fits to one block.
        if (len == 0)
        {
            // Add status if reply.
            if (p.GetStatus() != 0xFF)
            {
                if (p.GetStatus() != 0
                    && p.GetCommand() == DLMS_COMMAND_GET_RESPONSE)
                {
                    reply.SetUInt8(1);
                }
                reply.SetUInt8(p.GetStatus());
            }
            if (p.GetData() != NULL && p.GetData()->GetSize() != 0)
            {
                len = p.GetData()->GetSize() - p.GetData()->GetPosition();
                // Get request size can be bigger than PDU size.
                if (p.GetCommand() != DLMS_COMMAND_GET_REQUEST && len
                    + reply.GetSize() > p.GetSettings()->GetMaxPduSize())
                {
                    len = p.GetSettings()->GetMaxPduSize() - reply.GetSize()
                        - p.GetData()->GetPosition();
                }
                reply.Set(p.GetData(), p.GetData()->GetPosition(), len);
            }
        }
        if (ciphering)
        {
            CGXByteBuffer tmp;
            ret = p.GetSettings()->GetCipher()->Encrypt(
                p.GetSettings()->GetCipher()->GetSecurity(),
                DLMS_COUNT_TYPE_PACKET,
                p.GetSettings()->GetCipher()->GetFrameCounter(),
                GetGloMessage(p.GetCommand()),
                p.GetSettings()->GetCipher()->GetSystemTitle(),
                reply, tmp);
            if (ret != 0)
            {
                return ret;
            }
            reply.SetSize(0);
            if (p.GetSettings()->GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC)
            {
                AddLLCBytes(p.GetSettings(), reply);
            }
            reply.Set(&tmp, 0, tmp.GetSize());
        }
    }
    return 0;
}

int CGXDLMS::GetLnMessages(
    CGXDLMSLNParameters& p,
    std::vector<CGXByteBuffer>& messages)
{
    int ret;
    CGXByteBuffer reply, tmp;
    unsigned char frame = 0;
    if (p.GetCommand() == DLMS_COMMAND_AARQ)
    {
        frame = 0x10;
    }
    else if (p.GetCommand() == DLMS_COMMAND_EVENT_NOTIFICATION) {
        frame = 0x13;
    }
    do
    {
        if ((ret = GetLNPdu(p, reply)) != 0)
        {
            return ret;
        }
        p.SetLastBlock(true);
        if (p.GetAttributeDescriptor() == NULL)
        {
            p.GetSettings()->IncreaseBlockIndex();
        }
        while (reply.GetPosition() != reply.GetSize())
        {
            if (p.GetSettings()->GetInterfaceType() == DLMS_INTERFACE_TYPE_WRAPPER)
            {
                ret = GetWrapperFrame(*p.GetSettings(), reply, tmp);
            }
            else
            {
                ret = GetHdlcFrame(*p.GetSettings(), frame, &reply, tmp);
                if (ret == 0 && reply.GetPosition() != reply.GetSize())
                {
                    if (p.GetSettings()->IsServer())
                    {
                        frame = 0;
                    }
                    else
                    {
                        frame = p.GetSettings()->GetNextSend(0);
                    }
                }
            }
            if (ret != 0)
            {
                break;
            }
            messages.push_back(tmp);
            tmp.Clear();
        }
        reply.Clear();
    } while (ret == 0 && p.GetData() != NULL && p.GetData()->GetPosition() != p.GetData()->GetSize());
    return ret;
}

int AppendMultipleSNBlocks(
    CGXDLMSSNParameters& p,
    CGXByteBuffer& reply)
{
    bool ciphering = p.GetSettings()->GetCipher() != NULL && p.GetSettings()->GetCipher()->GetSecurity() != DLMS_SECURITY_NONE;
    unsigned long hSize = reply.GetSize() + 3;
    // Add LLC bytes.
    if (p.GetCommand() == DLMS_COMMAND_WRITE_REQUEST
        || p.GetCommand() == DLMS_COMMAND_READ_REQUEST)
    {
        hSize += 1 + GXHelpers::GetObjectCountSizeInBytes(p.GetCount());
    }
    unsigned long maxSize = p.GetSettings()->GetMaxPduSize() - hSize;
    if (ciphering)
    {
        maxSize -= CIPHERING_HEADER_SIZE;
        if (p.GetSettings()->GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC)
        {
            maxSize -= 3;
        }
    }
    maxSize -= GXHelpers::GetObjectCountSizeInBytes(maxSize);
    if (p.GetData()->GetSize() - p.GetData()->GetPosition() > maxSize)
    {
        // More blocks.
        reply.SetUInt8(0);
    }
    else
    {
        // Last block.
        reply.SetUInt8(1);
        maxSize = p.GetData()->GetSize() - p.GetData()->GetPosition();
    }
    // Add block index.
    reply.SetUInt16(p.GetBlockIndex());
    if (p.GetCommand() == DLMS_COMMAND_WRITE_REQUEST)
    {
        p.SetBlockIndex(p.GetBlockIndex() + 1);
        GXHelpers::SetObjectCount(p.GetCount(), reply);
        reply.SetUInt8(DLMS_DATA_TYPE_OCTET_STRING);
    }
    else if (p.GetCommand() == DLMS_COMMAND_READ_REQUEST)
    {
        p.SetBlockIndex(p.GetBlockIndex() + 1);
    }

    GXHelpers::SetObjectCount(maxSize, reply);
    return maxSize;
}

int CGXDLMS::GetSNPdu(
    CGXDLMSSNParameters& p,
    CGXByteBuffer& reply)
{
    int ret;
    unsigned char ciphering = p.GetSettings()->GetCipher() != NULL
        && p.GetSettings()->GetCipher()->GetSecurity() != DLMS_SECURITY_NONE;
    if (!ciphering
        && p.GetSettings()->GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC)
    {
        AddLLCBytes(p.GetSettings(), reply);
    }
    int cnt = 0, cipherSize = 0;
    if (ciphering)
    {
        cipherSize = CIPHERING_HEADER_SIZE;
    }
    if (p.GetData() != NULL)
    {
        cnt = p.GetData()->GetSize() - p.GetData()->GetPosition();
    }
    // Add command.
    if (p.GetCommand() == DLMS_COMMAND_INFORMATION_REPORT) {
        reply.SetUInt8(p.GetCommand());
        // Add date time.
        if (p.GetTime() == NULL)
        {
            reply.SetUInt8(DLMS_DATA_TYPE_NONE);
        }
        else
        {
            // Data is send in octet string. Remove data type.
            int pos = reply.GetSize();
            CGXDLMSVariant tmp = *p.GetTime();
            if ((ret = GXHelpers::SetData(reply, DLMS_DATA_TYPE_OCTET_STRING, tmp)) != 0)
            {
                return ret;
            }
            reply.Move(pos + 1, pos, reply.GetSize() - pos - 1);
        }
        GXHelpers::SetObjectCount(p.GetCount(), reply);
        reply.Set(p.GetAttributeDescriptor());
    }
    else if (p.GetCommand() != DLMS_COMMAND_AARQ && p.GetCommand() != DLMS_COMMAND_AARE)
    {
        reply.SetUInt8((unsigned char)p.GetCommand());
        if (p.GetCount() != 0xFF)
        {
            GXHelpers::SetObjectCount(p.GetCount(), reply);
        }
        if (p.GetRequestType() != 0xFF)
        {
            reply.SetUInt8(p.GetRequestType());
        }
        reply.Set(p.GetAttributeDescriptor());

        if (!p.IsMultipleBlocks())
        {
            p.SetMultipleBlocks(reply.GetSize() + cipherSize + cnt > p.GetSettings()->GetMaxPduSize());
            // If reply data is not fit to one PDU.
            if (p.IsMultipleBlocks())
            {
                reply.SetSize(0);
                if (!ciphering && p.GetSettings()->GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC)
                {
                    AddLLCBytes(p.GetSettings(), reply);
                }
                if (p.GetCommand() == DLMS_COMMAND_WRITE_REQUEST)
                {
                    p.SetRequestType(
                        DLMS_VARIABLE_ACCESS_SPECIFICATION_WRITE_DATA_BLOCK_ACCESS);
                }
                else if (p.GetCommand() == DLMS_COMMAND_READ_REQUEST)
                {
                    p.SetRequestType(DLMS_VARIABLE_ACCESS_SPECIFICATION_READ_DATA_BLOCK_ACCESS);
                }
                else if (p.GetCommand() == DLMS_COMMAND_READ_RESPONSE)
                {
                    p.SetRequestType(DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT);
                }
                else
                {
                    //Invalid command.
                    return DLMS_ERROR_CODE_INVALID_COMMAND;
                }
                reply.SetUInt8((unsigned char)p.GetCommand());
                // Set object count.
                reply.SetUInt8(1);
                if (p.GetRequestType() != 0xFF)
                {
                    reply.SetUInt8(p.GetRequestType());
                }
                cnt = AppendMultipleSNBlocks(p, reply);
            }
        }
        else
        {
            cnt = AppendMultipleSNBlocks(p, reply);
        }
    }
    // Add data.
    if (p.GetData() != NULL)
    {
        reply.Set(p.GetData(), p.GetData()->GetPosition(), cnt);
    }
    // If all data is transfered.
    if (p.GetData() != NULL && p.GetData()->GetPosition() == p.GetData()->GetSize())
    {
        p.GetSettings()->SetIndex(0);
        p.GetSettings()->SetCount(0);
    }
    // If Ciphering is used.
    if (ciphering && p.GetCommand() != DLMS_COMMAND_AARQ
        && p.GetCommand() != DLMS_COMMAND_AARE)
    {
        CGXByteBuffer tmp;
        ret = p.GetSettings()->GetCipher()->Encrypt(
            p.GetSettings()->GetCipher()->GetSecurity(),
            DLMS_COUNT_TYPE_PACKET,
            p.GetSettings()->GetCipher()->GetFrameCounter(),
            GetGloMessage(p.GetCommand()),
            p.GetSettings()->GetCipher()->GetSystemTitle(),
            reply, tmp);
        if (ret != 0)
        {
            return ret;
        }
        reply.SetSize(0);
        if (p.GetSettings()->GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC)
        {
            AddLLCBytes(p.GetSettings(), reply);
        }
        reply.Set(&tmp, 0, tmp.GetSize());
    }
    return 0;
}

int CGXDLMS::GetSnMessages(
    CGXDLMSSNParameters& p,
    std::vector<CGXByteBuffer>& messages)
{
    int ret;
    CGXByteBuffer data, reply;
    unsigned char frame = 0x0;
    if (p.GetCommand() == DLMS_COMMAND_AARQ)
    {
        frame = 0x10;
    }
    else if (p.GetCommand() == DLMS_COMMAND_INFORMATION_REPORT)
    {
        frame = 0x13;
    }
    else if (p.GetCommand() == DLMS_COMMAND_NONE)
    {
        frame = p.GetSettings()->GetNextSend(1);
    }
    do
    {
        ret = GetSNPdu(p, data);
        // Command is not add to next PDUs.
        while (data.GetPosition() != data.GetSize())
        {
            if (p.GetSettings()->GetInterfaceType() == DLMS_INTERFACE_TYPE_WRAPPER)
            {
                ret = GetWrapperFrame(*p.GetSettings(), data, reply);
            }
            else
            {
                ret = GetHdlcFrame(*p.GetSettings(), frame, &data, reply);
                if (data.GetPosition() != data.GetSize())
                {
                    if (p.GetSettings()->IsServer())
                    {
                        frame = 0;
                    }
                    else
                    {
                        frame = p.GetSettings()->GetNextSend(0);
                    }
                }
            }
            if (ret != 0)
            {
                break;
            }
            messages.push_back(reply);
            reply.Clear();
        }
        reply.Clear();
    } while (ret == 0 && p.GetData() != NULL && p.GetData()->GetPosition() != p.GetData()->GetSize());
    return 0;
}

int CGXDLMS::GetHdlcData(
    bool server,
    CGXDLMSSettings& settings,
    CGXByteBuffer& reply,
    CGXReplyData& data,
    unsigned char& frame)
{
    unsigned long packetStartID = reply.GetPosition(), frameLen = 0;
    unsigned long pos;
    unsigned char ch;
    int ret;
    unsigned short crc, crcRead;
    // If whole frame is not received yet.
    if (reply.GetSize() - reply.GetPosition() < 9)
    {
        data.SetComplete(false);
        return 0;
    }
    data.SetComplete(true);
    // Find start of HDLC frame.
    for (pos = reply.GetPosition(); pos < reply.GetSize(); ++pos)
    {
        if ((ret = reply.GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        if (ch == HDLC_FRAME_START_END)
        {
            packetStartID = pos;
            break;
        }
    }
    // Not a HDLC frame.
    // Sometimes meters can send some strange data between DLMS frames.
    if (reply.GetPosition() == reply.GetSize())
    {
        data.SetComplete(false);
        // Not enough data to parse;
        return 0;
    }
    if ((ret = reply.GetUInt8(&frame)) != 0)
    {
        return ret;
    }
    if ((frame & 0xF0) != 0xA0)
    {
        // If same data.
        return GetHdlcData(server, settings, reply, data, frame);
    }
    // Check frame length.
    if ((frame & 0x7) != 0)
    {
        frameLen = ((frame & 0x7) << 8);
    }
    if ((ret = reply.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    // If not enough data.
    frameLen += ch;
    if (reply.GetSize() - reply.GetPosition() + 1 < frameLen)
    {
        data.SetComplete(false);
        reply.SetPosition(packetStartID);
        // Not enough data to parse;
        return 0;
    }
    int eopPos = frameLen + packetStartID + 1;
    if ((ret = reply.GetUInt8(eopPos, &ch)) != 0)
    {
        return ret;
    }
    if (ch != HDLC_FRAME_START_END)
    {
        return DLMS_ERROR_CODE_NOT_REPLY;
    }

    // Check addresses.
//    ret = CheckHdlcAddress(server, settings, reply, eopPos);
//    if (ret != 0)
//    {
//        if (ret == DLMS_ERROR_CODE_FALSE)
//        {
//            // If echo,
//            return GetHdlcData(server, settings, reply, data, frame);
//        }
//        return ret;
//    }
    (void) reply.GetUInt8(&frame);
    (void) reply.GetUInt8(&frame);
    (void) reply.GetUInt8(&frame);
    (void) reply.GetUInt8(&frame);
    (void) reply.GetUInt8(&frame);

    // Is there more data available.
    if ((frame & 0x8) != 0)
    {
        data.SetMoreData((DLMS_DATA_REQUEST_TYPES)(data.GetMoreData() | DLMS_DATA_REQUEST_TYPES_FRAME));
    }
    else
    {
        data.SetMoreData((DLMS_DATA_REQUEST_TYPES)(data.GetMoreData() & ~DLMS_DATA_REQUEST_TYPES_FRAME));
    }
    // Get frame type.
    if ((ret = reply.GetUInt8(&frame)) != 0)
    {
        return ret;
    }
    if (!settings.CheckFrame(frame))
    {
        reply.SetPosition(eopPos + 1);
        return GetHdlcData(server, settings, reply, data, frame);
    }
    // Check that header CRC is correct.
    crc = CountFCS16(reply, packetStartID + 1,
        reply.GetPosition() - packetStartID - 1);

    if ((ret = reply.GetUInt16(&crcRead)) != 0)
    {
        return ret;
    }
    if (crc != crcRead)
    {
        return DLMS_ERROR_CODE_WRONG_CRC;
    }
    // Check that packet CRC match only if there is a data part.
    if (reply.GetPosition() != packetStartID + frameLen + 1)
    {
        crc = CountFCS16(reply, packetStartID + 1,
            frameLen - 2);
        if ((ret = reply.GetUInt16(packetStartID + frameLen - 1, &crcRead)) != 0)
        {
            return ret;
        }
        if (crc != crcRead)
        {
            return DLMS_ERROR_CODE_WRONG_CRC;
        }
        // Remove CRC and EOP from packet length.
        data.SetPacketLength(eopPos - 2);
    }
    else
    {
        data.SetPacketLength(reply.GetPosition() + 1);
    }

    if (frame != 0x13 && (frame & HDLC_FRAME_TYPE_U_FRAME) == HDLC_FRAME_TYPE_U_FRAME)
    {
        // Get Eop if there is no data.
        if (reply.GetPosition() == packetStartID + frameLen + 1)
        {
            // Get EOP.
            if ((ret = reply.GetUInt8(&ch)) != 0)
            {
                return ret;
            }
        }
        if (frame == 0x97)
        {
            return DLMS_ERROR_CODE_UNACCEPTABLE_FRAME;
        }
        data.SetCommand((DLMS_COMMAND)frame);
    }
    else if (frame != 0x13 && (frame & HDLC_FRAME_TYPE_S_FRAME) == HDLC_FRAME_TYPE_S_FRAME)
    {
        // If S-frame
        int tmp = (frame >> 2) & 0x3;
        // If frame is rejected.
        if (tmp == HDLC_CONTROL_FRAME_REJECT)
        {
            return DLMS_ERROR_CODE_REJECTED;
        }
        else if (tmp == HDLC_CONTROL_FRAME_RECEIVE_NOT_READY)
        {
            return DLMS_ERROR_CODE_REJECTED;
        }
        else if (tmp == HDLC_CONTROL_FRAME_RECEIVE_READY)
        {
            // Get next frame.
        }
        // Get Eop if there is no data.
        if (reply.GetPosition() == packetStartID + frameLen + 1)
        {
            // Get EOP.
            if ((ret = reply.GetUInt8(&ch)) != 0)
            {
                return ret;
            }
        }
    }
    else
    {
        // I-frame
        // Get Eop if there is no data.
        if (reply.GetPosition() == packetStartID + frameLen + 1)
        {
            // Get EOP.
            if ((ret = reply.GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            if ((frame & 0x1) == 0x1)
            {
                data.SetMoreData(DLMS_DATA_REQUEST_TYPES_FRAME);
            }
        }
        else
        {
            GetLLCBytes(server, reply);
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMS::GetHDLCAddress(
    CGXByteBuffer& buff,
    unsigned long& address)
{
    unsigned char ch;
    unsigned short s;
    unsigned long l;
    int ret, size = 0;
    address = 0;
    for (unsigned long pos = buff.GetPosition(); pos != buff.GetSize(); ++pos)
    {
        ++size;
        if ((ret = buff.GetUInt8(pos, &ch)) != 0)
        {
            return ret;
        }
        if ((ch & 0x1) == 1)
        {
            break;
        }
    }
    if (size == 1)
    {
        if ((ret = buff.GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        address = ((ch & 0xFE) >> 1);
    }
    else if (size == 2)
    {
        if ((ret = buff.GetUInt16(&s)) != 0)
        {
            return ret;
        }
        address = ((s & 0xFE) >> 1) | ((s & 0xFE00) >> 2);
    }
    else if (size == 4)
    {
        if ((ret = buff.GetUInt32(&l)) != 0)
        {
            return ret;
        }
        address = ((l & 0xFE) >> 1) | ((l & 0xFE00) >> 2)
            | ((l & 0xFE0000) >> 3) | ((l & 0xFE000000) >> 4);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

static void GetServerAddress(int address, int& logical, int& physical)
{
    if (address < 0x4000)
    {
        logical = address >> 7;
        physical = address & 0x7F;
    }
    else
    {
        logical = address >> 14;
        physical = address & 0x3FFF;
    }
}

int CGXDLMS::CheckHdlcAddress(
    bool server,
    CGXDLMSSettings& settings,
    CGXByteBuffer& reply,
    int index)
{
    unsigned char ch;
    unsigned long source, target;
    int ret;
    // Get destination and source addresses.
    if ((ret = GetHDLCAddress(reply, target)) != 0)
    {
        return ret;
    }
    if ((ret = GetHDLCAddress(reply, source)) != 0)
    {
        return ret;
    }
    if (server)
    {
        // Check that server addresses match.
        if (settings.GetServerAddress() != 0 && settings.GetServerAddress() != target)
        {
            // Get frame command.
            if (reply.GetUInt8(reply.GetPosition(), &ch) != 0)
            {
                return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
            }
            //If SNRM and client has not call disconnect and changes client ID.
            if (ch == DLMS_COMMAND_SNRM)
            {
                settings.SetServerAddress(target);
            }
            else
            {
                return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
            }
        }
        else
        {
            settings.SetServerAddress(target);
        }

        // Check that client addresses match.
        if (settings.GetClientAddress() != 0 && settings.GetClientAddress() != source)
        {
            // Get frame command.
            if (reply.GetUInt8(reply.GetPosition(), &ch) != 0)
            {
                return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
            }
            //If SNRM and client has not call disconnect and changes client ID.
            if (ch == DLMS_COMMAND_SNRM)
            {
                settings.SetClientAddress(source);
            }
            else
            {
                return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
            }
        }
        else
        {
            settings.SetClientAddress(source);
        }
    }
    else
    {
        // Check that client addresses match.
        if (settings.GetClientAddress() != target)
        {
            // If echo.
            if (settings.GetClientAddress() == source && settings.GetServerAddress() == target)
            {
                reply.SetPosition(index + 1);
            }
            return DLMS_ERROR_CODE_FALSE;
        }
        // Check that server addresses match.
        if (settings.GetServerAddress() != source)
        {
            //Check logical and physical address separately.
            //This is done because some meters might send four bytes
            //when only two bytes is needed.
            int readLogical, readPhysical, logical, physical;
            GetServerAddress(source, readLogical, readPhysical);
            GetServerAddress(settings.GetServerAddress(), logical, physical);
            if (readLogical != logical || readPhysical != physical)
            {
                return DLMS_ERROR_CODE_FALSE;
            }
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMS::HandleMethodResponse(
    CGXDLMSSettings& settings,
    CGXReplyData& data)
{
    int ret;
    unsigned char ch, type;
    // Get type.
    if ((ret = data.GetData().GetUInt8(&type)) != 0)
    {
        return ret;
    }
    // Get invoke ID and priority.
    if ((ret = data.GetData().GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    //Action-Response-Normal
    if (type == 1)
    {
        if ((ret = data.GetData().GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            return ch;
        }
        // Response normal. Get data if exists.
        if (data.GetData().GetPosition() < data.GetData().GetSize())
        {
            if ((ret = data.GetData().GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            if (ch == 0)
            {
                GetDataFromBlock(data.GetData(), 0);
            }
            else if (ch == 1)
            {
                //Get Data-Access-Result
                if ((ret = data.GetData().GetUInt8(&ch)) != 0)
                {
                    return ret;
                }
                if (ch != 0)
                {

                    if ((ret = data.GetData().GetUInt8(&type)) != 0)
                    {
                        return ret;
                    }
                    //Handle Texas Instrument missing byte here.
                    if (ch == 9 && type == 16)
                    {
                        data.GetData().SetPosition(data.GetData().GetPosition() - 2);
                    }
                    else
                    {
                        return type;
                    }
                }
                GetDataFromBlock(data.GetData(), 0);
            }
            else
            {
                //Invalid tag.
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
        }
    }
    else if (type == 2)
    {
        //Action-Response-With-Pblock
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    else if (type == 3)
    {
        // Action-Response-With-List.
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    else if (type == 4)
    {
        //Action-Response-Next-Pblock
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_COMMAND;
    }
    return DLMS_ERROR_CODE_OK;
}

/**
    * Handle data notification get data from block and/or update error status.
    *
    * @param settings
    *            DLMS settings.
    * @param reply
    *            Received data from the client.
    */
int CGXDLMS::HandleDataNotification(
    CGXDLMSSettings& settings,
    CGXReplyData& reply)
{
    unsigned long id;
    int ret;
    int start = reply.GetData().GetPosition() - 1;
    // Get invoke id.
    if ((ret = reply.GetData().GetUInt32(&id)) != 0)
    {
        return ret;
    }
    // Get date time.
    CGXDataInfo info;
    reply.SetTime(NULL);
    unsigned char len;
    if ((ret = reply.GetData().GetUInt8(&len)) != 0)
    {
        return ret;
    }
    if (len != 0)
    {
        CGXByteBuffer tmp;
        CGXDLMSVariant t;
        tmp.Set(&reply.GetData(), reply.GetData().GetPosition(), len);
        if ((ret = CGXDLMSClient::ChangeType(tmp, DLMS_DATA_TYPE_DATETIME, t)) != 0)
        {
            return ret;
        }
        reply.SetTime(&t.dateTime.GetValue());
    }
    if ((ret = GetDataFromBlock(reply.GetData(), start)) != 0)
    {
        return ret;
    }
    return GetValueFromData(settings, reply);
}

int CGXDLMS::HandleSetResponse(
    CGXDLMSSettings& settings,
    CGXReplyData& data)
{
    unsigned char ch, type, invokeId;
    int ret;
    if ((ret = data.GetData().GetUInt8(&type)) != 0)
    {
        return ret;
    }
    //Invoke ID and priority.
    if ((ret = data.GetData().GetUInt8(&invokeId)) != 0)
    {
        return ret;
    }

    // SetResponseNormal
    if (type == DLMS_SET_RESPONSE_TYPE_NORMAL)
    {
        if ((ret = data.GetData().GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            return ch;
        }
    }
    else if (type == DLMS_SET_RESPONSE_TYPE_DATA_BLOCK || type == DLMS_SET_RESPONSE_TYPE_LAST_DATA_BLOCK)
    {
        unsigned long  tmp;
        if ((ret = data.GetData().GetUInt32(&tmp)) != 0)
        {
            return ret;
        }
    }
    else
    {
        //Invalid data type.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMS::HandleGbt(CGXDLMSSettings& settings, CGXReplyData& data)
{
    int ret;
    unsigned char ch, bn, bna;
    data.SetGbt(true);
    int index = data.GetData().GetPosition() - 1;
    if ((ret = data.GetData().GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    // Is streaming active.
    //TODO: bool streaming = (ch & 0x40) == 1;
    unsigned char window = (ch & 0x3F);
    // Block number.
    if ((ret = data.GetData().GetUInt8(&bn)) != 0)
    {
        return ret;
    }
    // Block number acknowledged.
    if ((ret = data.GetData().GetUInt8(&bna)) != 0)
    {
        return ret;
    }
    // Get APU tag.
    if ((ret = data.GetData().GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0)
    {
        //Invalid APU.
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    // Get Addl tag.
    if ((ret = data.GetData().GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0)
    {
        //Invalid APU.
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = data.GetData().GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    data.SetCommand(DLMS_COMMAND_NONE);
    if (window != 0)
    {
        unsigned long len;
        if ((ret = GXHelpers::GetObjectCount(data.GetData(), len)) != 0)
        {
            return ret;
        }
        if (len != (unsigned long)(data.GetData().GetSize() - data.GetData().GetPosition()))
        {
            data.SetComplete(false);
            return 0;
        }
    }

    if ((ret = GetDataFromBlock(data.GetData(), index)) != 0 ||
        (ret = CGXDLMS::GetPdu(settings, data)) != 0)
    {
        return ret;
    }
    // Is Last block,
    if ((ch & 0x80) == 0)
    {
        data.SetMoreData((DLMS_DATA_REQUEST_TYPES)(data.GetMoreData() | DLMS_DATA_REQUEST_TYPES_BLOCK));
    }
    else
    {
        data.SetMoreData((DLMS_DATA_REQUEST_TYPES)(data.GetMoreData() & ~DLMS_DATA_REQUEST_TYPES_BLOCK));
    }
    // Get data if all data is read or we want to peek data.
    if (data.GetData().GetPosition() != data.GetData().GetSize()
        && (data.GetCommand() == DLMS_COMMAND_READ_RESPONSE
            || data.GetCommand() == DLMS_COMMAND_GET_RESPONSE)
        && (data.GetMoreData() == DLMS_DATA_REQUEST_TYPES_NONE
            || data.GetPeek()))
    {
        data.GetData().SetPosition(0);
        ret = CGXDLMS::GetValueFromData(settings, data);
    }
    return ret;
}

int CGXDLMS::HandledGloRequest(CGXDLMSSettings& settings,
    CGXReplyData& data)
{
    if (settings.GetCipher() == NULL)
    {
        //Secure connection is not supported.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    DLMS_SECURITY security;
    //If all frames are read.
    if ((data.GetMoreData() & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        int ret;
        unsigned char ch;
        data.GetData().SetPosition(data.GetData().GetPosition() - 1);
        if ((ret = settings.GetCipher()->Decrypt(settings.GetSourceSystemTitle(), data.GetData(), security)) != 0)
        {
            return ret;
        }
        // Get command.
        data.GetData().GetUInt8(&ch);
        data.SetCommand((DLMS_COMMAND)ch);
    }
    else
    {
        data.GetData().SetPosition(data.GetData().GetPosition() - 1);
    }
    return 0;
}

int CGXDLMS::HandledGloResponse(
    CGXDLMSSettings& settings,
    CGXReplyData& data, int index)
{
    if (settings.GetCipher() == NULL)
    {
        //Secure connection is not supported.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    //If all frames are read.
    if ((data.GetMoreData() & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        DLMS_SECURITY security;
        data.GetData().SetPosition(data.GetData().GetPosition() - 1);
        CGXByteBuffer bb;
        CGXByteBuffer& tmp = data.GetData();
        bb.Set(&tmp, data.GetData().GetPosition(), data.GetData().GetSize() - data.GetData().GetPosition());
        data.GetData().SetPosition(index);
        data.GetData().SetSize(index);
        settings.GetCipher()->Decrypt(settings.GetSourceSystemTitle(), bb, security);
        data.GetData().Set(&bb);
        data.SetCommand(DLMS_COMMAND_NONE);
        GetPdu(settings, data);
        data.SetCipherIndex((unsigned short)data.GetData().GetSize());
    }
    return 0;
}


int CGXDLMS::GetPdu(
    CGXDLMSSettings& settings,
    CGXReplyData& data)
{
    int ret = DLMS_ERROR_CODE_OK;
    unsigned char ch;
    DLMS_COMMAND cmd = data.GetCommand();
    // If header is not read yet or GBT message.
    if (cmd == DLMS_COMMAND_NONE || data.GetGbt())
    {
        // If PDU is missing.
        if (data.GetData().GetSize() - data.GetData().GetPosition() == 0)
        {
            // Invalid PDU.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        int index = data.GetData().GetPosition();
        // Get Command.
        if ((ret = data.GetData().GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        cmd = (DLMS_COMMAND)ch;
        data.SetCommand(cmd);
        switch (cmd)
        {
        case DLMS_COMMAND_READ_RESPONSE:
            if ((ret = HandleReadResponse(settings, data, index)) != 0)
            {
                if (ret == DLMS_ERROR_CODE_FALSE)
                {
                    return 0;
                }
                return ret;
            }
            break;
        case DLMS_COMMAND_GET_RESPONSE:
            if ((ret = HandleGetResponse(settings, data, index)) != 0)
            {
                if (ret == DLMS_ERROR_CODE_FALSE)
                {
                    return 0;
                }
                return ret;
            }
            break;
        case DLMS_COMMAND_SET_RESPONSE:
            ret = HandleSetResponse(settings, data);
            break;
        case DLMS_COMMAND_WRITE_RESPONSE:
            ret = HandleWriteResponse(data);
            break;
        case DLMS_COMMAND_METHOD_RESPONSE:
            ret = HandleMethodResponse(settings, data);
            break;
        case DLMS_COMMAND_GENERAL_BLOCK_TRANSFER:
            ret = HandleGbt(settings, data);
            break;
        case DLMS_COMMAND_AARQ:
        case DLMS_COMMAND_AARE:
            // This is parsed later.
            data.GetData().SetPosition(data.GetData().GetPosition() - 1);
            break;
        case DLMS_COMMAND_RELEASE_RESPONSE:
            break;
        case DLMS_COMMAND_EXCEPTION_RESPONSE:
            /* TODO:
            throw new GXDLMSException(
                StateError.values()[data.getData().getUInt8() - 1],
                ServiceError.values()[data.getData().getUInt8() - 1]);
                */
        case DLMS_COMMAND_GET_REQUEST:
        case DLMS_COMMAND_READ_REQUEST:
        case DLMS_COMMAND_WRITE_REQUEST:
        case DLMS_COMMAND_SET_REQUEST:
        case DLMS_COMMAND_METHOD_REQUEST:
        case DLMS_COMMAND_RELEASE_REQUEST:
            // Server handles this.
            if ((data.GetMoreData() & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
            {
                break;
            }
            break;
        case DLMS_COMMAND_GLO_READ_REQUEST:
        case DLMS_COMMAND_GLO_WRITE_REQUEST:
        case DLMS_COMMAND_GLO_GET_REQUEST:
        case DLMS_COMMAND_GLO_SET_REQUEST:
        case DLMS_COMMAND_GLO_METHOD_REQUEST:
            if (settings.GetCipher() == NULL)
            {
                //Secure connection is not supported.
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            // If all frames are read.
            if ((data.GetMoreData() & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
            {
                data.GetData().SetPosition(data.GetData().GetPosition() - 1);
                DLMS_SECURITY security;
                if ((ret = settings.GetCipher()->Decrypt(settings.GetSourceSystemTitle(), data.GetData(), security)) != 0)
                {
                    return ret;
                }
                // Get command
                if ((ret = data.GetData().GetUInt8(&ch)) != 0)
                {
                    return ret;
                }
                cmd = (DLMS_COMMAND)ch;
                data.SetCommand(cmd);
            }
            else
            {
                data.GetData().SetPosition(data.GetData().GetPosition() - 1);
            }
            // Server handles this.
            break;
        case DLMS_COMMAND_GLO_READ_RESPONSE:
        case DLMS_COMMAND_GLO_WRITE_RESPONSE:
        case DLMS_COMMAND_GLO_GET_RESPONSE:
        case DLMS_COMMAND_GLO_SET_RESPONSE:
        case DLMS_COMMAND_GLO_METHOD_RESPONSE:
            if (settings.GetCipher() == NULL)
            {
                //Secure connection is not supported.
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
            // If all frames are read.
            if ((data.GetMoreData() & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
            {
                data.GetData().SetPosition(data.GetData().GetPosition() - 1);
                CGXByteBuffer bb(data.GetData());
                data.GetData().SetPosition(index);
                data.GetData().SetSize(index);
                DLMS_SECURITY security;
                if ((ret = settings.GetCipher()->Decrypt(settings.GetSourceSystemTitle(), bb, security)) != 0)
                {
                    return ret;
                }
                data.GetData().Set(&bb, bb.GetPosition(), bb.GetSize() - bb.GetPosition());
                data.SetCommand(DLMS_COMMAND_NONE);
                ret = GetPdu(settings, data);
                data.SetCipherIndex((unsigned short)data.GetData().GetSize());
            }
            break;
        case DLMS_COMMAND_GLO_GENERAL_CIPHERING:
            if (settings.IsServer())
            {
                HandledGloRequest(settings, data);
            }
            else
            {
                HandledGloResponse(settings, data, index);
            }
            break;
        case DLMS_COMMAND_DATA_NOTIFICATION:
            ret = HandleDataNotification(settings, data);
            // Client handles this.
            break;
        case DLMS_COMMAND_EVENT_NOTIFICATION:
            // Client handles this.
            break;
        case DLMS_COMMAND_INFORMATION_REPORT:
            // Client handles this.
            break;
        default:
            // Invalid DLMS_COMMAND_
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    else if ((data.GetMoreData() & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
    {
        // Is whole block is read and if last packet and data is not try to
        // peek.
        if (!data.GetPeek() && data.GetMoreData() == DLMS_DATA_REQUEST_TYPES_NONE)
        {
            if (data.GetCommand() == DLMS_COMMAND_AARE
                || data.GetCommand() == DLMS_COMMAND_AARQ)
            {
                data.GetData().SetPosition(0);
            }
            else
            {
                data.GetData().SetPosition(1);
            }
            settings.ResetBlockIndex();
        }
        // Get command if operating as a server.
        if (settings.IsServer())
        {
            // Ciphered messages are handled after whole PDU is received.
            switch (cmd)
            {
            case DLMS_COMMAND_GLO_READ_REQUEST:
            case DLMS_COMMAND_GLO_WRITE_REQUEST:
            case DLMS_COMMAND_GLO_GET_REQUEST:
            case DLMS_COMMAND_GLO_SET_REQUEST:
            case DLMS_COMMAND_GLO_METHOD_REQUEST:
                data.SetCommand(DLMS_COMMAND_NONE);
                data.GetData().SetPosition(data.GetCipherIndex());
                ret = GetPdu(settings, data);
                break;
            default:
                break;
            }
        }
        else
        {
            // Client do not need a command any more.
            data.SetCommand(DLMS_COMMAND_NONE);
            // Ciphered messages are handled after whole PDU is received.
            switch (cmd)
            {
            case DLMS_COMMAND_GLO_READ_RESPONSE:
            case DLMS_COMMAND_GLO_WRITE_RESPONSE:
            case DLMS_COMMAND_GLO_GET_RESPONSE:
            case DLMS_COMMAND_GLO_SET_RESPONSE:
            case DLMS_COMMAND_GLO_METHOD_RESPONSE:
                data.GetData().SetPosition(data.GetCipherIndex());
                ret = GetPdu(settings, data);
                break;
            default:
                break;
            }
        }
    }

    // Get data only blocks if SN is used. This is faster.
    if (cmd == DLMS_COMMAND_READ_RESPONSE
        && data.GetCommandType() == DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT
        && (data.GetMoreData() &  DLMS_DATA_REQUEST_TYPES_FRAME) != 0) {
        return 0;
    }

    // Get data if all data is read or we want to peek data.
    if (data.GetData().GetPosition() != data.GetData().GetSize()
        && (cmd == DLMS_COMMAND_READ_RESPONSE || cmd == DLMS_COMMAND_GET_RESPONSE)
        && (data.GetMoreData() == DLMS_DATA_REQUEST_TYPES_NONE
            || data.GetPeek()))
    {
        ret = GetValueFromData(settings, data);
    }
    return ret;
}

int CGXDLMS::GetData(CGXDLMSSettings& settings,
    CGXByteBuffer& reply,
    CGXReplyData& data)
{
    int ret;
    unsigned char frame = 0;
    // If DLMS frame is generated.
    if (settings.GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC)
    {
        if ((ret = GetHdlcData(settings.IsServer(), settings, reply, data, frame)) != 0)
        {
            return ret;
        }
    }
    else if (settings.GetInterfaceType() == DLMS_INTERFACE_TYPE_WRAPPER)
    {
        if ((ret = GetTcpData(settings, reply, data)) != 0)
        {
            return ret;
        }
    }
    else
    {
        // Invalid Interface type.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    // If all data is not read yet.
    if (!data.IsComplete())\
    {
        return DLMS_ERROR_CODE_FALSE;
    }
    GetDataFromFrame(reply, data);
    // If keepalive or get next frame request.
    if (frame != 0x13 && (frame & 0x1) != 0)
    {
        if (settings.GetInterfaceType() == DLMS_INTERFACE_TYPE_HDLC && data.GetData().GetSize() != 0)
        {
            if (reply.GetPosition() != reply.GetSize()) {
                reply.SetPosition(reply.GetPosition() + 3);
            }
        }
        if (data.GetCommand() == DLMS_COMMAND_REJECTED)
        {
            return DLMS_ERROR_CODE_REJECTED;
        }
        return DLMS_ERROR_CODE_OK;
    }
    if ((ret = GetPdu(settings, data)) != 0)
    {
        return ret;
    }

    if (data.GetCommand() == DLMS_COMMAND_DATA_NOTIFICATION)
    {
        // Check is there more messages left. This is Push message special
        // case.
        if (reply.GetPosition() == reply.GetSize())
        {
            reply.SetSize(0);
        }
        else
        {
            int cnt = reply.GetSize() - reply.GetPosition();
            if ((ret = reply.Move(reply.GetPosition(), 0, cnt)) != 0)
            {
                return ret;
            }
            reply.SetPosition(0);
        }
    }
    return 0;
}

int CGXDLMS::HandleGetResponse(
    CGXDLMSSettings& settings,
    CGXReplyData& reply,
    int index)
{
    int ret;
    unsigned char ch;
    unsigned long number;
    short type;
    unsigned long count;
    CGXByteBuffer& data = reply.GetData();

    // Get type.
    if ((ret = data.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    type = ch;
    // Get invoke ID and priority.
    if ((ret = data.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    // Response normal
    if (type == 1)
    {
        // Result
        if ((ret = data.GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            if ((ret = data.GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            return ch;
        }
        ret = GetDataFromBlock(data, 0);
    }
    else if (type == 2)
    {
        // GetResponsewithDataBlock
        // Is Last block.
        if ((ret = data.GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        if (ch == 0)
        {
            reply.SetMoreData(
                (DLMS_DATA_REQUEST_TYPES)(reply.GetMoreData() | DLMS_DATA_REQUEST_TYPES_BLOCK));
        }
        else
        {
            reply.SetMoreData(
                (DLMS_DATA_REQUEST_TYPES)(reply.GetMoreData() & ~DLMS_DATA_REQUEST_TYPES_BLOCK));
        }
        // Get Block number.
        if ((ret = data.GetUInt32(&number)) != 0)
        {
            return ret;
        }
        // If meter's block index is zero based or Actaris is read.
        // Actaris SL7000 might return wrong block index sometimes.
        // It's not reseted to 1.
        if (number != 1 && settings.GetBlockIndex() == 1)
        {
            settings.SetBlockIndex(number);
        }
        if (number != settings.GetBlockIndex())
        {
            return DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
        }
        // Get status.
        if ((ret = data.GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            if ((ret = data.GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            return ch;
        }
        else
        {
            // Get data size.
            GXHelpers::GetObjectCount(data, count);
            // if whole block is read.
            if ((reply.GetMoreData() & DLMS_DATA_REQUEST_TYPES_FRAME) == 0)
            {
                // Check Block length.
                if (count > (unsigned long)(data.GetSize() - data.GetPosition()))
                {
                    return DLMS_ERROR_CODE_OUTOFMEMORY;
                }
                reply.SetCommand(DLMS_COMMAND_NONE);
            }
            if ((ret = GetDataFromBlock(data, index)) != 0)
            {
                return ret;
            }
            // If last packet and data is not try to peek.
            if (reply.GetMoreData() == DLMS_DATA_REQUEST_TYPES_NONE)
            {
                if (!reply.GetPeek())
                {
                    data.SetPosition(0);
                    settings.ResetBlockIndex();
                }
            }
        }
    }
    else if (type == 3)
    {
        CGXDLMSVariant values;
        values.vt = DLMS_DATA_TYPE_ARRAY;
        // Get response with list.
        //Get count.
        if ((ret = GXHelpers::GetObjectCount(data, count)) != 0)
        {
            return ret;
        }
        for (int pos = 0; pos != count; ++pos) {
            // Result
            if ((ret = data.GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            if (ch != 0)
            {
                if ((ret = data.GetUInt8(&ch)) != 0)
                {
                    return ret;
                }
                return ch;
            }
            else
            {
                reply.SetReadPosition(reply.GetData().GetPosition());
                GetValueFromData(settings, reply);
                reply.GetData().SetPosition(reply.GetReadPosition());
                values.Arr.push_back(reply.GetValue());
                reply.GetValue().Clear();
            }
        }
        reply.SetValue(values);
        return DLMS_ERROR_CODE_FALSE;
    }
    else
    {
        //Invalid Get response.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return ret;
}

int CGXDLMS::HandleWriteResponse(CGXReplyData& data)
{
    unsigned char ch;
    int ret;
    unsigned long count;
    if ((ret = GXHelpers::GetObjectCount(data.GetData(), count)) != 0)
    {
        return ret;
    }
    for (unsigned long pos = 0; pos != count; ++pos)
    {
        if ((ret = data.GetData().GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        if (ch != 0)
        {
            if ((ret = data.GetData().GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            return ch;
        }
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMS::ReadResponseDataBlockResult(
    CGXDLMSSettings& settings,
    CGXReplyData& reply,
    int index)
{
    int ret;
    unsigned short number;
    unsigned long blockLength;
    unsigned char lastBlock;
    if ((ret = reply.GetData().GetUInt8(&lastBlock)) != 0)
    {
        return ret;
    }
    // Get Block number.
    if ((ret = reply.GetData().GetUInt16(&number)) != 0)
    {
        return ret;
    }
    if ((ret = GXHelpers::GetObjectCount(reply.GetData(), blockLength)) != 0)
    {
        return ret;
    }
    // Is Last block.
    if (!lastBlock)
    {
        reply.SetMoreData((DLMS_DATA_REQUEST_TYPES)(reply.GetMoreData() | DLMS_DATA_REQUEST_TYPES_BLOCK));
    }
    else
    {
        reply.SetMoreData((DLMS_DATA_REQUEST_TYPES)(reply.GetMoreData() & ~DLMS_DATA_REQUEST_TYPES_BLOCK));
    }
    // If meter's block index is zero based.
    if (number != 1 && settings.GetBlockIndex() == 1)
    {
        settings.SetBlockIndex(number);
    }
    int expectedIndex = settings.GetBlockIndex();
    if (number != expectedIndex)
    {
        //Invalid Block number
        return DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
    }
    // If whole block is not read.
    if ((reply.GetMoreData() & DLMS_DATA_REQUEST_TYPES_FRAME) != 0)
    {
        GetDataFromBlock(reply.GetData(), index);
        return DLMS_ERROR_CODE_FALSE;
    }
    if (blockLength != reply.GetData().Available())
    {
        //Invalid block length.
        return DLMS_ERROR_CODE_BLOCK_UNAVAILABLE;
    }
    reply.SetCommand(DLMS_COMMAND_NONE);

    GetDataFromBlock(reply.GetData(), index);
    reply.SetTotalCount(0);
    // If last packet and data is not try to peek.
    if (reply.GetMoreData() == DLMS_DATA_REQUEST_TYPES_NONE)
    {
        settings.ResetBlockIndex();
    }
    return ret;
}


int CGXDLMS::HandleReadResponse(
    CGXDLMSSettings& settings,
    CGXReplyData& reply,
    int index)
{
    unsigned char ch;
    unsigned long pos, cnt = reply.GetTotalCount();
    int ret;
    // If we are reading value first time or block is handed.
    bool first = reply.GetTotalCount() == 0 || reply.GetCommandType() == DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT;
    if (first)
    {
        if ((ret = GXHelpers::GetObjectCount(reply.GetData(), cnt)) != 0)
        {
            return ret;
        }
        reply.SetTotalCount(cnt);
    }

    DLMS_SINGLE_READ_RESPONSE type;
    CGXDLMSVariant values;
    values.vt = DLMS_DATA_TYPE_ARRAY;
    for (pos = 0; pos != cnt; ++pos)
    {
        if (reply.GetData().Available() == 0)
        {
            if (cnt != 1)
            {
                GetDataFromBlock(reply.GetData(), 0);
                reply.SetValue(values);
            }
            return DLMS_ERROR_CODE_FALSE;
        }
        // Get status code. Status code is begin of each PDU.
        if (first) {
            if ((ret = reply.GetData().GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            reply.SetCommandType(ch);
            type = (DLMS_SINGLE_READ_RESPONSE)ch;
        }
        else {
            type = (DLMS_SINGLE_READ_RESPONSE)reply.GetCommandType();
        }
        switch (type)
        {
        case DLMS_SINGLE_READ_RESPONSE_DATA:
            if (cnt == 1)
            {
                ret = GetDataFromBlock(reply.GetData(), 0);
            }
            else
            {
                // If read multiple items.
                reply.SetReadPosition(reply.GetData().GetPosition());
                GetValueFromData(settings, reply);
                if (reply.GetData().GetPosition() == reply.GetReadPosition())
                {
                    // If multiple values remove command.
                    if (cnt != 1 && reply.GetTotalCount() == 0) {
                        ++index;
                    }
                    reply.SetTotalCount(0);
                    reply.GetData().SetPosition(index);
                    GetDataFromBlock(reply.GetData(), 0);
                    reply.GetValue().Clear();
                    // Ask that data is parsed after last block is received.
                    reply.SetCommandType(DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT);
                    return DLMS_ERROR_CODE_FALSE;
                }
                reply.GetData().SetPosition(reply.GetReadPosition());
                values.Arr.push_back(reply.GetValue());
                reply.GetValue().Clear();
            }
            break;
        case DLMS_SINGLE_READ_RESPONSE_DATA_ACCESS_ERROR:
            // Get error code.
            if ((ret = reply.GetData().GetUInt8(&ch)) != 0)
            {
                return ret;
            }
            return ch;
            break;
        case DLMS_SINGLE_READ_RESPONSE_DATA_BLOCK_RESULT:
            if ((ret = ReadResponseDataBlockResult(settings, reply, index)) != 0)
            {
                return ret;
            }
            break;
        case DLMS_SINGLE_READ_RESPONSE_BLOCK_NUMBER:
            // Get Block number.
            unsigned short number;
            if ((ret = reply.GetData().GetUInt16(&number)) != 0)
            {
                return ret;
            }
            if (number != settings.GetBlockIndex())
            {
                //Invalid Block number
                return DLMS_ERROR_CODE_DATA_BLOCK_NUMBER_INVALID;
            }
            settings.IncreaseBlockIndex();
            reply.SetMoreData((DLMS_DATA_REQUEST_TYPES)(reply.GetMoreData() | DLMS_DATA_REQUEST_TYPES_BLOCK));
            break;
        default:
            //HandleReadResponse failed. Invalid tag.
            return DLMS_ERROR_CODE_INVALID_TAG;
        }
    }
    if (values.Arr.size() != 0)
    {
        reply.SetValue(values);
    }
    if (cnt != 1)
    {
        return DLMS_ERROR_CODE_FALSE;
    }
    return 0;
}

int CGXDLMS::GetTcpData(
    CGXDLMSSettings& settings,
    CGXByteBuffer& buff,
    CGXReplyData& data)
{
    int ret;
    // If whole frame is not received yet.
    if (buff.GetSize() - buff.GetPosition() < 8)
    {
        data.SetComplete(false);
        return DLMS_ERROR_CODE_OK;
    }
    int pos = buff.GetPosition();
    unsigned short value;
    // Get version
    if ((ret = buff.GetUInt16(&value)) != 0)
    {
        return ret;
    }
    if (value != 1)
    {
        //Unknown version.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    // Check TCP/IP addresses.
    CheckWrapperAddress(settings, buff);
    // Get length.
    if ((ret = buff.GetUInt16(&value)) != 0)
    {
        return ret;
    }
    bool compleate = !((buff.GetSize() - buff.GetPosition()) < value);
    data.SetComplete(compleate);
    if (!compleate)
    {
        buff.SetPosition(pos);
        return DLMS_ERROR_CODE_FALSE;
    }
    else
    {
        data.SetPacketLength(buff.GetPosition() + value);
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMS::GetAddressBytes(unsigned long value, CGXByteBuffer& bytes)
{
    int ret;
    unsigned long address;
    int size;
    if ((ret = GetAddress(value, address, size)) != 0)
    {
        return ret;
    }
    size = 2;

    if (size == 1)
    {
        bytes.Capacity(1);
        bytes.SetUInt8((unsigned char)address);
    }
    else if (size == 2)
    {
        bytes.Capacity(2);
        bytes.SetUInt16((unsigned short)address);
    }
    else if (size == 4)
    {
        bytes.Capacity(4);
        bytes.SetUInt32((unsigned long)address);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMS::GetValueFromData(CGXDLMSSettings& settings, CGXReplyData& reply)
{
    int ret;
    CGXDataInfo info;
    if (reply.GetValue().vt == DLMS_DATA_TYPE_ARRAY)
    {
        info.SetType(DLMS_DATA_TYPE_ARRAY);
        info.SetCount(reply.GetTotalCount());
        info.SetIndex(reply.GetCount());
    }
    CGXDLMSVariant value;
    int index = reply.GetData().GetPosition();
    reply.GetData().SetPosition(reply.GetReadPosition());
    if ((ret = GXHelpers::GetData(reply.GetData(), info, value)) != 0)
    {
        return ret;
    }
    // If new data.
    if (value.vt != DLMS_DATA_TYPE_NONE)
    {
        if (value.vt != DLMS_DATA_TYPE_ARRAY)
        {
            reply.SetValueType(info.GetType());
            reply.SetValue(value);
            reply.SetTotalCount(0);
            reply.SetReadPosition(reply.GetData().GetPosition());
        }
        else
        {
            if (value.Arr.size() != 0)
            {
                if (reply.GetValue().vt == DLMS_DATA_TYPE_NONE)
                {
                    reply.SetValue(value);
                }
                else
                {
                    CGXDLMSVariant tmp = reply.GetValue();
                    tmp.Arr.insert(tmp.Arr.end(), value.Arr.begin(), value.Arr.end());
                    reply.SetValue(tmp);
                }
            }
            reply.SetReadPosition(reply.GetData().GetPosition());
            // Element count.
            reply.SetTotalCount(info.GetCount());
        }
    }
    else if (info.IsCompleate()
        && reply.GetCommand() == DLMS_COMMAND_DATA_NOTIFICATION)
    {
        // If last item is null. This is a special case.
        reply.SetReadPosition(reply.GetData().GetPosition());
    }
    reply.GetData().SetPosition(index);

    // If last data frame of the data block is read.
    if (reply.GetCommand() != DLMS_COMMAND_DATA_NOTIFICATION
        && info.IsCompleate()
        && reply.GetMoreData() == DLMS_DATA_REQUEST_TYPES_NONE)
    {
        // If all blocks are read.
        settings.ResetBlockIndex();
        reply.GetData().SetPosition(0);
    }
    return 0;
}

void CGXDLMS::GetDataFromFrame(CGXByteBuffer& reply, CGXReplyData& info)
{
    CGXByteBuffer& data = info.GetData();
    int offset = data.GetSize();
    int cnt = info.GetPacketLength() - reply.GetPosition();
    if (cnt != 0)
    {
        data.Capacity(offset + cnt);
        data.Set(&reply, reply.GetPosition(), cnt);
    }
    // Set position to begin of new data.
    data.SetPosition(offset);
}

void CGXDLMS::GetLLCBytes(bool server, CGXByteBuffer& data)
{
    if (server)
    {
        data.Compare((unsigned char*)LLC_SEND_BYTES, 3);
    }
    else
    {
        data.Compare((unsigned char*)LLC_REPLY_BYTES, 3);
    }
}

int CGXDLMS::CheckWrapperAddress(
    CGXDLMSSettings& settings,
    CGXByteBuffer& buff)
{
    int ret;
    unsigned short value;
    if (settings.IsServer())
    {
        if ((ret = buff.GetUInt16(&value)) != 0)
        {
            return ret;
        }
        // Check that client addresses match.
        if (settings.GetClientAddress() != 0
            && settings.GetClientAddress() != value)
        {
            return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
        }
        else
        {
            settings.SetClientAddress(value);
        }

        if ((ret = buff.GetUInt16(&value)) != 0)
        {
            return ret;
        }
        // Check that server addresses match.
        if (settings.GetServerAddress() != 0
            && settings.GetServerAddress() != value)
        {
            return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
        }
        else
        {
            settings.SetServerAddress(value);
        }
    }
    else
    {
        if ((ret = buff.GetUInt16(&value)) != 0)
        {
            return ret;
        }
        // Check that server addresses match.
        if (settings.GetServerAddress() != 0
            && settings.GetServerAddress() != value)
        {
            return DLMS_ERROR_CODE_INVALID_SERVER_ADDRESS;
        }
        else
        {
            settings.SetServerAddress(value);
        }

        if ((ret = buff.GetUInt16(&value)) != 0)
        {
            return ret;
        }
        // Check that client addresses match.
        if (settings.GetClientAddress() != 0
            && settings.GetClientAddress() != value)
        {
            return DLMS_ERROR_CODE_INVALID_CLIENT_ADDRESS;
        }
        else
        {
            settings.SetClientAddress(value);
        }
    }
    return DLMS_ERROR_CODE_OK;
}

unsigned short CGXDLMS::CountFCS16(CGXByteBuffer& buff, int index, int count)
{
    int ret;
    unsigned char ch;
    unsigned short fcs16 = 0xFFFF;
    for (short pos = 0; pos < count; ++pos)
    {
        if ((ret = buff.GetUInt8(index + pos, &ch)) != 0)
        {
            return ret;
        }
        fcs16 = (fcs16 >> 8) ^ FCS16Table[(fcs16 ^ ch) & 0xFF];
    }
    fcs16 = ~fcs16;
    fcs16 = ((fcs16 >> 8) & 0xFF) | (fcs16 << 8);
    return fcs16;
}

int CGXDLMS::GetActionInfo(DLMS_OBJECT_TYPE objectType, unsigned char& value, unsigned char& count)
{
    switch (objectType)
    {
    case DLMS_OBJECT_TYPE_DATA:
    case DLMS_OBJECT_TYPE_ACTION_SCHEDULE:
    case DLMS_OBJECT_TYPE_ALL:
    case DLMS_OBJECT_TYPE_AUTO_ANSWER:
    case DLMS_OBJECT_TYPE_AUTO_CONNECT:
    case DLMS_OBJECT_TYPE_MAC_ADDRESS_SETUP:
    case DLMS_OBJECT_TYPE_GPRS_SETUP:
    case DLMS_OBJECT_TYPE_IEC_HDLC_SETUP:
    case DLMS_OBJECT_TYPE_IEC_LOCAL_PORT_SETUP:
    case DLMS_OBJECT_TYPE_IEC_TWISTED_PAIR_SETUP:
    case DLMS_OBJECT_TYPE_MODEM_CONFIGURATION:
    case DLMS_OBJECT_TYPE_PPP_SETUP:
    case DLMS_OBJECT_TYPE_REGISTER_MONITOR:
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_STARTUP:
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_JOIN:
    case DLMS_OBJECT_TYPE_ZIG_BEE_SAS_APS_FRAGMENTATION:
    case DLMS_OBJECT_TYPE_ZIG_BEE_NETWORK_CONTROL:
    case DLMS_OBJECT_TYPE_SCHEDULE:
    case DLMS_OBJECT_TYPE_SMTP_SETUP:
    case DLMS_OBJECT_TYPE_STATUS_MAPPING:
    case DLMS_OBJECT_TYPE_TCP_UDP_SETUP:
    case DLMS_OBJECT_TYPE_UTILITY_TABLES:
        value = 00;
        count = 0;
        break;
    case DLMS_OBJECT_TYPE_IMAGE_TRANSFER:
        value = 0x40;
        count = 4;
        break;
    case DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR:
        value = 0x50;
        count = 1;
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME:
        value = 0x60;
        count = 4;
        break;
    case DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME:
        value = 0x20;
        count = 8;
        break;
    case DLMS_OBJECT_TYPE_CLOCK:
        value = 0x60;
        count = 6;
        break;
    case DLMS_OBJECT_TYPE_DEMAND_REGISTER:
        value = 0x48;
        count = 2;
        break;
    case DLMS_OBJECT_TYPE_EXTENDED_REGISTER:
        value = 0x38;
        count = 1;
        break;
    case DLMS_OBJECT_TYPE_IP4_SETUP:
        value = 0x60;
        count = 3;
        break;
    case DLMS_OBJECT_TYPE_MBUS_SLAVE_PORT_SETUP:
        value = 0x60;
        count = 8;
        break;
    case DLMS_OBJECT_TYPE_PROFILE_GENERIC:
        value = 0x58;
        count = 4;
        break;
    case DLMS_OBJECT_TYPE_REGISTER:
        value = 0x28;
        count = 1;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_ACTIVATION:
        value = 0x30;
        count = 3;
        break;
    case DLMS_OBJECT_TYPE_REGISTER_TABLE:
        value = 0x28;
        count = 2;
        break;
    case DLMS_OBJECT_TYPE_SAP_ASSIGNMENT:
    case DLMS_OBJECT_TYPE_SCRIPT_TABLE:
        value = 0x20;
        count = 1;
        break;
    case DLMS_OBJECT_TYPE_SPECIAL_DAYS_TABLE:
        value = 0x10;
        count = 2;
        break;
    case DLMS_OBJECT_TYPE_DISCONNECT_CONTROL:
        value = 0x20;
        count = 2;
        break;
    case DLMS_OBJECT_TYPE_PUSH_SETUP:
        value = 0x38;
        count = 1;
        break;
    default:
        count = value = 0;
        break;
    }
    return DLMS_ERROR_CODE_OK;
}

int CGXDLMS::AppendData(
    CGXDLMSObject* obj,
    unsigned char index,
    CGXByteBuffer& bb,
    CGXDLMSVariant& value)
{

    int ret;
    DLMS_DATA_TYPE tp;
    if ((ret = obj->GetDataType(index, tp)) != 0)
    {
        return ret;
    }
    if (tp == DLMS_DATA_TYPE_ARRAY)
    {
        if (value.vt == DLMS_DATA_TYPE_OCTET_STRING)
        {
            bb.Set(value.byteArr, value.GetSize());
            return 0;
        }
    }
    else
    {
        if (tp == DLMS_DATA_TYPE_NONE)
        {
            tp = value.vt;
            // If data type is not defined for Date Time it is write as
            // octet string.
            if (tp == DLMS_DATA_TYPE_DATETIME)
            {
                tp = DLMS_DATA_TYPE_OCTET_STRING;
            }
        }
    }
    return GXHelpers::SetData(bb, tp, value);
}
