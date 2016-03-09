/**
 * Copyright (c) 2016, Anthony Rabine
 * See LICENSE.txt
 *
 * AXDR utility function to serialize data
 */

#include "csm_axdr_codec.h"
#include "csm_ber.h"

// -------------------------------   DECODERS   ------------------------------------------

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


// -------------------------------   ENCODERS ------------------------------------------
int axdr_encode_octet_string(csm_array *array, const uint8_t *buffer, uint32_t size)
{
    int valid = csm_array_write_u8(array, AXDR_OCTET_STRING);
    valid = valid && csm_ber_write_len(array, size);
    valid = valid && csm_array_write_buff(array, buffer, size);
    return valid;
}
