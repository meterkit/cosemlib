
#include "fs.h"

// HAL includes
#include "bsp_flash.h"
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

#define FS_DATA_SIZE    (SECTOR_SIZE - sizeof(fs_header))

typedef struct {
    fs_header header;
    uint8_t data[FS_DATA_SIZE];
} fs_cache;


// File system state variables
static fs_cache gCache;
static uint8_t gLocked = 0U;
static uint8_t gFileOpen = 0U;
static fs_handle *gCurrentHandle = NULL;

// Read/write access variables
static uint16_t gRdIndex = 0U;
static uint16_t gWrIndex = 0U;

// Static filesystem parameters
static const fs_file_cfg *gConfFiles = NULL;
static int gConfSize = 0;


#define FS_LOCK do { \
                    os_lock(); \
                    gLocked = 1U; \
                } while(0)

#define FS_UNLOCK do { \
                    os_unlock(); \
                    gLocked = 0U; \
                } while(0)

#define FS_ISFREE   (gFileOpen == 0U)


static const uint16_t gNoFileName = 0xFFFFU;


// ============================================================================
// PRIVATE FUNCTIONS
// ============================================================================

static inline int fs_nb_blocks()
{

}


static inline void fs_init_cache()
{
    memset(&gCache, 0xFF, sizeof(gCache));
}

static inline int fs_is_open(fs_handle *handle)
{
    int ret = FS_FILE_NOT_OPEN;
    if ((gFileOpen == 1U) && (gCurrentHandle == handle))
    {
        ret = FS_OK;
    }
    return ret;
}

static inline void fs_take(fs_handle *handle)
{
    gFileOpen = 1U;
    gCurrentHandle = handle;
}

static inline int fs_free(fs_handle *handle)
{
    int ret = FS_BAD_HANDLE;
    gFileOpen = 0U;
    if (gCurrentHandle == handle)
    {
        ret = FS_OK;
    }

    return ret;
}


// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

void fs_initialize(const fs_file_cfg *files, int size)
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

int fs_open(fs_handle *handle, uint16_t name)
{
    int ret = FS_BUSY;

    FS_LOCK;

    if (FS_ISFREE)
    {
        fs_take(handle);

        int access = fs_get_file(handle, name);
        if (access == FS_OK)
        {
            // Check the first block of the file if it is the right file
            bsp_flash_read(&gCache, handle->file->block, SECTOR_SIZE);

            // Check physical file state
            if (handle->file->name == gNoFileName)
            {
                // Make sure that this block is fully erased
                bsp_flash_erase(handle->file->block);
                fs_init_cache();
                ret = FS_FILE_EMPTY;
            }
            else if (handle->file->name != gCache.header.name)
            {
                ret = FS_WRONG_FILE;
            }
            else
            {
                ret = FS_OK;
            }

            gRdIndex = 0U;
            gWrIndex = 0U;
        }
        else
        {
            // Copy failure reason
            ret = access;
        }
    }

    FS_UNLOCK;

    return ret;
}

int fs_read(fs_handle *handle, void *data, int size)
{
    int ret = FS_BUSY;

    FS_LOCK;

    if (FS_ISFREE)
    {
        fs_take(handle);

        int access = fs_is_open(handle);
        if (access == FS_OK)
        {
            // Check the first block of the file if it is the right file
            bsp_flash_read(&gCache, handle->file->block, SECTOR_SIZE);

            // Check physical file state
            if (handle->file->name == gNoFileName)
            {
                // Make sure that this block is fully erased
                bsp_flash_erase(handle->file->block);
                fs_init_cache();
                ret = FS_FILE_EMPTY;
            }
            else if (handle->file->name != gCache.header.name)
            {
                ret = FS_WRONG_FILE;
            }
            else
            {
                ret = FS_OK;
            }
        }
        else
        {
            // Copy failure reason
            ret = access;
        }
    }

    FS_UNLOCK;

    return ret;
}


int fs_close(fs_handle *handle)
{
    FS_LOCK;
    int ret = fs_free(handle);
    FS_UNLOCK;
    return ret;
}


