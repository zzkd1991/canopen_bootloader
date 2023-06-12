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
	packet_index_error = -4,
	packet_num_insufficent = -3,
	packet_crc_check_error = -2,
	packet_write_flash_error = -1,
	packet_ok = 0,
}HANDLE_RECEIVED_PACKET_STATUS;

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
	uint32_t last_packet_arrived_tick;
	uint64_t block_total_received_byte;
	uint8_t block_cur_percent_inc;
	int bin_received_success;
}PACKET_STATUS_INFO;

typedef struct RECEIVED_PROCEDURE_struct
{
	int first_procedure;
	int second_procedure;
}RECEIVED_PROCEDURE_INFO;

#endif


