#ifndef FLASH_UART_TASK_H
#define FLASH_UART_TASK_H

#include "W25Q128.h"
#include <stdio.h>
#include "stm32f3xx_hal.h"
#include "cmsis_os.h"

#define ADDRESS     (uint32_t)0x000100

// Function to initialize the UART task
void CreateFlashTask(void);

// Function to handle UART task
void Flash_Task(void const *argument);

// Function to read JEDEC ID from flash
void Read_JEDEC_ID(uint8_t *buf, uint32_t size);

//Delete data in register
void Delete_Data(uint32_t address);

//Save data to flash
void Save_Flash(uint32_t address, uint8_t *buf, uint32_t size);

//Read data from flash
void Read_Data(uint32_t address, uint32_t size);

void Arr_hex_view(uint8_t *result, int Length);
#endif /* FLASH_UART_TASK_H */
