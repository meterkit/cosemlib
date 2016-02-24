
#include <string.h>
#include "csm_ber.h"

int csm_ber_read_tag(csm_array *i_array, ber_tag *o_tag)
{
    int ret = FALSE;
    uint8_t b;

    memset(o_tag, 0, sizeof(ber_tag));

    if (csm_array_read(i_array, &b))
    {
        o_tag->nbytes = 1;
        o_tag->tag = b;
        o_tag->cls = b & CLASS_MASK;
        o_tag->isPrimitive = (b & TYPE_MASK) == 0;
        o_tag->id = b & TAG_MASK;

        if (o_tag->id == TAG_MASK)
        {
            // Long tag, encoded as a sequence of 7-bit values, not supported
            CSM_ERR("[BER] Long tags are not supported");
        }
        else
        {
            ret = TRUE;
        }
    }
    return ret;
}

int csm_ber_read_len(csm_array *i_array, ber_length *o_len)
{
    int ret = FALSE;
    uint8_t b;

    memset(o_len, 0, sizeof(ber_length));

    if (csm_array_read(i_array, &b))
    {
        o_len->nbytes = 1;
        o_len->length = b;

        if ((o_len->length & LEN_XTND) == LEN_XTND)
        {
            uint16_t numoct = o_len->length & LEN_MASK;

            o_len->length = 0;

            if (numoct > sizeof(o_len->length))
            {
                return ret;
            }

            for (uint32_t i = 0; i < numoct; i++)
            {
                if (csm_array_read(i_array, &b))
                {
                    o_len->length = (o_len->length << 8U) | b;
                    o_len->nbytes++;
                }
                else
                {
                    return ret;
                }
            }
            ret = TRUE;
        }
    }
    return ret;
}

void csm_ber_dump(csm_ber *i_ber)
{
    CSM_TRACE("BER fields are:\r\n");

    CSM_TRACE("Class: %d\r\n", i_ber->tag.cls);
    CSM_TRACE("Tag: %d\r\n", i_ber->tag.tag);
    CSM_TRACE("Primitive: %d\r\n", i_ber->tag.isPrimitive);
    CSM_TRACE("Value length: %d\r\n", i_ber->length.length);
}


#if 0

namespace csm
{



BerReader::BerReader(const ConstByteArrayPtr &i_array)
    : mArray(i_array)
{

}

void BerReader::Dump()
{
    Ber::ConstTlv tlv;
    do
    {
        tlv.Set(mArray.GetUnread());
        tlv.Dump();
        mArray.Advance(tlv.Size());
    }
    while (tlv.IsValid());
}
#endif

