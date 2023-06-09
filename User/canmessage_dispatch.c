#include "bsp_can.h"
#include "recover.h"
#include "internalFlash.h"
#include "spi_flash.h"
#include "can_queue.h"
#include "block_download.h"
#include "bin_backup.h"
#include "crc.h"
#include "led.h"
#include "menu.h"

int device_find(uint16_t id)
{
	int i = 0;
	for(i = 0; i < sizeof(packet_info.stored_area.deviceid_array)/ sizeof(packet_info.stored_area.deviceid_array[0]); i++)
	{
		if(packet_info.stored_area.deviceid_array[i] == id)
		{
			return 0;
		}
	}
	return 1;
}

void prepare_flow(Message *m)
{
	extern uint32_t master_nodeid;
	Message respond_message = {0};
	int i = 0;
	if(m == NULL)
	{
		return;
	}
	
	if(m->data[0] == 0xFF && m->data[1] == 0x00 && m->data[2] == 0x00 && m->data[3] == 0x00
		&& m->data[4] == 0x00 && m->data[5] == 0x00 && m->data[6] == 0x00 && m->data[7] == 0x00)
	{
		respond_message.data[0] = 0xFF;
		respond_message.len = 8;
		respond_message.rtr = CAN_RTR_DATA;
		respond_message.cob_id = master_nodeid;		
		for(i = 1; i < 7; i++)
		{
			respond_message.data[i] = 0x00;
		}
		
		if(CAN_SEND_OK != Can_Send(NULL, &respond_message))
		{
			Error_Handler();
		}		
		packet_info.state_machine_flag.enter_bootloader_flag = enter_bootloader;
	}

	if(m->data[0] == 0xFF && m->data[1] == 0xFF)
	{
		packet_info.file_length = m->data[2] | (m->data[3] << 8) | (m->data[4] << 16) | (m->data[5] << 24);
		packet_info.total_packet_num = packet_info.file_length / 7;
		packet_info.total_section_num = packet_info.total_packet_num / NUM_OF_PACKET_PER_BLOCK;
		packet_info.left_byte_num = packet_info.file_length % 7;
		packet_info.left_packet_num = packet_info.total_packet_num - NUM_OF_PACKET_PER_BLOCK * packet_info.total_section_num;
		packet_info.stored_area.bin_point = &packet_info.stored_area.bin_received_file[0];
		packet_info.stored_area.bin_point_last = &packet_info.stored_area.bin_received_file_last[0];

/*在保证内部flash擦除完成后，接收上位机的大量报文，可以正常进行*/
		if(erase_flash_ok != FLASH_If_Erase(APPLICATION_ADDRESS_START, APPLICATION_ADDRESS_END))
		{
			Error_Handler();
		}

		
		respond_message.len = 8;
		respond_message.rtr = CAN_RTR_DATA;
		respond_message.cob_id = master_nodeid;		
		respond_message.data[0] = 0xFF;
		respond_message.data[1] = 0xFF;
		for(i = 2; i < 7; i++)
		{
			respond_message.data[i] = 0x00;
		}

		if(CAN_SEND_OK != Can_Send(NULL, &respond_message))
		{
			Error_Handler();
		}
		packet_info.state_machine_flag.flow_flag = main_flow_flag;

/*这种情况 ，在擦除flash的同时，大量接收上位机的CAN报文，容易出现异常*/
#if 0
		if(erase_flash_ok != FLASH_If_Erase(0))
		{
			Error_Handler();
		}
#endif
		
	}
}

int NEW_Can_Message_Dispatch(Message *m)
{
	int status;
	extern uint32_t received_packet_num;
	if(packet_info.state_machine_flag.flow_flag == prepare_flow_flag)
	{
		prepare_flow(m);
		return packet_ok;
	}
	else
	{
		packet_info.block_received_packet_num++;
		status = pack_dispatch(m);
		if(status != packet_ok)
		{
			uint32_t application_address = APPLICATION_ADDRESS_START;
			//printf("received packet num %d\r\n", packet_info.block_received_packet_num);
			//printf("int event received packet num %d\r\n", received_packet_num);
			//printf("status %d\r\n", status);
			//HAL_Delay(100);			
			copy_bin_from_newaddress_to_oldaddress();
			Main_Menu(application_address);
			Error_Handler();
			return status;
		}
	}

	return status;
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

