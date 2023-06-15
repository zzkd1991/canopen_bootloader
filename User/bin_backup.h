#ifndef BIN_BACKUP_H_
#define BIN_BACKUP_H_

#include "spi_flash.h"
#include <stdint.h>

#define OLD_IMAGE_LENGTH_ADDR	(2 * SPI_FLASH_PAGE_SIZE) 

#define OTA_IMAGE_BCK_ADDR		(3 * SPI_FLASH_PAGE_SIZE)


int prv_spi2inner_copy(uint32_t srcAddr, int32_t imgLen);

int prv_inner2spi_copy(int32_t imgLen);

int store_old_image_length(uint32_t address, uint32_t image_length);

void get_old_image_length(uint32_t address, uint32_t* image_length);

void copy_bin_from_oldaddress_to_newaddress(void);
void copy_bin_from_newaddress_to_oldaddress(void);


#endif


