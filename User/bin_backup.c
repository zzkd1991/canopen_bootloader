#include "internalFlash.h"
#include "main.h"
#include "stm32f4xx.h"
#include "bsp_can.h"
#include "recover.h"
#include "spi_flash.h"
#include "bin_backup.h"
#include <stdio.h>

int prv_spi2inner_copy(uint32_t srcAddr, int32_t imgLen)
{
	int ret;
	int32_t copyLen;
	int remainLen;
	uint8_t buf[OTA_COPY_BUF_SIZE];
	uint32_t destAddr = APPLICATION_ADDRESS_START;

	ret = FLASH_If_Erase(APPLICATION_ADDRESS_START, APPLICATION_ADDRESS_END);

	if(erase_flash_ok != ret)
	{
		return OTA_ERRNO_INNER_FLASH_WRITE;
	}

	remainLen = imgLen;
	while(remainLen > 0)
	{
		copyLen = remainLen > OTA_COPY_BUF_SIZE ? OTA_COPY_BUF_SIZE : remainLen;
		spi_flash_read(srcAddr, buf, copyLen);

		srcAddr += copyLen;
		ret = FLASH_If_Write(&destAddr, (uint8_t *)buf, copyLen);
		if(ret != write_flash_ok)
		{
			return OTA_ERRNO_INNER_FLASH_WRITE;
		}

		remainLen -= copyLen;
	}


	return OTA_ERRNO_OK;
	
}

int prv_inner2spi_copy(int32_t imgLen)
{
	int ret;
	int remainLen;
	int32_t copyLen;
	int page_num = 0;
	uint8_t buf[OTA_COPY_BUF_SIZE];
	uint32_t srcAddr = APPLICATION_ADDRESS_START;
	uint32_t bckAddr = OTA_IMAGE_BCK_ADDR;

	page_num = imgLen / SPI_FLASH_PAGE_SIZE + 1;

	ret = spi_flash_erase_page(bckAddr, page_num, 1000);
	if(ret == 0)
	{
		return OTA_ERRNO_SPI_FLASH_WRITE;
	}

	remainLen = imgLen;
	while(remainLen > 0)
	{
		copyLen = imgLen > OTA_COPY_BUF_SIZE ? OTA_COPY_BUF_SIZE : remainLen;
		ret = FLASH_If_Read(&srcAddr, (uint8_t *)buf, copyLen);
		if(ret != 0)
		{
			return OTA_ERRNO_INNER_FLASH_READ;
		}

		srcAddr += copyLen;

		ret = spi_flash_write(bckAddr, buf, copyLen);

		if(ret == 0)
		{
			return OTA_ERRNO_INNER_FLASH_WRITE;
		}

		remainLen -= copyLen;
	}

	return OTA_ERRNO_OK;
}

int store_old_image_length(uint32_t address, uint32_t image_length)
{
	int ret;

	ret = spi_flash_erase_page(address, 1, 1000);
	if(ret == 0)
	{
		return OTA_ERRNO_SPI_FLASH_WRITE;
	}

	ret = spi_flash_write(address, (uint8_t *)&image_length, sizeof(image_length));

	if(ret == 0)
	{
		return OTA_ERRNO_INNER_FLASH_WRITE;
	}

	return OTA_ERRNO_OK;
	
}


void get_old_image_length(uint32_t address, uint32_t* image_length)
{
	spi_flash_read(address, (uint8_t *)image_length, sizeof(*image_length));
}

void copy_bin_from_oldaddress_to_newaddress(void)
{
	__IO uint8_t Data8 = 0;
	uint32_t source_address = 0;
	uint32_t dest_address = 0; 
	if(erase_flash_ok != FLASH_If_Erase(NEW_APPLICATION_ADDRESS_START, NEW_APPLICATION_ADDRESS_END))
	{
		Error_Handler();
	}

	source_address = APPLICATION_ADDRESS_START;
	dest_address = NEW_APPLICATION_ADDRESS_START;

	while(source_address < APPLICATION_ADDRESS_END)
	{
		Data8 = *(__IO uint8_t *)source_address;
		if(write_flash_error == FLASH_If_Write(&dest_address, (uint8_t *)&Data8, 1))
		{
			Error_Handler();
			return;
		}

		source_address += 1;
	}
}

void copy_bin_from_newaddress_to_oldaddress(void)
{
	__IO uint8_t Data8 = 0;
	uint32_t source_address = 0;
	uint32_t dest_address = 0;
	if(erase_flash_ok != FLASH_If_Erase(APPLICATION_ADDRESS_START, APPLICATION_ADDRESS_END))
	{
		Error_Handler();
	}

	source_address = NEW_APPLICATION_ADDRESS_START;
	dest_address = APPLICATION_ADDRESS_START;

	while(source_address < NEW_APPLICATION_ADDRESS_END)
	{
		Data8 = *(__IO uint8_t *)source_address;
		if(write_flash_error == FLASH_If_Write(&dest_address, (uint8_t *)&Data8, 1))
		{
			Error_Handler();
			return;
		}

		source_address += 1;
	}
}


