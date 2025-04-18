#include "GPS_GNSS.h"

uint8_t gps_rx_char;
uint16_t gps_index = 0;
uint8_t gps_line[GPS_LINE_BUFFER_SIZE];

// Function to create the GPS task
void CreateGPSTask(void) {
    osThreadDef(GPS, GPS_GNSS_Task, osPriorityNormal, 0, 128);
    osThreadId GPSTaskHandle = osThreadCreate(osThread(GPS), NULL);
    
    extern UART_HandleTypeDef huart1;
    if (GPSTaskHandle == NULL) {
        uint8_t err[] = "GPS task failed!\r\n";
        HAL_UART_Transmit(&huart1, err, sizeof(err) - 1, 100);
    }
}


// GPS task function
void GPS_GNSS_Task(void const *argument) {
    extern UART_HandleTypeDef huart2;
	
    GPS_ENABLE();
    HAL_UART_Receive_IT(&huart2, &gps_rx_char, 1);

    while (1) {

        osDelay(10);
    }
}
	
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		extern UART_HandleTypeDef huart2;
		extern UART_HandleTypeDef huart1;
		
        if (gps_rx_char == '\n') {
            gps_line[gps_index] = '\0';
            if (Check_Data(gps_line)) {
                HAL_UART_Transmit(&huart1, gps_line, gps_index, 10);
            }
            gps_index = 0;
        } else if (gps_index < GPS_LINE_BUFFER_SIZE - 1) {
                gps_line[gps_index++] = gps_rx_char;
            }

        HAL_UART_Receive_IT(&huart2, &gps_rx_char, 1);
    }
}

int Check_Data(uint8_t *data) {
    const uint8_t frame[] = "$GNRMC";
    
    for (uint8_t i = 0; i < 6; ++i) {
        if (data[i] != frame[i])
            return 0;
    }
    
    return 1;
}
