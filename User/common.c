/**
  ******************************************************************************
  * @file    STM32F4xx_IAP/src/common.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    10-October-2011
  * @brief   This file provides all the common functions.
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
#include "stdio.h"
#include "bsp_can.h"
#include "usart.h"
#include "main.h"
#include "stm32f4xx.h"
#include "block_download.h"
#include "can_queue.h"
#include "internalFlash.h"
#include "led.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The integer to be converted
  * @retval None
  */
void Int2Str(uint8_t* str, int32_t intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The integer value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}


uint32_t slave_nodeid;
uint32_t master_nodeid;
uint32_t device_id;


void Driver_Init(void)
{
	extern uint32_t id1;
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
}


void Init_Blockdownloaod_Proc()
{
	packet_info.dest_address = APPLICATION_ADDRESS_START;
	packet_info.state_machine_flag.enter_bootloader_flag = not_enter_bootloader;
	packet_info.state_machine_flag.flow_flag = prepare_flow_flag;
	packet_info.receive_flow = first_procedure;
}

void GetUpperComputerInfoAndWait3S(int* flag)
{
	extern __IO uint32_t TimingDelay1;
  	int cnt = 10;

	/* Waiting for user input */
	while (1)
	{
		
		Can_Data_Process();
		if (packet_info.state_machine_flag.enter_bootloader_flag == enter_bootloader && cnt > 0)
		{
			*flag = load_new_procedure;
			break;	
		}
		if(TimingDelay1 >= 1000)//1s
		{	
		   	TimingDelay1 = 0;		   
		   	cnt--;
		   	printf("%d s\n", cnt);
		   	if(cnt == 0 && packet_info.state_machine_flag.enter_bootloader_flag == not_enter_bootloader)
		   	{
		   		*flag = load_old_procedure;
		   		break;
		   	}
		}
	}
}

/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
void SerialPutChar(uint8_t c)
{
  HAL_StatusTypeDef result;
  result = HAL_UART_Transmit(&UartHandle, &c, sizeof(c), 1000);
  
  if(result == HAL_OK);
  else if(result == HAL_TIMEOUT)
  {
	while(1);
  }
}

/**
  * @brief  Print a string on the HyperTerminal
  * @param  s: The string to be printed
  * @retval None
  */
void Serial_PutString(uint8_t *s)
{
  while (*s != '\0')
  {
    SerialPutChar(*s);
    s++;
  }
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
