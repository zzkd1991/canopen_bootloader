#ifndef __INTERNAL_FLASH_H
#define __INTERNAL_FLASH_H

#include "stm32f4xx.h"

/* Base address of the Flash sectors */ 
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes  */

//End of the Flash address
#define USER_FLASH_END_ADDRESS	ADDR_FLASH_SECTOR_7
//Define the user application size
#define USER_FLASH_SIZE		(USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)

//Define the address from where user application will be loaded.
//Note: the 1st sector 0x08888888-0x08003FFF is reserved for hte IAP code
#define APPLICATION_ADDRESS	(uint32_t)0x08008000

#define OTA_ERRNO_OK		0
#define OTA_ERRNO_ILEGAL_PARAM	-1
#define OTA_ERRNO_INTEGRITY_CHECK	-2
#define OTA_ERRNO_SPI_FLASH_READ	-3
#define OTA_ERRNO_SPI_FLASH_WRITE	-4
#define OTA_ERRNO_INNER_FLASH_READ	-5
#define OTA_ERRNO_INNER_FLASH_WRITE	-6
#define OTA_ERRNO_ILEGAL_PC			-7
#define OTA_ERRNO_ILEGAL_STACK		-8
#define OTA_ERRNO_INTERNEL			-9


#define OTA_COPY_BUF_SIZE	0x1000

typedef enum
{
	write_flash_ok = 0,
	write_flash_error = 1, 
}WRITE_FLASH_STATUS;

typedef enum
{
	erase_flash_ok = 0,
	erase_flash_error = 1,
}ERASE_FLASH_STATUS;

int InternalFlash_Test(void);

uint32_t FLASH_If_Erase(uint32_t StartSector);

uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint8_t *Data, uint32_t DataLenght);

int FLASH_If_Read(__IO uint32_t *FlashAddr, uint8_t *buf, uint32_t DataLength);


uint16_t FLASH_If_GetWriteProtectionStatus(void);

void FLASH_If_Init(void);
#endif
