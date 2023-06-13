#include "spi_flash.h"

SPI_HandleTypeDef SpiHandle;

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/**
  * @brief SPI MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct = {0};
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
  SPIx_CS_GPIO_CLK_ENABLE();
  /* Enable SPI clock */
  SPIx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* SPI SCK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = SPIx_SCK_AF;
  
  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
    
  /* SPI MISO GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_MISO_AF;
  
  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
  
  /* SPI MOSI GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = SPIx_MOSI_AF;  
  HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);   

  GPIO_InitStruct.Pin = FLASH_CS_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(FLASH_CS_GPIO_PORT, &GPIO_InitStruct); 
}

void SPI_FLASH_Init(void)
{
   /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  
  SpiHandle.Init.Mode = SPI_MODE_MASTER;

  if(HAL_OK != HAL_SPI_Init(&SpiHandle))
  {
	Error_Handler();
  }
  
  __HAL_SPI_ENABLE(&SpiHandle);     
}

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

#if 0
void SPI_FLASH_SectorErase_4KByte(uint32_t SectorAddr)
{
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();

  SPI_FLASH_CS_LOW();

  SPI_FLASH_SendByte(SST25_4KSectorErase);

  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);

  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);

  SPI_FLASH_SendByte(SectorAddr & 0xFF);

  SPI_FLASH_CS_HIGH();

  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_BlockErase_32KByte(uint32_t Addr)
{
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();

  SPI_FLASH_CS_LOW();

  SPI_FLASH_SendByte(SST25_32KBlockErase);

  SPI_FLASH_SendByte((Addr & 0xFF0000) >> 16);

  SPI_FLASH_SendByte((Addr & 0xFF00) >> 8);

  SPI_FLASH_SendByte((Addr & 0xFF));

  SPI_FLASH_CS_HIGH();

  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_BlockErase_64KByte(uint32_t Addr)
{
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();

  SPI_FLASH_CS_LOW();

  SPI_FLASH_SendByte(SST25_64KBlockErase);

  SPI_FLASH_SendByte((Addr & 0xFF0000) >> 16);

  SPI_FLASH_SendByte((Addr & 0xFF00) >> 8);

  SPI_FLASH_SendByte((Addr & 0xFF));

  SPI_FLASH_CS_HIGH();

  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_ChipErase(void)
{
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();

  SPI_FLASH_CS_LOW();

  SPI_FLASH_SendByte(SST25_ChipErase);

  SPI_FLASH_CS_HIGH();

  SPI_FLASH_WaitForWriteEnd();	
}

void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{

  uint8_t cmd[4];
  
  cmd[0] = SST25_ReadData;
  cmd[1] = ReadAddr >> 16;
  cmd[2] = ReadAddr >> 8;
  cmd[3] = ReadAddr & 0xff;
  SPI_FLASH_CS_LOW();  
  spi_trx(4, cmd, 0);
  spi_trx(NumByteToRead, 0, pBuffer);

  
  SPI_FLASH_CS_HIGH();
}


void SPI_FLASH_ByteWrite(uint32_t WriteAddr, uint8_t dat)
{
  SPI_FLASH_WriteEnable();

  SPI_FLASH_CS_LOW();

  SPI_FLASH_SendByte(SST25_ByteProgram);

  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);

  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  SPI_FLASH_CS_HIGH();

  SPI_FLASH_WaitForWriteEnd();	
}

void SPI_FLASH_Write_State_Register(uint8_t dat)
{
	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(SST25_WriteStatusRegEnable);
	SPI_FLASH_CS_HIGH();

	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(SST25_WriteStatusReg);
	SPI_FLASH_SendByte(dat);
	SPI_FLASH_CS_HIGH();
	
	SPI_FLASH_WaitForWriteEnd();
}


uint16_t SPI_FLASH_MulByteWrite(uint32_t WriteAddr, uint8_t *buff, uint32_t cnt)
{
	uint32_t i = 0;
	SPI_FLASH_WriteEnable();
	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(SST25_MulByteProgram);
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);

	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);

	SPI_FLASH_SendByte(WriteAddr & 0xFF);

	SPI_FLASH_SendByte(buff[0]);

	SPI_FLASH_SendByte(buff[1]);

	SPI_FLASH_CS_HIGH();

	i = 2;
	while(i < cnt)
	{
		SPITimeout = SPIT_FLAG_TIMEOUT;
			while (__HAL_SPI_GET_FLAG( &SpiHandle, SPI_FLAG_TXE ) == RESET)
			{
				if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
			}

		SPI_FLASH_CS_LOW();
		SPI_FLASH_SendByte(SST25_MulByteProgram);
		SPI_FLASH_SendByte(buff[i++]);
		SPI_FLASH_SendByte(buff[i++]);
		SPI_FLASH_CS_HIGH();
	}

	SPITimeout = SPIT_FLAG_TIMEOUT;
	while (__HAL_SPI_GET_FLAG( &SpiHandle, SPI_FLAG_TXE ) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
	}

	SPI_FLASH_WriteEnable();

	SPITimeout = SPIT_FLAG_TIMEOUT;
	while (__HAL_SPI_GET_FLAG( &SpiHandle, SPI_FLAG_TXE ) == RESET)
	{
	if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
	}

	SPI_FLASH_WaitForWriteEnd();

}

uint32_t SPI_FLASH_ReadDeviceID(void)
{
	uint8_t cmd[2];

	SPI_FLASH_CS_HIGH();
	
	cmd[0] = SST25_DeviceID;
	cmd[1] = 0; 
	spi_trx(2, cmd, 0);
	cmd[0] = 0; cmd[1] = 0;
	spi_trx(2, cmd, 0);
	spi_trx(2, 0, cmd);

	return ((uint16_t)cmd[0] << 8) | cmd[1];
}
/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(SST25_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

uint8_t SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  SPITimeout = SPIT_FLAG_TIMEOUT;

  while (__HAL_SPI_GET_FLAG( &SpiHandle, SPI_FLAG_TXE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
   }

  WRITE_REG(SpiHandle.Instance->DR, byte);

  SPITimeout = SPIT_FLAG_TIMEOUT;

  while (__HAL_SPI_GET_FLAG( &SpiHandle, SPI_FLAG_RXNE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
   }

  return READ_REG(SpiHandle.Instance->DR);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord)
{
  
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* Loop while DR register in not emplty */
  while (__HAL_SPI_GET_FLAG( &SpiHandle,  SPI_FLAG_TXE ) == RESET)
  {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(2);
   }

  /* Send Half Word through the SPIx peripheral */
  WRITE_REG(SpiHandle.Instance->DR, HalfWord);

  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* Wait to receive a Half Word */
  while (__HAL_SPI_GET_FLAG( &SpiHandle, SPI_FLAG_RXNE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(3);
   }
  /* Return the Half Word read from the SPI bus */
  return READ_REG(SpiHandle.Instance->DR);
}

void SPI_FLASH_WriteEnable(void)
{
  SPI_FLASH_CS_LOW();

  SPI_FLASH_SendByte(SST25_WriteEnable);

  SPI_FLASH_CS_HIGH();
}

void SPI_FLASH_WriteDisable(void)
{
	SPI_FLASH_CS_LOW();

	SPI_FLASH_SendByte(SST25_WriteDisable);

	SPI_FLASH_CS_HIGH();
}

void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;

  SPI_FLASH_CS_LOW();

  SPI_FLASH_SendByte(SST25_ReadStatusReg);

  SPITimeout = SPIT_FLAG_TIMEOUT;
  do
  {
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 
    {
      if((SPITimeout--) == 0) 
      {
        SPI_TIMEOUT_UserCallback(4);
        return;
      }
    } 
  }
  while ((FLASH_Status & WIP_Flag) == SET);

  SPI_FLASH_CS_HIGH();
}

static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  FLASH_ERROR("SPI errorCode = %d",errorCode);
  return 0;
}

uint8_t buffer_out[50] = {0};
void flash_test_function(void)
{
	uint8_t buffer[30] = "hello world!";
	SPI_FLASH_ChipErase();
	SPI_FLASH_MulByteWrite(0, buffer, sizeof(buffer));

	SPI_FLASH_BufferRead(buffer_out, 0, 50);
}
   
/*********************************************END OF FILE**********************/
#else 
void fatal(char *p)
{
	printf("%s\r\n",p);

//  for(;;){
//    NVIC_SystemReset();
//  }
}

// init
void spi_flash_init(void)
{
  uint16_t i = 0;

  SPI_FLASH_CS_DIS();
  // wait until flash readable
  while((spi_flash_rdid()) != SPI_FLASH_MFG_ID){
    // try recover from wrong chip/bus state 
    spi_flash_wren();
    spi_flash_wrdi();
    //reload_iwdg();
    // reset system if retry fail
    if(i++ >= SPI_FLASH_RDID_RETRY){
      fatal("spi flash rdid fail");
    }
  }
}

/**
* read		Read data at the specified address.
*	@arg		addr	The flash address where you want to read data
*	@arg		*buf	Storage buf to read data 
*	@arg		len		Data length
*/
void spi_flash_read(uint32_t addr, uint8_t *buf, uint16_t len)
{
  uint8_t cmd[4];
  
  cmd[0] = SPI_FLASH_OP_RD;
  cmd[1] = addr >> 16;
  cmd[2] = addr >> 8;
  cmd[3] = addr & 0xff;
  
  SPI_FLASH_CS_ENA();
  spi_trx(4, cmd, 0);
  spi_trx(len, 0, buf);
  SPI_FLASH_CS_DIS();
}

// byte prog
uint8_t spi_flash_bprg(uint32_t addr, uint8_t data)
{
  uint8_t cmd[5];  
  uint8_t r = 1;
  
  // wr enble
  spi_flash_wren();
  // byte prog
  cmd[0] = SPI_FLASH_OP_BPRG;
  cmd[1] = addr >> 16;
  cmd[2] = addr >> 8;
  cmd[3] = addr & 0xff;
  cmd[4] = data;

  SPI_FLASH_CS_ENA();  
  spi_trx(5, cmd, 0);
  SPI_FLASH_CS_DIS();
  // wait until ready
	r=spi_flash_wait(SPI_FLASH_BPRG_WAIT);
  if(!(r)){
    goto L_EXIT;
  }
  
L_EXIT:  
  // wr disable
  spi_flash_wrdi();
  
  return r;
}

/**
* write		Before writing data, ensure that there is no data in the current flash address.
*					If there is data in the current flash address, use function spi_flash_erase_page or
*					spi_flash_erase_chip first.
*	@arg		addr	The flash address where you want to write data
*	@arg		*buf	Data to write
*	@arg		len	Data length
*/
uint8_t spi_flash_write(uint32_t addr, uint8_t *buf, uint16_t len)
{
  uint8_t cmd[6];
  uint8_t r = 1;

  if(len <= 0) return r;

#ifdef W25Q
  uint16_t pageLen = SPI_FLASH_W25_WRTMAXSIZE;
  cmd[0] = SPI_FLASH_OP_BPRG;
  pageLen = SPI_FLASH_W25_WRTMAXSIZE - (addr % SPI_FLASH_W25_WRTMAXSIZE);
  while(len > pageLen)
  {
		spi_flash_wren();
		cmd[1] = addr >> 16;
		cmd[2] = addr >> 8;
		cmd[3] = addr & 0xff;
		SPI_FLASH_CS_ENA();
		spi_trx(4, cmd, 0);
		spi_trx(pageLen, buf, 0);
		SPI_FLASH_CS_DIS();
		r = spi_flash_wait(50);
		if(!(r))
		{
			goto L_EXIT;
		}
		spi_flash_wrdi();
		len -= pageLen;
		addr += pageLen;
		buf += pageLen;
		pageLen = SPI_FLASH_W25_WRTMAXSIZE;
  }
	spi_flash_wren();
  	cmd[1] = addr >> 16;
	cmd[2] = addr >> 8;
	cmd[3] = addr & 0xff;
	SPI_FLASH_CS_ENA();
	spi_trx(4, cmd, 0);
	spi_trx(len, buf, 0);
	SPI_FLASH_CS_DIS();
	r = spi_flash_wait(50);
	if(!(r))
	{
		goto L_EXIT;
	}
#else
  // leading byte
  if(addr & 0x01){
		r = spi_flash_bprg(addr, buf[0]);
    if(!(r)){
      return r;
    }
    addr++;
    buf++;
    len--;
  }
  // tail byte
  if(len & 0x01){
    len--;
		r = spi_flash_bprg(addr+len, buf[len]);
    if(!(r)){
      return r;
    }
  }
  
  // wr enble
  spi_flash_wren();

  // aai program
  cmd[0] = SPI_FLASH_OP_AAIWP;
  cmd[1] = addr >> 16;
  cmd[2] = addr >> 8;
  cmd[3] = addr & 0xff;
  cmd[4] = buf[0];
  cmd[5] = buf[1];
  SPI_FLASH_CS_ENA();  
  spi_trx(6, cmd, 0);
  SPI_FLASH_CS_DIS();
  // wait until ready
	r=spi_flash_wait(SPI_FLASH_BPRG_WAIT);
  if(!(r)){
    goto L_EXIT;
  }
  
  // progam bytes
  for(uint16_t i = 2; i < len; i+=2){
    cmd[0] = SPI_FLASH_OP_AAIWP;
    cmd[1] = buf[i];
    cmd[2] = buf[i+1];
    SPI_FLASH_CS_ENA();  
    spi_trx(3, cmd, 0);
    SPI_FLASH_CS_DIS();
    // wait until ready
		r=spi_flash_wait(SPI_FLASH_BPRG_WAIT);
    if(!(r)){
      break;
    }
  }
#endif

L_EXIT:  
	// wr disable
	spi_flash_wrdi();
  
  return r;
}

// erase page
/**
* erase page	The data page of the specified address will be erased.
*							page begin address base is 0x1000U;
*							1 page data size is 0x1000U;
*	@arg		addr	The flash address where you want to write data
*	@arg		*buf	Data to write
*	@arg		len	Data length
*/
uint8_t spi_flash_erase_page(uint32_t addr, uint16_t num, uint32_t wait)
{
  uint32_t i, j;
  uint8_t cmd[4];
  uint8_t r = 0;

  // erase page
  for(i = 0, j = addr; i < num; i++, j += SPI_FLASH_PAGE_SIZE){
    // wr enable
    spi_flash_wren();
    // erage page    
    cmd[0] = SPI_FLASH_OP_SE;
    cmd[1] = j >> 16;
    cmd[2] = j >>  8;
    cmd[3] = j;
    SPI_FLASH_CS_ENA();  
    spi_trx(4, cmd, 0);
    SPI_FLASH_CS_DIS();
    // wait until ready
		r=spi_flash_wait(wait);
    if(!(r)){
      break;
    }
    //reload_iwdg();
  }
  // wr disable
  spi_flash_wrdi();

  return r;
}


/**
* erase chip	The all data of flash will be erased.
*	@arg		addr	The flash address where you want to write data
*	@arg		*buf	Data to write
*	@arg		len	Data length
*/
uint8_t spi_flash_erase_chip(uint32_t wait)
{
  uint8_t cmd[1];

  // wr enable
  spi_flash_wren();
  
  // chip erase  
  cmd[0] = SPI_FLASH_OP_CE;
  SPI_FLASH_CS_ENA();  
  spi_trx(1, cmd, 0);
  SPI_FLASH_CS_DIS();

  // wr disable
  spi_flash_wrdi();
  
  return spi_flash_wait(wait);
}

// check ready
uint8_t spi_flash_ready(void)
{
  if(spi_flash_rdsr() & SPI_FLASH_SR_BUSY){
    return 0;
  }else{
    return 1;
  }
}

// wait ready
uint8_t spi_flash_wait(uint32_t t)
{
	uint32_t ticks = (SystemCoreClock / 1000UL) * t;
  uint32_t lapse, now, last = SysTick->VAL;

  while (1) {
    now = SysTick->VAL;
    if(spi_flash_ready() == 1) return 1;
    if(now <= last) {
      lapse = last - now; 
    } else {
      lapse = last + SysTick->LOAD - now;
    }
    if (ticks <= lapse) 
      break;
    ticks -= lapse;
    last = now;
  }

  return 0;
}

// read sr
uint8_t spi_flash_rdsr(void)
{
  uint8_t cmd[1];
  
  cmd[0] = SPI_FLASH_OP_RDSR;
  SPI_FLASH_CS_ENA();  
  spi_trx(1, cmd, 0);
  spi_trx(1, 0, cmd);
  SPI_FLASH_CS_DIS();
  
  return cmd[0];
}

// read did
uint16_t spi_flash_rdid(void)
{
  uint8_t cmd[2];
  
  cmd[0] = SPI_FLASH_OP_RDID;
  cmd[1] = 0;
  SPI_FLASH_CS_ENA();  
  spi_trx(2, cmd, 0);
  cmd[0] = 0; cmd[1] = 0;
  spi_trx(2, cmd, 0);
  spi_trx(2, 0, cmd);
  SPI_FLASH_CS_DIS();

	//printf("%02X%02X",cmd[0],cmd[1]);
  
  return ((uint16_t)cmd[0]<<8)|cmd[1];
}

// write ena
void spi_flash_wren(void)
{
  uint8_t cmd[2];

  // enable SR
  cmd[0] = SPI_FLASH_OP_EWSR;
  SPI_FLASH_CS_ENA();  
  spi_trx(1, cmd, 0);
  SPI_FLASH_CS_DIS();
  
  // clear SR
  cmd[0] = SPI_FLASH_OP_WRSR;
  cmd[1] = 0x00;
  SPI_FLASH_CS_ENA();  
  spi_trx(2, cmd, 0);
  SPI_FLASH_CS_DIS();

  // enable write
  cmd[0] = SPI_FLASH_OP_WREN;
  SPI_FLASH_CS_ENA();  
  spi_trx(1, cmd, 0);
  SPI_FLASH_CS_DIS();
}

// write dis
void spi_flash_wrdi(void)
{
  uint8_t cmd[1];
  
  // wr disable
  cmd[0] = SPI_FLASH_OP_WRDI;
  SPI_FLASH_CS_ENA();  
  spi_trx(1, cmd, 0);
  SPI_FLASH_CS_DIS();
}

uint8_t buffer_out[50] = {0};
void flash_test_function(void)
{
	uint8_t buffer[30] = "0123456789";
	spi_flash_erase_chip(SPI_WAIT);
	
	spi_flash_write(0, buffer, sizeof(buffer));

	//SPI_FLASH_BufferRead(buffer_out, 0, 50);
	
	spi_flash_read(0, buffer_out, 50);
	
	printf("buffer_out %s\n", buffer_out);
}

void spi_flash_config(void)
{
	SPI_FLASH_Init();
	spi_flash_init();
}

#endif
