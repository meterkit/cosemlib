/**
 * Copyright (c) 2016, Anthony Rabine
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the BSD license.
 * See LICENSE.txt for more details.
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
    uint32_t offset; // optional offset where the logical 0 position begins
    uint32_t size;
    uint8_t *buff;
} csm_array;

void csm_array_init(csm_array *array, uint8_t *buffer, uint32_t max_size, uint32_t used_size, uint32_t offset);
void csm_array_dump(csm_array *array);

int csm_array_get(const csm_array *array, uint32_t index, uint8_t *byte);
int csm_array_set(csm_array *array, uint32_t index, uint8_t byte);

uint8_t *csm_array_rd_data(csm_array *array);
uint8_t *csm_array_wr_data(csm_array *array);

// Functions that advance the write pointer
int csm_array_write_buff(csm_array *array, const uint8_t *buff, uint32_t size);
int csm_array_write_u8(csm_array *array, uint8_t byte);
int csm_array_writer_jump(csm_array *array, uint32_t nb_bytes);

// Functions that advance the read pointer
int csm_array_read_buff(csm_array *array, uint8_t *to_buff, uint32_t size);
int csm_array_read_u8(csm_array *array, uint8_t *byte);
int csm_array_read_u16(csm_array *array, uint16_t *value);
int csm_array_read_u32(csm_array *array, uint32_t *value);
int csm_array_reader_jump(csm_array *array, uint32_t nb_bytes);

// return the remaining bytes to read
uint32_t csm_array_unread(csm_array *array);

#endif // CSM_ARRAY_H
