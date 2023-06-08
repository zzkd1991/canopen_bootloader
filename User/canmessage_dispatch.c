#include "bsp_can.h"
#include "recover.h"
#include "internalFlash.h"
#include "spi_flash.h"
#include "can_queue.h"
#include "block_download.h"
#include "crc.h"
#include "led.h"

uint32_t enter_bootloader_flag = 0;
uint64_t bin_file_length = 0;
uint16_t deviceid_array[20] = {0};
int cnt = 10;
uint32_t block_received_packet_num = 0;


int device_find(uint16_t id)
{
	int i = 0;
	for(i = 0; i < sizeof(deviceid_array)/ sizeof(deviceid_array[0]); i++)
	{
		if(deviceid_array[i] == id)
		{
			return 0;
		}
	}
	
	
	return 1;
}

int NEW_Can_Message_Dispatch(Message *m)
{
	//uint8_t ccs = 0;

	//ccs = getSDOcs(m->data[0]);

	//if(ccs == 3 || ccs == 7)
	{
		block_received_packet_num++;
		pack_dispatch(m);
	}
	return 0;
}

void report_device_id(void)
{
	extern uint32_t slave_nodeid;
  extern uint32_t master_nodeid;	
	extern __IO u32 TimingDelay2;
	Message m;
	int cur_id = (int)slave_nodeid;
	memset(&m, 0, sizeof(m));
		
	m.cob_id = master_nodeid;
	m.len = 8;
	m.rtr = CAN_RTR_DATA;
	m.data[0] = 0x60;
	m.data[1] = 0x0E;
	m.data[2] = 0x12;
	m.data[3] = 0x03;	
	
	if(TimingDelay2 >= 100000)//1s
	{
		TimingDelay2 = 0;
		m.data[7] = (cur_id & 0xFF000000) >> 24;
		m.data[6] = (cur_id & 0x00FF0000) >> 16;
		m.data[5] = (cur_id & 0xFF00) >> 8;
		m.data[4] = (cur_id & 0xFF);

		if(CAN_SEND_OK != Can_Send(NULL, &m))
		{
			Error_Handler();
		}
	}		
}

