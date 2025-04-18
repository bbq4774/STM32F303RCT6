#include "uart_task.h"

// Buffer variables
uint8_t rx_buffer[RX_BUFFER_SIZE];

// Function to create the UART task
void CreateUARTTask(void) {
	osThreadDef(UART, UART_Task, osPriorityBelowNormal, 0, 128);
	osThreadId uartTaskHandle = osThreadCreate(osThread(UART), NULL);
	
	if (uartTaskHandle == NULL) {
		uint8_t err[] = "UART task failed!\r\n";
		extern UART_HandleTypeDef huart1;
		
		HAL_UART_Transmit(&huart1, err, sizeof(err) - 1, HAL_MAX_DELAY);
	}
}

// UART task function with Normal DMA and timeout
void UART_Task(void const *argument) {
	// Declare huart1 to be used in uart_task.c
	extern UART_HandleTypeDef huart1;
	
	//Count time and size variables
	uint32_t lastRxTime = 0;
	uint32_t currentTime = 0;
	uint16_t prev_received_size = 0;
	
	//Start DMA
	HAL_UART_Receive_DMA(&huart1, rx_buffer, RX_BUFFER_SIZE);
	
	while (1) {
		if (Check_Have_Data(&lastRxTime, &currentTime, &prev_received_size)) {
			Process_Data(prev_received_size);
			
			HAL_UART_DMAStop(&huart1);
			HAL_UART_Receive_DMA(&huart1, rx_buffer, RX_BUFFER_SIZE);
			
			prev_received_size = 0;
		}
		
		osDelay(10);
	}
}

int Check_Have_Data(uint32_t *lastRxTime, uint32_t *currentTime, uint16_t *prev_received_size) {
	extern UART_HandleTypeDef huart1;
    *currentTime = osKernelSysTick();
    uint16_t received_size = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    
    if (received_size > *prev_received_size) {
        *lastRxTime = *currentTime;
        *prev_received_size = received_size;
    }
    
    if ((*prev_received_size > 0) && (*currentTime - *lastRxTime >= TIME_OUT)) {
        return 1;
    }
    
    return 0;
}

void Process_Data(uint16_t rx_size) {
	uint8_t start = 0;
	uint8_t end = 0;
	uint16_t tx_size = 0;
	uint8_t tx_buffer[TX_BUFFER_SIZE];
	
    tx_buffer[tx_size++] = '$';
    for (uint16_t i = 0; i < rx_size; ++i) {
        if (rx_buffer[i] == ':') {
            start = 1;
            continue;
        }

        if (start == 0) {
            continue;
        }

        if (rx_buffer[i] >= '0' && rx_buffer[i] <= '9') {
            tx_buffer[tx_size++] = rx_buffer[i];
            end = 1;
        }

        if (end == 1 && ((rx_buffer[i] >= 'A' && rx_buffer[i] <= 'Z') || rx_buffer[i] == ',')) {          
            tx_buffer[tx_size++] = rx_buffer[i];
        }

        if (rx_buffer[i] == '\n' && end == 0) {
            start = 0;
            continue;
        }

        if ((rx_buffer[i] == ' ' || rx_buffer[i] == '\n') && end == 1) {
            tx_buffer[tx_size++] = ';';
            start = 0;
            end = 0;
        }
    }
    
	extern UART_HandleTypeDef huart1;
    // Complete the message
    if (tx_size > 1) { // Only if we have data
        tx_buffer[tx_size - 1] = '#';
        HAL_UART_Transmit(&huart1, tx_buffer, tx_size, HAL_MAX_DELAY);
    }
    
    // Reset buffers
    tx_size = 0;
}
