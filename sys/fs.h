#ifndef FS_H
#define FS_H

#include <stdint.h>

typedef enum
{
    FS_OK = 0,
    FS_ERROR = 1,
    FS_FILE_NOT_FOUND = 2,
    FS_FILE_EMPTY = 3,
    FS_BUSY = 4,
    FS_WRONG_FILE,
} fs_status;

typedef struct
{
    uint16_t name; //< Unique file identifier
    uint8_t type; //< S (Structure) or L (Log)
    uint8_t block;
    uint8_t nb_blocks;
    uint32_t size;
    const void *default_data; //< Points to the default data, if null set everything to zero
} fs_file_cfg;

typedef struct
{
    const fs_file_cfg *file;

} fs_handle;

typedef struct
{
    uint32_t counter;
    uint16_t name;
    uint16_t size;          //< Physical size used for signature
    uint8_t next;           //< Next block number
    uint8_t info;           //<
    uint8_t signature[20];  //< signature ideally with a private key
} fs_header;


// FS API
void fs_initialize(const fs_file_cfg *files, int size);
int fs_open(fs_handle *handle, uint16_t name);


#endif // FS_H
