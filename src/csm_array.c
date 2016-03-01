/**
 * Copyright (c) 2016, Anthony Rabine
 * See LICENSE.txt
 *
 * Implementation of a protected array with read/write pointers
 */

#include "csm_array.h"
#include <string.h>


void csm_array_init(csm_array *io_array)
{
    CSM_ASSERT(io_array != NULL);
    io_array->buff = NULL;
    io_array->rd_index = 0U;
    io_array->wr_index = 0U;
    io_array->size = 0U;
}

void csm_array_alloc(csm_array *io_array, uint8_t *i_buff, uint32_t i_size)
{
    CSM_ASSERT(io_array != NULL);
    io_array->buff = i_buff;
    io_array->rd_index = 0U;
    io_array->wr_index = 0U;
    io_array->size = i_size;
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

int csm_array_write(csm_array *io_array, uint8_t i_byte)
{
    int ret = 0;
    CSM_ASSERT(io_array != NULL);
    if (io_array->wr_index < io_array->size)
    {
        io_array->buff[io_array->wr_index] = i_byte;
        io_array->wr_index++;
        ret = 1;
    }
    else
    {
        CSM_ERR("[ARRAY] Full");
    }
    return ret;
}

int csm_array_read(csm_array *io_array, uint8_t *io_byte)
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


int csm_array_sub(const csm_array *i_array, csm_array *o_array, uint32_t i_index)
{
    return csm_array_mid(i_array, o_array, i_index, i_array->size - i_index);
}


int csm_array_mid(const csm_array *i_array, csm_array *o_array, uint32_t i_index, uint32_t i_size)
{
    int ret = 1;
    CSM_ASSERT(i_array != NULL);
    CSM_ASSERT(o_array != NULL);

    if (i_index > i_array->size)
    {
        i_index = 0U;
        ret = 0;
    }
    if (i_size > (i_array->size - i_index))
    {
        i_size = (i_array->size - i_index);
        ret = 0;
    }

    o_array->buff = (i_array->buff + i_index);
    o_array->size = i_size;
    o_array->rd_index = 0U;
    o_array->wr_index = 0U;

    if (ret == 0)
    {
        CSM_ERR("[ARRAY] Sub array parameters out of range");
    }

    return ret;
}

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



int csm_array_jump(csm_array *array, uint32_t nb_bytes)
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
