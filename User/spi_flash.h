#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


#include "stm32f4xx.h"
#include "main.h"
#include <stdio.h>

//#define SPI_FLASH_PageSize            4096
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/
#define SST25_WriteEnable		      0x06 
#define SST25_WriteDisable		      0x04 
#define SST25_ReadStatusReg		    0x05
#define SST25_WriteStatusRegEnable	0x50
#define SST25_WriteStatusReg		  0x01 
#define SST25_FastReadData			 0x0B
#define SST25_ReadData				0x03
#define SST25_ByteProgram		      0x02 
#define SST25_MulByteProgram		  0xAD
#define SST25_ChipErase			      0x60
#define SST25_4KSectorErase			0x20
#define SST25_32KBlockErase			0x52
#define SST25_64KBlockErase			0xD8
#define SST25_DeviceID			        0x90 
#define SST25_JedecDeviceID		    0x9F 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte                0xFF

#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 
#define SPIx_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE() 

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()


#define SPIx_SCK_PIN                     GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT               GPIOA
#define SPIx_SCK_AF                      GPIO_AF5_SPI1

#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT              GPIOA
#define SPIx_MISO_AF                     GPIO_AF5_SPI1

#define SPIx_MOSI_PIN                    GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT              GPIOA
#define SPIx_MOSI_AF                     GPIO_AF5_SPI1

#define FLASH_CS_PIN                     GPIO_PIN_4               
#define FLASH_CS_GPIO_PORT               GPIOA                     
	
//#define	digitalHi(p,i)			    {p->BSRR=i;}		

//#define digitalLo(p,i)			    {p->BSRR=(uint32_t)i << 16;}				
#define SPI_FLASH_CS_LOW()      digitalLo(FLASH_CS_GPIO_PORT,FLASH_CS_PIN )
#define SPI_FLASH_CS_HIGH()     digitalHi(FLASH_CS_GPIO_PORT,FLASH_CS_PIN )



#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))


#define FLASH_DEBUG_ON         1

#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)
#if 0

void SPI_FLASH_Init(void);
void SPI_FLASH_ChipErase(void);
uint16_t SPI_FLASH_MulByteWrite(uint32_t WriteAddr, uint8_t *buff, uint32_t cnt);
void SPI_FLASH_ByteWrite(uint32_t WriteAddr, uint8_t dat);
void SPI_FLASH_SectorErase_4KByte(uint32_t SectorAddr);
void SPI_FLASH_BlockErase_32KByte(uint32_t Addr);
void SPI_FLASH_BlockErase_64KByte(uint32_t Addr);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadDeviceID(void);
//uint32_t SPI_FLASH_ReadID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);


uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

void flash_test_function(void);

#else
#define SPI_WAIT (128)
// byte progam wait
#define SPI_FLASH_BPRG_WAIT (1)

// cs 
#define SPI_FLASH_CS_PORT (GPIOA)
#define SPI_FLASH_CS_PIN (GPIO_PIN_4)
#define SPI_FLASH_CS_ENA() HAL_GPIO_WritePin(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN, GPIO_PIN_RESET)
#define SPI_FLASH_CS_DIS() HAL_GPIO_WritePin(SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN, GPIO_PIN_SET)

// OP code
#define SPI_FLASH_OP_RD  (0x03)
#define SPI_FLASH_OP_SE (0x20)
#define SPI_FLASH_OP_BE (0x52)
#define SPI_FLASH_OP_BPRG  (0x02)
#define SPI_FLASH_OP_AAIWP (0xad)
#define SPI_FLASH_OP_CE  (0x60)
#define SPI_FLASH_OP_RDSR  (0x05)
#define SPI_FLASH_OP_EWSR  (0x50)
#define SPI_FLASH_OP_WRSR  (0x01)
#define SPI_FLASH_OP_WREN  (0x06)
#define SPI_FLASH_OP_WRDI  (0x04)
#define SPI_FLASH_OP_RDID  (0x90)

// SR bit
#define SPI_FLASH_SR_BUSY (0x01)
#define SPI_FLASH_RDID_RETRY (64)

#define SPI_FLASH_PAGE_SIZE (0x1000U)
#define SPI_FLASH_SIZE (0x00200000U)
#define SPI_FLASH_PAGE_NUM (512)

#define W25Q
#ifdef W25Q
#define SPI_FLASH_W25_WRTMAXSIZE	(256)
#define SPI_FLASH_MFG_ID	0xef14
#define SPI_FLASH_PAGE_ERASE_WAIT	(400)
#define SPI_FLASH_CHIP_ERASE_WAIT	(25000)
#else
#define SPI_FLASH_MFG_ID 0xbf41
#define SPI_FLASH_PAGE_ERASE_WAIT (25) 
#define SPI_FLASH_CHIP_ERASE_WAIT (100)
#endif

void spi_flash_init(void);
void spi_flash_read(uint32_t addr, uint8_t *buf, uint16_t len);
uint8_t spi_flash_write(uint32_t addr, uint8_t *buf, uint16_t len);
uint8_t spi_flash_erase_page(uint32_t addr, uint16_t num, uint32_t wait);
uint8_t spi_flash_erase_chip(uint32_t wait);
uint8_t spi_flash_ready(void);
uint8_t spi_flash_wait(uint32_t ms);
uint8_t spi_flash_rdsr(void);
uint16_t spi_flash_rdid(void);
void spi_flash_wren(void);
void spi_flash_wrdi(void);
void flash_test_function(void);
void spi_flash_config(void);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __SPI_FLASH_H */


