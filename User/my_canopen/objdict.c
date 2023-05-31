#include "objdictdef.h"
#include "applicfg.h"
#include "my_canopen_adapter.h"
#include "data.h"


UNS8 ObjDict_bDeviceNodeId = 0xFF;

const UNS8 ObjDict_iam_a_slave = 1;


UNS32 ObjDict_valueRangeTest(UNS8 typeValue, void * value)
{
	/*switch(typeValue)
	{
		case valueRange_EMC:
			
	}*/

	return 0;
}

UNS32 ObjDict_obj1000 = 0x30191;
subindex ObjDict_Index1000[] = 
{
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1000}
};

UNS8 ObjDict_highestSubIndex_obj1200 = 3;//进入bootloader
UNS32 ObjDict_obj1200_COB_ID_Client_to_Server_Receive_SDO = 0x640;
UNS32 ObjDict_obj1200_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
extern UNS32 enter_bootloader_flag;

subindex ObjDict_Index1200[] =
{
	{uint8,  sizeof(UNS8),  (void*)&ObjDict_highestSubIndex_obj1200},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1200_COB_ID_Client_to_Server_Receive_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1200_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32,  sizeof(UNS32),  (void*)&enter_bootloader_flag}
};

UNS8 ObjDict_highestSubIndex_obj1201 = 3;//1号lvdt最小位置标定
UNS32 ObjDict_obj1201_COB_ID_Client_to_Server_Receive_SDO = 0x640;
UNS32 ObjDict_obj1201_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 min1_location_calibration = 0;

subindex ObjDict_Index1201[] = 
{
	{uint8,  sizeof(UNS8),  (void*)&ObjDict_highestSubIndex_obj1201},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1201_COB_ID_Client_to_Server_Receive_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1201_COB_ID_Client_to_Server_Receive_SDO},
	{uint32,  sizeof(UNS32),  (void*)&min1_location_calibration}
};

UNS8 ObjDict_highestSubIndex_obj1202 = 3;//2号lvdt最小位置标定
UNS32 ObjDict_obj1202_COB_ID_Client_to_Server_Receive_SDO = 0x640;
UNS32 ObjDict_obj1202_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 min2_location_calibration = 0;

subindex ObjDict_Index1202[] =
{
	{uint8, sizeof(UNS8), (void*)&ObjDict_highestSubIndex_obj1202},
	{uint32, sizeof(UNS32),	(void*)&ObjDict_obj1202_COB_ID_Client_to_Server_Receive_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1202_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&min2_location_calibration}
};

UNS8  ObjDict_highestSubIndex_obj1203 = 3;//1号lvdt最大位置标定
UNS32 ObjDict_obj1203_COB_ID_Client_to_Server_Receive_SDO = 0x640;
UNS32 ObjDict_obj1203_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32  max1_location_calibration = 0;

subindex ObjDict_Index1203[] = 
{
	{uint8,	 sizeof(UNS8),	(void*)&ObjDict_highestSubIndex_obj1203},
	{uint32, sizeof(UNS32),	(void*)&ObjDict_obj1203_COB_ID_Client_to_Server_Receive_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1203_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32,  sizeof(UNS32), (void*)&max1_location_calibration}
};

UNS8  ObjDict_highestSubIndex_obj1204 = 3;//2号lvdt最大位置标定
UNS32 ObjDict_obj1204_COB_ID_Client_to_Server_Receive_SDO = 0x640;
UNS32 ObjDict_obj1204_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32  max2_location_calibration = 0;

subindex ObjDict_Index1204[] = 
{
	{uint8,	 sizeof(UNS8),	(void*)&ObjDict_highestSubIndex_obj1204},
	{uint32, sizeof(UNS32),	(void*)&ObjDict_obj1204_COB_ID_Client_to_Server_Receive_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1204_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32,  sizeof(UNS32), (void*)&max2_location_calibration}
};


UNS8  ObjDict_highestSubIndex_obj1205 = 4;//读取1号lvdt的当前位置
UNS32 ObjDict_obj1205_COB_ID_Client_to_Server_Recievd_SDO = 0x640;
UNS32 ObjDict_obj1205_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 lvdt1_current_location = 0;

subindex ObjDict_Index1205[] = 
{
	{uint8,  sizeof(UNS8),  (void*)&ObjDict_highestSubIndex_obj1203},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1205_COB_ID_Client_to_Server_Recievd_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1205_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&lvdt1_current_location}
};

UNS8  ObjDict_highestSubIndex_obj1206 = 4;//读取2号lvdt的当前位置
UNS32 ObjDict_obj1206_COB_ID_Client_to_Server_Recievd_SDO = 0x640;
UNS32 ObjDict_obj1206_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 lvdt2_current_location = 0;

subindex ObjDict_Index1206[] = 
{
	{uint8,  sizeof(UNS8),  (void*)&ObjDict_highestSubIndex_obj1206},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1206_COB_ID_Client_to_Server_Recievd_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1206_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&lvdt2_current_location}
};


UNS8 ObjDict_highestSubIndex_obj1207 = 4;//设置1号lvdt零位置
UNS32 ObjDict_obj1207_COB_ID_Client_to_Server_Received_SDO = 0x640;
UNS32 ObjDict_obj1207_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 unused_lvdt1_zero_location = 0;

subindex ObjDict_Index1207[] = 
{
	{uint8,  sizeof(UNS8), (void*)&ObjDict_highestSubIndex_obj1207},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1207_COB_ID_Client_to_Server_Received_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1207_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&unused_lvdt1_zero_location}
};

UNS8 ObjDict_highestSubIndex_obj1208 = 4;//设置2号lvdt零位置
UNS32 ObjDict_obj1208_COB_ID_Client_to_Server_Received_SDO = 0x640;
UNS32 ObjDict_obj1208_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 unused_lvdt2_zero_location = 0;

subindex ObjDict_Index1208[] = 
{
	{uint8,  sizeof(UNS8), (void*)&ObjDict_highestSubIndex_obj1208},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1208_COB_ID_Client_to_Server_Received_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1208_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&unused_lvdt2_zero_location}	
};

UNS8 ObjDict_highestSubIndex_obj1209 = 4;//设置/读取每联ID
UNS32 ObjDict_obj1209_COB_ID_Client_to_Server_Received_SDO = 0x640;
UNS32 ObjDict_obj1209_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0; 
UNS32 device_id = 0;

subindex ObjDict_Index1209[] = 
{
	{uint8,  sizeof(UNS8), (void*)&ObjDict_highestSubIndex_obj1209},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1209_COB_ID_Client_to_Server_Received_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1209_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&device_id}
};

UNS8 ObjDict_highestSubIndex_obj120A = 4;//保存设备ID
UNS32 ObjDict_obj120A_COB_ID_Client_to_Server_Recievd_SDO = 0x640;
UNS32 ObjDict_obj120A_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 unusedvalue0 = 0;

subindex ObjDict_Index120A[] = 
{
	{uint8, sizeof(UNS8), (void*)&ObjDict_highestSubIndex_obj120A},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj120A_COB_ID_Client_to_Server_Recievd_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj120A_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&unusedvalue0}
};

UNS8 ObjDict_highestSubIndex_obj120B = 4;//分段传输启动bin文件下载
UNS32 ObjDict_obj120B_COB_ID_Client_to_Server_Received_SDO = 0x640;
UNS32 ObjDict_obj120B_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 bin1_file_length = 0;

subindex ObjDict_Index120B[] = 
{
	{uint8, sizeof(UNS8), (void*)&ObjDict_highestSubIndex_obj120B},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj120B_COB_ID_Client_to_Server_Received_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj120B_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&bin1_file_length}
};

UNS8 ObjDict_highestSubIndex_obj120C = 4;//块传输启动bin文件下载
UNS32 ObjDict_obj120C_COB_ID_Client_to_Server_Received_SDO = 0x640;
UNS32 ObjDict_obj120C_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 bin2_file_length = 0;

subindex ObjDict_Index120C[] = 
{
	{uint8, sizeof(UNS8), (void*)&ObjDict_highestSubIndex_obj120C},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj120C_COB_ID_Client_to_Server_Received_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj120C_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&bin2_file_length}
};

UNS8 ObjDict_highestSubIndex_obj1210 = 4;//关闭lvdt位置上报和ID上报功能
UNS32 ObjDict_obj1210_COB_ID_Client_to_Server_Received_SDO = 0x640;
UNS32 ObjDict_obj1210_COB_ID_Server_to_Client_Transmit_SDO = 0x5C0;
UNS32 unusedvalue1 = 0;

subindex ObjDict_Index1210[] = 
{
	{uint8, sizeof(UNS8), (void*)&ObjDict_highestSubIndex_obj1210},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1210_COB_ID_Client_to_Server_Received_SDO},
	{uint32, sizeof(UNS32), (void*)&ObjDict_obj1210_COB_ID_Server_to_Client_Transmit_SDO},
	{uint32, sizeof(UNS32), (void*)&unusedvalue1}
};

//ODCallback_t ObjDict_Index1200_callbacks[4] = {0};
ODCallback_t ObjDict_Index1201_callbacks[4] = {0};
ODCallback_t ObjDict_Index1202_callbacks[4] = {0};
ODCallback_t ObjDict_Index1203_callbacks[4] = {0};
ODCallback_t ObjDict_Index1204_callbacks[4] = {0};
//ODCallback_t ObjDict_Index1205_callbacks[4] = {0};
//ODCallback_t ObjDict_Index1206_callbacks[4] = {0};
ODCallback_t ObjDict_Index1207_callbacks[4] = {0};
ODCallback_t ObjDict_Index1208_callbacks[4] = {0};
ODCallback_t ObjDict_Index1209_callbacks[4] = {0};
ODCallback_t ObjDict_Index120A_callbacks[4] = {0};
ODCallback_t ObjDict_Index120B_callbacks[4] = {0};
ODCallback_t ObjDict_Index120C_callbacks[4] = {0};
ODCallback_t ObjDict_Index1210_callbacks[4] = {0};


const indextable ObjDict_objdict[] = 
{
	{(subindex*)ObjDict_Index1000, sizeof(ObjDict_Index1000)/sizeof(ObjDict_Index1000[0]), 0x1000},//0
	{(subindex*)ObjDict_Index1200, sizeof(ObjDict_Index1200)/sizeof(ObjDict_Index1200[0]), 0x1200},//1
	{(subindex*)ObjDict_Index1201, sizeof(ObjDict_Index1201)/sizeof(ObjDict_Index1201[0]), 0x1201},//2
	{(subindex*)ObjDict_Index1202, sizeof(ObjDict_Index1202)/sizeof(ObjDict_Index1202[0]), 0x1202},//3
	{(subindex*)ObjDict_Index1203, sizeof(ObjDict_Index1203)/sizeof(ObjDict_Index1203[0]), 0x1203},//4
	{(subindex*)ObjDict_Index1204, sizeof(ObjDict_Index1204)/sizeof(ObjDict_Index1204[0]), 0x1204},//5
	{(subindex*)ObjDict_Index1205, sizeof(ObjDict_Index1205)/sizeof(ObjDict_Index1205[0]), 0x1205},//6
	{(subindex*)ObjDict_Index1206, sizeof(ObjDict_Index1206)/sizeof(ObjDict_Index1206[0]), 0x1206},//7
	{(subindex*)ObjDict_Index1207, sizeof(ObjDict_Index1207)/sizeof(ObjDict_Index1207[0]), 0x1207},//8
	{(subindex*)ObjDict_Index1208, sizeof(ObjDict_Index1208)/sizeof(ObjDict_Index1208[0]), 0x1208},//9
	{(subindex*)ObjDict_Index1209, sizeof(ObjDict_Index1209)/sizeof(ObjDict_Index1209[0]), 0x1209},//10
	{(subindex*)ObjDict_Index120A, sizeof(ObjDict_Index120A)/sizeof(ObjDict_Index120A[0]), 0x120A},//11
	{(subindex*)ObjDict_Index120B, sizeof(ObjDict_Index120B)/sizeof(ObjDict_Index120B[0]), 0x120B},//12
	{(subindex*)ObjDict_Index120C, sizeof(ObjDict_Index120C)/sizeof(ObjDict_Index120C[0]), 0x120C},//13
	{(subindex*)ObjDict_Index1210, sizeof(ObjDict_Index1210)/sizeof(ObjDict_Index1210[0]), 0x1210}//14
};

const indextable *ObjDict_scanIndexOD(UNS16 wIndex, UNS32 *errorCode, ODCallback_t **callbacks)
{
	int i;
	*callbacks = NULL;
	switch(wIndex)
	{
		case 0x1000 : i = 0;  break;
		case 0x1200 : i = 1;  break;
		case 0x1201 : i = 2;  *callbacks = &ObjDict_Index1201_callbacks[0];break;
		case 0x1202 : i = 3;  *callbacks = &ObjDict_Index1202_callbacks[0];break;
		case 0x1203 : i = 4;  *callbacks = &ObjDict_Index1203_callbacks[0];break;
		case 0x1204 : i = 5;  *callbacks = &ObjDict_Index1204_callbacks[0];break;
		case 0x1205 : i = 6;  break;
		case 0x1206 : i = 7;  break;
		case 0x1207 : i = 8;  *callbacks = &ObjDict_Index1207_callbacks[0];break;
		case 0x1208 : i = 9;  *callbacks = &ObjDict_Index1208_callbacks[0];break;
		case 0x1209 : i = 10;  *callbacks = &ObjDict_Index1209_callbacks[0];break;
		case 0x120A : i = 11; *callbacks = &ObjDict_Index120A_callbacks[0];break;
		case 0x120B : i = 12; *callbacks = &ObjDict_Index120B_callbacks[0];break;
		case 0x120C : i = 13; *callbacks = &ObjDict_Index120C_callbacks[0];break;
		case 0x1210 : i = 14; *callbacks = &ObjDict_Index1210_callbacks[0];break;
		default:
			*errorCode = OD_NO_SUCH_OBJECT;
			return NULL;
	}

	*errorCode = OD_SUCCESSFUL;
	return &ObjDict_objdict[i];
}

quick_index ObjDict_firstIndex = {
	1,
	14,
};

quick_index ObjDict_lastIndex = {
	14,
	14,
};

UNS16 ObjDict_ObjdictSize = sizeof(ObjDict_objdict) / sizeof(ObjDict_objdict[0]);

CO_Data ObjDict_Data = CANOPEN_NODE_DATA_INITIALIZER(ObjDict);

