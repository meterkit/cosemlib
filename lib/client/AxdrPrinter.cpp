
#include "os_util.h"
#include "hdlc.h"
#include "csm_axdr_codec.h"
#include "csm_array.h"
#include "JsonWriter.h"
#include "JsonValue.h"
#include "AxdrPrinter.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <bitset>


struct tag_t
{
    uint8_t tag;
    std::string name;
};

static const tag_t tags[] = {
        { AXDR_TAG_NULL,            "Null" },
        { AXDR_TAG_ARRAY,           "Array"},
        { AXDR_TAG_STRUCTURE,       "Structure"},
        { AXDR_TAG_BOOLEAN,         "Boolean"},
        { AXDR_TAG_BITSTRING,       "BitString"},
        { AXDR_TAG_INTEGER32,       "Integer32"},
        { AXDR_TAG_UNSIGNED32,      "Unsigned32"},
        { AXDR_TAG_OCTETSTRING,     "OctetString"},
        { AXDR_TAG_VISIBLESTRING,   "VisibleString"},
        { AXDR_TAG_UTF8_STRING,     "UTF8String"},
        { AXDR_TAG_BCD,             "BCD"},
        { AXDR_TAG_INTEGER8,        "Integer8"},
        { AXDR_TAG_INTEGER16,       "Integer16"},
        { AXDR_TAG_UNSIGNED8,       "Unsigned8"} ,
        { AXDR_TAG_UNSIGNED16,      "Unsigned16"},
        { AXDR_TAG_INTEGER64,       "Integer64"},
        { AXDR_TAG_UNSIGNED64,      "Unsigned64"},
        { AXDR_TAG_ENUM,            "Enum"},
        { AXDR_TAG_UNKNOWN,         "Unknown"}
};

static const uint32_t tags_size = sizeof(tags) / sizeof(tags[0]);

std::string TagName(uint8_t tag)
{
    std::string name = "UnkownTag";

    for (uint32_t i = 0U; i < tags_size; i++)
    {
        if (tags[i].tag == tag)
        {
            name = tags[i].name;
            break;
        }
    }
    return name;
}

//JsonObject root;
//std::vector<JsonValue> levels;
//JsonValue current;




void AxdrPrinter::PrintIndent()
{
    for (uint32_t i = 0U; i < mLevels.size(); i++)
    {
        mStream << "    ";
    }
}

std::string AxdrPrinter::DataToString(uint8_t type, uint32_t size, uint8_t *data)
{
    std::stringstream ss;
    switch (type)
    {
        case AXDR_TAG_NULL:
            break;
        case AXDR_TAG_BOOLEAN:
            if (*data == 0)
            {
                ss << "false";
            }
            else
            {
                ss << "true";
            }
            break;
        case AXDR_TAG_BITSTRING:
        {
            uint32_t bytes = BITFIELD_BYTES(size);
            for (uint32_t i = 0U; i < bytes; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    uint8_t bit = data[i] >> (7-j) & 0x01U;
                    if (bit)
                    {
                        ss << "1";
                    }
                    else
                    {
                        ss << "0";
                    }
                }
            }
            break;
        }
        case AXDR_TAG_INTEGER32:
        {
            uint32_t value = GET_BE32(data);
            ss << static_cast<long>(value);
            break;
        }
        case AXDR_TAG_UNSIGNED32:
        {
            uint32_t value = GET_BE32(data);
            ss << static_cast<unsigned long>(value);
            break;
        }
        case AXDR_TAG_OCTETSTRING:
        case AXDR_TAG_VISIBLESTRING:
        case AXDR_TAG_UTF8_STRING:
        {
            char out[2];
            for (uint32_t i = 0U; i < size; i++)
            {
                byte_to_hex(data[i], &out[0]);

                ss << out[0] << out[1];
            }
            break;
        }
        case AXDR_TAG_INTEGER8:
        {
            ss << static_cast<int16_t>(data[0]);
            break;
        }
        case AXDR_TAG_INTEGER16:
        {
            int16_t value = GET_BE16(data);
            ss << static_cast<int16_t>(value);
            break;
        }
        case AXDR_TAG_BCD:
        case AXDR_TAG_UNSIGNED8:
        case AXDR_TAG_ENUM:
        {
            ss << static_cast<int16_t>(data[0]);
            break;
        }
        case AXDR_TAG_UNSIGNED16:
        {
            uint16_t value = GET_BE16(data);
            ss << static_cast<uint16_t>(value);
            break;
        }
        case AXDR_TAG_INTEGER64:
        {
            uint64_t value = GET_BE64(data);
            ss << static_cast<uint64_t>(value);
            break;
        }
        case AXDR_TAG_UNSIGNED64:
        {
            uint64_t value = GET_BE64(data);
            ss << static_cast<uint64_t>(value);
            break;
        }
        case AXDR_TAG_UNKNOWN:
            break;

        default:
            break;
    }

    return ss.str();
}

std::string AxdrPrinter::Get()
{
    return mStream.str();
}

void AxdrPrinter::Append(uint8_t type, uint32_t size, uint8_t *data)
{
    std::string name = TagName(type);
    //std::cout << "found type: " << name << " size: " << size << std::endl;

    // current level is finished
    if (mLevels.size() > 0)
    {
        if (mLevels.back().counter >= mLevels.back().size)
        {
            mLevels.pop_back();
        }
    }

    PrintIndent();

    if ((type == AXDR_TAG_ARRAY) ||
        (type == AXDR_TAG_STRUCTURE))
    {
        mStream << name << " of size " << size << std::endl;

        if (mLevels.size() > 0)
        {
            mLevels.back().counter++;
        }

        element curr;

        curr.counter = 0;
        curr.size = size;

        mLevels.push_back(curr);
    }
    else
    {
        mStream << name << "\t" << DataToString(type, size, data) << std::endl;

        if (mLevels.size() > 0)
        {
            mLevels.back().counter++;
            if (mLevels.back().counter >= mLevels.back().size)
            {
                mLevels.pop_back();
            }
        }

    }
}

