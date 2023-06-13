/**
  ******************************************************************************
  * @file    STM32F4xx_IAP/src/menu.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    10-October-2011
  * @brief   This file provides the software which contains the main menu routine.
  *          The main menu gives the options of:
  *             - downloading a new binary file, 
  *             - uploading internal flash memory,
  *             - executing the binary file already loaded 
  *             - disabling the write protection of the Flash sectors where the 
  *               user loads his binary file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/** @addtogroup STM32F4xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "internalFlash.h"
#include "menu.h"
#include "bsp_can.h"
#include "main.h"
#include "usart.h"
#include "spi.h"
#include "block_download.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;
__IO uint32_t FlashProtection = 0;
extern PACKET_STATUS_INFO packet_status_info;
extern UART_HandleTypeDef UartHandle;



/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
void Main_Menu(void)
{	
	if(packet_status_info.bin_received_success == 0xff)//进入bootloader模式，引导主程序
	{
		  //HAL_NVIC_DisableIRQ(SysTick_IRQn);
		  if(((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000)//检查栈顶地址是否合法
		  {
			  JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);
			  Jump_To_Application = (pFunction)JumpAddress;
			  printf("enter\n");
			  __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);  
#if 1	  
			  HAL_NVIC_DisableIRQ(SysTick_IRQn);
				
			  HAL_CAN_MspDeInit(&hcan1);
			  HAL_UART_MspDeInit(&UartHandle);
				extern SPI_HandleTypeDef SpiHandle;
			  if (HAL_SPI_DeInit(&SpiHandle) != HAL_OK)
			  {
				 Error_Handler();
			  }
			  /*int i = 0;
			  for(i = 0; i < 8; i++)
			  {
				 NVIC->ICER[i] = 0xFFFFFFFF;
				 NVIC->ICPR[i] = 0xFFFFFFFF;
			  }*/ 		
			  //LED_GPIO_DeConfig();
			  HAL_RCC_DeInit();
			  /*SysTick->CTRL = 0;
			  SysTick->LOAD = 0;
			  SysTick->VAL = 0;*/ 

#else
		  	int i = 0;
		  	for(i = 0; i < 8; i++)
		  	{
				NVIC->ICER[i] = 0xFFFFFFFF;
				NVIC->ICPR[i] = 0xFFFFFFFF;
		  	}
		  	HAL_RCC_DeInit();
		  	SysTick->CTRL = 0;
		  	SysTick->LOAD = 0;
		  	SysTick->VAL = 0;
#endif				  
			  Jump_To_Application();
		  } 
    }
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
