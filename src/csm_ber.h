#ifndef CSM_BER_H
#define CSM_BER_H

#include <stdint.h>
#include "csm_array.h"

enum
{
    CLASS_MASK	= 0xC0,
    TYPE_MASK	= 0x20,
    TAG_MASK	= 0x1F,
    LEN_XTND	= 0x80,
    LEN_MASK	= 0x7F
};

typedef struct
{
    uint8_t cls;
    uint8_t isPrimitive;
    uint8_t id;
    uint8_t tag;    //< We limit to one-byte tag
    uint8_t nbytes;
} ber_tag;

typedef struct
{
    uint16_t length; //< Cosem APDU size is coded on Unsigned16
    uint8_t nbytes;
} ber_length;

struct ber_struct
{
    ber_tag tag;
    ber_length length;
    unsigned int nbytes;
    unsigned int depth;
    uint8_t *value;
    struct ber_struct *next;
};

typedef struct ber_struct csm_ber;


// We only manage one-byte identifier
int csm_ber_read_tag(csm_array *i_array, ber_tag *o_tag);
// We only manage two-bytes length
int csm_ber_read_len(csm_array *i_array, ber_length *o_len);

void csm_ber_dump(csm_ber *i_ber);

#if 0
#include <cstdint>
#include <limits>


namespace csm
{

class Ber
{
public:
    /*
    Class                   Bit 8	Bit 7
    universal               0       0
    application             0       1
    context-specific        1       0
    private                 1       1
    */
    static const std::uint8_t cUniversal        = 0x00U;
    static const std::uint8_t cApplication      = 0x40U;
    static const std::uint8_t cContextSpecific  = 0x80U;
    static const std::uint8_t cPrivate          = 0xC0U;

    // Bit 6 is set: constructed, otherwise primitive
    static const std::uint8_t cConstructedMask   = 0x20U;
    static const std::uint8_t cLongFormMask      = 0x80U;
    static const std::uint8_t cLongFormSizeMask  = 0x7FU;


    template <class T>
    struct TlvStruct
    {
        ArrayPtr<T> tag;
        ArrayPtr<T> len;
        ArrayPtr<T> val;

        TlvStruct()
        {

        }

        TlvStruct(const ArrayPtr<T> &i_array)
        {
            Set(i_array);
        }

        bool IsValid() const
        {
            return ((tag != nullptr) && (len != nullptr) && (val != nullptr));
        }

        bool IsConstructed() const
        {
            return ((tag.Get(0U) & Ber::cConstructedMask) != 0U);
        }

        bool NullSize() const
        {
            return (*len == 0x80U);
        }

        bool IsLongForm() const
        {
            return ((*len & Ber::cLongFormMask) != 0U);
        }

        std::uint32_t LongFormSize()
        {
            return ((*len & Ber::cLongFormSizeMask));
        }

        /**
         * @brief Encode and output the length according to BER-TLV encoding rules
         *
         * Values larger than 0x7F must be encoded in the form (Length-Bytes) (Length)
         *
         * @param value
         * @param io_array
         */
        void EncodeLength(std::uint32_t value, ByteArrayWriter &io_array)
        {
            if (value < 0x80)
            {
                io_array.Append((std::uint8_t)(value & 0x7F));
            }
            else
            {
                std::uint32_t size = sizeof(value);
                while(0 == (value & cHighByte) && size > 0)
                {
                    value <<= 8;
                    size--; // lower number of bytes to store
                }
                io_array.Append((std::uint8_t)(0x80 | size));
                for(std::uint32_t i = 0U; i < size; i++)
                {
                    io_array.Append((std::uint8_t)((value >> cShiftByte) & 0xFF));
                    value <<= 8U;
                }
            }
        }

        /**
         * @brief DecodeLength
         *
         * (0x80 + N) [N-bytes]
         *
         * @param i_array Point to the first byte of the length field
         * @param o_length
         * @return
         */
        bool DecodeLength(const ByteArrayPtr &i_array, std::uint32_t o_length)
        {
            o_length = 0U;
            bool success = false;

            std::uint32_t arraySize = i_array.Size();

            if (arraySize >= 1U)
            {
                if (i_array.Get(0U) < 0x80U)
                {
                    o_length = i_array.Get(0U);
                }
                else
                {
                    std::uint32_t size = LongFormSize(); // Number of bytes to get

                    if (size <= sizeof(o_length))
                    {
                        if (i_array.Size() >= (size+1U))
                        {
                            // Array must be sized enough to get all the bytes
                            for (std::uint32_t i = 0U; i < size; i++)
                            {
                                std::uint8_t byte = i_array.Get(1U + i);
                                o_length |= static_cast<std::uint32_t>(byte) << (cShiftByte - 8U*i);
                            }
                        }
                        else
                        {
                            CSM_ERR("Array size too small to decode the length");
                        }
                    }
                    else
                    {
                        CSM_ERR("Length size is limited to 4 bytes");
                    }
                }
            }
            return success;
        }

        /**
         * @brief EncodedLength
         * @param value
         * @return The number of bytes to store the length information
         */
        std::uint32_t EncodedLength(std::uint32_t value)
        {
            if(value < 0x80U)
            {
                return 1U;
            }

            std::uint32_t size = sizeof(value);
            /* Check for the highest byte that contains a value */
            while(0U == (value & cHighByte) && size > 0U)
            {
                value <<= 8U;
                size--;
            }
            /* + 1 for byte-size byte
             * Size encoded as (0x80 + N) [N-bytes]
             * Max size-bytes == 127
             */
            return size + 1U;
        }

        // Size of the TLV element
        std::uint32_t Size() const
        {
            std::uint32_t size = 0U;
            if (IsValid())
            {
                if (NullSize() || !IsLongForm())
                {
                    size = 3U;
                }
                else
                {
                    size = 4U;
                }
            }
            return size;
        }

        bool Set(const ArrayPtr<T> &i_array)
        {
            if (i_array.Size() >= 3U)
            {
                tag = i_array.SubArray(0U, 1U);
                len = i_array.At(1U);

                if (IsLongForm())
                {
                    std::uint32_t size = EncodedLength(*len);
                    CSM_LOG("[BER] Data size: %d", size);
                    // Tag + length fields
                    if ((i_array.Size() >= 1U + size))
                    {
                        val = &i_array.At(1U+size);
                    }
                    else
                    {
                        CSM_ERR("[BER] Array too small for long form encoding");
                        tag = nullptr;
                        len = nullptr;
                        val = nullptr;
                    }
                }
                else
                {
                    val = &i_array.At(2U);
                }
            }

            return IsValid();
        }

        void Dump()
        {
            if (IsValid())
            {
                if (!IsConstructed())
                {
                    CSM_LOG("[BER] Type is: primitive, definite-length");
                }
                else if (NullSize())
                {
                    CSM_LOG("[BER] Type is: constructed, definite-length");
                }
                else
                {
                    CSM_LOG("[BER] Type is: constructed, indefinite-length");
                }
            }
        }

    private:
        static const std::uint32_t cMaxValue = std::numeric_limits<std::uint32_t>::max();
        static const std::uint32_t cHighByte = (cMaxValue ^ (cMaxValue >> 8U)); /* EX: 0xFF000000 - for std::uint32_t == 32-bit */
        static const std::uint32_t cShiftByte = (sizeof(std::uint32_t) - 1U) * 8U;
    };

    typedef TlvStruct<std::uint8_t> Tlv;
    typedef TlvStruct<const std::uint8_t> ConstTlv;

};

class BerReader
{

public:
    BerReader(const ConstByteArrayPtr &i_array);

    void Dump();

private:
    ByteArrayReader mArray;
};

} // namespace csm

#endif

#endif // CSM_BER_H
