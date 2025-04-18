#ifndef GPS_GNSS_TASK_H
#define GPS_GNSS_TASK_H

#include "stm32f3xx_hal.h"
#include "cmsis_os.h"

#define GNSS_PWR_CTRL_PORT      GPIOC
#define GNSS_PWR_CTRL_PIN       GPIO_PIN_2

#define GPS_LINE_BUFFER_SIZE    50

void GPS_GNSS_Task(void const *argument);
void CreateGPSTask(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void ParseGPSLine(char *line);
int Check_Data(uint8_t *data);

#define GPS_ENABLE()   HAL_GPIO_WritePin(GNSS_PWR_CTRL_PORT, GNSS_PWR_CTRL_PIN, GPIO_PIN_RESET)
#define GPS_DISABLE()  HAL_GPIO_WritePin(GNSS_PWR_CTRL_PORT, GNSS_PWR_CTRL_PIN, GPIO_PIN_SET)

#endif /* GPS_GNSS_TASK_H */