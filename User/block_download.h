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

int get_canmessage_num(void);


typedef struct
{
	Message m;
	uint8_t index;
	LOS_DL_LIST my_list;
}my_message;



#endif


