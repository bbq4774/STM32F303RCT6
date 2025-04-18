#include "w25q128.h"

// Return 3 byte JEDEC Manufacturer and device ID (requires 3 byte buffer)
// Winbond 8.2.29 Read JEDEC ID (9Fh)
int W25_ReadJedecID(uint8_t *buf, int bufSize) {
  int retval;
  uint8_t idcmd = W25_CMD_READ_JEDEC_ID;
  if(bufSize < W25_JEDEC_ID_BUF_SIZE)
    return HAL_ERROR; // buffer too small
  
  W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
  retval = HAL_SPI_Transmit(&hspi1, &idcmd , sizeof(idcmd ), TIMEOUT); // Send the ID command
  if(retval == HAL_OK)
    retval = HAL_SPI_Receive(&hspi1, buf, W25_JEDEC_ID_BUF_SIZE, TIMEOUT);
  W25_CS_DISABLE();
  
  return retval;
} // W25_ReadJEDECID()

// Return 8 byte Unique ID Number (requires 8 byte return buffer)
// Winbond 8.2.28 Read Unique ID Number (4bh)
int W25_ReadUniqueID(uint8_t *buf, int bufSize) {
  int retval;
  uint8_t cmddata[5] = {W25_CMD_READ_UNIQUE_ID,0,0,0,0};
  if(bufSize < W25_UNIQUE_ID_BUF_SIZE)
    return HAL_ERROR; // buffer too small
  
  W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
  retval = HAL_SPI_Transmit(&hspi1, cmddata , sizeof(cmddata ), TIMEOUT); // Send the ID command
  if(retval == HAL_OK)
    retval = HAL_SPI_Receive(&hspi1, buf, W25_UNIQUE_ID_BUF_SIZE, TIMEOUT);
  W25_CS_DISABLE();
  
  return retval;
} // W25_ReadUniqueID()

// Returns value of Status Register-1 (byte)
// Winbond 8.2.4 Read Status Register-1 (05h)
// See section 7.1 for bit values
uint8_t W25_ReadStatusReg1(void) {
  uint8_t cmd = W25_CMD_READ_STATUS_REG_1;
  uint8_t status_reg1;
  int retval;
    
  W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
  retval = HAL_SPI_Transmit(&hspi1, &cmd , sizeof(cmd), TIMEOUT); // Send Read Status Reg 1 command
  if(retval == HAL_OK)
    retval = HAL_SPI_Receive(&hspi1, &status_reg1, sizeof(status_reg1), TIMEOUT);
  W25_CS_DISABLE();
  
  return retval == HAL_OK ? status_reg1:0xFF; // return 0xFF if error
} // W25_ReadStatusReg1()

// Send Write Enable command
// Winbond 8.2.1 Write Enable (06h)
// See section 7.1, page 17, and section 8.2.1, page 30
// This sets the WEL bit, S1, in status register 1, allowing the part to be written.
int W25_WriteEnable(void) {
  uint8_t cmd = W25_CMD_WRITE_ENABLE;
    
  W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
  int retval = HAL_SPI_Transmit(&hspi1, &cmd , sizeof(cmd), TIMEOUT); // Send Write Enable command
  W25_CS_DISABLE();
    
  return retval;
} // W25_WriteEnable()

// Send Write Disable command
// Winbond 8.2.1 Write Disable (04h)
// See section 7.1, page 17, and section 8.2.1, page 30
// This clears the WEL bit, S1, in status register 1, preventing writing to the part
int W25_WriteDisable(void) {
  uint8_t cmd = W25_CMD_WRITE_DISABLE;
    
  W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
  int retval = HAL_SPI_Transmit(&hspi1, &cmd , sizeof(cmd), TIMEOUT); // Send Write Disable command
  W25_CS_DISABLE();
    
  return retval;
} // W25_WriteDisable()

// Winbond 8.2.6 Read Data (03h)
// The only limit for quantity of data is memory / device size
int W25_ReadData(uint32_t address, uint8_t *buf, int bufSize) {
  int retval;
  uint8_t cmdaddr[4] = {W25_CMD_READ_DATA,address>>16,address>>8,address};
  
  W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
  retval = HAL_SPI_Transmit(&hspi1, cmdaddr , sizeof(cmdaddr), 500); // Send Read Data command with address
  if(retval != HAL_OK) {
    return retval;
  }  
  retval = HAL_SPI_Receive(&hspi1, buf, bufSize, 2000); // need longer time-outs when using slow SPI clock
  W25_CS_DISABLE();
  
  return retval;
} // W25_ReadData()

// Winbond 8.2.15 Page Program (02h)
// Initially, this function could only support a page or partial page write.
// Updated to perform multiple writes - accross pages as required
// LittleFS is unaware of the page boundary issue.  Manage the issue with multiple writes
int W25_PageProgram(uint32_t address, uint8_t *buf, uint32_t count) {
  int retval;
  W25_WriteEnable(); // Make sure we can write...
  while(count) {
    uint8_t cmdaddr[4] = {W25_CMD_PAGE_PROGRAM,address>>16,address>>8,address};
    uint32_t space_left_in_page = 0x100 - (address & 0xFF);
    uint32_t count_this_pass = count <= space_left_in_page? count:space_left_in_page;
    
    W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
    retval = HAL_SPI_Transmit(&hspi1, cmdaddr , sizeof(cmdaddr ), TIMEOUT); // Send Page Program command with address
    if(retval == HAL_OK)
      retval = HAL_SPI_Transmit(&hspi1, buf, count_this_pass, TIMEOUT); // Write page data
    W25_CS_DISABLE();
    
    count -= count_this_pass;
    address += count_this_pass;
    buf += count_this_pass;
    W25_DelayWhileBusy(PAGE_PROGRAM_TIMEOUT);
  }
  return retval;
} // W25_PageProgram()

// Winbond 8.2.17 Sector Erase (20h)
// Erase all data within the addressed 4K sector.
int W25_SectorErase(uint32_t address) {
  int retval;
  uint8_t cmdaddr[4] = {W25_CMD_SECTOR_ERASE,address>>16,address>>8,address};
  
  W25_WriteEnable(); // Make sure we can write...
  W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
  retval = HAL_SPI_Transmit(&hspi1, cmdaddr , sizeof(cmdaddr ), TIMEOUT); // Send Sector Erase command with address
  W25_CS_DISABLE();
  
  W25_DelayWhileBusy(SECTOR_ERASE_TIMEOUT);
  return retval;
} // W25_SectorErase()

// Winbond 8.2.20 Chip Erase (60h)
// Erase all data within the FLASH device
int W25_ChipErase(void) {
  int retval;
  uint8_t cmd = {W25_CMD_CHIP_ERASE};
  
  W25_WriteEnable(); // Make sure we can write...
  W25_CS_ENABLE(); // Drive Winbond chip select, /CS low
  retval = HAL_SPI_Transmit(&hspi1, &cmd , sizeof(cmd ), TIMEOUT); // Send Chip Erase command
  W25_CS_DISABLE();
  
  W25_DelayWhileBusy(CHIP_ERASE_TIMEOUT);
  return retval;
} // W25_ChipErase()

// Returns 0:Not busy, or 1:Busy
int W25_Busy(void) {
  return (W25_ReadStatusReg1() & W25_STATUS1_BUSY);
}

// Loop while busy and not timeout
int W25_DelayWhileBusy(uint32_t msTimeout) {
  uint32_t initial_count = HAL_GetTick();
  int busy;
  uint32_t deltaticks;
  uint32_t count = 0;
  do {
    busy = W25_Busy();
    deltaticks = HAL_GetTick() - initial_count;
    count++;
  } while(busy && deltaticks < msTimeout);
  int retval = busy ? HAL_TIMEOUT:HAL_OK;
  return retval;
}
