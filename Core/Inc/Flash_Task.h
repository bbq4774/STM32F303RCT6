#ifndef FLASH_UART_TASK_H
#define FLASH_UART_TASK_H

#include "W25Q128.h"
#include <stdio.h>
#include "stm32f3xx_hal.h"
#include "cmsis_os.h"

// Macros
#define ADDRESS     (uint32_t)0x000111

// Function Declarations
void CreateFlashTask(void);
void Flash_Task(void const *argument);
void Read_JEDEC_ID(UART_HandleTypeDef *huart, uint8_t *buf, uint32_t size);
void Delete_Data(UART_HandleTypeDef *huart, uint32_t address);
void Save_Flash(UART_HandleTypeDef *huart, uint32_t address, uint8_t *buf, uint32_t size);
void Read_Data(UART_HandleTypeDef *huart, uint32_t address, uint32_t size);
void Arr_hex_view(UART_HandleTypeDef *huart, uint8_t *result, int Length);

#endif /* FLASH_UART_TASK_H */
