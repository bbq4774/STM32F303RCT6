#ifndef HAL_UART_Callback_H
#define HAL_UART_Callback_H

#include "stm32f3xx_hal.h"
#include "GPS_GNSS.h"

extern GPS_Data_t *gps_data_ptr;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* HAL_UART_Callback_H */
