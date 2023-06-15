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
#include "spi.h"
#include "usart.h"
#include "block_download.h"
#include "bin_backup.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
extern uint32_t id1;
extern uint32_t device_id;

uint32_t slave_nodeid;
uint32_t master_nodeid;
uint32_t device_id;

int main(void)
{
    int flag = 0;
	int i = 0;
	uint32_t application_address;
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
    
    memset(&packet_info.stored_area.packet_index_array[0], 0xff, sizeof(packet_info.stored_area.packet_index_array));
    led1_show_white();
    led2_show_white();
    CAN_Hardware_Config(500);
    ClearCanQueue();
	

	application_address = APPLICATION_ADDRESS_START;
	//application_address = 0x08030000;
	//application_address = NEW_APPLICATION_ADDRESS_START;

	packet_info.dest_address = APPLICATION_ADDRESS_START;
	packet_info.state_machine_flag.enter_bootloader_flag = not_enter_bootloader;
	packet_info.state_machine_flag.flow_flag = prepare_flow_flag;
	packet_info.receive_flow = first_procedure;
  	printf("hello world\n");


#if 1
	while(1)
	{	
		Can_Data_Process();
		if(packet_info.state_machine_flag.enter_bootloader_flag == not_enter_bootloader)
		{
			GetUpperComputerInfoAndWait3S(&flag);		
		}
		if(flag == load_new_procedure)
		{
			if(packet_info.bin_received_success == 0xff)
			{
				/*确保在执行应用程序之前CAN报文发送完成*/
				HAL_Delay(100);
				copy_bin_from_oldaddress_to_newaddress();				
				//HAL_Delay(500);
				Main_Menu(application_address);	
			}				
		}
		else if(flag == load_old_procedure)
		{
			Main_Menu(application_address);
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



