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

#include "../include/GXCipher.h"
#include "../include/chipperingenums.h"
#include "../include/GXHelpers.h"

// Consts.
const unsigned char BLOCK_SIZE = 16;
static unsigned char TAG_SIZE = 0x10;

CGXGMacBlock::CGXGMacBlock()
{
    c0 = c1 = c2 = c3 = 0;
    bytesRemaining = 0;
    totalLength = 0;
}

void CGXCipher::Init(
    unsigned char* systemTitle,
    unsigned char count)
{
    const unsigned char BLOCKCIPHERKEY[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    const unsigned char AUTHENTICATIONKEY[] =
    {
        0xD0,  0xD1,  0xD2, 0xD3,  0xD4,  0xD5,  0xD6,  0xD7,
        0xD8,  0xD9,  0xDA,  0xDB,  0xDC, 0xDD,  0xDE,  0xDF
    };
    m_FrameCounter = 0;
    m_Security = DLMS_SECURITY_NONE;
    m_WorkingKey = m_mArray = NULL;
    m_SystemTitle.Set(systemTitle, count);
    m_BlockCipherKey.Set(BLOCKCIPHERKEY, sizeof(BLOCKCIPHERKEY));
    m_AuthenticationKey.Set(AUTHENTICATIONKEY, sizeof(AUTHENTICATIONKEY));
}

CGXCipher::CGXCipher(CGXByteBuffer& systemTitle)
{
    Init((unsigned char*)systemTitle.GetData(), (unsigned char)systemTitle.GetSize());
}

CGXCipher::CGXCipher(const char* systemTitle)
{
    Init((unsigned char*)systemTitle, (unsigned char)strlen(systemTitle));
}

CGXCipher::CGXCipher(unsigned char* systemTitle, unsigned char count)
{
    Init(systemTitle, count);
}

CGXCipher::~CGXCipher()
{
    if (m_WorkingKey != NULL)
    {
        free(m_WorkingKey);
        m_WorkingKey = NULL;
    }
    if (m_mArray != NULL)
    {
        free(m_mArray);
        m_mArray = NULL;
    }
}

static void SetUInt32ByIndexLE(CGXByteBuffer* arr, int index, unsigned long item)
{
    arr->SetUInt8(index, item & 0xFF);
    arr->SetUInt8(index + 1, (item >> 8) & 0xFF);
    arr->SetUInt8(index + 2, (item >> 16) & 0xFF);
    arr->SetUInt8(index + 3, (item >> 24) & 0xFF);
}

static void PackBlock(
    CGXCipher* settings,
    CGXByteBuffer* bytes,
    int offset,
    CGXGMacBlock *block)
{
    SetUInt32ByIndexLE(bytes, offset, block->c0);
    SetUInt32ByIndexLE(bytes, offset + 4, block->c1);
    SetUInt32ByIndexLE(bytes, offset + 8, block->c2);
    SetUInt32ByIndexLE(bytes, offset + 12, block->c3);
}

int GetUInt32ByIndexLE(
    CGXByteBuffer *arr,
    unsigned long index,
    unsigned long* value)
{
    if (index + 4 > arr->GetSize())
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = ((unsigned char*)arr->GetData())[index + 3] << 24 |
        ((unsigned char*)arr->GetData())[index + 2] << 16 |
        ((unsigned char*)arr->GetData())[index + 1] << 8 |
        ((unsigned char*)arr->GetData())[index];
    return 0;
}

static int UnPackBlock(
    CGXCipher* settings,
    CGXByteBuffer* bytes,
    int offset,
    CGXGMacBlock *block)
{
    int ret;
    if ((ret = GetUInt32ByIndexLE(bytes, offset, &block->c0)) != 0)
    {
        return ret;
    }
    if ((ret = GetUInt32ByIndexLE(bytes, offset + 4, &block->c1)) != 0)
    {
        return ret;
    }
    if ((ret = GetUInt32ByIndexLE(bytes, offset + 8, &block->c2)) != 0)
    {
        return ret;
    }
    if ((ret = GetUInt32ByIndexLE(bytes, offset + 12, &block->c3)) != 0)
    {
        return ret;
    }
    return ret;
}

int CGXCipher::ProcessBlock(
    CGXCipher* settings,
    CGXByteBuffer* input,
    unsigned long inOffset,
    CGXByteBuffer* output,
    unsigned long outOffset,
    CGXGMacBlock *block)
{
    if ((inOffset + (32 / 2)) > input->GetSize())
    {
        //Input buffer too short.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    if ((outOffset + (32 / 2)) > output->GetSize())
    {
        // output buffer too short.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }

    UnPackBlock(settings, input, inOffset, block);
    if (settings->m_Encrypt)
    {
        EncryptBlock(settings, block);
    }
    else
    {
        //DecryptBlock(settings, block);
    }
    PackBlock(settings, output, outOffset, block);
    return 0;
}

static unsigned long* GetCell(unsigned long *start, int row, int col)
{
    return start + 4 * row + col;
}


/**
* Shift block to right.
*
* @param block
*            Block list.
* @param count
*            Blocks to shift.
*/
static void ShiftRight(
    unsigned long* block,
    int count)
{
    int i, bit = 0;
    for (i = 0; i < 4; ++i)
    {
        unsigned long b = block[i];
        block[i] = (b >> count) | bit;
        bit = b << (32 - count);
    }
}

/**
    * Make Xor for 128 bits.
    *
    * @param block
    *            block.
    * @param val
    */
static void Xor128(
    unsigned long* block,
    unsigned long* value)
{
    int pos;
    for (pos = 0; pos != 4; ++pos)
    {
        block[pos] ^= value[pos];
    }
}

static void MultiplyP(
    unsigned long* x)
{
    unsigned char lsb = (x[3] & 1) != 0;
    ShiftRight(x, 1);
    if (lsb)
    {
        x[0] ^= 0xe1000000L;
    }
}

static void MultiplyP8(
    unsigned long* x)
{
    char pos;
    unsigned long lsw = x[3];
    ShiftRight(x, 8);
    for (pos = 0; pos != 8; ++pos)
    {
        if ((lsw & (1 << pos)) != 0)
        {
            x[0] ^= ((0xe1000000L >> (7 - pos)) & 0xFFFFFFFF);
        }
    }
}

int CGXCipher::Init2(
    CGXCipher* settings)
{
    unsigned long *tmp;
    int pos, pos1, pos2, k, ret;
    //If array is not created yet.
    if (settings->m_mArray == 0)
    {
        //Array must set to zero.
        settings->m_mArray = (unsigned long*)calloc(1, sizeof(unsigned long) * 32 * 16 * 4);
        if ((ret = settings->m_H.GetUInt128(0, (unsigned char*)GetCell(settings->m_mArray, 1 * 16, 8 * 4))) != 0)
        {
            return ret;
        }

        for (pos = 4; pos >= 1; pos >>= 1)
        {
            tmp = GetCell(settings->m_mArray, 1 * 16, pos * 4);
            memcpy(tmp, GetCell(settings->m_mArray, 1 * 16, (pos + pos) * 4), 4 * sizeof(long));
            MultiplyP(tmp);
        }

        tmp = GetCell(settings->m_mArray, 0 * 16, 8 * 4);
        memcpy(tmp, GetCell(settings->m_mArray, 1 * 16, 1 * 4), 4 * sizeof(long));
        MultiplyP(tmp);

        for (pos = 4; pos >= 1; pos >>= 1)
        {
            tmp = GetCell(settings->m_mArray, 0 * 16, pos * 4);
            memcpy(tmp, GetCell(settings->m_mArray, 0 * 16, (pos + pos) * 4), 4 * sizeof(long));
            MultiplyP(tmp);
        }

        for (pos1 = 0;;)
        {
            for (pos2 = 2; pos2 < 16; pos2 += pos2)
            {
                for (k = 1; k < pos2; ++k)
                {
                    tmp = GetCell(settings->m_mArray, pos1 * 16, (pos2 + k) * 4);
                    memcpy(tmp, GetCell(settings->m_mArray, pos1 * 16, pos2 * 4), 4 * sizeof(long));
                    Xor128(tmp, GetCell(settings->m_mArray, pos1 * 16, k * 4));
                }
            }

            if (++pos1 == 32)
            {
                return 0;
            }

            if (pos1 > 1)
            {
                for (pos = 8; pos > 0; pos >>= 1)
                {
                    tmp = GetCell(settings->m_mArray, pos1 * 16, pos * 4);
                    memcpy(tmp, GetCell(settings->m_mArray, (pos1 - 2) * 16, pos * 4), 4 * sizeof(long));
                    MultiplyP8(tmp);
                }
            }
        }
    }
    return 0;
}

int CGXCipher::GetAuthenticatedData(
    DLMS_SECURITY security,
    CGXByteBuffer& authenticationKey,
    CGXByteBuffer& plainText,
    CGXByteBuffer& result)
{
    result.Clear();
    if (security == DLMS_SECURITY_AUTHENTICATION)
    {
        result.SetUInt8(security);
        result.Set(&authenticationKey, 0, -1);
        result.Set(plainText.GetData() + plainText.GetPosition(),
            plainText.GetSize() - plainText.GetPosition());
    }
    else if (security == DLMS_SECURITY_ENCRYPTION)
    {
        result.Set(&authenticationKey, 0, -1);
    }
    else if (security == DLMS_SECURITY_AUTHENTICATION_ENCRYPTION)
    {
        result.SetUInt8(security);
        result.Set(&authenticationKey, 0, -1);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    return 0;
}

static unsigned int SubWord(unsigned int value)
{
    int tmp = S_BOX[value & 0xFF] & 0xFF;
    tmp |= (((S_BOX[(value >> 8) & 0xFF]) & 0xFF) << 8) & 0xFF00;
    tmp |= (((S_BOX[(value >> 16) & 0xFF]) & 0xFF) << 16) & 0xFF0000;
    tmp |= (((S_BOX[(value >> 24) & 0xFF]) & 0xFF) << 24) & 0xFF000000;
    return tmp;
}


/**
* Shift value.
*
* @param r
* @param shift
* @return
*/
static unsigned int Shift(
    unsigned long value,
    unsigned long shift)
{
    return (value >> shift) | (value << (32 - shift));
}

/**
* Initialise the key schedule from the user supplied key.
*
* @return
*/
static unsigned int starX(
    unsigned int value)
{
    unsigned int m1, m2, m3;
    m1 = (int)0x80808080;
    m2 = 0x7f7f7f7f;
    m3 = 0x0000001b;
    return ((value & m2) << 1) ^ (((value & m1) >> 7) * m3);
}

static int ImixCol(unsigned int x)
{
    int f2 = starX(x);
    int f4 = starX(f2);
    int f8 = starX(f4);
    int f9 = x ^ f8;
    return f2 ^ f4 ^ f8 ^ Shift(f2 ^ f9, 8) ^ Shift(f4 ^ f9, 16)
        ^ Shift(f9, 24);
}

/**
* Convert uint32 to Big Endian byte array.
*
* @param value
* @param buff
* @param offset
*/
static void UInt32ToBE(unsigned int value, unsigned char* buff, int offset)
{
    buff[offset] = (value >> 24);
    buff[offset + 1] = (value >> 16);
    buff[offset + 2] = (value >> 8);
    buff[offset + 3] = (value);
}

void CGXCipher::MultiplyH(
    CGXCipher* settings,
    unsigned char* value)
{
    int pos;
    long tmp[4] = { 0 };
    unsigned long* m;
    for (pos = 0; pos != 16; ++pos)
    {
        m = GetCell(settings->m_mArray, 16 * (pos + pos), 4 * (value[pos] & 0x0f));
        tmp[0] ^= m[0];
        tmp[1] ^= m[1];
        tmp[2] ^= m[2];
        tmp[3] ^= m[3];
        m = GetCell(settings->m_mArray, 16 * (pos + pos + 1), 4 * ((value[pos] & 0xf0) >> 4));
        tmp[0] ^= m[0];
        tmp[1] ^= m[1];
        tmp[2] ^= m[2];
        tmp[3] ^= m[3];
    }

    UInt32ToBE(tmp[0], value, 0);
    UInt32ToBE(tmp[1], value, 4);
    UInt32ToBE(tmp[2], value, 8);
    UInt32ToBE(tmp[3], value, 12);
}

/**
    * Make Xor for 128 bits.
    *
    * @param block
    *            block.
    * @param val
    */
void Xor(unsigned char* block, unsigned char* value)
{
    int pos;
    for (pos = 0; pos != 16; ++pos)
    {
        block[pos] ^= value[pos];
    }
}

/**
* Count GHash.
*/
void CGXCipher::GetGHash(
    CGXCipher* settings,
    CGXByteBuffer *aad)
{
    unsigned long pos, cnt;
    unsigned char x[16];
    settings->m_S.Zero(0, 16);
    for (pos = 0; pos < aad->GetSize(); pos += 16)
    {
        memset(x, 0, sizeof(x));
        cnt = aad->GetSize() - pos;
        if (cnt > 16)
        {
            cnt = 16;
        }
        memcpy(x, aad->GetData() + pos, cnt);
        Xor(settings->m_S.GetData(), x);
        MultiplyH(settings, settings->m_S.GetData());
    }
}


/**
* Encrypt data block.
*
*/
void CGXCipher::EncryptBlock(
    CGXCipher* settings,
    CGXGMacBlock *block)
{
    int r, rounds;
    unsigned int r0, r1, r2, r3;
    block->c0 ^= *GetCell(settings->m_WorkingKey, 0, 0);
    block->c1 ^= *GetCell(settings->m_WorkingKey, 0, 1);
    block->c2 ^= *GetCell(settings->m_WorkingKey, 0, 2);
    block->c3 ^= *GetCell(settings->m_WorkingKey, 0, 3);
    rounds = GetRounds(settings);
    for (r = 1; r < rounds - 1;)
    {
        r0 = AES[block->c0 & 0xFF];
        r0 ^= Shift(AES[(block->c1 >> 8) & 0xFF], 24);
        r0 ^= Shift(AES[(block->c2 >> 16) & 0xFF], 16);
        r0 ^= Shift(AES[(block->c3 >> 24) & 0xFF], 8);
        r0 ^= *GetCell(settings->m_WorkingKey, r, 0);

        r1 = AES[block->c1 & 0xFF];
        r1 ^= Shift(AES[(block->c2 >> 8) & 0xFF], 24);
        r1 ^= Shift(AES[(block->c3 >> 16) & 0xFF], 16);
        r1 ^= Shift(AES[(block->c0 >> 24) & 0xFF], 8);
        r1 ^= *GetCell(settings->m_WorkingKey, r, 1);

        r2 = AES[block->c2 & 0xFF];
        r2 ^= Shift(AES[(block->c3 >> 8) & 0xFF], 24);
        r2 ^= Shift(AES[(block->c0 >> 16) & 0xFF], 16);
        r2 ^= Shift(AES[(block->c1 >> 24) & 0xFF], 8);
        r2 ^= *GetCell(settings->m_WorkingKey, r, 2);

        r3 = AES[block->c3 & 0xFF];
        r3 ^= Shift(AES[(block->c0 >> 8) & 0xFF], 24);
        r3 ^= Shift(AES[(block->c1 >> 16) & 0xFF], 16);
        r3 ^= Shift(AES[(block->c2 >> 24) & 0xFF], 8);
        r3 ^= *GetCell(settings->m_WorkingKey, r, 3);
        ++r;
        block->c0 = AES[r0 & 0xFF];
        block->c0 ^= Shift(AES[(r1 >> 8) & 0xFF], 24);
        block->c0 ^= Shift(AES[(r2 >> 16) & 0xFF], 16);
        block->c0 ^= Shift(AES[(r3 >> 24) & 0xFF], 8);
        block->c0 ^= *GetCell(settings->m_WorkingKey, r, 0);

        block->c1 = AES[r1 & 0xFF];
        block->c1 ^= Shift(AES[(r2 >> 8) & 0xFF], 24);
        block->c1 ^= Shift(AES[(r3 >> 16) & 0xFF], 16);
        block->c1 ^= Shift(AES[(r0 >> 24) & 0xFF], 8);
        block->c1 ^= *GetCell(settings->m_WorkingKey, r, 1);

        block->c2 = AES[r2 & 0xFF];
        block->c2 ^= Shift(AES[(r3 >> 8) & 0xFF], 24);
        block->c2 ^= Shift(AES[(r0 >> 16) & 0xFF], 16);
        block->c2 ^= Shift(AES[(r1 >> 24) & 0xFF], 8);
        block->c2 ^= *GetCell(settings->m_WorkingKey, r, 2);

        block->c3 = AES[r3 & 0xFF];
        block->c3 ^= Shift(AES[(r0 >> 8) & 0xFF], 24);
        block->c3 ^= Shift(AES[(r1 >> 16) & 0xFF], 16);
        block->c3 ^= Shift(AES[(r2 >> 24) & 0xFF], 8);
        block->c3 ^= *GetCell(settings->m_WorkingKey, r, 3);
        ++r;
    }
    r0 = AES[block->c0 & 0xFF];
    r0 ^= Shift(AES[(block->c1 >> 8) & 0xFF], 24);
    r0 ^= Shift(AES[(block->c2 >> 16) & 0xFF], 16);
    r0 ^= Shift(AES[block->c3 >> 24], 8);
    r0 ^= *GetCell(settings->m_WorkingKey, r, 0);
    r1 = AES[block->c1 & 0xFF];
    r1 ^= Shift(AES[(block->c2 >> 8) & 0xFF], 24);
    r1 ^= Shift(AES[(block->c3 >> 16) & 0xFF], 16);
    r1 ^= Shift(AES[block->c0 >> 24], 8);
    r1 ^= *GetCell(settings->m_WorkingKey, r, 1);
    r2 = AES[block->c2 & 0xFF];
    r2 ^= Shift(AES[(block->c3 >> 8) & 0xFF], 24);
    r2 ^= Shift(AES[(block->c0 >> 16) & 0xFF], 16);
    r2 ^= Shift(AES[block->c1 >> 24], 8);
    r2 ^= *GetCell(settings->m_WorkingKey, r, 2);
    r3 = AES[block->c3 & 0xFF];
    r3 ^= Shift(AES[(block->c0 >> 8) & 0xFF], 24);
    r3 ^= Shift(AES[(block->c1 >> 16) & 0xFF], 16);
    r3 ^= Shift(AES[block->c2 >> 24], 8);
    r3 ^= *GetCell(settings->m_WorkingKey, r++, 3);
    block->c0 = S_BOX[r0 & 0xFF] & 0xFF;
    block->c0 ^= ((S_BOX[(r1 >> 8) & 0xFF] & 0xFF) << 8);
    block->c0 ^= ((S_BOX[(r2 >> 16) & 0xFF] & 0xFF) << 16);
    block->c0 ^= ((S_BOX[r3 >> 24] & 0xFF) << 24);
    block->c0 ^= *GetCell(settings->m_WorkingKey, r, 0);
    block->c1 = (S_BOX[r1 & 0xFF] & 0xFF);
    block->c1 ^= ((S_BOX[(r2 >> 8) & 0xFF] & 0xFF) << 8);
    block->c1 ^= ((S_BOX[(r3 >> 16) & 0xFF] & 0xFF) << 16);
    block->c1 ^= ((S_BOX[r0 >> 24] & 0xFF) << 24);
    block->c1 ^= *GetCell(settings->m_WorkingKey, r, 1);
    block->c2 = (S_BOX[r2 & 0xFF] & 0xFF);
    block->c2 ^= ((S_BOX[(r3 >> 8) & 0xFF] & 0xFF) << 8);
    block->c2 ^= ((S_BOX[(r0 >> 16) & 0xFF] & 0xFF) << 16);
    block->c2 ^= ((S_BOX[r1 >> 24] & 0xFF) << 24);
    block->c2 ^= *GetCell(settings->m_WorkingKey, r, 2);
    block->c3 = (S_BOX[r3 & 0xFF] & 0xFF) & 0xFFFFFFFF;
    block->c3 ^= ((S_BOX[(r0 >> 8) & 0xFF] & 0xFF) << 8);
    block->c3 ^= ((S_BOX[(r1 >> 16) & 0xFF] & 0xFF) << 16);
    block->c3 ^= ((S_BOX[r2 >> 24] & 0xFF) << 24);
    block->c3 ^= *GetCell(settings->m_WorkingKey, r, 3);
}

int CGXCipher::GetRounds(CGXCipher* settings)
{
    int keyLen = settings->m_BlockCipherKey.GetSize() / 4;
    return keyLen + 6;
}

int GetUInt32LE(
    CGXByteBuffer* arr,
    unsigned long* value)
{
    if (arr->GetPosition() + 4 > arr->GetSize())
    {
        return DLMS_ERROR_CODE_OUTOFMEMORY;
    }
    *value = ((unsigned char*)arr->GetData())[arr->GetPosition() + 3] << 24 |
        ((unsigned char*)arr->GetData())[arr->GetPosition() + 2] << 16 |
        ((unsigned char*)arr->GetData())[arr->GetPosition() + 1] << 8 |
        ((unsigned char*)arr->GetData())[arr->GetPosition()];
    arr->SetPosition(arr->GetPosition() + 4);
    return 0;
}

int CGXCipher::GenerateKey(CGXCipher* settings)
{
    if (settings->m_WorkingKey == 0)
    {
        // Key length in words.
        unsigned long i, k;
        unsigned long keyLen = settings->m_BlockCipherKey.GetSize() / 4;
        int rounds, ret, j, t = 0;
        rounds = GetRounds(settings);
        // 4 words make one block.
        settings->m_WorkingKey = (unsigned long*)malloc(4 * sizeof(unsigned long) * (rounds + 1));
        // Copy the key into the round key array.
        settings->m_BlockCipherKey.SetPosition(0);
        for (i = 0; i < settings->m_BlockCipherKey.GetSize(); t++)
        {
            if ((ret = GetUInt32LE(&settings->m_BlockCipherKey, GetCell(settings->m_WorkingKey, t >> 2, t & 3))) != 0)
            {
                return ret;
            }
            i += 4;
        }
        // while not enough round key material calculated calculate new values.
        k = (rounds + 1) << 2;
        for (i = keyLen; (i < k); i++)
        {
            t = *GetCell(settings->m_WorkingKey, (i - 1) >> 2, (i - 1) & 3);
            if ((i % keyLen) == 0)
            {
                t = SubWord(Shift(t, 8)) ^ (R_CON[(i / keyLen) - 1] & 0xFF);
            }
            else if ((keyLen > 6) && ((i % keyLen) == 4))
            {
                t = SubWord(t);
            }
            *GetCell(settings->m_WorkingKey, i >> 2, i & 3) = *GetCell(settings->m_WorkingKey, (i - keyLen) >> 2, (i - keyLen) & 3) ^ t;
            if (!settings->m_Encrypt)
            {
                for (j = 1; j < rounds; j++)
                {
                    for (i = 0; i < 4; i++)
                    {
                        *GetCell(settings->m_WorkingKey, j, i) = ImixCol(*GetCell(settings->m_WorkingKey, j, i));
                    }
                }
            }
        }
    }
    return 0;
}


/**
* Get nonse from frame counter and system title.
*
* @param frameCounter
*            Frame counter.
* @param systemTitle
*            System title.
* @return
*/
static int GetNonse(
    unsigned long frameCounter,
    CGXByteBuffer& systemTitle,
    CGXByteBuffer& nonce)
{
    if (systemTitle.GetSize() != 8)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    nonce.SetSize(0);
    nonce.Capacity(12);
    nonce.Set(systemTitle.GetData(), systemTitle.GetSize());
    nonce.SetUInt32(frameCounter);
    return 0;
}

int CGXCipher::Init(
    CGXCipher* settings,
    CGXByteBuffer& aad,
    CGXByteBuffer& iv,
    unsigned long frameCounter,
    CGXByteBuffer& systemTitle,
    CGXByteBuffer& bufBlock,
    CGXGMacBlock& block)
{
    int ret, bufLength;
    if (block.tag.GetSize() == 0)
    {
        // Tag size is 12 bytes.
        block.tag.Capacity(12);
    }
    else if (block.tag.GetSize() != 12)
    {
        //Invalid tag.
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = GenerateKey(settings)) != 0)
    {
        return ret;
    }
    if (settings->m_Encrypt)
    {
        bufLength = BLOCK_SIZE;
    }
    else
    {
        bufLength = (BLOCK_SIZE + TAG_SIZE);
    }
    if ((ret = GetNonse(frameCounter, systemTitle, iv)) != 0)
    {
        return ret;
    }

    bufBlock.Capacity(bufLength);
    settings->m_H.Zero(0, BLOCK_SIZE);
    if ((ret = ProcessBlock(settings, &settings->m_H, 0, &settings->m_H, 0, &block)) != 0)
    {
        return ret;
    }
    if ((ret = Init2(settings)) != 0)
    {
        return ret;
    }

    settings->m_J0.Zero(0, 16);
    settings->m_J0.SetSize(0);
    settings->m_J0.Set(&iv, 0, -1);
    settings->m_J0.SetSize(16);
    settings->m_J0.GetData()[15] = 0x01;
    GetGHash(settings, &aad);
    settings->m_Counter.Clear();
    settings->m_Counter.Set(&settings->m_J0, 0, -1);
    block.bytesRemaining = 0;
    block.totalLength = 0;
    return 0;
}

void CGXCipher::gCTRBlock(
    CGXCipher *settings,
    CGXByteBuffer *buf,
    int bufCount,
    CGXGMacBlock *block,
    CGXByteBuffer *output)
{
    int pos, ret;
    CGXByteBuffer *hashBytes;
    CGXByteBuffer tmp;
    tmp.Zero(0, BLOCK_SIZE);
    for (pos = 15; pos >= 12; --pos)
    {
        if (++settings->m_Counter.GetData()[pos] != 0)
        {
            break;
        }
    }

    if ((ret = ProcessBlock(settings, &settings->m_Counter, 0, &tmp, 0, block)) != 0)
    {
        return;
    }
    if (settings->m_Encrypt)
    {
        tmp.Zero(bufCount, BLOCK_SIZE - bufCount);
        hashBytes = &tmp;
    }
    else
    {
        hashBytes = buf;
    }
    for (pos = 0; pos != bufCount; ++pos)
    {
        tmp.GetData()[pos] ^= buf->GetData()[pos];
    }
    output->Set(&tmp, 0, bufCount);
    Xor(settings->m_S.GetData(), hashBytes->GetData());
    MultiplyH(settings, settings->m_S.GetData());
    block->totalLength += (long)bufCount;
}

/**
* Write bytes to decrypt/encrypt.
*
* @param input
*/
void CGXCipher::Write(
    CGXCipher *settings,
    CGXByteBuffer *data,
    CGXByteBuffer *bufBlock,
    CGXGMacBlock *block,
    CGXByteBuffer* output)
{
    unsigned long pos, cnt;
    for (pos = 0; pos < data->GetSize(); pos += 16)
    {
        cnt = 16;
        if (data->GetSize() - pos < 16)
        {
            cnt = data->GetSize() - pos;
            block->bytesRemaining = cnt;
        }
        bufBlock->Set(data, pos, cnt);
        if (cnt == 16)
        {
            gCTRBlock(settings, bufBlock, BLOCK_SIZE, block, output);
            if (!settings->m_Encrypt)
            {
                bufBlock->Move(BLOCK_SIZE, 0, block->tag.GetSize());
            }
            block->bytesRemaining = 0;
            bufBlock->SetSize(0);
        }
    }
}

/**
* Reset
*/
void CGXCipher::Reset(
    CGXCipher *settings,
    CGXByteBuffer *aad)
{
    GetGHash(settings, aad);
    settings->m_Counter.Capacity(0);
    settings->m_Counter.Set(&settings->m_J0, 0, -1);
}

/**
* Set packet length to byte array.
*
* @param length
* @param buff
* @param offset
*/
static void SetPackLength(
    unsigned long length,
    CGXByteBuffer* buff,
    unsigned int offset)
{
    buff->SetUInt64(length);
}

int CGXCipher::FlushFinalBlock(
    DLMS_SECURITY security,
    CGXCipher *settings,
    CGXByteBuffer *aad,
    CGXByteBuffer *bufBlock,
    CGXGMacBlock *block,
    CGXByteBuffer *output)
{
    CGXByteBuffer tmp;
    // Crypt/Uncrypt remaining bytes.
    if (block->bytesRemaining > 0)
    {
        tmp.Set(bufBlock, 0, block->bytesRemaining);
        tmp.Zero(block->bytesRemaining, BLOCK_SIZE - block->bytesRemaining);
        gCTRBlock(settings, &tmp, block->bytesRemaining, block, output);
    }
    // If tag is not needed.
    if (security == DLMS_SECURITY_ENCRYPTION)
    {
        Reset(settings, aad);
        return 0;
    }
    // Count HASH.
    tmp.Clear();
    tmp.Capacity(16);
    SetPackLength(aad->GetSize() * 8, &tmp, 0);
    SetPackLength(block->totalLength * 8, &tmp, 8);
    Xor(settings->m_S.GetData(), tmp.GetData());
    CGXCipher::MultiplyH(settings, settings->m_S.GetData());
    tmp.Zero(0, BLOCK_SIZE);
    ProcessBlock(settings, &settings->m_J0, 0, &tmp, 0, block);
    Xor(tmp.GetData(), settings->m_S.GetData());
    if (!settings->m_Encrypt)
    {
        if (memcmp(block->tag.GetData(), tmp.GetData(), 12) != 0)
        {
            return DLMS_ERROR_CODE_INVALID_TAG;
        }
    }
    else
    {
        // Tag size is 12 bytes.
        block->tag.SetSize(0);
        block->tag.Set(&tmp, 0, 12);
    }
    Reset(settings, aad);
    return 0;
}

int CGXCipher::Encrypt(
    DLMS_SECURITY security,
    DLMS_COUNT_TYPE type,
    unsigned long frameCounter,
    unsigned char tag,
    CGXByteBuffer& systemTitle,
    CGXByteBuffer& plainText,
    CGXByteBuffer& encrypted)
{
    int ret;
    CGXGMacBlock block;
    CGXByteBuffer data, aad, iv, bufBlock;
    m_Encrypt = 1;
    systemTitle.SetPosition(0);
    if ((ret = GetAuthenticatedData(security, m_AuthenticationKey, plainText, aad)) != 0)
    {
        return ret;
    }
    if ((ret = Init(this, aad, iv, frameCounter, systemTitle, bufBlock, block)) != 0)
    {
        return ret;
    }
    if (type == DLMS_COUNT_TYPE_PACKET)
    {
        data.SetUInt8(m_Security);
    }
    // Encrypt the secret message
    if (security != DLMS_SECURITY_AUTHENTICATION)
    {
        Write(this, &plainText, &bufBlock, &block, &encrypted);
    }
    FlushFinalBlock(security, this, &aad, &bufBlock, &block, &encrypted);
    if (security == DLMS_SECURITY_AUTHENTICATION)
    {
        if (type == DLMS_COUNT_TYPE_PACKET)
        {
            data.SetUInt32(frameCounter);
        }
        if ((type & DLMS_COUNT_TYPE_DATA) != 0)
        {
            data.Set(plainText.GetData() + plainText.GetPosition(),
                plainText.GetSize() - plainText.GetPosition());
        }
        if ((type & DLMS_COUNT_TYPE_TAG) != 0)
        {
            data.Set(&block.tag, 0, -1);
        }
    }
    else if (security == DLMS_SECURITY_ENCRYPTION)
    {
        if (type == DLMS_COUNT_TYPE_PACKET)
        {
            data.SetUInt32(frameCounter);
        }
        data.Set(&encrypted, 0, -1);
    }
    else if (security == DLMS_SECURITY_AUTHENTICATION_ENCRYPTION)
    {
        if (type == DLMS_COUNT_TYPE_PACKET)
        {
            data.SetUInt32(frameCounter);
        }
        if ((type & DLMS_COUNT_TYPE_DATA) != 0)
        {
            data.Set(&encrypted, 0, -1);
        }
        if ((type & DLMS_COUNT_TYPE_DATA) != 0 ||
            (type & DLMS_COUNT_TYPE_TAG) != 0)
        {
            data.Set(&block.tag, 0, -1);
        }
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    encrypted.SetSize(0);
    if (type == DLMS_COUNT_TYPE_PACKET)
    {
        encrypted.Capacity(10 + data.GetSize());
        encrypted.SetUInt8(tag);
        GXHelpers::SetObjectCount(data.GetSize(), encrypted);
    }
    else
    {
        encrypted.Capacity(data.GetSize());
    }
    encrypted.Set(&data, 0, -1);
    return 0;
}

/// <summary>
/// Are tags equals.
/// </summary>
/// <param name="tag1"></param>
/// <param name="tag2"></param>
/// <returns></returns>
static bool TagsEquals(CGXByteBuffer& tag1, CGXByteBuffer& tag2)
{
    if (tag1.GetSize() < 12 || tag2.GetSize() < 12)
    {
        return false;
    }
    for (int pos = 0; pos != 12; ++pos)
    {
        if (tag1.GetData()[pos] != tag2.GetData()[pos])
        {
            return false;
        }
    }
    return true;
}

int CGXCipher::Decrypt(
    CGXByteBuffer& title,
    CGXByteBuffer& data,
    DLMS_SECURITY& security)
{
    CGXByteBuffer tag, ciphertext, systemTitle, countTag;
    DLMS_COMMAND cmd = DLMS_COMMAND_NONE;
    int length;
    int ret;
    unsigned char ch;
    unsigned long len, frameCounter;
    if (data.GetSize() - data.GetPosition() < 2)
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = data.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    cmd = (DLMS_COMMAND)ch;
    switch (cmd)
    {
    case DLMS_COMMAND_GLO_GENERAL_CIPHERING:
        if ((ret = GXHelpers::GetObjectCount(data, len)) != 0)
        {
            return ret;
        }
        systemTitle.Set(&data, data.GetPosition(), len);
        break;
    case DLMS_COMMAND_GLO_INITIATE_REQUEST:
    case DLMS_COMMAND_GLO_INITIATE_RESPONSE:
    case DLMS_COMMAND_GLO_READ_REQUEST:
    case DLMS_COMMAND_GLO_READ_RESPONSE:
    case DLMS_COMMAND_GLO_WRITE_REQUEST:
    case DLMS_COMMAND_GLO_WRITE_RESPONSE:
    case DLMS_COMMAND_GLO_GET_REQUEST:
    case DLMS_COMMAND_GLO_GET_RESPONSE:
    case DLMS_COMMAND_GLO_SET_REQUEST:
    case DLMS_COMMAND_GLO_SET_RESPONSE:
    case DLMS_COMMAND_GLO_METHOD_REQUEST:
    case DLMS_COMMAND_GLO_METHOD_RESPONSE:
    case DLMS_COMMAND_GLO_EVENT_NOTIFICATION_REQUEST:
        break;
    default:
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    if ((ret = GXHelpers::GetObjectCount(data, len)) != 0)
    {
        return ret;
    }
    if ((ret = data.GetUInt8(&ch)) != 0)
    {
        return ret;
    }
    security = (DLMS_SECURITY)ch;
    if ((ret = data.GetUInt32(&frameCounter)) != 0)
    {
        return ret;
    }

    if (security == DLMS_SECURITY_AUTHENTICATION)
    {
        length = data.GetSize() - data.GetPosition() - 12;
        //Get tag
        tag.Set(data.GetData() + data.GetPosition() + length, 12);
        //Skip tag from data.
        data.SetSize(data.GetPosition() + length);
        // Check tag.
        if ((ret = Encrypt(
            security,
            DLMS_COUNT_TYPE_TAG,
            frameCounter,
            0,
            cmd == DLMS_COMMAND_GLO_GENERAL_CIPHERING ? systemTitle : title,
            data,
            countTag)) != 0)
        {
            return ret;
        }
        if (!TagsEquals(tag, countTag))
        {
            //Decrypt failed. Invalid tag.
            return DLMS_ERROR_CODE_INVALID_TAG;
        }
        return 0;
    }
    else if (security == DLMS_SECURITY_ENCRYPTION)
    {
        length = data.GetSize() - data.GetPosition();
        ciphertext.Set(&data, data.GetPosition(), length);
    }
    else if (security == DLMS_SECURITY_AUTHENTICATION_ENCRYPTION)
    {
        length = data.GetSize() - data.GetPosition() - 12;
        ciphertext.Set(&data, data.GetPosition(), length);
        tag.Set(&data, data.GetPosition(), 12);
    }
    else
    {
        return DLMS_ERROR_CODE_INVALID_PARAMETER;
    }
    data.Clear();
    if ((ret = Encrypt(
        security,
        DLMS_COUNT_TYPE_DATA,
        frameCounter,
        0,
        cmd == DLMS_COMMAND_GLO_GENERAL_CIPHERING ? systemTitle : title,
        ciphertext,
        data)) != 0)
    {
        return ret;
    }
    //Check tag.
    if (security == DLMS_SECURITY_AUTHENTICATION_ENCRYPTION)
    {
        //Remove tag.
        data.SetSize(data.GetSize() - 12);
    }
    return 0;
}

/**
* multiply by 2 in the Galois field
*
* @param value
*            value to multiply.
* @return Value multiply by 2.
*/
static unsigned char GaloisMultiply(unsigned char value)
{
    if (value >> 7 != 0)
    {
        value = (value << 1);
        return (value ^ 0x1b);
    }
    else
    {
        return (value << 1);
    }
}

/**
     * Encrypt data using AES.
     *
     * @param data
     *            Encrypted data.
     * @param offset
     *            Data offset.
     * @param secret
     *            Secret.
     */
int CGXCipher::Aes1Encrypt(
    CGXByteBuffer& buff,
    unsigned short offset,
    CGXByteBuffer& secret)
{
    unsigned char buf1, buf2, buf3, buf4, round, i;
    unsigned char *key = secret.GetData();
    unsigned char *data = buff.GetData();
    for (round = 0; round < 10; ++round)
    {
        for (i = 0; i < 16; ++i)
        {
            data[i + offset] = S_BOX[(data[i + offset] ^ key[i]) & 0xFF];
        }
        // shift rows
        buf1 = data[1 + offset];
        data[1 + offset] = data[5 + offset];
        data[5 + offset] = data[9 + offset];
        data[9 + offset] = data[13 + offset];
        data[13 + offset] = buf1;

        buf1 = data[2 + offset];
        buf2 = data[6 + offset];
        data[2 + offset] = data[10 + offset];
        data[6 + offset] = data[14 + offset];
        data[10 + offset] = buf1;
        data[14 + offset] = buf2;

        buf1 = data[15 + offset];
        data[15 + offset] = data[11 + offset];
        data[11 + offset] = data[7 + offset];
        data[7 + offset] = data[3 + offset];
        data[3 + offset] = buf1;

        if (round < 9)
        {
            for (i = 0; i < 4; i++)
            {
                buf4 = (i << 2);
                buf1 = (data[buf4 + offset] ^ data[buf4 + 1 + offset]
                    ^ data[buf4 + 2 + offset]
                    ^ data[buf4 + 3 + offset]);
                buf2 = data[buf4 + offset];
                buf3 = (data[buf4 + offset]
                    ^ data[buf4 + 1 + offset]);
                buf3 = GaloisMultiply(buf3);
                data[buf4 + offset] =
                    (data[buf4 + offset] ^ buf3 ^ buf1);
                buf3 = (data[buf4 + 1 + offset]
                    ^ data[buf4 + 2 + offset]);
                buf3 = GaloisMultiply(buf3);
                data[buf4 + 1 + offset] =
                    (data[buf4 + 1 + offset] ^ buf3 ^ buf1);
                buf3 = (data[buf4 + 2 + offset]
                    ^ data[buf4 + 3 + offset]);
                buf3 = GaloisMultiply(buf3);
                data[buf4 + 2 + offset] = (data[buf4 + 2 + offset] ^ buf3 ^ buf1);
                buf3 = (data[buf4 + 3 + offset] ^ buf2);
                buf3 = GaloisMultiply(buf3);
                data[buf4 + 3 + offset] = (data[buf4 + 3 + offset] ^ buf3 ^ buf1);
            }
        }

        key[0] = (S_BOX[key[13] & 0xFF] ^ key[0] ^ R_CON[round]);
        key[1] = (S_BOX[key[14] & 0xFF] ^ key[1]);
        key[2] = (S_BOX[key[15] & 0xFF] ^ key[2]);
        key[3] = (S_BOX[key[12] & 0xFF] ^ key[3]);
        for (i = 4; i < 16; i++)
        {
            key[i] = (key[i] ^ key[i - 4]);
        }
    }

    for (i = 0; i < 16; i++)
    {
        data[i + offset] = (data[i + offset] ^ key[i]);
    }
    return 0;
}

bool CGXCipher::IsCiphered()
{
    return m_Security != DLMS_SECURITY_NONE;
}

DLMS_SECURITY CGXCipher::GetSecurity()
{
    return m_Security;
}

void CGXCipher::SetSecurity(DLMS_SECURITY value)
{
    m_Security = value;
}

CGXByteBuffer& CGXCipher::GetSystemTitle()
{
    return m_SystemTitle;
}

void CGXCipher::SetSystemTitle(CGXByteBuffer& value)
{
    m_SystemTitle.Clear();
    m_SystemTitle.Set(&value);
}

CGXByteBuffer& CGXCipher::GetBlockCipherKey()
{
    return m_BlockCipherKey;
}

void CGXCipher::SetBlockCipherKey(CGXByteBuffer& value)
{
    m_BlockCipherKey.Clear();
    m_BlockCipherKey.Set(&value);
}

CGXByteBuffer& CGXCipher::GetAuthenticationKey()
{
    return m_AuthenticationKey;
}

void CGXCipher::SetAuthenticationKey(CGXByteBuffer& value)
{
    m_AuthenticationKey.Clear();
    m_AuthenticationKey.Set(&value);
}

unsigned long CGXCipher::GetFrameCounter()
{
    return m_FrameCounter;
}

void CGXCipher::SetFrameCounter(unsigned long value)
{
    m_FrameCounter = value;
}

void CGXCipher::Reset()
{

}
