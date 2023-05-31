/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

#if 0

/* USER CODE BEGIN 0 */
#define SPI_WAIT (128)

/* USER CODE END 0 */

SPI_HandleTypeDef SpiHandle;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  SpiHandle.Instance = SPI1;
  SpiHandle.Init.Mode = SPI_MODE_MASTER;
  SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
  SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
  SpiHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
  SpiHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
  SpiHandle.Init.NSS = SPI_NSS_SOFT;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
  SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
// trx
uint8_t spi_trx(uint16_t len, uint8_t *w, uint8_t *r)
{
  HAL_StatusTypeDef s = HAL_OK;

  if(w){
    if(r){
      s = HAL_SPI_TransmitReceive(&SpiHandle, w, r, len, SPI_WAIT);    
    }else{
      s = HAL_SPI_Transmit(&SpiHandle, w, len, SPI_WAIT);
    }
  }else{
    if(r){
      s = HAL_SPI_Receive(&SpiHandle, r, len, SPI_WAIT);
    }else{
      s = HAL_OK;  
    }
  }
  return s == HAL_OK ? 1: 0;
}
/* USER CODE END 1 */
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
