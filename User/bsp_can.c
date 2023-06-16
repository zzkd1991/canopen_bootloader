/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "bsp_can.h"
#include "can_queue.h"
#include "block_download.h"

//2个3级深度的FIFIO
#define CAN1FIFO CAN_RX_FIFO0
#define CAN2FIFO CAN_RX_FIFO1
CAN_TxHeaderTypeDef TxMeg;
CAN_RxHeaderTypeDef RxMeg;

/* USER CODE END 0*/

uint32_t id1 = 0x0A;

CAN_HandleTypeDef hcan1;

#define CAN1_TX_IRQ		CAN1_TX_IRQn + 16
#define CAN1_RX_IRQ		CAN1_RX0_IRQn + 16

// pclk1 = sysclk(168M) / 4 = 42M
// baudrate = PCLK1(42M) / (SJW+BS1+BS2) / Prescaler
 const can_baudrate_config_t baudrate_config[] = {
 { 1000000,   3, CAN_SJW_1TQ, CAN_BS1_6TQ, CAN_BS2_7TQ},
 {	500000,   6, CAN_SJW_1TQ, CAN_BS1_6TQ, CAN_BS2_7TQ},
 {	250000,  12, CAN_SJW_1TQ, CAN_BS1_6TQ, CAN_BS2_7TQ},
 {	125000,  24, CAN_SJW_1TQ, CAN_BS1_6TQ, CAN_BS2_7TQ},
 {	100000,  30, CAN_SJW_1TQ, CAN_BS1_6TQ, CAN_BS2_7TQ},
 {	 50000,  60, CAN_SJW_1TQ, CAN_BS1_6TQ, CAN_BS2_7TQ},
 };

void CAN_User_Init(CAN_HandleTypeDef* hcan) //用户初始化函数
{
	CAN_FilterTypeDef sFilterConfig;
	HAL_StatusTypeDef HAL_Status;
	uint8_t slaveNode = id1;
	extern uint32_t slave_nodeid;

#if 1
	TxMeg.IDE = CAN_ID_STD;//CAN_ID_EXT;
	TxMeg.RTR = CAN_RTR_DATA;

	sFilterConfig.FilterBank = 0; //过滤器0
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;//设置为列表模式
	sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;

	sFilterConfig.FilterIdHigh = slave_nodeid << 5;//0x60A
	sFilterConfig.FilterIdLow = 0x20A << 5;

	sFilterConfig.FilterMaskIdHigh = 0x30A << 5;
	sFilterConfig.FilterMaskIdLow = 0x40A << 5;

#else
	TxMeg.IDE = CAN_ID_STD;
	TxMeg.RTR = CAN_RTR_DATA;
	actual_nodeid = 0x600 + slaveNode;

	sFilterConfig.FilterBank = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

	sFilterConfig.FilterIdHigh = ((((uint32_t)actual_nodeid << 21) | CAN_ID_STD | CAN_RTR_DATA) & 0xFFFF0000) >> 16;
	sFilterConfig.FilterIdLow = (((uint32_t)actual_nodeid << 21) | CAN_ID_STD | CAN_RTR_DATA) & 0xFFFF;

#if 0
	sFilterConfig.FilterMaskIdHigh = 0xFFFF;
	sFilterConfig.FilterMaskIdLow = 0xFFFF;

#else
	sFilterConfig.FilterMaskIdHigh = 0x1FF;//高位只匹配0x0A
	sFilterConfig.FilterMaskIdLow = 0xFFFF;//低位完全匹配

#endif
	
#endif

	sFilterConfig.FilterFIFOAssignment = CAN1FIFO;//接收到的报文放入到FIFO0中

	sFilterConfig.FilterActivation = ENABLE;//激活过滤器
	sFilterConfig.SlaveStartFilterBank = 14;

	HAL_Status = HAL_CAN_ConfigFilter(hcan, &sFilterConfig);
	
	HAL_Status = HAL_CAN_Start(hcan); //开启CAN

	if(HAL_Status != HAL_OK)
	{
		CAN_ERROR("Start CAN error !\r\n");
	}

	HAL_Status = HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);//FIFO0 消息挂起中断使能
	if(HAL_Status != HAL_OK)
	{
		CAN_ERROR("ActivateNotification CAN error\r\n");
	}

	HAL_Status = HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);//FIFO1 消息挂起中断使能
	if(HAL_Status != HAL_OK)
	{
		CAN_ERROR("ActivateNotification CAN error\r\n");
	}

	HAL_Status = HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY);//传输邮箱空中断
	if(HAL_Status != HAL_OK)
	{
		CAN_ERROR("ActivateNotification CAN error\r\n");
	}

	HAL_Status = HAL_CAN_ActivateNotification(&hcan1, CAN_IT_ERROR_WARNING | CAN_IT_ERROR_PASSIVE | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE | CAN_IT_ERROR);
	if(HAL_Status != HAL_OK)
	{
		CAN_ERROR("Error IT Enable Failed\r\n");
	}
}


//发送数据函数
uint8_t CANx_SendNormalData(CAN_HandleTypeDef *hcan, uint16_t ID, uint8_t *pData, uint16_t Len)
{
	HAL_StatusTypeDef HAL_RetVal;
	uint16_t SendTimes, SendCNT = 0;
	uint8_t FreeTxNum = 0;
	uint32_t tx_mailbox;
	TxMeg.StdId = ID;
	if(!hcan || ! pData || !Len) return 1;
	SendTimes = Len / 8 + ( Len % 8 ? 1 : 0);
	FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
	TxMeg.DLC = 8;
	while(SendTimes--){
		if(0 == SendTimes){
			if(Len % 8)
				TxMeg.DLC = Len % 8;
		}
		while(0 == FreeTxNum){
			FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
		}

		HAL_RetVal = HAL_CAN_AddTxMessage(&hcan1, &TxMeg, pData + SendCNT, (uint32_t*)&tx_mailbox);
		if(HAL_RetVal != HAL_OK)
		{
			return 2;
		}
		SendCNT += 8;
	
	}

	return 0;
	
}


uint8_t Can_Send(CAN_PORT notused, Message *m)
{
	uint16_t messageid;
	uint8_t result;

	messageid = m->cob_id;

	result = CANx_SendNormalData(&hcan1, messageid, m->data, m->len);

	return result;
}


void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	packet_info.can_message_static.tx_mailbox0_snd_num++;
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef * hcan)
{
	packet_info.can_message_static.tx_mailbox1_snd_num++;
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	packet_info.can_message_static.tx_mailbox2_snd_num++;
}


int Set_Can_Baud_Rate(int rate)
{
	int i = 0;
	for(i = 0; i < sizeof(baudrate_config) / sizeof(baudrate_config[0]); i++)
	{
		if(baudrate_config[i].bsp == rate * 1000)
		{
			break;
		}
	}

	if(i == (sizeof(baudrate_config) / sizeof(baudrate_config[0]) + 1))
	{
		CAN_ERROR("rate is not supported\n");
		return 1;
	}


	hcan1.Init.Prescaler = baudrate_config[i].prescaler;
	hcan1.Init.SyncJumpWidth = baudrate_config[i].SWJ;
	hcan1.Init.TimeSeg1 = baudrate_config[i].BS1;
	hcan1.Init.TimeSeg2 = baudrate_config[i].BS2;
	if(HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}

	return 0;
}

void stm32can_rxinterrupt(CAN_HandleTypeDef *hcan, int Fifo_Num)
{
	HAL_StatusTypeDef HAL_RetVal;
	uint8_t Data[8];
	Message m;
	uint32_t i = 0;
	uint32_t len = 0;

	memset(&m, 0, sizeof(m));

	if(hcan == &hcan1){
		if(Fifo_Num == 0)
		{
			packet_info.can_message_static.fifo0_recv_num++;
			HAL_RetVal = HAL_CAN_GetRxMessage(&hcan1, CAN1FIFO, &RxMeg, Data);
		}
		else if(Fifo_Num == 1)
		{
			packet_info.can_message_static.fifo1_recv_num++;
			HAL_RetVal = HAL_CAN_GetRxMessage(&hcan1, CAN2FIFO, &RxMeg, Data);
		}
		else
		{
			return;
		}
		if(HAL_OK == HAL_RetVal)
		{
			if(RxMeg.IDE == CAN_ID_STD)
			{
				m.cob_id = RxMeg.StdId;
			}
			else
			{
				m.cob_id = RxMeg.ExtId;
			}

			if(RxMeg.RTR == CAN_RTR_REMOTE)
			{
				m.rtr = 1;
			}
			else
			{
				m.rtr = 0;
			}

			len = RxMeg.DLC;

			for(i = 0; i < len; i++)
			{
				m.data[i] = Data[i];
			}

			for(i = len; i < 8; i++)
			{
				memset((void*)&m.data[i], 0, sizeof(m.data[i]));
			}

			m.len = len;

			if(0 == InsertCanQueueRx(m))
			{
				Error_Handler();
			}
		}
	}	
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)//FIFO0 接收回调函数
{
	stm32can_rxinterrupt(hcan, 0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)//FIFO1 接收回调函数
{
	stm32can_rxinterrupt(hcan, 1);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	__IO uint32_t error_code = 0;
	error_code = hcan->ErrorCode;
	if(error_code & HAL_CAN_ERROR_EWG)
	{
		packet_info.can_message_static.can_it_error_warning++;
	}

	if(error_code & HAL_CAN_ERROR_EPV)
	{
		packet_info.can_message_static.can_it_error_passive++;
	}
	
	if(error_code & HAL_CAN_ERROR_BOF)
	{
		packet_info.can_message_static.can_it_busoff++;
	}
	
	if(error_code & HAL_CAN_ERROR_STF)
	{
		packet_info.can_message_static.can_error_stf++;
	}
	
	if(error_code & HAL_CAN_ERROR_FOR)
	{
		packet_info.can_message_static.can_error_for++;
	}
	
	if(error_code & HAL_CAN_ERROR_ACK)
	{
		packet_info.can_message_static.can_error_ack++;
	}
	
	if(error_code & HAL_CAN_ERROR_BR)
	{
		packet_info.can_message_static.can_error_br++;
	}
	
	if(error_code & HAL_CAN_ERROR_CRC)
	{
		packet_info.can_message_static.can_error_crc++;
	}

}



/* CAN1 init function */
void MX_CAN1_Init(void)
{
	/* USER CODE BEGIN CAN1_Init 0*/

	/* USER CODE END CAN1_Init 0 */

	/* USER CODE BEGIN CAN1_Init 1 */

	/* USER CODE END CAN1_Init 1 */
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 12;//500kbsp
	//hcan1.Init.Prescaler = 24;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = ENABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if(HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
}


void HAL_CAN_MspInit(CAN_HandleTypeDef *canHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(canHandle->Instance == CAN1)
	{
		/* USER CODE BEGIN CAN1_MspInit 0 */

		/* USER CODE END CAN1_MspInit 0  */
		/* CAN1 clock enable */

		__HAL_RCC_CAN1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/** CAN1 GPIO  Configuration
		PA11	----------> CAN1_RX
		PA12    ----------> CAN1_TX
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* CAN1 interrupt Init */
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

		/* CAN1 interrupt Init */
		HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
		HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);

		HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
		
		/* USER CODE BEGIN CAN1_MspInit 1 */

		/* USER CODE CAN1_MspInit 1*/
	}
	
}


void HAL_CAN_MspDeInit(CAN_HandleTypeDef *canHandle)
{
	if(canHandle->Instance == CAN1)
	{
	/* USER CODE BEGIN CAN1_MspDeInit 0 */

	/* USER CODE END CAN1_MspDeInit 0 */
	/* Peripheral clock disable */
	__HAL_RCC_CAN1_CLK_DISABLE();

	/** CAN1 GPIO Configuration
	PA11	------------> CAN1_RX
	PA12	------------> CAN1_TX
	*/
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

	/* CAN1 interrupt DeInit */
	HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);

	/* CAN1 interrupt DeInit */
	HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
	/* USER CODE BEGIN CAN1_MspInit 1*/

	/* USER CODE END CAN1_MspDeInit 1*/

	}
}

void CAN_Config(uint16_t can_baud)
{
	Set_Can_Baud_Rate(can_baud);
}

void CAN_Hardware_Config(uint16_t can_baud)
{
	MX_CAN1_Init();
	CAN_Config(can_baud);
	CAN_User_Init(&hcan1);	//用户初始化函数
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

