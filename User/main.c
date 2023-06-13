/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "usart.h"
#include "internalFlash.h"
#include "common.h"
#include "menu.h"
#include "bsp_SysTick.h"
#include "bsp_can.h"
#include "usart.h"
#include "spi_flash.h"
#include "recover.h"
#include "can_queue.h"
#include "led.h"
#include "block_download.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern void Main_Menu(void);
extern pFunction Jump_To_Application;
extern void Can_data_Process(void);
extern uint32_t JumpAddress;
extern CAN_HandleTypeDef hcan1;
extern uint32_t id1;
uint32_t *sp = NULL;
extern SPI_HandleTypeDef SpiHandle;
extern UART_HandleTypeDef UartHandle;
extern uint32_t device_id;
extern PACKET_STATUS_INFO packet_status_info;

uint32_t slave_nodeid;
uint32_t master_nodeid;
uint32_t device_id;

int main(void)
{
    int flag = 0;
	int i = 0;	
    SystemClock_Config();
    SysTick_Init();
    FLASH_If_Init();
    LED_GPIO_Config();
    DEBUG_UART_Config();
    //spi_flash_config();
    //Data_Recover();
    
    if(id1 >= 101 || id1 <= 0)
    {
    	id1 = 0x0A;		
    }
    
    device_id = id1;
    master_nodeid = 0x580 + id1;
    slave_nodeid = 0x600 + id1;
    
    memset(&packet_status_info.stored_area.packet_index_array[0], 0xff, sizeof(packet_status_info.stored_area.packet_index_array));
    led1_show_white();
    led2_show_white();
    Can_Config();	
    ClearCanQueue();
    packet_status_info.dest_address = APPLICATION_ADDRESS;
	  packet_status_info.state_machine_flag.enter_bootloader_flag = not_enter_bootloader;
		packet_status_info.state_machine_flag.flow_flag = prepare_flow_flag;
		packet_status_info.receive_flow = first_procedure;
    printf("hello world\n");
		
		if(0 != FLASH_If_Erase(0))
		{
			Error_Handler();
			return -1;
		}

#if 1
		while(1)
		{	
			Can_data_Process();
			if(packet_status_info.state_machine_flag.enter_bootloader_flag == not_enter_bootloader)
			{
				GetUpperComputerInfoAndWait3S(&flag);		
			}
			if(flag == load_new_procedure)
			{
				Main_Menu();
			}
			else if(flag == load_old_procedure)
			{
				sp = (uint32_t *)APPLICATION_ADDRESS;
				//没有进入bootloader模式，直接引导主程序
				if(((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000)//检查栈顶地址是否合法
				{
					JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);
					Jump_To_Application = (pFunction)JumpAddress;
					__set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
		#if 1	  
					HAL_NVIC_DisableIRQ(SysTick_IRQn);

					HAL_CAN_MspDeInit(&hcan1);
					HAL_UART_MspDeInit(&UartHandle);
					if (HAL_SPI_DeInit(&SpiHandle) != HAL_OK)
					{
						Error_Handler();
					}

					for(i = 0; i < 8; i++)
					{
						NVIC->ICER[i] = 0xFFFFFFFF;
						NVIC->ICPR[i] = 0xFFFFFFFF;
					}			
					LED_GPIO_DeConfig();
					HAL_RCC_DeInit();
					SysTick->CTRL = 0;
					SysTick->LOAD = 0;
					SysTick->VAL = 0;			
		#else
					//跳转前需要关闭无关中断，防止APP中未使用该中断，却因为中断而跳转到中断向量表时找不到对应函数入口
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
#endif
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
 void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



