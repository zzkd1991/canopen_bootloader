#include "block_download.h"
#include "internalFlash.h"
#include "crc.h"
#include "bsp_can.h"
#include "main.h"
#include "can_queue.h"
#include "block_download.h"
#include "bin_backup.h"
#include <string.h>
#include <stdlib.h>

extern uint32_t id1;
PACKET_STATUS_INFO packet_info = {0};

PACKET_INDEX_STATUS packet_index_preservation(int index, int last_packet_flag)
{
	int i = 0;
	extern __IO uint32_t uwTick;

	if(index < 0 || index > NUM_OF_PACKET_PER_BLOCK)
	{
		return packet_index_range_error;
	}
	else
	{
		if(last_packet_flag == 0)//不是上位机发送的最后一个报文（block的最后一个报文）
		{
			for(i = 0 ; i < packet_info.current_index; i++)
			{
				if(index == packet_info.stored_area.packet_index_array[i])//已经收到该索引的数据报文
				{
					return packet_index_repeat;
				}
			}

			packet_info.stored_area.packet_index_array[packet_info.current_index] = index;
			packet_info.current_index++;
		}
		else if(last_packet_flag == 1)
		{
			for(i = 0; i < packet_info.current_index; i++)
			{
				if(index == packet_info.stored_area.packet_index_array[i])
				{
					return packet_index_repeat;
				}
			}

			packet_info.stored_area.packet_index_array[packet_info.current_index] = index;
			packet_info.current_index++;
			
			if(packet_info.current_index == (NUM_OF_PACKET_PER_BLOCK + 1))
			{
				return packet_index_ok;
			}
			/*else
			{
				packet_info.last_packet_arrived_tick = uwTick;
				return packet_index_num_insufficent;
			}*/
		}

		return packet_index_ok;
	}
}

PACKET_INDEX_STATUS packet_index_preservation_last(int index, int last_packet_flag)
{
	int i = 0;
	extern __IO uint32_t uwTick;

	if((index < 0) || (index > (packet_info.left_packet_num + 2) * 7))
	{
		return packet_index_range_error;
	}
	else
	{
		if(last_packet_flag == 0)//不是上位机发送的最后一个报文
		{
			for(i = 0 ; i < packet_info.current_index; i++)
			{
				if(index == packet_info.stored_area.packet_index_array[i])//已经收到该索引的数据报文
				{
					return packet_index_repeat;
				}
			}

			packet_info.stored_area.packet_index_array[packet_info.current_index] = index;
			packet_info.current_index++;
		}
		else if(last_packet_flag == 1)
		{
			for(i = 0; i < packet_info.current_index; i++)
			{
				if(index == packet_info.stored_area.packet_index_array[i])
				{
					return packet_index_repeat;
				}
			}

			packet_info.stored_area.packet_index_array[packet_info.current_index] = index;
			packet_info.current_index++;

			if(packet_info.left_byte_num == 0)
			{
				if(packet_info.current_index == (packet_info.left_packet_num + 1))
				{
					return packet_index_ok;
				}
				/*else
				{
					packet_info.last_packet_arrived_tick = uwTick;
					return packet_index_num_insufficent;
				}*/
			}
			else if(packet_info.left_byte_num != 0)
			{
				if(packet_info.current_index == (packet_info.left_packet_num + 2))
				{
					return packet_index_ok;
				}
				/*else
				{
					packet_info.last_packet_arrived_tick = uwTick;
					return packet_index_num_insufficent;
				}*/
			}
		}

		return packet_index_ok;
	}
}

void packet_value_reset_flow(void)
{
	//uint8_t packet_index_array[NUM_OF_PACKET_PER_BLOCK + 1] = {0};
	//uint8_t bin_received_file[(NUM_OF_PACKET_PER_BLOCK + 1) * 7] = {0};
	memset(&packet_info.stored_area.bin_received_file[0], 0, sizeof(packet_info.stored_area.bin_received_file));
	memset(&packet_info.stored_area.packet_index_array[0], 0xff, sizeof(packet_info.stored_area.packet_index_array));
	packet_info.current_index = 0;
	packet_info.first_pro_first_packet_tick = 0;
	packet_info.second_pro_first_packet_tick = 0;
	packet_info.first_pro_first_packet_flag = 0;
	packet_info.second_pro_first_packet_flag = 0;
	packet_info.packet_receive_timeout = 0;
	packet_info.stored_area.bin_point = &packet_info.stored_area.bin_received_file[0];
	packet_info.stored_area.bin_point_last = &packet_info.stored_area.bin_received_file_last[0];
	memset(&packet_info.stored_area.current_packet[0], 0, sizeof(packet_info.stored_area.current_packet));
}

void determine_first_flow_packet_receved_timeout(void)
{
	extern __IO uint32_t uwTick;
	if(packet_info.first_pro_first_packet_tick > 0)
	{
		if((uwTick - packet_info.first_pro_first_packet_tick >= 1000) && (packet_info.current_index < (NUM_OF_PACKET_PER_BLOCK + 1)))
		{
			packet_info.packet_receive_timeout = 1;
		}
	}
}

void determine_second_flow_packet_receved_timeout(void)
{
	extern __IO uint32_t uwTick;
	if(packet_info.second_pro_first_packet_tick > 0)
	{
		if(uwTick - packet_info.second_pro_first_packet_tick >= 1000)
		{
			if(packet_info.left_byte_num == 0)
			{
				if(packet_info.current_index != packet_info.left_packet_num + 1)
				{
					packet_info.packet_receive_timeout = 1;
				}
			}
			else
			{
				if(packet_info.current_index != packet_info.left_packet_num + 2)
				{
					packet_info.packet_receive_timeout = 1;
				}
			}
		}
	}
}

HANDLE_RECEIVED_PACKET_STATUS new_receive_block_packet(Message *m)
{
	int packet_index = 0;
	int result = 0;
	int c = 0;
	int i = 0;
	int j = 0;
	uint8_t temp_num;
	uint8_t my_index;
	Message ack_message;
	extern uint32_t master_nodeid;
	c = (m->data[0] & 0x80) >> 7;//是否为一个段的最后一个数据包
	packet_index = (m->data[0]) & 0x1F;//前3bit代表ccs，后5位代表index

	memset(&ack_message, 0, sizeof(ack_message));

	packet_info.first_pro_first_packet_flag++;
	if(packet_info.first_pro_first_packet_flag == 1)
	{
		packet_info.first_pro_first_packet_tick = HAL_GetTick();
		packet_info.first_pro_first_packet_flag = 0xff;
	}

	if(packet_info.packet_receive_timeout == 1)
	{
		/*收到报文数量不足*/
		printf("%s, %d\n", __FUNCTION__, __LINE__);
		packet_info.packet_index_info.index_num_insufficent++;
		
		ClearCanRxQueue();
		form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_info.received_section_num, 0x60);

		packet_info.packet_index_info.index_num_insufficent = 1;
		
		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}
		
		return packet_num_insufficent;		
	}
	
 	result = packet_index_preservation(packet_index, c);//将接收报文索引存入数值
	if(result == packet_index_range_error)
	{
		//接收报文索引错误报文
		printf("%s, %d\n", __FUNCTION__, __LINE__);
		ClearCanRxQueue();
		form_ack_message(&ack_message, 0x02, 0x01, 0xFD, packet_info.received_section_num, 0x60);

		packet_info.packet_index_info.index_range_error = 1;

		
		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}		
		return packet_index_error;
	}
	else if(result == packet_index_repeat)
	{
		ClearCanRxQueue();
		form_ack_message(&ack_message, 0x02, 0x01, 0xFC, packet_info.received_section_num, 0x60);

		packet_info.packet_index_info.index_repeat_error = 1;

		return packet_repeat;
	}
	else if(result == packet_index_ok)
	{
		memcpy(&packet_info.stored_area.current_packet[packet_info.current_index - 1], m, sizeof(*m));
	}


	if(packet_info.current_index < (NUM_OF_PACKET_PER_BLOCK + 1))
	{
		return packet_ok;
	}
	else
	{
		packet_info.packet_index_info.index_ok = 1;
		packet_info.received_section_num++;
	}

	//将接收的数据包内容按顺序存入数组中，并进行CRC校验
	temp_num = packet_info.stored_area.packet_index_array[0];

	for(i = 0; i < packet_info.current_index; i++)
	{
		for(j = 0; j < packet_info.current_index; j++)
		{
			if(packet_info.stored_area.packet_index_array[j] == 0xFF)
			{
				continue;
			}
		
			if(temp_num >= packet_info.stored_area.packet_index_array[j])
			{
				temp_num = packet_info.stored_area.packet_index_array[j];
				my_index = j;
			}
		}

		temp_num = 0xff;

		packet_info.stored_area.packet_index_array[my_index] = 0xFF;

		memcpy(packet_info.stored_area.bin_point, &packet_info.stored_area.current_packet[my_index].data[1], 7);
		packet_info.stored_area.bin_point += 7;
	}
	
	packet_info.cal_crc = calc_crc32(0, &packet_info.stored_area.bin_received_file[7], sizeof(packet_info.stored_area.bin_received_file) - 7);

	packet_info.received_crc = (packet_info.stored_area.bin_received_file[3] << 24) | (packet_info.stored_area.bin_received_file[2] << 16) | (packet_info.stored_area.bin_received_file[1] << 8) | packet_info.stored_area.bin_received_file[0];
	
	if(packet_info.cal_crc == packet_info.received_crc)
	{
		if(write_flash_error == FLASH_If_Write(&packet_info.dest_address, &packet_info.stored_area.bin_received_file[7], NUM_OF_PACKET_PER_BLOCK * 7))
		{
			//发送写入FLASH错误报文
			printf("%s, %d\n", __FUNCTION__, __LINE__);
			form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_info.received_section_num, 0x60);

			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}

			packet_info.received_section_num--;

			return packet_write_flash_error;
		}
		else
		{
			//发送正确ACK报文
			form_ack_message(&ack_message, 0x02, 0x01, 0, 0, 0x60);
			
			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}

			packet_info.block_total_received_byte += NUM_OF_PACKET_PER_BLOCK * 7;
			packet_info.block_cur_percent_inc = (uint8_t)(((float)packet_info.block_total_received_byte / packet_info.file_length) * 100);
			ack_message.data[0] = 0x60;
			ack_message.data[1] = 0x01;
			ack_message.data[2] = 0x00;
			ack_message.data[3] = 0x0a;//发送百分比
			ack_message.data[4] = packet_info.block_cur_percent_inc;
			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}		
		}
	}
	else
	{
		/*CRC校验错误*/
		form_ack_message(&ack_message, 0x02, 0x01, 0xFF, packet_info.received_section_num, 0x60);

		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}

		packet_info.received_section_num--;
		return packet_crc_check_error;
	}

	return packet_ok;

}

HANDLE_RECEIVED_PACKET_STATUS new_received_last_section(Message *m)
{
	int packet_index = 0;
	int result = 0;
	int c = 0;
	int i = 0;
	int j = 0;
	uint8_t temp_num;
	uint8_t my_index;
	uint32_t flash_write_result = 0;
	Message ack_message;
	//uint32_t cal_crc;
	extern uint32_t master_nodeid;
	c = (m->data[0] & 0x80) >> 7;//是否为一个段的最后一个数据包
	packet_index = m->data[0] & 0x1F;

	memset(&ack_message, 0, sizeof(ack_message));

	packet_info.second_pro_first_packet_flag++;
	if(packet_info.second_pro_first_packet_flag == 1)
	{
		packet_info.second_pro_first_packet_tick = HAL_GetTick();
		packet_info.second_pro_first_packet_flag = 0xff;
	}

	if(packet_info.packet_receive_timeout == 1)
	{
		/*收到报文数量不足*/
		printf("%s, %d\n", __FUNCTION__, __LINE__);
		packet_info.packet_index_info.index_num_insufficent++;
		
		ClearCanRxQueue();
		form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_info.received_section_num, 0x60);
		
		packet_info.packet_index_info.index_num_insufficent = 1;
		
		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}
		
		return packet_num_insufficent;
	}
	
	result = packet_index_preservation_last(packet_index, c);
	if(result == packet_index_range_error)
	{
		//发送接收报文索引错误报文
		printf("%s, %d\n", __FUNCTION__, __LINE__);
		ClearCanRxQueue();
		form_ack_message(&ack_message, 0x02, 0x01, 0xFD, packet_info.received_section_num, 0x60);
		
		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}		
		return packet_index_error;
	}
	else if(result == packet_index_repeat)
	{
		ClearCanRxQueue();
		form_ack_message(&ack_message, 0x02, 0x01, 0xFC, packet_info.received_section_num, 0x60);

		packet_info.packet_index_info.index_repeat_error = 1;

		return packet_repeat;
	}
	else if(result == packet_index_ok)
	{
		memcpy(&packet_info.stored_area.current_packet[packet_info.current_index - 1], m, sizeof(*m));
	}


	if(packet_info.left_byte_num != 0)
	{
		if((packet_info.current_index < packet_info.left_packet_num + 2))
		{
			return packet_ok;
		}
		else
		{
			packet_info.received_section_num++;
		}
	}
	else
	{
		if((packet_info.current_index < packet_info.left_packet_num + 1))
		{
			return packet_ok;
		}
		else
		{
			packet_info.received_section_num++;
		}
	}
		
	temp_num = packet_info.stored_area.packet_index_array[0];
	for(i = 0; i < packet_info.current_index; i++)
	{
		for(j = 0; j < packet_info.current_index; j++)
		{
			if(packet_info.stored_area.packet_index_array[j] == 0xFF)
			{
				continue;
			}
			
			if(temp_num >= packet_info.stored_area.packet_index_array[j])
			{
				temp_num = packet_info.stored_area.packet_index_array[j];
				my_index = j;
			}
		}
		
		packet_info.stored_area.packet_index_array[my_index] = 0xFF;
		
		temp_num = 0xff;
		memcpy(packet_info.stored_area.bin_point_last, &packet_info.stored_area.current_packet[my_index].data[1], 7);
		packet_info.stored_area.bin_point_last += 7;
	}

	if(packet_info.left_byte_num != 0)
	{
		packet_info.cal_crc = calc_crc32(0, &packet_info.stored_area.bin_received_file_last[7], (packet_info.left_packet_num + 1) * 7);
	}
	else
	{
		packet_info.cal_crc = calc_crc32(0, &packet_info.stored_area.bin_received_file_last[7], (packet_info.left_packet_num * 7));
	}
	packet_info.received_crc = (packet_info.stored_area.bin_received_file_last[3] << 24) | (packet_info.stored_area.bin_received_file_last[2] << 16) | (packet_info.stored_area.bin_received_file_last[1] << 8) | packet_info.stored_area.bin_received_file_last[0];


	if(packet_info.cal_crc == packet_info.received_crc)
	{
		if(packet_info.left_byte_num == 0)
		{

			flash_write_result = FLASH_If_Write(&packet_info.dest_address, &packet_info.stored_area.bin_received_file_last[7], packet_info.left_packet_num * 7);
		}
		else if(packet_info.left_byte_num != 0)
		{
			flash_write_result = FLASH_If_Write(&packet_info.dest_address, &packet_info.stored_area.bin_received_file_last[7], (packet_info.left_packet_num + 1) * 7);
		}
		if(write_flash_error == flash_write_result)
		{
			//发送写入FLASH错误报文
			//发送写入FLASH错误报文
			printf("%s, %d\n", __FUNCTION__, __LINE__);
			form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_info.received_section_num, 0x60);

			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}
			packet_info.received_section_num--;

			return packet_write_flash_error;				
		}
		else
		{
			//发送正确ACK报文
			printf("%s, %d\n", __FUNCTION__, __LINE__);
			form_ack_message(&ack_message, 0x02, 0x01, 0x0, 0, 0x60);

			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}
			
			packet_info.block_total_received_byte += packet_info.left_packet_num * 7;//发送百分比
			packet_info.block_total_received_byte += packet_info.left_byte_num;
			packet_info.block_cur_percent_inc = (uint8_t)(((float)packet_info.block_total_received_byte / packet_info.file_length) * 100);
			ack_message.data[0] = 0x60;
			ack_message.data[1] = 0x01;
			ack_message.data[2] = 0x00;
			ack_message.data[3] = 0x0a;//发送百分比
			ack_message.data[4] = packet_info.block_cur_percent_inc;
			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}		
		}
	}
	else
	{
		form_ack_message(&ack_message, 0x02, 0x01, 0xFF, packet_info.received_section_num, 0x60);

		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}
		packet_info.received_section_num--;
		return packet_crc_check_error;	
	}
	
	return packet_ok;
}

int pack_dispatch(Message *m)
{
	int status;
	
	if(packet_info.receive_flow == first_procedure)
	{
		status = new_receive_block_packet(m);
		if(packet_ok != status)
		{
			packet_value_reset_flow();
			/*get_old_image_length(OLD_IMAGE_LENGTH_ADDR, &image_length);
	
			if(OTA_ERRNO_OK != prv_spi2inner_copy(OTA_IMAGE_BCK_ADDR, image_length))
			{
				Error_Handler();
				return 1;
			}*/
			
			return status;
		}

		if(packet_info.current_index == (NUM_OF_PACKET_PER_BLOCK + 1))
		{
			packet_value_reset_flow();
		}

		if(packet_info.received_section_num == packet_info.total_section_num)
		{
			packet_info.receive_flow = second_procedure;
			return status;
		}
	}

	if(packet_info.receive_flow == second_procedure)
	{
		status = new_received_last_section(m);
		if(packet_ok != status)
		{
			packet_value_reset_flow();
			/*get_old_image_length(OLD_IMAGE_LENGTH_ADDR, &image_length);
	
			if(OTA_ERRNO_OK != prv_spi2inner_copy(OTA_IMAGE_BCK_ADDR, image_length))
			{
				Error_Handler();
				return 1;
			}*/

			
			return status;
		}

		if(packet_info.received_section_num == packet_info.total_section_num + 1)
		{
			//int ret = OTA_ERRNO_OK;

			/*ret = store_old_image_length(OLD_IMAGE_LENGTH_ADDR, bin_file_length);
			if(ret != OTA_ERRNO_OK)
			{
				return 1;
			}
			
			ret = prv_inner2spi_copy(bin_file_length);
			if(ret != OTA_ERRNO_OK)
			{
				return 1;
			}*/
			
			packet_info.receive_flow = 0xff;
			packet_info.receive_flow = 0xff;
			packet_info.bin_received_success = 0xff;
		}
	}
	return status;
}


void form_ack_message(Message *ack_message, uint16_t index, uint8_t subindex, uint8_t ackcode, uint32_t error_section, uint8_t command_code)
{
	extern uint32_t master_nodeid;

	ack_message->len = 8;
	ack_message->rtr = CAN_RTR_DATA;
	ack_message->cob_id = master_nodeid;
	ack_message->data[0] = command_code;
	ack_message->data[1] = index & 0xFF;
	ack_message->data[2] = index << 8;
	ack_message->data[3] = subindex;
	ack_message->data[4] = ackcode;
	ack_message->data[5] = (error_section & 0xFF0000) >> 16;
	ack_message->data[6] = (error_section & 0x00FF00) >> 8;
	ack_message->data[7] = (error_section & 0x0000FF);
}

