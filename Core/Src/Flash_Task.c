#include "Flash_Task.h"

// Function to create the Flash task
void CreateFlashTask(void) {
    osThreadDef(Flash, Flash_Task, osPriorityNormal, 0, 128);
    osThreadId flashTaskHandle = osThreadCreate(osThread(Flash), NULL);
    
    if (flashTaskHandle == NULL) {
        uint8_t err[] = "Flash task failed!\r\n";
        extern UART_HandleTypeDef huart1;
		
        HAL_UART_Transmit(&huart1, err, sizeof(err) - 1, HAL_MAX_DELAY);
    }
}

// UART task function with Normal DMA and timeout
void Flash_Task(void const *argument) {
	extern UART_HandleTypeDef huart1;
    uint8_t buf_jedec_id[W25_JEDEC_ID_BUF_SIZE] = {0};
    
	osDelay(500);
    Read_JEDEC_ID(&huart1, buf_jedec_id, W25_JEDEC_ID_BUF_SIZE);
    Delete_Data(&huart1, ADDRESS);
    Save_Flash(&huart1, ADDRESS, buf_jedec_id, W25_JEDEC_ID_BUF_SIZE);
    
    while (1) {
        
        osDelay(10);
    }
}

void Delete_Data(UART_HandleTypeDef *huart, uint32_t address) {
    if (W25_SectorErase(address) != HAL_OK) {
		uint8_t mess[] = "Failed to delete\r\n";
		
        HAL_UART_Transmit(huart, mess, sizeof(mess) - 1, HAL_MAX_DELAY);
    }
}

void Read_Data(UART_HandleTypeDef *huart, uint32_t address, uint32_t size) {
    uint8_t buf_read[W25_JEDEC_ID_BUF_SIZE] = {0};
    
    if (W25_ReadData(address, buf_read, size) == HAL_OK) {
        uint8_t mess[] = "\r\nRead data : \t";
        HAL_UART_Transmit(huart, mess, sizeof(mess) - 1, HAL_MAX_DELAY);
        Arr_hex_view(huart, buf_read, size);
    } else {
        uint8_t mess[] = "\r\nFailed to read data";
        HAL_UART_Transmit(huart, mess, sizeof(mess) - 1, HAL_MAX_DELAY);
    }
}

void Save_Flash(UART_HandleTypeDef *huart, uint32_t address, uint8_t *buf, uint32_t size) {
    if (W25_PageProgram(address, buf, size) == HAL_OK) {
        Read_Data(huart, address, W25_JEDEC_ID_BUF_SIZE);
    } else {
        uint8_t mess[] = "\r\nFailed to save";
        HAL_UART_Transmit(huart, mess, sizeof(mess) - 1, HAL_MAX_DELAY);
    }
}

void Read_JEDEC_ID(UART_HandleTypeDef *huart, uint8_t *buf, uint32_t size) {
    if (W25_ReadJedecID(buf, size) == HAL_OK) {
        uint8_t mess[] = "\r\nJEDEC ID : \t";
        HAL_UART_Transmit(huart, mess, sizeof(mess) - 1, HAL_MAX_DELAY);
        Arr_hex_view(huart, buf, size);
    } else {
        uint8_t mess[] = "Failed to read JEDEC ID\r\n";
        HAL_UART_Transmit(huart, mess, sizeof(mess) - 1, HAL_MAX_DELAY);
    }
}

void Arr_hex_view(UART_HandleTypeDef *huart, uint8_t *result, int Length){
    int pos = 0;
    char result_view[200];
    for (int j = 0; j < Length; j++) {
        pos += sprintf(result_view + pos, "%02X ", result[j]);
    }
    
    HAL_UART_Transmit(huart, (uint8_t*)result_view, pos, 100);
}
