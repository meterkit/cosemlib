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

#ifndef GXCIPHER_H
#define GXCIPHER_H

#include "GXBytebuffer.h"

//This is reserved for internal use to save block info.
class CGXGMacBlock
{
public:
    unsigned long c0;
    unsigned long c1;
    unsigned long c2;
    unsigned long c3;
    // How many bytes are not crypted/encrypted.
    int bytesRemaining;
    long totalLength;
    CGXByteBuffer tag;
    CGXGMacBlock();
};

class CGXCipher
{
private:
    DLMS_SECURITY m_Security;
    /**
    * System title.
    */
    CGXByteBuffer m_SystemTitle;

    /**
    * Is data encrypted.
    */
    bool m_Encrypt;

    /**
    *  Block cipher key.
    */
    CGXByteBuffer m_BlockCipherKey;
    /**
    *  Block Authentication key.
    */
    CGXByteBuffer m_AuthenticationKey;

    /**
     * Frame counter. AKA Invocation counter.
     */
    unsigned long m_FrameCounter;


    /**
    * Working key is counted only once from block cipher key.
    */
    unsigned long* m_WorkingKey;
    CGXByteBuffer m_H;
    CGXByteBuffer m_J0;
    CGXByteBuffer m_S;
    CGXByteBuffer m_Counter;
    unsigned long* m_mArray;

    static int GetRounds(
        CGXCipher* settings);

    /**
    * Count GHash.
    */
    static void GetGHash(
        CGXCipher* settings,
        CGXByteBuffer *aad);

    /**
    * Generate AES keys.
    *
    */
    static int GenerateKey(
        CGXCipher* settings);

    static void MultiplyH(
        CGXCipher* settings,
        unsigned char* value);

    static int GetAuthenticatedData(
        DLMS_SECURITY security,
        CGXByteBuffer& authenticationKey,
        CGXByteBuffer& plainText,
        CGXByteBuffer& result);

    /**
    * Encrypt data block.
    *
    */
    static void EncryptBlock(
        CGXCipher* settings,
        CGXGMacBlock *block);

    void Init(
        unsigned char* systemTitle,
        unsigned char count);

    static int Init(
        CGXCipher* settings,
        CGXByteBuffer& aad,
        CGXByteBuffer& iv,
        unsigned long frameCounter,
        CGXByteBuffer& systemTitle,
        CGXByteBuffer& bufBlock,
        CGXGMacBlock& block);

    /**
    * Write bytes to decrypt/encrypt.
    *
    * @param input
    */
    static void Write(
        CGXCipher *settings,
        CGXByteBuffer *data,
        CGXByteBuffer *bufBlock,
        CGXGMacBlock *block,
        CGXByteBuffer* output);

    static int Init2(
        CGXCipher* settings);

    /**
    * Process encrypting/decrypting.
    *
    * @return
    */
    static int FlushFinalBlock(
        DLMS_SECURITY security,
        CGXCipher *settings,
        CGXByteBuffer *aad,
        CGXByteBuffer *bufBlock,
        CGXGMacBlock *block,
        CGXByteBuffer *output);

    /**
    * Reset
    */
    static void Reset(
        CGXCipher *settings,
        CGXByteBuffer *aad);

    static int ProcessBlock(
        CGXCipher* settings,
        CGXByteBuffer* input,
        unsigned long inOffset,
        CGXByteBuffer* output,
        unsigned long outOffset,
        CGXGMacBlock *block);

    static void gCTRBlock(
        CGXCipher *settings,
        CGXByteBuffer *buf,
        int bufCount,
        CGXGMacBlock *block,
        CGXByteBuffer *output);

public:
    /**
    * Constructor.
    */
    CGXCipher(CGXByteBuffer& systemTitle);

    /**
    * Constructor.
    */
    CGXCipher(const char* systemTitle);

    /**
    * Constructor.
    */
    CGXCipher(
        unsigned char* systemTitle,
        unsigned char count);

    /**
    * Destructor.
    */
    ~CGXCipher();


    /**
      * Encrypt PDU.
      *
      * @param tag
      *            Tag.
      * @param systemTitle
      *            System Title.
      * @param data
      *            Data to encrypt.
      * @param reply
      *            Encrypted data.
      */
    int Encrypt(
        DLMS_SECURITY security,
        DLMS_COUNT_TYPE type,
        unsigned long frameCounter,
        unsigned char tag,
        CGXByteBuffer& systemTitle,
        CGXByteBuffer& plainText,
        CGXByteBuffer& encrypted);

    /**
      * Decrypt data.
      *
      * @param systemTitle
      *            System Title.
      * @param data
      *            Decrypted data.
      * @param security
      *            Used security level.
      */
    int Decrypt(
        CGXByteBuffer& title,
        CGXByteBuffer& data,
        DLMS_SECURITY& security);

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
    static int Aes1Encrypt(
        CGXByteBuffer& data,
        unsigned short offset,
        CGXByteBuffer& secret);

    /**
     * @return Is ciphering used.
     */
    bool IsCiphered();

    /**
     * @return Used security.
     */
    DLMS_SECURITY GetSecurity();

    /**
    * @param value
    *            Used security.
    */
    void SetSecurity(DLMS_SECURITY value);

    /**
     * @return System title.
     */
    CGXByteBuffer& GetSystemTitle();

    /**
    *  @param value System title.
    */
    void SetSystemTitle(CGXByteBuffer& value);

    /**
     * @return Block cipher key.
     */
    CGXByteBuffer& GetBlockCipherKey();

    /**
    *  @param value Block cipher key.
    */
    void SetBlockCipherKey(CGXByteBuffer& value);

    /**
     * @return Authentication key.
     */
    CGXByteBuffer& GetAuthenticationKey();

    /**
     * @param value
     *            Authentication key.
     */
    void SetAuthenticationKey(CGXByteBuffer& value);

    /**
     * @return Frame counter. Invocation counter.
     */
    unsigned long GetFrameCounter();

    void SetFrameCounter(unsigned long value);

    void Reset();
};
#endif //GXCIPHER_H
