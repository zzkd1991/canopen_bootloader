#include "bsp_can.h"
#include "recover.h"
#include "internalFlash.h"
#include "spi_flash.h"
#include "can_queue.h"
#include "block_download.h"
#include "crc.h"
#include "my_canopen_adapter.h"
#include "objdictdef.h"
#include "data.h"
#include "sdo.h"
#include "objacces.h"
#include "applicfg.h"
#include "led.h"



int mm32_ack_flag = 0;
uint8_t output_array[30] = {0};
uint32_t enter_bootloader_flag = 0;
//uint32_t enter_bootloader_flag = 0xFF;
int min_flag = 0;
int max_flag = 0;
uint64_t bin_file_length = 0;
uint64_t total_received_byte = 0;
uint8_t percent_flag = 0;
uint32_t can_num = 0;
uint16_t cur_flag = 0;
uint8_t cur_percent_inc = 0;
uint8_t past_percent_inc = 0;
uint32_t ack_num = 0;
uint32_t master_nodeid = 0;
uint32_t slave_nodeid = 0;
uint32_t id2;
uint32_t id3;
uint32_t device_num = 0;
uint16_t deviceid_array[20] = {0};
int wait_flag = 0;
int lvdt_flag;
int cnt = 10;
float Double_Lvdt_Average_ConvertedValue[2] = {0};
float Double_Lvdt_Transfer_Complete_Count[2] = {0};
float Double_Lvdt_AVG[2] = {0};
uint16_t Lvdt_Transfer_Complete_Count = 0;
float Lvdt_AVG = 0;
int test = 0;



float lvdt_zero_locaiton[2] = {0};

float min_location = 0;
float max_location = 0;

Message mym = {0};


uint32_t block_received_packet_num = 0;

extern int lvdt_report_button;
extern int deviceid_report_button;


void report_node_info(void)
{	
	if(lvdt_report_button == 1)
	{
		report_lvdt_loaction();
	}
	
	if(deviceid_report_button == 1)
	{
		report_device_id();
	}
}

void SingleValve_lvdtFormOutputStr(int location, int min_or_max, int* length)
{
	char const_arraymin[7] = "CAL:0,";
	char const_arraymax[7] = "CAL:1,";
	
	int temp_location = 0;
	int index = 0;
	int temp_index = 0;
	
	if((location < 0) || (location > 2000) || (length == NULL))
	{
		return;
	}	
	
	temp_location = location;
	while(temp_location / 10)
	{
		index++;
		temp_location = temp_location / 10;
	}

	temp_index = index;
	
	if(min_or_max == 0)
	{
		strncpy((char *)output_array, const_arraymin, 6);
	}
	else if(min_or_max == 1)
	{
		strncpy((char *)output_array, const_arraymax, 6);
	}
	do
	{
		output_array[temp_index + 6] = '0' + location % 10;
		location = location / 10;
		temp_index--;
	}
	while(location > 0);

	output_array[index + 7] = '\r';
	output_array[index + 8] = '\n';
	//printf("output_array %s\n", output_array);
	*length = index + 9;

}

void DoubleValve_lvdtFormOutputStr(int location, int min_or_max, int num, int* length)
{
	char valve1_const_arraymin[9] = "CAL:0,0,";
	char valve1_const_arraymax[9] = "CAL:0,1,";
	char valve2_const_arraymin[9] = "CAL:1,0,";
	char valve2_const_arraymax[9] = "CAL:1,1,";
 	
	int temp_location = 0;
	int index = 0;
	int temp_index = 0;
	
	if((location < 0) || (location > 2000) || (length == NULL) || (num > 3) || (num < 0))
	{
		return;
	}	
	
	temp_location = location;
	while(temp_location / 10)
	{
		index++;
		temp_location = temp_location / 10;
	}

	temp_index = index;

	if((num == 1) && (min_or_max == 0))
	{
		strncpy((char *)output_array, valve1_const_arraymin, 8);
	}
	else if((num == 1) && (min_or_max == 1))
	{
		strncpy((char *)output_array, valve1_const_arraymax, 8);
	}
	else if((num == 2) && (min_or_max == 0))
	{
		strncpy((char *)output_array, valve2_const_arraymin, 8);
	}
	else if((num == 2) && (min_or_max == 1))
	{
		strncpy((char *)output_array, valve2_const_arraymax, 8);
	}
	do
	{
		output_array[temp_index + 8] = '0' + location % 10;
		location = location / 10;
		temp_index--;
	}
	while(location > 0);

	output_array[index + 9] = '\r';
	output_array[index + 10] = '\n';
	printf("output_array %s\n", output_array);
	*length = index + 11;

}


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
	extern UNS8 proceedSDO(CO_Data *d, Message *m);
	extern CO_Data ObjDict_Data;
	UNS8 result = 0;

	uint8_t ccs = 0;

	ccs = getSDOcs(m->data[0]);

	if(ccs == 3 || ccs == 7)
	{
		block_received_packet_num++;
		pack_dispatch(m);
	}
	else
	{
		result = proceedSDO(&ObjDict_Data, m);
		if(result == 0xFF)
		{
			Error_Handler();
		}
	}
	
	return 0;
}


void report_lvdt_loaction(void)
{
	extern uint32_t id1;
	extern uint32_t master_nodeid;
	extern __IO u32 TimingDelay;
	Message m;

	extern float location[2];
	
	memset(&m, 0, sizeof(m));
		
	m.cob_id = master_nodeid;
	m.len = 8;
	m.rtr = CAN_RTR_DATA;
	
	if(TimingDelay >= 100000)//1s
	{
		/*if(test == 0)
		{
			USART3_ON;
		}
		
		test++;

		if(test == 2)
		{
			USART3_OFF;
		}*/
				
		TimingDelay = 0;
		location[0] = 200.0f;
		location[1] = 400.0f;
	
		m.data[0] = 0x60;
		m.data[1] = 0x0D;
		m.data[2] = 0x12;
		m.data[3] = 0x03;
		m.data[7] = ((uint32_t)location[0] & 0xFF000000) >> 24;
		m.data[6] = ((uint32_t)location[0] & 0x00FF0000) >> 16;
		m.data[5] = ((uint32_t)location[0] & 0xFF00) >> 8;
		m.data[4] = ((uint32_t)location[0] & 0xFF);
		if(location[0] > 2000 || location[1] > 2000 || location[0] < 0 || location[1] < 0)
		{
			return;
		}
		if(CAN_SEND_OK != Can_Send(NULL, &m))
		{
			Error_Handler();
		}

		m.data[3] = 0x04;
		m.data[7] = ((uint32_t)location[1] & 0xFF000000) >> 24;
		m.data[6] = ((uint32_t)location[1] & 0x00FF0000) >> 16;
		m.data[5] = ((uint32_t)location[1] & 0xFF00) >> 8;
		m.data[4] = ((uint32_t)location[1] & 0xFF);

		if(CAN_SEND_OK != Can_Send(NULL, &m))
		{
			Error_Handler();
		}

	}
}

void report_device_id(void)
{
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

