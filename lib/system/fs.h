#ifndef FS_H
#define FS_H

#include <stdint.h>

#include "bsp_flash.h"
#include "fs_config.h"

typedef enum
{
    FS_FILE_NOT_OPEN = -7,
    FS_BAD_HANDLE = -6,
    FS_WRONG_FILE = -5,
    FS_BUSY = -4,
    FS_FILE_EMPTY = -3,
    FS_FILE_NOT_FOUND = -2,
    FS_ERROR = -1,
    FS_OK = 0,
} fs_status;

typedef struct
{
    uint16_t name; //< Unique file identifier
    uint16_t type; //< S (Structure) or F (FIFO)
    uint16_t first_block;
    uint16_t nb_records;     //< Number of records (a record can contains one or more blocks)
    uint32_t record_size;   //< Size of one record
    uint8_t status; //< 0=OK, 1=obsolete
} fs_config;


/**
 * @brief Block header structure
 *
 * Make sure the signature is a multiple of 4 bytes to avoid padding
 */
typedef struct
{
    uint8_t signature[FS_SIGNATURE_SIZE];  //< Optional signature of the data, written when the block is full FIXME: make the size configurable
    uint32_t counter;       //< Counter written when the block is full
    uint16_t name;          //< File name (for consistency checks)
    uint16_t next;           //< Absolute next logical block number
    uint8_t info;           //< Block information
} fs_header;

#define FS_HEADER_SIZE  (sizeof(fs_header))
#define FS_DATA_SIZE    (FS_BLOCK_SIZE - FS_HEADER_SIZE)

typedef struct {
    fs_header header;
    uint8_t data[FS_DATA_SIZE];
} fs_cache;


/**
 * @brief File state structure, independent for each opened file
 */
typedef struct {
 //   fs_cache cache;
    fs_header header;

    uint8_t open;
    // Read/write access variables
    uint32_t rd_index;
    uint32_t wr_index;

    const fs_config *file;

} fs_handle;


// FS API
void fs_initialize(const fs_config *files, int size);
int fs_open(fs_handle *handle, uint16_t name);
int fs_close(fs_handle *handle);
int fs_read(fs_handle *handle, void *data, uint32_t size);

#endif // FS_H
