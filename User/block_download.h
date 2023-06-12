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

#endif


