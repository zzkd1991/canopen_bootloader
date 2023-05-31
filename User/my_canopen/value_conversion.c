#include "my_canopen_adapter.h"
#include "objdictdef.h"
#include "data.h"
#include "sdo.h"
#include "objacces.h"
#include "applicfg.h"
#include "spi_flash.h"
#include "recover.h"
#include "internalFlash.h"
#include "block_download.h"
#include "value_conversion.h"

UNS32 ObjDict_Index1201_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//1号lvdt最小位置标定
{
	extern UNS8 min1_location_calibration;
	extern UART_HandleTypeDef huart2;
	extern uint8_t output_array[30];
	extern __IO u32 TimingDelay4;
	extern int mm32_ack_flag;
  extern void DoubleValve_lvdtFormOutputStr(int location, int min_or_max, int num, int* length);
	
	u32 CurrTimingDelay4 = 0;
	int length = 0;

	DoubleValve_lvdtFormOutputStr(min1_location_calibration, 0, 1, &length);
	if(HAL_OK != HAL_UART_Transmit(&huart2, output_array, length, 0x1000))
	{
		return OD_NO_SUCH_SUBINDEX;
	}

	CurrTimingDelay4 = TimingDelay4;

	while(TimingDelay4 - CurrTimingDelay4 <= 1000)
	{
	}

	if((mm32_ack_flag == 0x11) || (mm32_ack_flag == 0))
	{
		mm32_ack_flag = 0;
		return OD_NO_SUCH_SUBINDEX;		
	}
	else
	{
		mm32_ack_flag = 0;
		return OD_SUCCESSFUL;
	}
}

UNS32 ObjDict_Index1202_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//2号lvdt最小位置标定
{
	extern UNS8 min2_location_calibration;
	extern UART_HandleTypeDef huart2;
	extern uint8_t output_array[30];
	extern __IO u32 TimingDelay4;
	extern int mm32_ack_flag;
  extern void DoubleValve_lvdtFormOutputStr(int location, int min_or_max, int num, int* length);
	
	u32 CurrTimingDelay4 = 0;
	int length = 0;

	DoubleValve_lvdtFormOutputStr(min2_location_calibration, 0, 2, &length);
	if(HAL_OK != HAL_UART_Transmit(&huart2, output_array, length, 0x1000))
	{
		return OD_NO_SUCH_SUBINDEX;
	}

	CurrTimingDelay4 = TimingDelay4;

	while(TimingDelay4 - CurrTimingDelay4 <= 1000)
	{
	}

	if((mm32_ack_flag == 0x11) || (mm32_ack_flag == 0))
	{
		mm32_ack_flag = 0;
		return OD_NO_SUCH_SUBINDEX;		
	}
	else
	{
		mm32_ack_flag = 0;
		return OD_SUCCESSFUL;
	}
}


UNS32 ObjDict_Index1203_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//1号最大位置标定
{
	extern UNS8 max1_location_calibration;
	extern UART_HandleTypeDef huart2;
	extern uint8_t output_array[30];
	extern __IO u32 TimingDelay4;
	extern int mm32_ack_flag;
  extern void DoubleValve_lvdtFormOutputStr(int location, int min_or_max, int num, int* length);
	
	u32 CurrTimingDelay4 = 0;
	int length = 0;

	DoubleValve_lvdtFormOutputStr(max1_location_calibration, 1, 1, &length);
	if(HAL_OK != HAL_UART_Transmit(&huart2, output_array, length, 0x1000))
	{
		return OD_NO_SUCH_SUBINDEX;
	}

	CurrTimingDelay4 = TimingDelay4;

	while(TimingDelay4 - CurrTimingDelay4 <= 1000)
	{
	}

	if((mm32_ack_flag == 0x11) || (mm32_ack_flag == 0))
	{
		mm32_ack_flag = 0;
		return OD_NO_SUCH_SUBINDEX;		
	}
	else
	{
		mm32_ack_flag = 0;
		return OD_SUCCESSFUL;
	}
}

UNS32 ObjDict_Index1204_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//2号最大位置标定
{
	extern UNS8 max2_location_calibration;
	extern UART_HandleTypeDef huart2;
	extern uint8_t output_array[30];
	extern __IO u32 TimingDelay4;
	extern int mm32_ack_flag;
  extern void DoubleValve_lvdtFormOutputStr(int location, int min_or_max, int num, int* length);
	
	u32 CurrTimingDelay4 = 0;
	int length = 0;

	DoubleValve_lvdtFormOutputStr(max2_location_calibration, 1, 2, &length);
	if(HAL_OK != HAL_UART_Transmit(&huart2, output_array, length, 0x1000))
	{
		return OD_NO_SUCH_SUBINDEX;
	}

	CurrTimingDelay4 = TimingDelay4;

	while(TimingDelay4 - CurrTimingDelay4 <= 1000)
	{
	}

	if((mm32_ack_flag == 0x11) || (mm32_ack_flag == 0))
	{
		mm32_ack_flag = 0;
		return OD_NO_SUCH_SUBINDEX;		
	}
	else
	{
		mm32_ack_flag = 0;
		return OD_SUCCESSFUL;
	}
}

UNS32 ObjDict_Index1207_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//1号lvdt设置零位置
{
	extern float location[2];
	uint32_t lvdt1_zero_location;
	lvdt1_zero_location = (uint32_t)location[0];
	spi_flash_erase_page(LVDT1_ZERO_LOCATION_VALVE_ADDRESS, 1, SPI_FLASH_PAGE_ERASE_WAIT);
	spi_flash_write(LVDT1_ZERO_LOCATION_VALVE_ADDRESS, (uint8_t*)&lvdt1_zero_location, sizeof(lvdt1_zero_location));

	return OD_SUCCESSFUL;
}

UNS32 ObjDict_Index1208_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//2号lvdt设置零位置
{
	extern float location[2];
	uint32_t lvdt2_zero_location;
	lvdt2_zero_location = (uint32_t)location[1];
	spi_flash_erase_page(LVDT2_ZERO_LOCATION_VALVE_ADDRESS, 1, SPI_FLASH_PAGE_ERASE_WAIT);
	spi_flash_write(LVDT2_ZERO_LOCATION_VALVE_ADDRESS, (uint8_t*)&lvdt2_zero_location, sizeof(lvdt2_zero_location));

	return OD_SUCCESSFUL;
	
}

UNS32 ObjDict_Index1209_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//设置每联ID
{
	extern UNS32 device_id;
	extern Data_Store_Format array[3];

	array[0].id = device_id;

	return OD_SUCCESSFUL;
}

UNS32 ObjDict_Index120A_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//保存设备ID
{
	extern Data_Store_Format array[3];

	spi_flash_erase_page(0, 1, SPI_FLASH_PAGE_ERASE_WAIT);
	
	spi_flash_write(0, (uint8_t *)array, sizeof(array));

	return OD_SUCCESSFUL;
	
}

UNS32 ObjDict_Index120B_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//分段传输bin文件下载
{		
		extern UNS32 bin1_file_length;
		extern uint64_t bin_file_length;
		extern uint32_t master_nodeid;
		bin_file_length = bin1_file_length;
		Message ack_message = {0};		
		ack_message.len = 8;
		ack_message.rtr = CAN_RTR_DATA;
		ack_message.cob_id = master_nodeid; 				
		ack_message.data[0] = 0x60;
		ack_message.data[1] = 0x0B;
		ack_message.data[2] = 0x12;
		ack_message.data[3] = 0x04;

		
		ack_message.data[3] = 0x05;//发出正在擦除flash消息
		if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
		{
			Error_Handler();
		}		
		if(0 != FLASH_If_Erase(0))
		{
			Error_Handler();
		}

		return OD_SUCCESSFUL;

}

UNS32 ObjDict_Index120C_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//块传输bin文件下载
{		
	extern uint8_t bin_received_file[(NUM_OF_PACKET_PER_BLOCK + 1) * 7];
	extern uint8_t bin_received_file_last[NUM_OF_PACKET_PER_BLOCK * 7];
	extern uint8_t packet_index_array[NUM_OF_PACKET_PER_BLOCK + 1];
	extern uint8_t *bin_point;
	extern uint8_t *bin_point_last;
	extern uint32_t first_packet_received;
	extern uint32_t total_packet_num;
	extern uint32_t total_section_num;
	extern uint32_t left_packet_num;
	extern uint32_t left_byte_num;
	extern uint32_t received_section_num;
	extern UNS32 bin2_file_length;
	extern uint32_t file_length;
	extern uint64_t bin_file_length;
	extern uint32_t master_nodeid;
	//extern int lvdt_report_button;
	//extern int deviceid_report_button;


	Message ack_message = {0};		



	//lvdt_report_button = 0;
	//deviceid_report_button = 0;

	file_length = bin2_file_length;	
	
	bin_point = &bin_received_file[0];
	bin_point_last = &bin_received_file_last[0];
	first_packet_received = 1;
	total_packet_num = file_length / 7;
	
	left_byte_num = file_length % 7;
	
	total_section_num = total_packet_num / NUM_OF_PACKET_PER_BLOCK;
	
	//left_packet_num = total_packet_num % 127;//不足一个段剩余包的数量
	left_packet_num = total_packet_num - NUM_OF_PACKET_PER_BLOCK * total_section_num;
	
	bin_file_length = file_length;
	
	ack_message.len = 8;
	ack_message.rtr = CAN_RTR_DATA;
	ack_message.cob_id = master_nodeid; 				
	ack_message.data[0] = 0x60;
	ack_message.data[1] = 0x0C;
	ack_message.data[2] = 0x12; 	
	ack_message.data[3] = 0x04;//发出正在擦除flash消息
	if(CAN_SEND_OK != Can_Send(NULL, &ack_message))
	{
		Error_Handler();
	}
	
	
	if(0 != FLASH_If_Erase(0))
	{
		return OD_NO_SUCH_SUBINDEX;
	}	

	return OD_SUCCESSFUL;

}

UNS32 ObjDict_Index1210_Conv_Value(CO_Data *d, const indextable *table, UNS8 bSubindex)//关闭lvdt位置上报和ID上报功能
{
	extern int lvdt_report_button;
	extern int deviceid_report_button;

	lvdt_report_button = 0;
	deviceid_report_button = 0;

	return OD_SUCCESSFUL;
}

void Set_Objdict_Callback_Func(void)
{
	extern ODCallback_t ObjDict_Index1201_callbacks[4];
	extern ODCallback_t ObjDict_Index1202_callbacks[4];
	extern ODCallback_t ObjDict_Index1203_callbacks[4];
	extern ODCallback_t ObjDict_Index1204_callbacks[4];
	extern ODCallback_t ObjDict_Index1207_callbacks[4];
	extern ODCallback_t ObjDict_Index1208_callbacks[4];
	extern ODCallback_t ObjDict_Index1209_callbacks[4];
	extern ODCallback_t ObjDict_Index120A_callbacks[4];
	extern ODCallback_t ObjDict_Index120B_callbacks[4];
	extern ODCallback_t ObjDict_Index120C_callbacks[4];
	extern ODCallback_t ObjDict_Index1210_callbacks[4];



	int i;

	for(i = 0 ; i < 4; i++)
	{
		ObjDict_Index1201_callbacks[i] = ObjDict_Index1201_Conv_Value;
	}

	for(i = 0 ; i < 4; i++)
	{
		ObjDict_Index1202_callbacks[i] = ObjDict_Index1202_Conv_Value;
	}

	for(i = 0; i < 4; i++)
	{
		ObjDict_Index1203_callbacks[i] = ObjDict_Index1203_Conv_Value;
	}

	for(i = 0; i < 4; i++)
	{
		ObjDict_Index1204_callbacks[i] = ObjDict_Index1204_Conv_Value;
	}

	for(i = 0 ; i < 4; i++)
	{
		ObjDict_Index1207_callbacks[i] = ObjDict_Index1207_Conv_Value;
	}

	for(i = 0 ; i < 4; i++)
	{
		ObjDict_Index1208_callbacks[i] = ObjDict_Index1208_Conv_Value;
	}

	for(i = 0 ; i < 4; i++)
	{
		ObjDict_Index1209_callbacks[i] = ObjDict_Index1209_Conv_Value;
	}

	for(i = 0 ; i < 4; i++)
	{
		ObjDict_Index120A_callbacks[i] = ObjDict_Index120A_Conv_Value;
	}

	for(i = 0; i < 4; i++)
	{
		ObjDict_Index120B_callbacks[i] = ObjDict_Index120B_Conv_Value;
	}

	for(i = 0; i < 4; i++)
	{
		ObjDict_Index120C_callbacks[i] = ObjDict_Index120C_Conv_Value;
	}

	for(i = 0; i < 4; i++)
	{
		ObjDict_Index1210_callbacks[i] = ObjDict_Index1210_Conv_Value;
	}
	
}



