#include "csm_axdr_codec.h"



int axdr_decode_null(csm_array *array)
{
    int ret = FALSE;
    uint8_t byte = 0xFFU;
    if (csm_array_read_u8(array, &byte))
    {
        if (byte == AXDR_TAG_NULL)
        {
            ret = TRUE;
        }
    }
    return ret;
}
