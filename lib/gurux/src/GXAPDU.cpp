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

#include "../include/GXAPDU.h"

/**
 * Retrieves the string that indicates the level of authentication, if any.
 */
int GetAuthenticationString(
    CGXDLMSSettings& settings,
    CGXByteBuffer& data)
{
    // If authentication is used.
    if (settings.GetAuthentication() != DLMS_AUTHENTICATION_NONE)
    {
        // Add sender ACSE-requirements field component.
        data.SetUInt8(BER_TYPE_CONTEXT
            | PDU_TYPE_SENDER_ACSE_REQUIREMENTS);
        data.SetUInt8(2);
        data.SetUInt8(BER_TYPE_BIT_STRING
            | BER_TYPE_OCTET_STRING);
        data.SetUInt8(0x80);

        data.SetUInt8(BER_TYPE_CONTEXT
            | PDU_TYPE_MECHANISM_NAME);
        // Len
        data.SetUInt8(7);
        // OBJECT IDENTIFIER
        unsigned char p[] = { 0x60, 0x85, 0x74, 0x05, 0x08, 0x02, (unsigned char)settings.GetAuthentication() };
        data.Set(p, 7);
        // Add Calling authentication information.
        CGXByteBuffer *callingAuthenticationValue;
        if (settings.GetAuthentication() == DLMS_AUTHENTICATION_LOW)
        {
            callingAuthenticationValue = &settings.GetPassword();
        }
        else
        {
            callingAuthenticationValue = &settings.GetCtoSChallenge();
        }
        // 0xAC
        data.SetUInt8(BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AUTHENTICATION_VALUE);
        // Len
        GXHelpers::SetObjectCount(2 + callingAuthenticationValue->GetSize(), data);
        // Add authentication information.
        data.SetUInt8(BER_TYPE_CONTEXT);
        // Len.
        GXHelpers::SetObjectCount(callingAuthenticationValue->GetSize(), data);
        if (callingAuthenticationValue->GetSize() != 0)
        {
            data.Set(callingAuthenticationValue);
        }
    }
    return 0;
}

/**
* Code application context name.
*
* @param settings
*            DLMS settings.
* @param data
*            Byte buffer where data is saved.
* @param cipher
*            Is ciphering settings.
*/
int GenerateApplicationContextName(
    CGXDLMSSettings& settings,
    CGXByteBuffer& data,
    CGXCipher* cipher)
{
    // Application context name tag
    data.SetUInt8((BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_APPLICATION_CONTEXT_NAME));
    // Len
    data.SetUInt8(0x09);
    data.SetUInt8(BER_TYPE_OBJECT_IDENTIFIER);
    // Len
    data.SetUInt8(0x07);
    bool ciphered = cipher != NULL && cipher->IsCiphered();
    if (settings.GetUseLogicalNameReferencing())
    {
        if (ciphered)
        {
            data.Set(LOGICAL_NAME_OBJECT_ID_WITH_CIPHERING, sizeof(LOGICAL_NAME_OBJECT_ID_WITH_CIPHERING));
        }
        else
        {
            data.Set(LOGICAL_NAME_OBJECT_ID, sizeof(LOGICAL_NAME_OBJECT_ID));
        }
    }
    else
    {
        if (ciphered)
        {
            data.Set(SHORT_NAME_OBJECT_ID_WITH_CIPHERING, sizeof(SHORT_NAME_OBJECT_ID_WITH_CIPHERING));
        }
        else
        {
            data.Set(SHORT_NAME_OBJECT_ID, sizeof(SHORT_NAME_OBJECT_ID));
        }
    }
    // Add system title.
    if (!settings.IsServer() &&
        (ciphered || settings.GetAuthentication() == DLMS_AUTHENTICATION_HIGH_GMAC))
    {
        if (cipher->GetSystemTitle().GetSize() == 0)
        {
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
        // Add calling-AP-title
        data.SetUInt8((BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | 6));
        // LEN
        GXHelpers::SetObjectCount(2 + cipher->GetSystemTitle().GetSize(), data);
        data.SetUInt8(BER_TYPE_OCTET_STRING);
        // LEN
        GXHelpers::SetObjectCount(cipher->GetSystemTitle().GetSize(), data);
        data.Set(cipher->GetSystemTitle().GetData(), cipher->GetSystemTitle().GetSize());
    }
    return 0;
}

/**
 * Generate User information initiate request.
 *
 * @param settings
 *            DLMS settings.
 * @param cipher
 * @param data
 */
int GetInitiateRequest(
    CGXDLMSSettings& settings,
    CGXCipher* cipher, CGXByteBuffer& data)
{
    // Tag for xDLMS-Initiate request
    data.SetUInt8(DLMS_COMMAND_INITIATE_REQUEST);
    // Usage field for the response allowed component.

    // Usage field for dedicated-key component. Not used
    data.SetUInt8(0x00);

    // encoding of the response-allowed component (bool DEFAULT TRUE)
    // usage flag (FALSE, default value TRUE conveyed)
    data.SetUInt8(0);

    // Usage field of the proposed-quality-of-service component. Not used
    data.SetUInt8(0x00);
    data.SetUInt8(settings.GetDLMSVersion());
    // Tag for conformance block
    data.SetUInt8(0x5F);
    data.SetUInt8(0x1F);
    // length of the conformance block
    data.SetUInt8(0x04);
    // encoding the number of unused bits in the bit string
    data.SetUInt8(0x00);
    //Add conformance block.
    CGXByteBuffer bb(4);
    bb.SetUInt32((unsigned long)settings.GetProposedConformance());
    data.Set(&bb, 1, 3);
    data.SetUInt16(settings.GetMaxPduSize());
    return 0;
}

int CGXAPDU::GenerateUserInformation(
    CGXDLMSSettings& settings,
    CGXCipher* cipher,
    CGXByteBuffer* encryptedData,
    CGXByteBuffer& data)
{
    int ret;
    data.SetUInt8(BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_USER_INFORMATION);
    if (cipher == NULL || !cipher->IsCiphered())
    {
        // Length for AARQ user field
        data.SetUInt8(0x10);
        // Coding the choice for user-information (Octet STRING, universal)
        data.SetUInt8(BER_TYPE_OCTET_STRING);
        // Length
        data.SetUInt8(0x0E);
        if ((ret = GetInitiateRequest(settings, cipher, data)) != 0)
        {
            return ret;
        }
    }
    else
    {
        if (encryptedData != NULL && encryptedData->GetSize() != 0)
        {
            //Length for AARQ user field
            data.SetUInt8((unsigned char)(4 + encryptedData->GetSize()));
            //Tag
            data.SetUInt8(BER_TYPE_OCTET_STRING);
            data.SetUInt8((unsigned char)(2 + encryptedData->GetSize()));
            //Coding the choice for user-information (Octet STRING, universal)
            data.SetUInt8(DLMS_COMMAND_GLO_INITIATE_REQUEST);
            data.SetUInt8((unsigned char)encryptedData->GetSize());
            data.Set(encryptedData);
        }
        else
        {
            CGXByteBuffer tmp, crypted;
            if ((ret = GetInitiateRequest(settings, cipher, tmp)) != 0)
            {
                return ret;
            }
            if ((ret = cipher->Encrypt(cipher->GetSecurity(),
                DLMS_COUNT_TYPE_PACKET,
                settings.GetCipher()->GetFrameCounter(),
                DLMS_COMMAND_GLO_INITIATE_REQUEST,
                cipher->GetSystemTitle(),
                tmp,
                crypted)) != 0)
            {
                return ret;
            }
            // Length for AARQ user field
            GXHelpers::SetObjectCount(2 + crypted.GetSize(), data);
            // Coding the choice for user-information (Octet string, universal)
            data.SetUInt8(BER_TYPE_OCTET_STRING);
            GXHelpers::SetObjectCount(crypted.GetSize(), data);
            data.Set(&crypted);
        }
    }
    return 0;
}



/**
 * Parse User Information from PDU.
 */
int ParseUserInformation(
    CGXDLMSSettings& settings,
    CGXCipher* cipher,
    CGXByteBuffer& data)
{
    int ret;
    unsigned short pduSize;
    unsigned char ch, len, tag;
    if ((ret = data.GetUInt8(&len)) != 0)
    {
        return ret;
    }
    if (data.GetSize() - data.GetPosition() < len)
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    // Encoding the choice for user information
    if ((ret = data.GetUInt8(&tag)) != 0)
    {
        return ret;
    }
    if (tag != 0x4)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = data.GetUInt8(&len)) != 0)
    {
        return ret;
    }
    // Tag for xDLMS-Initate.response
    if ((ret = data.GetUInt8(&tag)) != 0)
    {
        return ret;
    }
    if (tag == DLMS_COMMAND_GLO_INITIATE_RESPONSE)
    {
        data.SetPosition(data.GetPosition() - 1);
        DLMS_SECURITY security = DLMS_SECURITY_NONE;
        if ((ret = cipher->Decrypt(settings.GetSourceSystemTitle(), data, security)) != 0)
        {
            return ret;
        }
        cipher->SetSecurity(security);
        if ((ret = data.GetUInt8(&tag)) != 0)
        {
            return ret;
        }
    }
    else if (tag == DLMS_COMMAND_GLO_INITIATE_REQUEST)
    {
        data.SetPosition(data.GetPosition() - 1);
        // InitiateRequest
        DLMS_SECURITY security = DLMS_SECURITY_NONE;
        if ((ret = cipher->Decrypt(settings.GetSourceSystemTitle(), data, security)) != 0)
        {
            return ret;
        }
        cipher->SetSecurity(security);
        if ((ret = data.GetUInt8(&tag)) != 0)
        {
            return ret;
        }
    }
    bool response = tag == DLMS_COMMAND_INITIATE_RESPONSE;
    if (response)
    {
        // Optional usage field of the negotiated quality of service
        // component
        if ((ret = data.GetUInt8(&tag)) != 0)
        {
            return ret;
        }
        if (tag != 0)
        {
            if ((ret = data.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            data.SetPosition(data.GetPosition() + len);
        }
    }
    else if (tag == DLMS_COMMAND_INITIATE_REQUEST)
    {
        // Optional usage field of the negotiated quality of service
        // component
        if ((ret = data.GetUInt8(&tag)) != 0)
        {
            return ret;
        }
        // CtoS.
        if (tag != 0)
        {
            if ((ret = data.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            CGXByteBuffer tmp;
            tmp.Set(&data, data.GetPosition());
            settings.SetCtoSChallenge(tmp);
        }
        // Optional usage field of the negotiated quality of service
        // component
        if ((ret = data.GetUInt8(&tag)) != 0)
        {
            return ret;
        }
        // Skip if used.
        if (tag != 0)
        {
            if ((ret = data.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            data.SetPosition(data.GetPosition() + len);
        }
        // Optional usage field of the proposed quality of service component
        if ((ret = data.GetUInt8(&tag)) != 0)
        {
            return ret;
        }
        // Skip if used.
        if (tag != 0)
        {
            if ((ret = data.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            data.SetPosition(data.GetPosition() + len);
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    // Get DLMS version number.
    if (settings.IsServer())
    {
        if ((ret = data.GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        settings.SetDLMSVersion(ch);
    }
    else
    {
        if ((ret = data.GetUInt8(&ch)) != 0)
        {
            return ret;
        }
        if (ch != 6)
        {
            //Invalid DLMS version number.
            return DLMS_ERROR_CODE_INVALID_VERSION_NUMBER;
        }
    }

    // Tag for conformance block
    if ((ret = data.GetUInt8(&tag)) != 0)
    {
        return ret;
    }
    if (tag != 0x5F)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    // Old Way...
    if ((ret = data.GetUInt8(data.GetPosition(), &tag)) != 0)
    {
        return ret;
    }
    if (tag == 0x1F)
    {
        data.SetPosition(data.GetPosition() + 1);
    }
    if ((ret = data.GetUInt8(&len)) != 0)
    {
        return ret;
    }
    // The number of unused bits in the bit string.
    if ((ret = data.GetUInt8(&tag)) != 0)
    {
        return ret;
    }
    unsigned long v;
    unsigned char tmp[3];
    CGXByteBuffer bb(4);
    data.Get(tmp, 3);
    bb.SetUInt8(0);
    bb.Set(tmp, 3);
    bb.GetUInt32(&v);
    if (settings.IsServer())
    {
        settings.SetNegotiatedConformance((DLMS_CONFORMANCE)(v & settings.GetProposedConformance()));
    }
    else
    {
        settings.SetNegotiatedConformance((DLMS_CONFORMANCE)v);
    }

    if (settings.IsServer())
    {
        if ((ret = data.GetUInt16(&pduSize)) != 0)
        {
            return ret;
        }
        //If client asks too high PDU.
        if (pduSize > settings.GetMaxServerPDUSize())
        {
            pduSize = settings.GetMaxServerPDUSize();
        }
        settings.SetMaxReceivePDUSize(pduSize);
    }
    else
    {
        if ((ret = data.GetUInt16(&pduSize)) != 0)
        {
            return ret;
        }
        settings.SetMaxReceivePDUSize(pduSize);
    }
    if (response)
    {
        // VAA Name
        unsigned short vaa;
        if ((ret = data.GetUInt16(&vaa)) != 0)
        {
            return ret;
        }
        if (vaa == 0x0007)
        {
            // If LN
            if (!settings.GetUseLogicalNameReferencing())
            {
                //Invalid VAA.
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
        }
        else if (vaa == 0xFA00)
        {
            // If SN
            if (settings.GetUseLogicalNameReferencing())
            {
                //Invalid VAA.
                return DLMS_ERROR_CODE_INVALID_PARAMETER;
            }
        }
        else
        {
            // Unknown VAA.
            return DLMS_ERROR_CODE_INVALID_PARAMETER;
        }
    }
    return 0;
}

/**
 * Parse application context name.
 *
 * @param settings
 *            DLMS settings.
 * @param buff
 *            Received data.
 */
int ParseApplicationContextName(
    CGXDLMSSettings& settings,
    CGXByteBuffer& buff)
{
    int ret;
    unsigned char len, ch;
    // Get length.
    if ((ret = buff.GetUInt8(&len)) != 0)
    {
        return ret;
    }
    if (buff.GetSize() - buff.GetPosition() < len)
    {
        //Encoding failed. Not enough data.
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x6)
    {
        //Encoding failed. Not an Object ID.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if (settings.IsServer() && settings.GetCipher() != NULL)
    {
        settings.GetCipher()->SetSecurity(DLMS_SECURITY_NONE);
    }
    // Object ID length.
    if ((ret = buff.GetUInt8(&len)) != 0)
    {
        return ret;
    }
    if (settings.GetUseLogicalNameReferencing())
    {
        if (buff.Compare((unsigned char*)LOGICAL_NAME_OBJECT_ID, sizeof(LOGICAL_NAME_OBJECT_ID)))
        {
            return 0;
        }
        // If ciphering is used.
        if (!buff.Compare((unsigned char*)LOGICAL_NAME_OBJECT_ID_WITH_CIPHERING, sizeof(LOGICAL_NAME_OBJECT_ID_WITH_CIPHERING)))
        {
            return DLMS_ERROR_CODE_FALSE;
        }
        else
        {
            return 0;
        }
    }
    if (buff.Compare((unsigned char*)SHORT_NAME_OBJECT_ID, sizeof(SHORT_NAME_OBJECT_ID)))
    {
        return 0;
    }
    // If ciphering is used.
    if (!buff.Compare((unsigned char*)SHORT_NAME_OBJECT_ID_WITH_CIPHERING, sizeof(SHORT_NAME_OBJECT_ID_WITH_CIPHERING)))
    {
        return DLMS_ERROR_CODE_FALSE;
    }
    return 0;
}

int ValidateAare(
    CGXDLMSSettings& settings,
    CGXByteBuffer& buff)
{
    int ret;
    unsigned char tag;
    if ((ret = buff.GetUInt8(&tag)) != 0)
    {
        return ret;
    }
    if (settings.IsServer())
    {
        if (tag != (BER_TYPE_APPLICATION
            | BER_TYPE_CONSTRUCTED
            | PDU_TYPE_PROTOCOL_VERSION))
        {
            return DLMS_ERROR_CODE_INVALID_TAG;
        }
    }
    else
    {
        if (tag != (BER_TYPE_APPLICATION
            | BER_TYPE_CONSTRUCTED
            | PDU_TYPE_APPLICATION_CONTEXT_NAME))
        {
            return DLMS_ERROR_CODE_INVALID_TAG;
        }
    }
    return 0;
}

int UpdatePassword(
    CGXDLMSSettings& settings,
    CGXByteBuffer& buff)
{
    CGXByteBuffer tmp;
    int ret;
    unsigned char ch, len;
    if ((ret = buff.GetUInt8(&len)) != 0)
    {
        return ret;
    }

    // Get authentication information.
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x80)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = buff.GetUInt8(&len)) != 0)
    {
        return ret;
    }
    tmp.Set(&buff, buff.GetPosition(), len);
    if (settings.GetAuthentication() == DLMS_AUTHENTICATION_LOW)
    {
        settings.SetPassword(tmp);
    }
    else
    {
        settings.SetCtoSChallenge(tmp);
    }
    return 0;
}

int UpdateAuthentication(
    CGXDLMSSettings& settings,
    CGXByteBuffer& buff)
{
    int ret;
    unsigned char ch;
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }

    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x60)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x85)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x74)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x05)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x08)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch != 0x02)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    if ((ret = buff.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    if (ch > DLMS_AUTHENTICATION_HIGH_SHA256)
    {
        return DLMS_ERROR_CODE_INVALID_TAG;
    }
    settings.SetAuthentication((DLMS_AUTHENTICATION)ch);
    return 0;
}

int GetUserInformation(
    CGXDLMSSettings& settings,
    CGXCipher* cipher,
    CGXByteBuffer& data)
{
    data.Clear();
    data.SetUInt8(DLMS_COMMAND_INITIATE_RESPONSE); // Tag for xDLMS-Initiate
    // response
    data.SetUInt8(0x01);
    data.SetUInt8(0x00); // Usage field for the response allowed component
    // (not used)
    // DLMS Version Number
    data.SetUInt8(06);
    data.SetUInt8(0x5F);
    data.SetUInt8(0x1F);
    // length of the conformance block
    data.SetUInt8(0x04);
    // encoding the number of unused bits in the bit string
    data.SetUInt8(0x00);
    CGXByteBuffer bb(4);
    bb.SetUInt32((unsigned long)settings.GetNegotiatedConformance());
    data.Set(&bb, 1, 3);
    data.SetUInt16(settings.GetMaxPduSize());
    // VAA Name VAA name (0x0007 for LN referencing and 0xFA00 for SN)
    if (settings.GetUseLogicalNameReferencing())
    {
        data.SetUInt16(0x0007);
    }
    else
    {
        data.SetUInt16(0xFA00);
    }
    if (cipher != NULL && cipher->IsCiphered())
    {
        CGXByteBuffer tmp(data);
        data.Clear();
        return cipher->Encrypt(cipher->GetSecurity(),
            DLMS_COUNT_TYPE_PACKET,
            settings.GetCipher()->GetFrameCounter(),
            DLMS_COMMAND_GLO_INITIATE_RESPONSE,
            cipher->GetSystemTitle(),
            tmp,
            data);
    }
    return 0;
}

int CGXAPDU::GenerateAarq(
    CGXDLMSSettings& settings,
    CGXCipher* cipher,
    CGXByteBuffer* encryptedData,
    CGXByteBuffer& data)
{
    int ret;
    // AARQ APDU Tag
    data.SetUInt8(BER_TYPE_APPLICATION | BER_TYPE_CONSTRUCTED);
    // Length is updated later.
    unsigned long offset = data.GetSize();
    data.SetUInt8(0);
    ///////////////////////////////////////////
    // Add Application context name.
    if ((ret = GenerateApplicationContextName(settings, data, cipher)) != 0)
    {
        return ret;
    }
    if ((ret = GetAuthenticationString(settings, data)) != 0)
    {
        return ret;
    }
    if ((ret = GenerateUserInformation(settings, cipher, encryptedData, data)) != 0)
    {
        return ret;
    }
    data.SetUInt8(offset, (unsigned char)(data.GetSize() - offset - 1));
    return 0;
}

int handleResultComponent(DLMS_SOURCE_DIAGNOSTIC value)
{
    int ret;
    switch ((int)value)
    {
    case DLMS_SOURCE_DIAGNOSTIC_NO_REASON_GIVEN:
        ret = DLMS_ERROR_CODE_NO_REASON_GIVEN;
        break;
    case DLMS_SOURCE_DIAGNOSTIC_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED:
        ret = DLMS_ERROR_CODE_APPLICATION_CONTEXT_NAME_NOT_SUPPORTED;
        break;
    case DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED:
        ret = DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_NOT_RECOGNISED;
        break;
    case DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED:
        ret = DLMS_ERROR_CODE_AUTHENTICATION_MECHANISM_NAME_REQUIRED;
        break;
    case DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE:
        ret = DLMS_ERROR_CODE_AUTHENTICATION_FAILURE;
        break;
    default:
        //OK.
        ret = 0;
        break;
    }
    return ret;
}

int CGXAPDU::ParsePDU(
    CGXDLMSSettings& settings,
    CGXCipher* cipher,
    CGXByteBuffer& buff,
    DLMS_SOURCE_DIAGNOSTIC& diagnostic)
{
    CGXByteBuffer tmp;
    unsigned char tag, len;
    int ret;
    diagnostic = DLMS_SOURCE_DIAGNOSTIC_NONE;
    // Get AARE tag and length
    if ((ret = ValidateAare(settings, buff)) != 0)
    {
        return ret;
    }
    if ((ret = buff.GetUInt8(&len)) != 0)
    {
        return ret;
    }
    int size = buff.GetSize() - buff.GetPosition();
    if (len > size)
    {
        //Encoding failed. Not enough data.
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    DLMS_ASSOCIATION_RESULT resultComponent = DLMS_ASSOCIATION_RESULT_ACCEPTED;
    while (buff.GetPosition() < buff.GetSize())
    {
        if ((ret = buff.GetUInt8(&tag)) != 0)
        {
            return ret;
        }
        switch (tag)
        {
            //0xA1
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_APPLICATION_CONTEXT_NAME:
        {
            if ((ret = ParseApplicationContextName(settings, buff)) != 0)
            {
                return DLMS_ERROR_CODE_REJECTED_PERMAMENT;
            }
        }
        break;
        // 0xA2
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AP_TITLE:
            // Get len.
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            if (len != 3)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            // Choice for result (INTEGER, universal)

            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            if (tag != BER_TYPE_INTEGER)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            // Get len.
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            if (len != 1)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            resultComponent = (DLMS_ASSOCIATION_RESULT)tag;
            break;
            // 0xA3 SourceDiagnostic
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AE_QUALIFIER:
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            // ACSE service user tag.
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            // Result source diagnostic component.
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            if (tag != BER_TYPE_INTEGER)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            if (len != 1)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            diagnostic = (DLMS_SOURCE_DIAGNOSTIC)tag;
            break;
            // 0xA4 Result
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AP_INVOCATION_ID:
            // Get len.
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            if (len != 0xA)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            // Choice for result (Universal, Octet string type)
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            if (tag != BER_TYPE_OCTET_STRING)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            // responding-AP-title-field
            // Get len.
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            tmp.Clear();
            tmp.Set(&buff, buff.GetPosition(), len);
            settings.SetSourceSystemTitle(tmp);
            break;
            // 0xA6 Client system title.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AP_TITLE:
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            tmp.Clear();
            tmp.Set(&buff, buff.GetPosition(), len);
            settings.SetSourceSystemTitle(tmp);
            break;
            // 0xAA Server system title.
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_SENDER_ACSE_REQUIREMENTS:
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            tmp.Clear();
            tmp.Set(&buff, buff.GetPosition(), len);
            settings.SetStoCChallenge(tmp);
            break;
            //  0x8A or 0x88
        case BER_TYPE_CONTEXT | PDU_TYPE_SENDER_ACSE_REQUIREMENTS:
        case BER_TYPE_CONTEXT | PDU_TYPE_CALLING_AP_INVOCATION_ID:
            // Get sender ACSE-requirements field component.
            if ((ret = buff.GetUInt8(&len)) != 0)
            {
                return ret;
            }
            if (len != 2)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            if (tag != BER_TYPE_OBJECT_DESCRIPTOR)
            {
                return DLMS_ERROR_CODE_INVALID_TAG;
            }
            //Get only value because client app is sending system title with LOW authentication.
            if ((ret = buff.GetUInt8(&tag)) != 0)
            {
                return ret;
            }
            break;
            //  0x8B or 0x89
        case BER_TYPE_CONTEXT | PDU_TYPE_MECHANISM_NAME:
        case BER_TYPE_CONTEXT | PDU_TYPE_CALLING_AE_INVOCATION_ID:
            if ((ret = UpdateAuthentication(settings, buff)) != 0)
            {
                return ret;
            }
            break;
            // 0xAC
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLING_AUTHENTICATION_VALUE:
            if ((ret = UpdatePassword(settings, buff)) != 0)
            {
                return ret;
            }
            break;
            // 0xBE
        case BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_USER_INFORMATION:
            //Check result component. Some meters are returning invalid user-information if connection failed.
            if (resultComponent != DLMS_ASSOCIATION_RESULT_ACCEPTED
                && diagnostic != DLMS_SOURCE_DIAGNOSTIC_NONE)
            {
                return handleResultComponent(diagnostic);
            }
            if ((ret = ParseUserInformation(settings, cipher, buff)) != 0)
            {
                return ret;
            }
            break;
        default:
            // Unknown tags.
            if (buff.GetPosition() < buff.GetSize())
            {
                if ((ret = buff.GetUInt8(&len)) != 0)
                {
                    return ret;
                }
                buff.SetPosition(buff.GetPosition() + len);
            }
            break;
        }
    }
    //All meters don't send user-information if connection is failed.
    //For this reason result component is check again.
    return handleResultComponent(diagnostic);
}

/**
 * Server generates AARE message.
 */
int CGXAPDU::GenerateAARE(
    CGXDLMSSettings& settings,
    CGXByteBuffer& data,
    DLMS_ASSOCIATION_RESULT result,
    DLMS_SOURCE_DIAGNOSTIC diagnostic,
    CGXCipher* cipher)
{
    int ret;
    unsigned long offset = data.GetSize();
    // Set AARE tag and length 0x61
    data.SetUInt8(BER_TYPE_APPLICATION | BER_TYPE_CONSTRUCTED | PDU_TYPE_APPLICATION_CONTEXT_NAME);
    // Length is updated later.
    data.SetUInt8(0);
    if ((ret = GenerateApplicationContextName(settings, data, cipher)) != 0)
    {
        return ret;
    }
    // Result 0xA2
    data.SetUInt8(BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | BER_TYPE_INTEGER);
    data.SetUInt8(3); // len
    // Tag
    data.SetUInt8(BER_TYPE_INTEGER);
    // Choice for result (INTEGER, universal)
    data.SetUInt8(1); // Len
    // ResultValue
    data.SetUInt8(result);
    // SourceDiagnostic
    data.SetUInt8(0xA3);
    data.SetUInt8(5); // len
    data.SetUInt8(0xA1); // Tag
    data.SetUInt8(3); // len
    data.SetUInt8(2); // Tag
    // Choice for result (INTEGER, universal)
    data.SetUInt8(1); // Len
    // diagnostic
    data.SetUInt8(diagnostic);
    // SystemTitle
    if (cipher != NULL
        && (settings.GetAuthentication() == DLMS_AUTHENTICATION_HIGH_GMAC
            || cipher->IsCiphered()))
    {
        data.SetUInt8(BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_CALLED_AP_INVOCATION_ID);
        GXHelpers::SetObjectCount(2 + cipher->GetSystemTitle().GetSize(), data);
        data.SetUInt8(BER_TYPE_OCTET_STRING);
        GXHelpers::SetObjectCount(cipher->GetSystemTitle().GetSize(), data);
        data.Set(&cipher->GetSystemTitle());
    }

    if (result != DLMS_ASSOCIATION_RESULT_PERMANENT_REJECTED
        && diagnostic == DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_REQUIRED)
    {
        // Add server ACSE-requirenents field component.
        data.SetUInt8(0x88);
        data.SetUInt8(0x02); // Len.
        data.SetUInt16(0x0780);
        // Add tag.
        data.SetUInt8(0x89);
        data.SetUInt8(0x07); // Len
        data.SetUInt8(0x60);
        data.SetUInt8(0x85);
        data.SetUInt8(0x74);
        data.SetUInt8(0x05);
        data.SetUInt8(0x08);
        data.SetUInt8(0x02);
        data.SetUInt8(settings.GetAuthentication());
        // Add tag.
        data.SetUInt8(0xAA);
        GXHelpers::SetObjectCount(2 + settings.GetStoCChallenge().GetSize(), data); // Len
        data.SetUInt8(BER_TYPE_CONTEXT);
        GXHelpers::SetObjectCount(settings.GetStoCChallenge().GetSize(), data);
        data.Set(settings.GetStoCChallenge().GetData(), settings.GetStoCChallenge().GetSize());
    }
    // Add User Information
    // Tag 0xBE
    data.SetUInt8(BER_TYPE_CONTEXT | BER_TYPE_CONSTRUCTED | PDU_TYPE_USER_INFORMATION);
    CGXByteBuffer tmp;
    if ((ret = GetUserInformation(settings, cipher, tmp)) != 0)
    {
        return ret;
    }
    GXHelpers::SetObjectCount(2 + tmp.GetSize(), data);
    // Coding the choice for user-information (Octet STRING, universal)
    data.SetUInt8(BER_TYPE_OCTET_STRING);
    // Length
    GXHelpers::SetObjectCount(tmp.GetSize(), data);
    data.Set(&tmp);
    data.SetUInt8(offset + 1, (unsigned char)(data.GetSize() - offset - 2));
    return 0;
}