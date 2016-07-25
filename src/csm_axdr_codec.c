/**
 * AXDR utility function to serialize data
 *
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
 *
 */

#include "csm_axdr_codec.h"
#include "csm_ber.h"

// -------------------------------   DECODERS   ------------------------------------------

int csm_axdr_rd_null(csm_array *array)
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

int csm_axdr_rd_octetstring(csm_array *array)
{
    int ret = FALSE;
    uint8_t byte = 0xFFU;
    if (csm_array_read_u8(array, &byte))
    {
        if (byte == AXDR_TAG_OCTETSTRING)
        {
            ber_length len;
            csm_ber_read_len(array, &len);

            // Check if size is somewhat possible
            if (len.length <= csm_array_unread(array))
            {
                ret = TRUE;
            }
        }
    }
    return ret;
}


// -------------------------------   ENCODERS ------------------------------------------
int csm_axdr_wr_octetstring(csm_array *array, const uint8_t *buffer, uint32_t size)
{
    int valid = csm_array_write_u8(array, AXDR_TAG_OCTETSTRING);
    valid = valid && csm_ber_write_len(array, size);
    valid = valid && csm_array_write_buff(array, buffer, size);
    return valid;
}

int csm_axdr_wr_boolean(csm_array *array, uint8_t value)
{
    int valid = csm_array_write_u8(array, AXDR_TAG_OCTETSTRING);
    valid = valid && csm_array_write_u8(array, value);
    return valid;
}
