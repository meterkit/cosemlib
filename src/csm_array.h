/**
 * Copyright (c) 2016, Anthony Rabine
 * See LICENSE.txt
 *
 * Implementation of a protected array with read/write pointers
 */

#ifndef CSM_ARRAY_H
#define CSM_ARRAY_H

// standard includes
#include <stdint.h>

// library includes
#include "csm_config.h"

typedef struct {
    uint32_t rd_index;
    uint32_t wr_index;
    uint32_t size;
    uint8_t *buff;
} csm_array;

void csm_array_init(csm_array *io_array, uint8_t *buffer, uint32_t max_size, uint32_t used_size);
int csm_array_get(const csm_array *i_array, uint32_t i_index, uint8_t *io_byte);

// Copy an array to another
// The size is clamped to the minimum of both arrays
int csm_array_copy(csm_array *to_array, csm_array *from_array);
int csm_array_equal(csm_array *array1, csm_array *array2);

int csm_array_resize(csm_array *array, uint32_t new_size);
uint8_t *csm_array_rd_data(csm_array *array);
uint8_t *csm_array_wr_data(csm_array *array);

int csm_array_set(csm_array *io_array, uint32_t i_index, uint8_t i_byte);

// Functions that advance the write pointer
int csm_array_write_buff(csm_array *array, const uint8_t *buff, uint32_t size);
int csm_array_write_u8(csm_array *array, uint8_t byte);
int csm_array_writer_jump(csm_array *array, uint32_t nb_bytes);

int csm_array_read_buff(csm_array *array, uint8_t *to_buff, uint32_t size);
int csm_array_read_u8(csm_array *io_array, uint8_t *io_byte);
int csm_array_read_u16(csm_array *array, uint16_t *value);
int csm_array_read_u32(csm_array *array, uint32_t *value);
int csm_array_reader_jump(csm_array *array, uint32_t nb_bytes);
int csm_array_sub(const csm_array *i_array, csm_array *o_array, uint32_t i_index);
int csm_array_mid(const csm_array *i_array, csm_array *o_array, uint32_t index, uint32_t i_size);
void csm_array_dump(csm_array *i_array);

// return the remaining bytes to read
uint32_t csm_array_remaining(csm_array *array);

#endif // CSM_ARRAY_H
