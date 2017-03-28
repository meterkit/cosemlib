
#include "fs.h"

// HAL includes
#include "os.h"

/**
  Super simple file system for NOR serial flash memories
  This FS manages all the files as a log (N entries, FIFO style). Wear leveling is natural and
  left on the static configuration of files.

    - Two types of files: Structure or Log
    - It must be possible to declare an optional default file value
    - Static file configuration
    - Stream reading (memorisation of a read index)
    - Random access, dichonomy search (for Cosem selective access searches)
    - Signed (implementation specific with default algorithm)
    - Manage file extension (after firmware upgrade)

 */





// Global shared variables
static const fs_config *gConfFiles = NULL;
static int gConfSize = 0;

// Big FS lock, not used if API called in one unique context
//static uint8_t gLocked = 0U;



#define FS_LOCK(handle) do { \
                } while(0)

#define FS_UNLOCK(handle) do { \
                } while(0)

/*
#define FS_LOCK(handle) do { \
                    os_lock(); \
                    handle->gLocked = 1U; \
                } while(0)

#define FS_UNLOCK(handle) do { \
                    os_unlock(); \
                    handle->gLocked = 0U; \
                } while(0)

*/

#define FS_ISFREE(handle)   (handle->open == 0U)



static const uint16_t gNoFileName = 0xFFFFU;


// ============================================================================
// PRIVATE FUNCTIONS
// ============================================================================

static inline int fs_is_open(fs_handle *handle)
{
    int ret = FS_FILE_NOT_OPEN;
    if (handle->open == 1U)
    {
        ret = FS_OK;
    }
    return ret;
}

static inline void fs_take(fs_handle *handle)
{
    handle->open = 1U;
}

static inline int fs_release(fs_handle *handle)
{
    int ret = FS_OK;
    handle->open = 0U;

    return ret;
}

/*
// Calculate how many block is taken by a record
static inline int fs_nb_blocks(fs_handle *handle)
{
    int total_size = handle->file->record_size + FS_HEADER_SIZE;
    return div_round_up(total_size, FS_BLOCK_SIZE);
}

static inline int fs_get_block(fs_handle *handle, int record)
{
    int total_size = handle->file->nb_records * handle->file->record_size;
    return div_round_up(total_size, FS_DATA_SIZE);
}
*/
/*

static inline void fs_init_cache(fs_handle *handle)
{
    memset(&handle->cache, 0xFF, sizeof(handle->cache));
}


static inline int fs_cache_hit(fs_handle *handle, uint32_t index)
{
    int ret = (handle->cache.header.name == handle->file->name); // right file

    // Check the block currently targeted
    int block = index / FS_DATA_SIZE;

    ret = ret && handle->cache.header

    return ret;
}
*/


// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

void fs_initialize(const fs_config *files, int size)
{
    gConfFiles = files;
    gConfSize = size;

    bsp_flash_initialize();
}


static fs_status fs_get_file(fs_handle *handle, uint16_t name)
{
    fs_status ret = FS_FILE_NOT_FOUND;

    for (uint8_t i = 0U; i < gConfSize; i++)
    {
        if (gConfFiles[i].name == name)
        {
            handle->file = &gConfFiles[i];
            ret = FS_OK;
            break;
        }
    }

    return ret;
}

typedef struct
{
    uint16_t block;
    uint32_t offset;
} fs_pos;

int fs_seek(fs_handle *handle, uint32_t index, fs_pos *pos)
{
    int ret = FS_ERROR;

    // Determine the relative block currently targeted
    uint16_t block = (index / FS_DATA_SIZE);
    // Determine the relative offset within the block
    uint32_t offset = FS_HEADER_SIZE + (index - (block * FS_BLOCK_SIZE));

    // 1. First read the header of the block
    bsp_flash_read(&handle->header, block + handle->file->first_block, 0U, FS_HEADER_SIZE);

    if (handle->header.name == gNoFileName)
    {
        // Make sure that this block is fully erased
        bsp_flash_erase(handle->file->first_block);
        handle->header.name = handle->file->name;

        ret = FS_OK;

    }
    else if (handle->file->name != handle->header.name)
    {
        ret = FS_WRONG_FILE;
    }
    else
    {
        ret = FS_OK;
    }

    // Save where we are
    pos->block = block;
    pos->offset = offset;


    handle->rd_index = offset;
    handle->wr_index = offset;

    return ret;
}

int fs_open(fs_handle *handle, uint16_t name)
{
    int ret = FS_BUSY;

    fs_take(handle);

    int access = fs_get_file(handle, name);
    if (access == FS_OK)
    {
        fs_pos pos;
        ret = fs_seek(handle, 0U, &pos);
    }
    else
    {
        // Copy failure reason
        ret = access;
    }

    return ret;
}



int fs_read(fs_handle *handle, void *data, uint32_t size)
{
    int ret = FS_BUSY;
    uint8_t *data_ptr = data;

    fs_take(handle);

    int access = fs_is_open(handle);
    if (access == FS_OK)
    {
        while (size > 0U)
        {
            uint32_t len = size;
            fs_pos pos;
            if (fs_seek(handle, handle->rd_index, &pos) == FS_OK)
            {
                uint32_t max_size = FS_DATA_SIZE - pos.offset;
                if (len > max_size)
                {
                    len = max_size;
                }
                bsp_flash_read(data_ptr, pos.block + handle->file->first_block, pos.offset + FS_HEADER_SIZE, len);

                size -= len;
                data_ptr += len;
            }
        }
    }
    else
    {
        // Copy failure reason
        ret = access;
    }

    return ret;
}


int fs_close(fs_handle *handle)
{
    FS_LOCK(handle);
    int ret = fs_release(handle);
    FS_UNLOCK(handle);
    return ret;
}


