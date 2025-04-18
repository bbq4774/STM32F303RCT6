#ifndef UART_TASK_H
#define UART_TASK_H

#include "stm32f3xx_hal.h"
#include "cmsis_os.h"

// Define buffer sizes
#define RX_BUFFER_SIZE  1000
#define TX_BUFFER_SIZE  500
#define TIME_OUT        1000

// Function to initialize the UART task
void CreateUARTTask(void);

//Check for new data
int Check_Have_Data(uint32_t *lastRxTime, uint32_t *currentTime, uint16_t *prev_received_size);

// Function to handle UART task
void UART_Task(void const *argument);

//Process data
void Process_Data(uint16_t rx_size);

#endif /* UART_TASK_H */
