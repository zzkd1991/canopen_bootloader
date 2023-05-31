#ifndef _RECOVER_H
#define _RECOVER_H

#include <string.h>
#include <stdint.h>

typedef struct
{
	uint16_t index;
	uint16_t subindex;
	uint32_t id;
}Data_Store_Format;

#define STORE_DEVICE_ID_ADDRESS			0

#define MODIDY_DEVICE_ID_ADDRESS		0x100

#define LVDT2_ZERO_LOCATION_VALVE_ADDRESS	SPI_FLASH_PAGE_SIZE	* 7
#define LVDT1_ZERO_LOCATION_VALVE_ADDRESS	SPI_FLASH_PAGE_SIZE * 6


void Data_Recover(void);

#endif
