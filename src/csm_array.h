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

void csm_array_init(csm_array *io_array);
void csm_array_alloc(csm_array *io_array, uint8_t *i_buff, uint32_t i_size);
int csm_array_get(const csm_array *i_array, uint32_t i_index, uint8_t *io_byte);

// Copy an array to another
// The size is clamped to the minimum of both arrays
int csm_array_copy(csm_array *to_array, csm_array *from_array);

int csm_array_set(csm_array *io_array, uint32_t i_index, uint8_t i_byte);
int csm_array_write(csm_array *io_array, uint8_t i_byte);
int csm_array_read(csm_array *io_array, uint8_t *io_byte);
int csm_array_jump(csm_array *array, uint32_t nb_bytes);
int csm_array_sub(const csm_array *i_array, csm_array *o_array, uint32_t i_index);
int csm_array_mid(const csm_array *i_array, csm_array *o_array, uint32_t i_index, uint32_t i_size);
void csm_array_dump(csm_array *i_array);

#endif // CSM_ARRAY_H
