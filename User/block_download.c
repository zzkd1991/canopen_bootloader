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


uint8_t bin_received_file[(NUM_OF_PACKET_PER_BLOCK + 1) * 7] = {0};
uint8_t bin_received_file_last[NUM_OF_PACKET_PER_BLOCK * 7] = {0};
uint8_t packet_index_array[NUM_OF_PACKET_PER_BLOCK + 1] = {0};
uint8_t *bin_point = NULL;
uint8_t *bin_point_last = NULL;
extern uint32_t id1;
Message current_packet[NUM_OF_PACKET_PER_BLOCK + 1] = {0};
RECEIVED_PROCEDURE_INFO received_procedure = {1, 0};

PACKET_STATUS_INFO packet_status_info = {0};

int packet_index_preservation(int index, int last_packet_flag)
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
			for(i = 0 ; i < packet_status_info.current_index; i++)
			{
				if(index == packet_index_array[i])//已经收到该索引的数据报文
				{
					return packet_index_repeat;
				}
			}

			packet_index_array[packet_status_info.current_index] = index;
			packet_status_info.current_index++;
		}
		else if(last_packet_flag == 1)
		{
			for(i = 0; i < packet_status_info.current_index; i++)
			{
				if(index == packet_index_array[i])
				{
					return packet_index_repeat;
				}
			}

			packet_index_array[packet_status_info.current_index] = index;
			packet_status_info.current_index++;
			
			if(packet_status_info.current_index == (NUM_OF_PACKET_PER_BLOCK + 1))
			{
				return packet_index_ok;
			}
			else
			{
				packet_status_info.last_packet_arrived_tick = uwTick;
				return packet_index_num_insufficent;
			}
		}
	}
}

int packet_index_preservation_last(int index, int last_packet_flag)
{
	int i = 0;
	extern __IO uint32_t uwTick;

	if((index < 0) || (index > (packet_status_info.left_packet_num + 2) * 7))
	{
		return packet_index_range_error;
	}
	else
	{
		if(last_packet_flag == 0)//不是上位机发送的最后一个报文
		{
			for(i = 0 ; i < packet_status_info.current_index; i++)
			{
				if(index == packet_index_array[i])//已经收到该索引的数据报文
				{
					return packet_index_repeat;
				}
			}

			packet_index_array[packet_status_info.current_index] = index;
			packet_status_info.current_index++;
		}
		else if(last_packet_flag == 1)
		{
			for(i = 0; i < packet_status_info.current_index; i++)
			{
				if(index == packet_index_array[i])
				{
					return packet_index_repeat;
				}
			}

			packet_index_array[packet_status_info.current_index] = index;
			packet_status_info.current_index++;

			if(packet_status_info.left_byte_num == 0)
			{
				if(packet_status_info.current_index == (packet_status_info.left_packet_num + 1))
				{
					return packet_index_ok;
				}
				else
				{
					packet_status_info.last_packet_arrived_tick = uwTick;
					return packet_index_num_insufficent;
				}
			}
			else if(packet_status_info.left_byte_num != 0)
			{
				if(packet_status_info.current_index == (packet_status_info.left_packet_num + 2))
				{
					return packet_index_ok;
				}
				else
				{
					packet_status_info.last_packet_arrived_tick = uwTick;
					return packet_index_num_insufficent;
				}
			}
		}

		return packet_index_ok;
	}
}

void packet_value_reset_flow(void)
{
	//uint8_t packet_index_array[NUM_OF_PACKET_PER_BLOCK + 1] = {0};
	//uint8_t bin_received_file[(NUM_OF_PACKET_PER_BLOCK + 1) * 7] = {0};
	memset(&bin_received_file[0], 0, sizeof(bin_received_file));
	memset(&packet_index_array[0], 0xff, sizeof(packet_index_array));
	packet_status_info.current_index = 0;
	packet_status_info.last_packet_arrived_tick = 0;	
	bin_point = &bin_received_file[0];
	bin_point_last = &bin_received_file_last[0];
	memset(&current_packet[0], 0, sizeof(current_packet));
}

int new_receive_block_packet(Message *m)
{
	int packet_index = 0;
	int result = 0;
	int c = 0;
	int i = 0;
	int j = 0;
	uint8_t temp_num;
	uint8_t my_index;
	uint32_t cal_crc = 0;
	uint32_t received_crc = 0;
	Message ack_message;
	extern uint32_t master_nodeid;
	c = (m->data[0] & 0x80) >> 7;//是否为一个段的最后一个数据包
	packet_index = (m->data[0]) & 0x1F;//前3bit代表ccs，后5位代表index

	memset(&ack_message, 0, sizeof(ack_message));

	//while(1)//直到将块中所有CAN报文内容和索引存入到数组中，才退出
	{
	 	result = packet_index_preservation(packet_index, c);//将接收报文索引存入数值
		if(result == packet_index_range_error)
		{
			//发送接收报文索引错误报文
			printf("%s, %d\n", __FUNCTION__, __LINE__);
			ClearCanRxQueue();
			form_ack_message(&ack_message, 0x02, 0x01, 0xFD, packet_status_info.received_section_num, 0x60);
			
			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}		
			return packet_index_error;
		}
		else if(result == packet_index_ok)
		{
			memcpy(&current_packet[packet_status_info.current_index - 1], m, sizeof(*m));
		}
		else if(result == packet_index_num_insufficent)//收到最后一个报文，但报文数量不足。
		{
			while(HAL_GetTick() - packet_status_info.last_packet_arrived_tick < 500);
			if(HAL_GetTick() - packet_status_info.last_packet_arrived_tick >= 500)//等待500ms
			{
				if(packet_status_info.current_index !=  (NUM_OF_PACKET_PER_BLOCK + 1))//500ms后仍然没有收到全部报文，向上位机报错
				{
					//发送写入FLASH错误报文
					printf("%s, %d\n", __FUNCTION__, __LINE__);
					ClearCanRxQueue();
					form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_status_info.received_section_num, 0x60);
					
					if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
					{
						Error_Handler();
					}
					
					return packet_num_insufficent;
				}
			}
		}
	}


	if(packet_status_info.current_index < (NUM_OF_PACKET_PER_BLOCK + 1))
	{
		return packet_ok;
	}
	else
	{
		packet_status_info.received_section_num++;
	}

	//将接收的数据包内容按顺序存入数组中，并进行CRC校验
	temp_num = packet_index_array[0];

	for(i = 0; i < packet_status_info.current_index; i++)
	{
		for(j = 0; j < packet_status_info.current_index; j++)
		{
			if(packet_index_array[j] == 0xFF)
			{
				continue;
			}
		
			if(temp_num >= packet_index_array[j])
			{
				temp_num = packet_index_array[j];
				my_index = j;
			}
		}

		temp_num = 0xff;

		packet_index_array[my_index] = 0xFF;

		memcpy(bin_point, &current_packet[my_index].data[1], 7);
		bin_point += 7;
	}
	
	cal_crc = calc_crc32(0, &bin_received_file[8], sizeof(bin_received_file) - 8);

	received_crc = (bin_received_file[1] << 24) | (bin_received_file[2] << 16) | (bin_received_file[3] << 8) | bin_received_file[4];
	
	if(cal_crc == received_crc)
	{
		if(write_flash_error == FLASH_If_Write(&packet_status_info.dest_address, &bin_received_file[8], NUM_OF_PACKET_PER_BLOCK * 7))
		{
			//发送写入FLASH错误报文
			printf("%s, %d\n", __FUNCTION__, __LINE__);
			form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_status_info.received_section_num, 0x60);

			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}

			packet_status_info.received_section_num--;

			return packet_write_flash_error;
		}
		else
		{
			//发送正确ACK报文
			form_ack_message(&ack_message, 0x02, 0x01, 0, 0, 0x60);
			
			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
				return -1;
			}

			packet_status_info.block_total_received_byte += NUM_OF_PACKET_PER_BLOCK * 7;
			packet_status_info.block_cur_percent_inc = (uint8_t)(((float)packet_status_info.block_total_received_byte / packet_status_info.file_length) * 100);
			ack_message.data[0] = 0x60;
			ack_message.data[1] = 0x01;
			ack_message.data[2] = 0x00;
			ack_message.data[3] = 0x0a;//发送百分比
			ack_message.data[4] = packet_status_info.block_cur_percent_inc;
			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}		
		}
	}
	else
	{
		form_ack_message(&ack_message, 0x02, 0x01, 0xFF, packet_status_info.received_section_num, 0x60);

		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}

		packet_status_info.received_section_num--;
		return packet_crc_check_error;
	}

	return packet_ok;

}

int new_received_last_section(Message *m)
{
	int packet_index = 0;
	int result = 0;
	int c = 0;
	int i = 0;
	int j = 0;
	uint8_t temp_num;
	uint8_t my_index;
	uint32_t cal_crc = 0;
	uint32_t received_crc = 0;
	uint32_t flash_write_result = 0;
	Message ack_message;
	extern uint32_t master_nodeid;
	c = (m->data[0] & 0x80) >> 7;//是否为一个段的最后一个数据包
	packet_index = m->data[0] & 0x1F;

	memset(&ack_message, 0, sizeof(ack_message));
	//while(1)
	{
		result = packet_index_preservation_last(packet_index, c);
		if(result == packet_index_range_error)
		{
			//发送接收报文索引错误报文
			printf("%s, %d\n", __FUNCTION__, __LINE__);
			ClearCanRxQueue();
			form_ack_message(&ack_message, 0x02, 0x01, 0xFD, packet_status_info.received_section_num, 0x60);
			
			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}		
			return packet_index_error;
		}
		else if(result == 0)
		{
			memcpy(&current_packet[packet_status_info.current_index - 1], m, sizeof(*m));
		}
		else if(result == 1)//收到最后一个报文， 但报文数量不足。
		{
			while(HAL_GetTick() - packet_status_info.last_packet_arrived_tick < 500);
			if(HAL_GetTick() - packet_status_info.last_packet_arrived_tick >= 500)
			{
				if(packet_status_info.left_byte_num == 0)
				{
					if(packet_status_info.current_index != packet_status_info.left_packet_num)
					{
						ClearCanRxQueue();
						form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_status_info.received_section_num, 0x60);
						
						if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
						{
							Error_Handler();
						}						
						return packet_num_insufficent;
					}
				}
				else if(packet_status_info.left_byte_num != 0)
				{
					if(packet_status_info.current_index != (packet_status_info.left_packet_num + 1))
					{
						ClearCanRxQueue();
						form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_status_info.received_section_num, 0x60);
						
						if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
						{
							Error_Handler();
						}						
						return packet_num_insufficent;
					}
				}
			}
		}
	}

	if(packet_status_info.left_byte_num != 0)
	{
		if((packet_status_info.current_index < packet_status_info.left_packet_num + 2))
		{
			return 0;
		}
		else
		{
			packet_status_info.received_section_num++;
		}
	}
	else
	{
		if((packet_status_info.current_index < packet_status_info.left_packet_num + 1))
		{
			return 0;
		}
		else
		{
			packet_status_info.received_section_num++;
		}
	}
		
	temp_num = packet_index_array[0];
	for(i = 0; i < packet_status_info.current_index; i++)
	{
		for(j = 0; j < packet_status_info.current_index; j++)
		{
			if(packet_index_array[j] == 0xFF)
			{
				continue;
			}
			
			if(temp_num >= packet_index_array[j])
			{
				temp_num = packet_index_array[j];
				my_index = j;
			}
		}
		
		packet_index_array[my_index] = 0xFF;
		
		temp_num = 0xff;
		memcpy(bin_point_last, &current_packet[my_index].data[1], 7);
		bin_point_last += 7;

	}

	if(packet_status_info.left_byte_num != 0)
	{
		cal_crc = calc_crc32(0, &bin_received_file_last[8], (packet_status_info.left_packet_num + 1) * 7);
	}
	else
	{
		cal_crc = calc_crc32(0, &bin_received_file_last[8], (packet_status_info.left_packet_num * 7));
	}
	received_crc = (bin_received_file_last[1] << 24) | (bin_received_file_last[2] << 16) | (bin_received_file_last[3] << 8) | bin_received_file_last[4];


	if(cal_crc == received_crc)
	{
		if(packet_status_info.left_byte_num == 0)
		{

			flash_write_result = FLASH_If_Write(&packet_status_info.dest_address, &bin_received_file_last[7], packet_status_info.left_packet_num * 7);
		}
		else if(packet_status_info.left_byte_num != 0)
		{
			flash_write_result = FLASH_If_Write(&packet_status_info.dest_address, &bin_received_file_last[7], (packet_status_info.left_packet_num + 1) * 7);
		}
		if(write_flash_error == flash_write_result)
		{
			//发送写入FLASH错误报文
			//发送写入FLASH错误报文
			printf("%s, %d\n", __FUNCTION__, __LINE__);
			form_ack_message(&ack_message, 0x02, 0x01, 0xFE, packet_status_info.received_section_num, 0x60);

			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}
			packet_status_info.received_section_num--;

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


			packet_status_info.block_total_received_byte += packet_status_info.left_packet_num * 7;//发送百分比
			packet_status_info.block_total_received_byte += packet_status_info.left_byte_num;
			packet_status_info.block_cur_percent_inc = (uint8_t)(((float)packet_status_info.block_total_received_byte / packet_status_info.file_length) * 100);
			ack_message.data[0] = 0x60;
			ack_message.data[1] = 0x01;
			ack_message.data[2] = 0x00;
			ack_message.data[3] = 0x0a;//发送百分比
			ack_message.data[4] = packet_status_info.block_cur_percent_inc;
			if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
			{
				Error_Handler();
			}
			
			return packet_ok;			
		}
	}
	else
	{
		form_ack_message(&ack_message, 0x02, 0x01, 0xFF, packet_status_info.received_section_num, 0x60);

		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}
		packet_status_info.received_section_num--;
		return packet_crc_check_error;	
	}
	return 0;
}

int pack_dispatch(Message *m)
{
	int ret = 0;
	
	if(received_procedure.first_procedure == 1)
	{
		if(packet_ok != new_receive_block_packet(m))
		{
			packet_value_reset_flow();
			/*get_old_image_length(OLD_IMAGE_LENGTH_ADDR, &image_length);
	
			if(OTA_ERRNO_OK != prv_spi2inner_copy(OTA_IMAGE_BCK_ADDR, image_length))
			{
				Error_Handler();
				return 1;
			}*/
			
			return 1;
		}

		if(packet_status_info.current_index == (NUM_OF_PACKET_PER_BLOCK + 1))
		{
			packet_value_reset_flow();
		}

		if(packet_status_info.received_section_num == packet_status_info.total_section_num)
		{
			received_procedure.first_procedure = 0;
			received_procedure.second_procedure = 1;
			//extern Message mym;
			//memcpy(&mym, m, sizeof(*m));
			return 0;
		}
	}

	if(received_procedure.second_procedure == 1)
	{
		if(packet_ok != new_received_last_section(m))
		{
			packet_value_reset_flow();
			/*get_old_image_length(OLD_IMAGE_LENGTH_ADDR, &image_length);
	
			if(OTA_ERRNO_OK != prv_spi2inner_copy(OTA_IMAGE_BCK_ADDR, image_length))
			{
				Error_Handler();
				return 1;
			}*/

			
			return 1;
		}

		if(packet_status_info.received_section_num == packet_status_info.total_section_num + 1)
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
			
			received_procedure.first_procedure = 0;
			received_procedure.second_procedure = 0;
			packet_status_info.bin_received_success = 0xff;
		}
	}
	return 0;
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

