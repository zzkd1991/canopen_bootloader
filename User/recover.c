#include "recover.h"
#include "spi_flash.h"

extern uint32_t id1;
extern uint32_t master_nodeid;

Data_Store_Format array[3];

void Data_Recover(void)
{
	spi_flash_read(0, (uint8_t *)&array[0], sizeof(array));
	
	id1 = array[0].id;
	//id1 = 0x20;
}



