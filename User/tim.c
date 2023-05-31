/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "stdio.h"
#include "applicfg.h"

	/* USER CODE BEGIN 0 */
#define PWM_SIZE						(4000) //1000/4000=25%,
#define PWM_LOCATION_BASE		(1000.0f) //1000/4000=25%,
	uint32_t capture_data1[2] = {0,0};
	uint32_t capture_data2[2] = {0,0};
	float freq_capture[2] = {0.0f, 0.0f};
	float location[2] = {0.0f,0.0f};
	/* USER CODE END 0 */

	TIM_HandleTypeDef htim2;
	TIM_HandleTypeDef htim4;

	/* TIM3 init function */
	/* TIM3 init function */
	void MX_TIM2_Init(void)
	{

	  /* USER CODE BEGIN TIM2_Init 0 */

	  /* USER CODE END TIM2_Init 0 */

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};
	  TIM_IC_InitTypeDef sConfigIC = {0};

	  /* USER CODE BEGIN TIM3_Init 1 */

	  /* USER CODE END TIM3_Init 1 */
	  htim2.Instance = TIM2;
	  htim2.Init.Prescaler = 16;
	  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim2.Init.Period = 0xffffffff;
	  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	  sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
	  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	  sSlaveConfig.TriggerFilter = 0;
	  if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	  sConfigIC.ICFilter = 0;
	  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /* USER CODE BEGIN TIM2_Init 2 */

	  /* USER CODE END TIM2_Init 2 */

	}

	/* TIM3 init function */
	/* TIM3 init function */
	void MX_TIM4_Init(void)
	{

	  /* USER CODE BEGIN TIM2_Init 0 */

	  /* USER CODE END TIM2_Init 0 */

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};
	  TIM_IC_InitTypeDef sConfigIC = {0};

	  /* USER CODE BEGIN TIM3_Init 1 */

	  /* USER CODE END TIM3_Init 1 */
	  htim4.Instance = TIM4;
	  htim4.Init.Prescaler = 16;
	  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim4.Init.Period = 0xffff;
	  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
	  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	  sSlaveConfig.TriggerFilter = 0;
	  if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	  sConfigIC.ICFilter = 0;
	  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
	  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /* USER CODE BEGIN TIM2_Init 2 */

	  /* USER CODE END TIM2_Init 2 */

	}


	void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
	{
		
		GPIO_InitTypeDef GPIO_InitStruct = {0};
	  if(tim_baseHandle->Instance==TIM2)
	  {
	  /* USER CODE BEGIN TIM2_MspInit 0 */

	  /* USER CODE END TIM2_MspInit 0 */
	    /* TIM2 clock enable */
	    __HAL_RCC_TIM2_CLK_ENABLE();

	    __HAL_RCC_GPIOB_CLK_ENABLE();
	    /**TIM3 GPIO Configuration
	    PB3     ------> TIM3_CH1
	    */
	    GPIO_InitStruct.Pin = GPIO_PIN_3;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    /* TIM3 interrupt Init */
	    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	    HAL_NVIC_EnableIRQ(TIM2_IRQn);
	  /* USER CODE BEGIN TIM3_MspInit 1 */

	  /* USER CODE END TIM3_MspInit 1 */
	  }
	  else if(tim_baseHandle->Instance == TIM4)
	  {
		/*	USER CODE BEGIN TIM4_MspInit 0*/
		
		/*	USER CODE END TIM4_MspInit 0 */
		/*	TIM4 clock enable */
		__HAL_RCC_TIM4_CLK_ENABLE();
		
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/*	TIM4 GPIO Configuration		
			PB6  --------------> TIM4_CH1
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		/* TIM4 interrupt Init*/
		HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM4_IRQn);
		/*	USER CODE BEGIN TIM4_MspInit 1*/
		
		/* USER CODE END TIM4_MspInit 1*/
	  }
	}

	void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
	{

	  if(tim_baseHandle->Instance==TIM2)
	  {
	  /* USER CODE BEGIN TIM3_MspDeInit 0 */

	  /* USER CODE END TIM3_MspDeInit 0 */
	    /* Peripheral clock disable */
	    __HAL_RCC_TIM2_CLK_DISABLE();

	    /**TIM3 GPIO Configuration
	    PB3     ------> TIM3_CH1
	    */
	    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);

	    /* TIM3 interrupt Deinit */
	    HAL_NVIC_DisableIRQ(TIM2_IRQn);
	  /* USER CODE BEGIN TIM3_MspDeInit 1 */

	  /* USER CODE END TIM3_MspDeInit 1 */
	  }
	  else if(tim_baseHandle->Instance == TIM4)
	  {
		/*	USER CODE BEGIN TIM4_MspDeInit 0*/
		
		/*	USER CODE END TIM4_MspInit 0*/
		/*	Peripheral clock disable*/
		__HAL_RCC_TIM4_CLK_DISABLE();
		
		/* TIM4 GPIO Configuration
			PB6  ---------->TIM4_CH1
		*/
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
		
		/* TIM4 interrupt Deinit */
		HAL_NVIC_DisableIRQ(TIM4_IRQn);
	  }
	}

	/* USER CODE BEGIN 1 */
	void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	extern float Double_Lvdt_Average_ConvertedValue[2];
	extern float Double_Lvdt_Transfer_Complete_Count[2];
	extern float Double_Lvdt_AVG[2];
	extern UNS32 lvdt1_current_location;
	extern UNS32 lvdt2_current_location;
		if(htim == &htim2){
			if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
				capture_data1[0] = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
				if(capture_data1[0] != 0){
					freq_capture[0] = 1000000.0f/ capture_data1[0];
					location[0] = capture_data2[0] * PWM_SIZE / capture_data1[0] - PWM_LOCATION_BASE;
					lvdt1_current_location = location[0];
					Double_Lvdt_Average_ConvertedValue[0] += location[0];
					if(Double_Lvdt_Transfer_Complete_Count[0] >= 20)
					{
						Double_Lvdt_AVG[0] = (uint32_t)Double_Lvdt_Average_ConvertedValue[0] / 21;
						Double_Lvdt_Average_ConvertedValue[0] = 0;
						Double_Lvdt_Transfer_Complete_Count[0] = 0;				
					}
					Double_Lvdt_Transfer_Complete_Count[0]++;				
				}
			}else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
				capture_data2[0] = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
			}
		}else if(htim == &htim4){
			if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
				capture_data1[1] = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_1);
				if(capture_data1[1] != 0){
					freq_capture[1] = 1000000.0f/ capture_data1[1];
					location[1] = capture_data2[1] * PWM_SIZE / capture_data1[1] - PWM_LOCATION_BASE;
					lvdt2_current_location = location[1];
					Double_Lvdt_Average_ConvertedValue[1] += location[1];
					if(Double_Lvdt_Transfer_Complete_Count[1] >= 20)
					{
						Double_Lvdt_AVG[1] = (uint32_t)Double_Lvdt_Average_ConvertedValue[1] / 21;
						Double_Lvdt_Average_ConvertedValue[1] = 0;
						Double_Lvdt_Transfer_Complete_Count[1] = 0;				
					}
					Double_Lvdt_Transfer_Complete_Count[1]++;			
				}
			}else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
				capture_data2[1] = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_2);
			}
		}

	}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/	

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
