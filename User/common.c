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
#include "main.h"
#include "stm32f4xx.h"


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

/**
  * @brief  Get an integer from the HyperTerminal
  * @param  num: The integer
  * @retval 1: Correct
  *         0: Error
  */
void GetUpperComputerInfoAndWait3S(int* flag)
{
  extern __IO uint32_t TimingDelay1;
  extern uint32_t enter_bootloader_flag;
  extern void Can_data_Process(void);
  extern void report_node_info(void);
	extern int cnt;
  extern uint32_t id1;
	extern uint32_t master_nodeid;
	extern uint32_t slave_nodeid;
  Message m;
  memset(&m, 0, sizeof(m));
  m.len = 8;
  m.rtr = CAN_RTR_DATA;
  //m.cob_id = 0x5C0;
	//m.cob_id = id1;
  m.cob_id = master_nodeid;
  m.data[0] = 0x60;
  m.data[1] = 0x0F;
  m.data[2] = 0x12;
  m.data[3] = 0x03;


  /* Waiting for user input */
  while (1)
  {
		
  	Can_data_Process();
	report_node_info();
    if (enter_bootloader_flag == 0xff) 
	{
		*flag = 1;
		break;	
	}
	if(TimingDelay1 >= 100000)//1s
	{	
	   	TimingDelay1 = 0;
	   	memcpy(&m.data[4], &cnt, sizeof(cnt));
	   	if(CAN_SEND_OK != Can_Send(NULL, &m))
	   	{
	   		//Error_Handler();
	   	}
	   
	   	cnt--;
	   	printf("%d s\n", cnt);
	   	if(cnt == 0)
	   	{
	     	/*m.data[1] = 0x00;
	   		m.data[2] = 0x01;
	   		m.data[3] = 0x09;
	   		if(CAN_SEND_OK != Can_Send(NULL, &m))
	   		{
	   			Error_Handler();
	   		}*/
	   		//printf("enter app programme\n");
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
  extern UART_HandleTypeDef huart3;
  HAL_StatusTypeDef result;

  result = HAL_UART_Transmit(&huart3, &c, sizeof(c), 1000);
  
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
