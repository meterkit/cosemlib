#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#include <stdint.h>

#define SECTOR_SIZE		(4U * 1024U)

void bsp_flash_initialize();
int bsp_flash_read(void * data, uint32_t block, uint32_t datalen);
int bsp_flash_write(void * data, uint32_t block, uint32_t size);
int bsp_flash_erase(uint32_t block);
void bsp_flash_suspend();
void bsp_flash_resume();
void bsp_flash_stop();

#endif // BSP_FLASH_H
