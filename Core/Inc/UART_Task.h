#ifndef UART_TASK_H
#define UART_TASK_H

#include "stm32f3xx_hal.h"
#include "cmsis_os.h"

// Macros for defining buffer sizes and timeout values
#define RX_BUFFER_SIZE       1000
#define TX_BUFFER_SIZE       200
#define TIME_OUT             1000

// Function Declarations
void CreateUARTTask(void);
void UART_Task(void const *argument);
int Check_Have_Data(uint32_t *lastRxTime, uint32_t *currentTime, uint16_t *prev_received_size);
void Process_Data(UART_HandleTypeDef *huart, uint8_t *rx_buffer, uint16_t rx_size);

#endif /* UART_TASK_H */
