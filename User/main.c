/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "internalFlash.h"
#include "common.h"
#include "menu.h"
#include "bsp_SysTick.h"
#include "bsp_can.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"
#include "spi_flash.h"
#include "recover.h"
#include "can_queue.h"
#include "led.h"
#include "block_download.h"
#include "my_canopen_adapter.h"
#include "value_conversion.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern void Main_Menu(void);
extern pFunction Jump_To_Application;
extern void Can_data_Process(void);
extern void report_node_info(void);
extern uint32_t JumpAddress;
extern int bin_receved_succeed;
extern CAN_HandleTypeDef hcan1;
extern uint32_t id1;
extern uint32_t master_nodeid;
extern uint32_t enter_bootloader_flag;
extern int cnt;


uint32_t dest_address = 0;
int lvdt_report_button = 1;
int deviceid_report_button = 1;
uint32_t *sp = NULL;
extern uint32_t slave_nodeid;
my_message *my_m;
my_message *my_point;
extern CO_Data ObjDict_Data;

uint32_t unusedvalue = 0;

extern void USART3_GPIO_Config(void);

extern SPI_HandleTypeDef SpiHandle;


int main(void)
{
	
  int flag = 0;
  int i = 0;
	
  //HAL_Init();        
  SystemClock_Config();
  SysTick_Init();
  FLASH_If_Init();
  MX_GPIO_Init();
  LED_GPIO_Config();

  
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
	//USART3_GPIO_Config();

  MX_TIM2_Init();
  MX_TIM4_Init();
	
spi_flash_config();

  Data_Recover();

  if(id1 >= 101 || id1 <= 0)
  {
		id1 = 0x0A;		
  }
  
  extern UNS32 device_id;

  device_id = id1;
  master_nodeid = 0x580 + id1;
  slave_nodeid = 0x600 + id1;
	
  extern uint8_t packet_index_array[NUM_OF_PACKET_PER_BLOCK + 1];


  memset(&packet_index_array[0], 0xff, sizeof(packet_index_array));

	
	LED1_ON;
	LED2_ON;
	LED3_ON;
	
  Can_Config();

  	Set_Objdict_Callback_Func();

	InitCANdevice(1000, id1);
	InitNode(&ObjDict_Data, 0);

	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);  	
		
  ClearCanQueue();

  dest_address = APPLICATION_ADDRESS;

  printf("hello world\n");

	//USART3_OFF;
  /*my_m = (my_message *)malloc(sizeof(my_message) * 30);
  if(my_m == NULL)
  {
	return -1;
  }

  for(i = 0; i < NUM_OF_PACKET_PER_BLOCK; i++)
  {
  	my_point->index = 0;
		memset(&(my_point->m), 0, sizeof(Message));
		LOS_ListInit(&(my_point->my_list));
		my_point++;
  }

  my_point = my_m;*/
  
#if 1
  while(1)
  {	
  	Can_data_Process();
	if(enter_bootloader_flag == 0xff)
	{
		//report_node_info();		
	}
	if(enter_bootloader_flag != 0xff)
	{
		GetUpperComputerInfoAndWait3S(&flag);		
	}
  	if(flag == 1)//进入bootloader模式
  	{
  		Main_Menu();
 	}
	if(cnt == 0)
  //extern __IO uint32_t TimingDelay1;
	//if(TimingDelay1 >= 200000)//2s
	{
	  sp = (uint32_t *)APPLICATION_ADDRESS;
	  //printf("*sp %x\n", *sp);
	//没有进入bootloader模式，直接引导主程序
	  if(((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000)//检查栈顶地址是否合法
	  {
		  JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);
		  Jump_To_Application = (pFunction)JumpAddress;
		  //printf("Jump_To_Application %p", Jump_To_Application);
		  //printf("JumpAddress %d\n", JumpAddress);	
		  //printf("enter\n");
		  //printf("dest_address %d\n", dest_address);
		  __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
#if 1	  
		  HAL_NVIC_DisableIRQ(SysTick_IRQn);

		  HAL_TIM_Base_DeInit(&htim2);
		  HAL_TIM_Base_DeInit(&htim4);
	  
		  HAL_CAN_MspDeInit(&hcan1);
		  HAL_UART_MspDeInit(&huart2);
		  HAL_UART_MspDeInit(&huart3);
  		if (HAL_SPI_DeInit(&SpiHandle) != HAL_OK)
  		{
    		Error_Handler();
  		}
		  int i = 0;
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
	
#if 0	
extern void delay_1000ms(void);
  while(1)
	{
		LED1_ON;
		LED2_ON;
		LED3_ON;
		delay_1000ms();
		LED1_OFF;
		LED2_OFF;
		LED3_OFF;
		delay_1000ms();	
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
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  //RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  //RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  //RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  //RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  //RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
  SystemCoreClockUpdate();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



