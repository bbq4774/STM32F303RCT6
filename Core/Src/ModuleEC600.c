#include "ModuleEC600.h"

// Function to create the Module EC600 task
void CreateEC600Task(void) {
	osThreadDef(EC600, Module_EC600_Task, osPriorityNormal, 0, 128);
	osThreadId EC600TaskHandle = osThreadCreate(osThread(EC600), NULL);
	
	extern UART_HandleTypeDef huart1;
	if (EC600TaskHandle == NULL) {
		uint8_t err[] = "Module EC600 task failed!\r\n";
		HAL_UART_Transmit(&huart1, err, sizeof(err) - 1, 100);
	}
}
	
// Module EC600 task function
void Module_EC600_Task(void const *argument) {
	extern UART_HandleTypeDef huart1;
	extern UART_HandleTypeDef huart3;
	uint8_t buf[BUF_SIZE] = {0};
	
	//Turn on ModuleSim
	Power_On();  
	
	//Sent AT cmd
	Send_AT(&huart3, buf);
	
	uint8_t cmd[] = "AT+CGSN\r\n";
	//Send cmd to get IMEI
	Send_cmd(&huart3, cmd, sizeof(cmd) - 1, buf);
	
	uint8_t dat[] = "IMEI : ";
	HAL_UART_Transmit(&huart1, dat, sizeof(dat) - 1, 100);
	Process_Data_EC600_IMEI(&huart1, buf, BUF_SIZE);
	while (1) {
		
		osDelay(10);
	}
}
	
void Power_On(void) {
	MD_PWR_CTRL_HIGH();
	MD_PWKEY_LOW();
	osDelay(2000);
	MD_PWKEY_HIGH();
	osDelay(6000);
}
	
void Process_Data_EC600_IMEI(UART_HandleTypeDef *huart, uint8_t *buf, uint8_t size) {
	uint8_t data[40];
	uint8_t data_size = 0;
	
	for (int i = 0; i < size; ++i) {
		if (buf[i] >= '0' && buf[i] <= '9') {
			data[data_size++] = buf[i];
		}
	}
	data[data_size++] = '\0';
	
	HAL_UART_Transmit(huart, data, data_size - 1, 100);
	uint8_t dat[] = "\r\n";
	HAL_UART_Transmit(huart, dat, sizeof(dat) - 1, 100);
}

void Send_AT(UART_HandleTypeDef *huart, uint8_t *buf) {
	uint8_t at[] = "AT\r\n";
	HAL_UART_Transmit(huart, at, sizeof(at) - 1, 100);
	HAL_UART_Receive_DMA(huart, buf, BUF_SIZE); 
	while (1) {     
		if (strstr((char *)buf, "RDY") != NULL) {
			HAL_UART_DMAStop(huart);
			break;
		}
		osDelay(1000);
	}
}

void Send_cmd(UART_HandleTypeDef *huart, uint8_t *cmd, uint8_t size_cmd, uint8_t *buf) {
	HAL_UART_Transmit(huart, cmd, size_cmd, 100);
	memset(buf, 0, BUF_SIZE);
	HAL_UART_Receive_DMA(huart, buf, BUF_SIZE);
	osDelay(500);
	HAL_UART_DMAStop(huart);
}
