#include "GPS_GNSS.h"

void CreateGPSTask(void) {
	static GPS_Data_t gps_data;

	gps_data_ptr = &gps_data;

	osThreadDef(GPS, GPS_GNSS_Task, osPriorityBelowNormal, 0, 128);
	osThreadId GPSTaskHandle = osThreadCreate(osThread(GPS), &gps_data);

	extern UART_HandleTypeDef huart1;
	if (GPSTaskHandle == NULL) {
		uint8_t err[] = "GPS task failed!\r\n";
		HAL_UART_Transmit(&huart1, err, sizeof(err) - 1, 100);
	}
}

// Hàm task GPS
void GPS_GNSS_Task(void const *argument) {
	GPS_Data_t *gps = (GPS_Data_t *)argument;
	gps_data_ptr = gps;

	extern UART_HandleTypeDef huart2;

	GPS_ENABLE();
	HAL_UART_Receive_IT(&huart2, &gps->rx_char, 1);

	while (1) {
		osDelay(10);
	}
}

void GPS_UART_Callback(GPS_Data_t *gps) {
	extern UART_HandleTypeDef huart2;
	extern UART_HandleTypeDef huart1;

	if (gps->rx_char == '\n') {
		gps->line[gps->index] = '\0';

		if (Check_Data(gps->line)) {
			HAL_UART_Transmit(&huart1, gps->line, gps->index, 10);
		}

		gps->index = 0;
	} else if (gps->index < GPS_LINE_BUFFER_SIZE - 1) {
		gps->line[gps->index++] = gps->rx_char;
	}

	HAL_UART_Receive_IT(&huart2, &gps->rx_char, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2 && gps_data_ptr != NULL) {
		GPS_UART_Callback(gps_data_ptr);
	}
}

int Check_Data(uint8_t *data) {
	const uint8_t frame[] = "$GNRMC";
	for (uint8_t i = 0; i < 6; ++i) {
		if (data[i] != frame[i]) return 0;
	}
	return 1;
}
