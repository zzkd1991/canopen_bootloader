#include "internalFlash.h"
#include <stdio.h>
#include <string.h>

#define DATA_32   ((uint32_t)0x87564321)


#define FLASH_USER_START_ADDR	ADDR_FLASH_SECTOR_2

#define FLASH_USER_END_ADDR		ADDR_FLASH_SECTOR_7

#define FLASH_SECTOR_ILEGAL		0xFFFFFFFF

static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_SECTOR_7;  
  }
  return sector;
}

uint32_t FLASH_If_Erase(uint32_t StartSector, uint32_t EndSector)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError = 0;
	uint32_t FirstSector = 0;
	uint32_t NbOfSectors = 0;

	FirstSector = GetSector(StartSector);
	NbOfSectors = GetSector(EndSector) - FirstSector + 1;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = FirstSector;
	EraseInitStruct.NbSectors = NbOfSectors;

	if(HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	{
		return erase_flash_error;
	}

	return erase_flash_ok;
}

uint32_t FLASH_If_Write(__IO uint32_t *FlashAddress, uint8_t *Data, uint32_t DataLength)
{
	extern void   FLASH_Program_Byte(uint32_t Address, uint8_t Data);
	uint32_t i = 0;
	
	HAL_StatusTypeDef status = HAL_ERROR;
	
	for(i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS - 1)); i++)
	{
		FLASH_Program_Byte(*FlashAddress, *(uint8_t *)(Data + i));
		
		status = FLASH_WaitForLastOperation((uint32_t)50000);
		
		if(status != HAL_OK)
		{
			return write_flash_error;
		}
		
		FLASH->CR &= (~FLASH_CR_PG);
		
		if(*(uint8_t *)*FlashAddress != *(uint8_t *)(Data + i))
		{
			return write_flash_error;
		}
		*FlashAddress += 1;
	}

	return write_flash_ok;
}

uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
	extern uint16_t FLASH_OB_GetWRP(void);
	uint32_t UserStartSector = FLASH_SECTOR_1;

	//Get the secotor where start the user flash area
	UserStartSector = GetSector(APPLICATION_ADDRESS_START);

	//Check if there are write protected sectors inside the user flash area
	if((FLASH_OB_GetWRP() >> (UserStartSector/8)) == (0xFF >> (UserStartSector/8)))
	{	//No write protected sectors inside the user flash area
		return 1;
	}
	else
	{
		//Some sectors inside the user flash area are write protected
		return 0;
	}
}

int FLASH_If_Read(__IO uint32_t *FlashAddr, uint8_t *buf, uint32_t DataLength)
{
	uint32_t location_cur;

	if((buf == NULL) || (DataLength >= FLASH_USER_END_ADDR - FLASH_USER_START_ADDR))
	{
		return -1;
	}

	location_cur = *FlashAddr;

	if((GetSector(location_cur) != FLASH_SECTOR_ILEGAL) &&
		(GetSector(location_cur + DataLength) != FLASH_SECTOR_ILEGAL))
	{
		memcpy(buf, (uint8_t *)FlashAddr, DataLength);
		return 0;
	}

	return -1;
}

int InternalFlash_Test(void)
{
	uint32_t FirstSector = 0;
	uint32_t NbOfSectors = 0;

	uint32_t SECTORError = 0;

	uint32_t Address = 0;

	__IO uint32_t Data32 = 0;
	__IO uint32_t MemoryProgramStatus = 0;
	static FLASH_EraseInitTypeDef EraseInitStruct;

	HAL_FLASH_Unlock();

	FirstSector = GetSector(FLASH_USER_START_ADDR);
	NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = FirstSector;
	EraseInitStruct.NbSectors = NbOfSectors;

	if(HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	{
		return -1;
	}

	Address = FLASH_USER_START_ADDR;

	while(Address < FLASH_USER_END_ADDR)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DATA_32) == HAL_OK)
		{
			Address = Address + 4;
		}
		else
		{
			return -1;
		}
	}

	HAL_FLASH_Lock();

	Address = FLASH_USER_START_ADDR;
	MemoryProgramStatus = 0;

	while(Address < FLASH_USER_END_ADDR)
	{
		Data32 = *(__IO uint32_t*)Address;

		if(Data32 != DATA_32)
		{
			MemoryProgramStatus++;
		}

		Address = Address + 4;
	}

	if(MemoryProgramStatus)
	{
		return -1;
	}
	else
	{
		return 0;
	}
	
}

void FLASH_If_Init(void)
{
	HAL_FLASH_Unlock();

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
							FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

