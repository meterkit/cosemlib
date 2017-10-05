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

#ifndef GXHELPERS_H
#define GXHELPERS_H

#include "errorcodes.h"
#include "GXDLMSVariant.h"
#include "GXDataInfo.h"

class GXHelpers
{
    /*
    * Convert char hex value to byte value.
    * @param c Character to convert hex.
    * @return Byte value of hex char value.
    */
    static unsigned char GetValue(char c);
public:

    /**
    * Convert object to DLMS bytes.
    *
    * buff : Byte buffer where data is write.
    * type : Data type.
    * value : Added Value.
    */
    static int SetData(CGXByteBuffer& buff, DLMS_DATA_TYPE type, CGXDLMSVariant& value);

    /**
        * Get data from DLMS frame.
        *
        * @param data
        *            received data.
        * @param info
        *            Data info.
        * @return Received data.
        */
    static int GetData(CGXByteBuffer& data, CGXDataInfo& info, CGXDLMSVariant& value);

    static void GetLogicalName(unsigned char* buff, std::string& ln)
    {
        int dataSize;
        char tmp[25];
        //If Script Action target is not set it is null
        if (buff == NULL)
        {
            ln.clear();
            ln.append("0.0.0.0.0.0");
        }
        else
        {
#if _MSC_VER > 1000
            dataSize = sprintf_s(tmp, 25, "%d.%d.%d.%d.%d.%d", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]) + 1;
#else
            dataSize = sprintf(tmp, "%d.%d.%d.%d.%d.%d", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]) + 1;
#endif
            if (dataSize > 25)
            {
                assert(0);
            }
            ln.clear();
            ln.append(tmp, dataSize - 1);
        }
    }

    static void GetLogicalName(CGXByteBuffer& buff, std::string& ln)
    {
        unsigned char tmp[6];
        buff.Get(tmp, 6);
        GetLogicalName(tmp, ln);
    }

    /////////////////////////////////////////////////////////////////////////////
    //Set logical name from std::string.
    /////////////////////////////////////////////////////////////////////////////
    static int SetLogicalName(const char* name, CGXDLMSVariant& value);

    /////////////////////////////////////////////////////////////////////////////
    //Set logical name from std::string.
    /////////////////////////////////////////////////////////////////////////////
    static int SetLogicalName(const char* name, unsigned char ln[6]);

    /////////////////////////////////////////////////////////////////////////////
    // Get object count. If first byte is 0x80 or higger it will tell bytes count.
    // data ; received data.
    // Returns Object count.
    /////////////////////////////////////////////////////////////////////////////
    static int GetObjectCount(CGXByteBuffer& data, unsigned long& count);

    /**
        * Return how many bytes object count takes.
        *
        * @param count
        *            Value
        * @return Value size in bytes.
        */
    static unsigned char GetObjectCountSizeInBytes(unsigned long count);

    /////////////////////////////////////////////////////////////////////////////
    // Set item count.
    /////////////////////////////////////////////////////////////////////////////
    // count : Item count.
    // buff : Byte buffer.
    /////////////////////////////////////////////////////////////////////////////
    static void SetObjectCount(unsigned long count, CGXByteBuffer& buff);

    static std::vector< std::string > Split(std::string& s, char separator);

    static std::vector< std::string > Split(std::string& s, std::string separators, bool ignoreEmpty);

    static void Replace(std::string& str, std::string oldString, std::string newString);

    /////////////////////////////////////////////////////////////////////////////
    // Trim from start.
    /////////////////////////////////////////////////////////////////////////////
    static std::string &ltrim(std::string& s);

    /////////////////////////////////////////////////////////////////////////////
    // Trim from end.
    /////////////////////////////////////////////////////////////////////////////
    static inline std::string &rtrim(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Trim from both ends
    /////////////////////////////////////////////////////////////////////////////
    static inline std::string &trim(std::string& s)
    {
        return ltrim(rtrim(s));
    }

    static std::string BytesToHex(unsigned char* pBytes, int count);

    static std::string BytesToHex(unsigned char* pBytes, int count, char separator);

    /**
     * Convert std::string to byte array.
     *
     * @param value
     *            Hex std::string.
     * @param buffer
     *            byte array.
     * @return Occurred error.
     */
    static void HexToBytes(std::string value, CGXByteBuffer& buffer);

    static void Write(char* fileName, char* pData, int len);

    static void Write(std::string fileName, std::string data);

    static bool GetBits(unsigned char& tmp, unsigned char BitMask);

    static inline bool StringCompare(const char* c1, const char* c2);

    /**
    * Get data type in bytes.
    *
    * @param type
    *            Data type.
    * @return Size of data type in bytes.
    */
    static int GetDataTypeSize(DLMS_DATA_TYPE type);

};
#endif //GXHELPERS_H
