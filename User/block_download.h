#ifndef BLOCK_DOWNLOAD_H_
#define BLOCK_DOWNLOAD_H_

#include "bsp_can.h"
#include "message_list.h"

#ifndef NUM_OF_PACKET_PER_BLOCK
#define NUM_OF_PACKET_PER_BLOCK		30
#endif

void packet_value_reset_flow(void);
int pack_dispatch(Message *m);
void form_ack_message(Message *ack_message, uint16_t index, uint8_t subindex, uint8_t ackcode, uint32_t error_section, uint8_t command_code);
int NEW_Can_Message_Dispatch(Message *m);
void determine_first_flow_packet_receved_timeout(void);
void determine_second_flow_packet_receved_timeout(void);

typedef enum
{
	enter_bootloader = 0,
	not_enter_bootloader = 0xff,
}ENTER_BOOTLOADER_FLAG;

typedef enum
{
	load_new_procedure = 0,
	load_old_procedure = 0xff,
}LOAD_PROCEDURE_FLAG;

typedef enum
{
	packet_index_range_error = -3,
	packet_index_repeat = -2,
	packet_index_num_insufficent = -1,
	packet_index_ok = 0,
}PACKET_INDEX_STATUS;

typedef enum
{
	packet_index_error = -5,
	packet_num_insufficent = -4,
	packet_crc_check_error = -3,
	packet_write_flash_error = -2,
	packet_repeat = -1,
	packet_ok = 0,
}HANDLE_RECEIVED_PACKET_STATUS;

typedef enum
{
  	prepare_flow_flag = 0,
    main_flow_flag = 1,	
}PREPARE_MAIN_FLAG_struct;

typedef enum
{
	 first_procedure = 0,
	 second_procedure = 1,
}RECEIVED_PROCEDURE_INFO;


typedef struct PACKET_INDEX_INFO_struct
{
	int index_range_error;
	int index_ok;
	int index_num_insufficent;
	int index_repeat_error;
}PACKET_INDEX_INFO;

typedef struct STORED_AREA_FOR_PACKET_struct
{
	uint8_t bin_received_file[(NUM_OF_PACKET_PER_BLOCK + 1) * 7];
	uint8_t bin_received_file_last[NUM_OF_PACKET_PER_BLOCK * 7];
	uint8_t packet_index_array[NUM_OF_PACKET_PER_BLOCK + 1];
	Message current_packet[NUM_OF_PACKET_PER_BLOCK + 1];
	uint16_t deviceid_array[20];
	uint8_t *bin_point;
	uint8_t *bin_point_last;
}STORED_AREA_FOR_PACKET;

typedef struct STATE_MACHINE_FLAG_struct
{
	uint32_t enter_bootloader_flag;
	uint16_t flow_flag;
}STATE_MACHINE_FLAG;

typedef struct PACKET_STATUS_INFO_struct
{
	uint32_t file_length;
	uint32_t total_packet_num;
	uint32_t total_section_num;
	uint32_t left_packet_num;
	uint32_t left_byte_num;
	uint32_t received_section_num;
	int bin_received_succeed;
	uint32_t dest_address;
	uint8_t current_index;
	uint64_t block_total_received_byte;
	uint8_t block_cur_percent_inc;
	int bin_received_success;
	uint32_t cal_crc;
	uint32_t received_crc;
	uint32_t block_received_packet_num;
	PACKET_INDEX_INFO packet_index_info;
	STORED_AREA_FOR_PACKET stored_area;
	STATE_MACHINE_FLAG state_machine_flag;
	uint32_t receive_flow;
	uint32_t first_pro_first_packet_tick;
	uint32_t first_pro_first_packet_flag;
	uint32_t second_pro_first_packet_tick;
	uint32_t second_pro_first_packet_flag;
	uint8_t packet_receive_timeout;
}PACKET_STATUS_INFO;

extern PACKET_STATUS_INFO packet_info;


#endif


