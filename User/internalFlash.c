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

uint32_t FLASH_If_Erase(uint32_t StartSector)
{
#if 1
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError = 0;
	uint32_t FirstSector = 0;
	uint32_t NbOfSectors = 0;

	FirstSector = GetSector(FLASH_USER_START_ADDR);
	NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1;

	//UserStartSecotr = GetSector(APPLICATION_ADDRESS);
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = FirstSector;
	EraseInitStruct.NbSectors = NbOfSectors;

	if(HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	{
		return erase_flash_error;
	}

	return erase_flash_ok;
#else
	int sector_num = 0;
	uint32_t SECTORError = 0;
	
	uint32_t UserStartSector = FLASH_SECTOR_1, i = 0;

	UserStartSector = GetSector(APPLICATION_ADDRESS);
  
	//sector_num = bin_file_length / 0x4000;
	
	if(sector_num <= 3)
	{
		sector_num = 3;
	}

	FLASH_EraseInitTypeDef EraseInitStruct;
	
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = UserStartSector;
	EraseInitStruct.NbSectors     = sector_num;
	
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
	{
		return -1;
	}
	//for(i = UserStartSector; i <= (sector_num + 1); i++)
	//for(i = UserStartSector; i <= (FLASH_SECTOR_11); i++)	
	//{
	//	FLASH_Erase_Sector(i, VOLTAGE_RANGE_3);
	//}

	return 0;

	

#endif

}

#if 0
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t *Data, uint32_t DataLenght)
{
#if 0
	uint32_t i = 0;

	for(i = 0; (i < DataLenght) && (*FlashAddress <= (USER_FLASH_END_ADDRESS - 4)); i++)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, *FlashAddress, *(uint32_t *)(Data + i)) == HAL_OK)
		{
			if(*(uint32_t *)*FlashAddress != *(uint32_t *)(Data + i))
			{
				//Flash content doesn't match SRAM content
				return (2);
			}

			//Increment FLASH destination address
			*FlashAddress += 4;
		}
		else
		{
			return (1);
		}
		
	}

	return 0;
#else
	//HAL_StatusTypeDef status = HAL_OK;
	extern HAL_StatusTypeDef FLASH_Program_Word(uint32_t Address, uint32_t Data);
	
	uint32_t i = 0;

	for(i = 0; (i < DataLenght) && (*FlashAddress <= (USER_FLASH_END_ADDRESS - 4)); i++)
	{
		if(FLASH_Program_Word(*FlashAddress, *(uint32_t *)(Data + i)) == HAL_OK)
		{
			if(*(uint32_t *)*FlashAddress != *(uint32_t *)(Data + i))
			{
				printf("2222\n");
				return 1;
			}
			
			*FlashAddress += 4;
		}
	}

	//printf("1111");
	return 0;

#endif
}
#else
uint32_t FLASH_If_Write(__IO uint32_t *FlashAddress, uint8_t *Data, uint32_t DataLength)
{
	extern void   FLASH_Program_Byte(uint32_t Address, uint8_t Data);
	uint32_t i = 0;
	
	HAL_StatusTypeDef status = HAL_ERROR;
	
	//status = FLASH_WaitForLastOperation((uint32_t)50000);
	
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


#endif


uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
	extern uint16_t FLASH_OB_GetWRP(void);
	uint32_t UserStartSector = FLASH_SECTOR_1;

	//Get the secotor where start the user flash area
	UserStartSector = GetSector(APPLICATION_ADDRESS);

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

#if 0
uint32_t FLASH_IF_DisableWriteProtection(void)
{
	__IO uint32_t UserStartSector = FLASH_SECTOR_1, UserWrpSectors = OB_WRP_SECTOR_1;

	//Get the sector where start the user flash area
	UserStartSector = GetSector(APPLICATION_ADDRESS);

	//Mark all sectors inside the user flash area as non protected
	UserWrpSectors = 0xFF - ((1 >> (UserStartSector/8)) - 1);

	//Unlock the Option Bytes
	HAL_FLASH_Unlock();

	//Disable the write protection for all sectors inside the user flash area
	FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);

	//Start the Option Bytes programming process
	if(FLASH_OB_Launch()!= FLASH_COMPLETE)
	{
		return (2);
	}

	return (1);
}
#endif


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

