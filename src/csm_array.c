/**
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
 *
 * Implementation of a protected array with read/write pointers
 */

#include "csm_array.h"
#include "os.h"
#include <string.h>


void csm_array_init(csm_array *io_array, uint8_t *buffer, uint32_t max_size, uint32_t used_size)
{
    CSM_ASSERT(io_array != NULL);
    CSM_ASSERT(used_size <= max_size);

    io_array->buff = buffer;
    io_array->rd_index = 0U;
    io_array->wr_index = used_size;
    io_array->size = max_size;
}

int csm_array_get(const csm_array *i_array, uint32_t i_index, uint8_t *io_byte)
{
    int ret = 0;
    CSM_ASSERT(i_array != NULL);
    if (i_index < i_array->size)
    {
        *io_byte = i_array->buff[i_index];
        ret = 1;
    }
    else
    {
        *io_byte = 0U;
    }
    return ret;
}

int csm_array_set(csm_array *io_array, uint32_t i_index, uint8_t i_byte)
{
    int ret = 0;
    CSM_ASSERT(io_array != NULL);
    if (i_index < io_array->size)
    {
        io_array->buff[i_index] = i_byte;
        ret = 1;
    }
    return ret;
}

int csm_array_write_u8(csm_array *array, uint8_t byte)
{
    int ret = 0;
    CSM_ASSERT(array != NULL);
    if (array->wr_index < array->size)
    {
        array->buff[array->wr_index] = byte;
        array->wr_index++;
        ret = 1;
    }
    else
    {
        CSM_ERR("[ARRAY] Full");
    }
    return ret;
}


int csm_array_resize(csm_array *array, uint32_t new_size)
{
    int ret = TRUE;
    array->size = new_size;
    if (array->rd_index > new_size)
    {
        array->rd_index = 0U;
        ret = FALSE;
    }
    if (array->wr_index > new_size)
    {
        array->wr_index = 0U;
        ret = FALSE;
    }
    return ret;
}

int csm_array_sub(const csm_array *i_array, csm_array *o_array, uint32_t i_index)
{
    return csm_array_mid(i_array, o_array, i_index, i_array->size - i_index);
}

int csm_array_mid(const csm_array *i_array, csm_array *o_array, uint32_t index, uint32_t i_size)
{
    int ret = TRUE;
    CSM_ASSERT(i_array != NULL);
    CSM_ASSERT(o_array != NULL);

    if (index > i_array->size)
    {
        index = 0U;
        ret = FALSE;
    }
    if (i_size > (i_array->size - index))
    {
        i_size = (i_array->size - index);
        ret = FALSE;
    }

    o_array->buff = (i_array->buff + index);
    o_array->size = i_size;
    o_array->rd_index = 0U;
    o_array->wr_index = 0U;

    if (ret == FALSE)
    {
        CSM_ERR("[ARRAY] Sub array parameters out of range");
    }

    return ret;
}


/*
void hexdump(void *ptr, int buflen)
{
    unsigned char *buf = (unsigned char*)ptr;
    int i, j;
    for (i = 0; i < buflen; i += 16)
    {
        for (j=0; j<16; j++)
        {
            if (i+j < buflen)
                printf("%02x ", buf[i+j]);
            else
                printf("   ");
        }
        printf(" ");
        printf("\n");
    }
}
*/

void csm_array_dump(csm_array *i_array)
{
    for (uint32_t i = 0U; i < i_array->size; i++)
    {
        if (i > 0U)
        {
            CSM_TRACE(":");
        }
        CSM_TRACE("%02X", i_array->buff[i]);
    }
    CSM_TRACE("\r\n");
}

int csm_array_writer_jump(csm_array *array, uint32_t nb_bytes)
{
    int ret = TRUE;

    CSM_ASSERT(array != NULL);
    array->wr_index += nb_bytes;
    if (array->wr_index >= array->size)
    {
        // saturate
        array->wr_index = array->size;
        ret = FALSE;
    }

    return ret;
}

int csm_array_reader_jump(csm_array *array, uint32_t nb_bytes)
{
    int ret = TRUE;

    CSM_ASSERT(array != NULL);
    array->rd_index += nb_bytes;
    if (array->rd_index >= array->size)
    {
        // saturate
        array->rd_index = array->size;
        ret = FALSE;
    }

    return ret;
}

int csm_array_copy(csm_array *to_array, csm_array *from_array)
{
    int ret = FALSE;
    CSM_ASSERT(from_array != NULL);
    CSM_ASSERT(to_array != NULL);

    // Clamp size to the minimum
    uint32_t size = (from_array->size <= to_array->size) ? from_array->size : to_array->size;
    // Actually perform the copy
    memcpy(to_array->buff, from_array->buff, size);

    if ((size == from_array->size) && (size == from_array->size))
    {
        ret = TRUE;
    }
    return ret;
}

int csm_array_read_buff(csm_array *array, uint8_t *to_buff, uint32_t size)
{
    CSM_ASSERT(array != NULL);
    CSM_ASSERT(to_buff != NULL);

    (void) memcpy(to_buff, csm_array_rd_data(array), size);
    return csm_array_reader_jump(array, size);
}

int csm_array_write_buff(csm_array *array, const uint8_t *buff, uint32_t size)
{
    int ret = FALSE;
    CSM_ASSERT(array != NULL);

    if ((array->wr_index + size) <= array->size)
    {
        (void) memcpy(csm_array_wr_data(array), &buff[0], size);
        array->wr_index += size;
        ret = TRUE;
    }
    else
    {
        CSM_ERR("[ARRAY] Full");
    }
    return ret;
}

uint32_t csm_array_remaining(csm_array *array)
{
    return (array->size - array->rd_index);
}

int csm_array_equal(csm_array *array1, csm_array *array2)
{
    int ret = FALSE;
    CSM_ASSERT(array1 != NULL);
    CSM_ASSERT(array2 != NULL);

    // Clamp size to the minimum
    uint32_t size = (array1->size <= array2->size) ? array1->size : array2->size;

    if (!memcmp(array1->buff, array2->buff, size))
    {
        ret = TRUE;
    }
    return ret;
}

uint8_t *csm_array_rd_data(csm_array *array)
{
    return (array->buff+array->rd_index);
}

uint8_t *csm_array_wr_data(csm_array *array)
{
    return (array->buff+array->wr_index);
}

int csm_array_read_u8(csm_array *io_array, uint8_t *io_byte)
{
    int ret = 0;
    CSM_ASSERT(io_array != NULL);
    if (io_array->rd_index < io_array->size)
    {
        *io_byte = io_array->buff[io_array->rd_index];
        io_array->rd_index++;
        ret = 1;
    }
    else
    {
        CSM_ERR("[ARRAY] No more data");
    }
    return ret;
}

int csm_array_read_u32(csm_array *array, uint32_t *value)
{
    int ret = FALSE;
    if (csm_array_remaining(array) >= 4U)
    {
        uint8_t *start = csm_array_rd_data(array);
        *value = GET_BE32(start);
        ret = csm_array_reader_jump(array, 4U);
    }
    return ret;
}

int csm_array_read_u16(csm_array *array, uint16_t *value)
{
    int ret = FALSE;
    if (csm_array_remaining(array) >= 2U)
    {
        uint8_t *start = csm_array_rd_data(array);
        *value = GET_BE16(start);
        ret = csm_array_reader_jump(array, 2U);
    }
    return ret;
}
