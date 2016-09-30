
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bsp_flash.h"
#include "unistd.h"

// Simulated flash: M25PE32 with 4KB subsectors, 64KB sectors, 4MB memory size
 //Minimum 100,000 ERASE cycles per sector

#define MEMORY_SIZE		(4U * 1024U * 1024U)
#define NUMBER_OF_SECTORS 	(MEMORY_SIZE / FS_BLOCK_SIZE)

uint8_t  gMemory[MEMORY_SIZE]; /* in-memory array for flash simulation */
uint32_t  gEraseCounter[NUMBER_OF_SECTORS];
static uint8_t  loaded = 0U;

void bsp_flash_initialize()
{
	if (!loaded)
	{
	    if( access( "mem.dat", F_OK ) != -1 )
	    {
	        // file exists
	        FILE * file = fopen( "mem.dat", "wb" );

            if (file)
            {
                fread( gMemory, 1, sizeof( gMemory ), file );
                fclose( file );
            }

	    }
	    else
	    {
	        // file doesn't exist
	        memset(&gMemory[0], 0xFF, MEMORY_SIZE);
	    }

		loaded = 1;
	}
}

void bsp_flash_suspend()
{
	
}

void bsp_flash_resume()
{
	
}

void bsp_flash_stop()
{
	FILE * file = file = fopen( "mem.dat", "wb+" );

	if (file)
	{
        fwrite( gMemory, 1, sizeof(gMemory), file );
		fclose( file );
		loaded = 1;
	}
}

int bsp_flash_read(void * data, uint32_t block, uint32_t offset, uint32_t datalen)
{
    memcpy( data, &gMemory[block*FS_BLOCK_SIZE + offset], datalen );
	return 1;
}


int bsp_flash_erase(uint32_t block)
{
	gEraseCounter[block]++;
    memset(&gMemory[block*FS_BLOCK_SIZE], 0xFF, FS_BLOCK_SIZE);
	return 1;
}


int bsp_flash_write(void * data, uint32_t block, uint32_t offset, uint32_t size)
{
    memcpy( &gMemory[block*FS_BLOCK_SIZE + offset], data, size );
	return 1;
}

