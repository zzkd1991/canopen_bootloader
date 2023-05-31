/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "stdio.h"
#include "string.h"

/* USER CODE BEGIN 0 */
#define RX_BUFF_SIZE			(128)
char RxBuffer1[RX_BUFF_SIZE];
uint8_t a1RxBuffer;
uint16_t Uart3_Rx_Cnt=0;
char RxBuffer2[RX_BUFF_SIZE];
uint8_t a2RxBuffer;
uint16_t Uart2_Rx_Cnt=0;

/* USER CODE END 0 */

UART_HandleTypeDef huart3;
UART_HandleTypeDef huart2;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
	
  HAL_UART_Receive_IT(&huart3,(uint8_t *)&a1RxBuffer, 1);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_MultiProcessor_Init(&huart2, 0, UART_WAKEUPMETHOD_IDLELINE) != HAL_OK)
  {
    Error_Handler();
  }
	
	HAL_UART_Receive_IT(&huart2,(uint8_t *)&a2RxBuffer, 1);

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART3 GPIO Configuration    
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART3_MspInit 1 */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PC10     ------> USART3_TX
    PC11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

    HAL_NVIC_DisableIRQ(USART3_IRQn);	
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
	
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

  	HAL_NVIC_DisableIRQ(USART2_IRQn);   

  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
	}	
} 

/* USER CODE BEGIN 1 */
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	int a1,a2;
	UNUSED(huart);
	
	if(huart == (&huart3)){
		if(Uart3_Rx_Cnt >= 255){
			Uart3_Rx_Cnt = 0;
			memset(RxBuffer1, 0, sizeof(RxBuffer1));
			HAL_UART_Transmit(&huart3, (uint8_t *)"Input overflow\r\n\r\n", 16, 0xFFFF);
		}else{
			RxBuffer1[Uart3_Rx_Cnt++] = a1RxBuffer;
			if(RxBuffer1[Uart3_Rx_Cnt-1] == 0x0A && RxBuffer1[Uart3_Rx_Cnt-2] == 0x0D){
				if(memcmp(RxBuffer1, "DID\r", 4) == 0){
						printf("LVDTDevice,SV1.05,HV1.23\r\n");
				}
				//printf("%s\n", RxBuffer1);
				if(sscanf(RxBuffer1, "CAL:%d,", &a1)==1){
					//printf("\n1111\n");
					HAL_UART_Transmit(&huart2, (uint8_t *)RxBuffer1, Uart3_Rx_Cnt, 0xFFFF);
					HAL_UART_Transmit(&huart3, (uint8_t *)RxBuffer1, Uart3_Rx_Cnt, 0xFFFF);
				}
				Uart3_Rx_Cnt = 0;
				memset(RxBuffer1, 0, sizeof(RxBuffer1));
			}
		}

		HAL_UART_Receive_IT(&huart3,(uint8_t *)&a1RxBuffer, 1);
	}
	
	if(huart == (&huart2)){
		if(Uart2_Rx_Cnt >= 255){
			Uart2_Rx_Cnt = 0;
			memset(RxBuffer2, 0, sizeof(RxBuffer2));
			HAL_UART_Transmit(&huart3, (uint8_t *)"Sensor COMIN overflow\r\n\r\n", 25, 0xFFFF);
		}else{
			RxBuffer2[Uart2_Rx_Cnt++] = a2RxBuffer;
			if(RxBuffer2[Uart2_Rx_Cnt-1] == 0x0A && RxBuffer2[Uart2_Rx_Cnt-2] == 0x0D){
				//printf("RxBuffer2 is %s\n", RxBuffer2);
				HAL_UART_Transmit(&huart3, (uint8_t *)"Sensor COMIN <<<", 16, 0xFFFF);
				HAL_UART_Transmit(&huart3, (uint8_t *)RxBuffer2, Uart2_Rx_Cnt-1, 0xFFFF);
				HAL_UART_Transmit(&huart3, (uint8_t *)"\r\n", 2, 0xFFFF);
				extern int mm32_ack_flag;
				if(strstr(RxBuffer2, "ERROR") != NULL)
				{
					mm32_ack_flag = 0x11;
				}
				else
				{
					mm32_ack_flag = 0xff;
				}

				if(sscanf(RxBuffer2, "B:%ui,\t", &a1)==1){
					char *p;
					uint32_t a0;
					//setBSolenoid(a1);
					p = strchr(RxBuffer2, 'A');
					a0 = p - RxBuffer2;
//					printf("%d, \t%d\r\n", a1, a2);
					if(sscanf(RxBuffer2+a0, "A:%ui", &a2)==1){

						//setASolenoid(a2);
						p = 0;
					}
				}
				Uart2_Rx_Cnt = 0;
				memset(RxBuffer2, 0, sizeof(RxBuffer2));
			}
		}

		HAL_UART_Receive_IT(&huart2,(uint8_t *)&a2RxBuffer, 1);
	}
	
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
