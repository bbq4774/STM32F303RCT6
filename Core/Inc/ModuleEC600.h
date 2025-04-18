#ifndef Module_EC600_TASK_H
#define Module_EC600_TASK_H

#include "stm32f3xx_hal.h"
#include "cmsis_os.h"
#include "string.h"

#define BUF_SIZE 				100

#define MD_PWKEY_PORT			GPIOC
#define MD_PWKEY_PIN			GPIO_PIN_1

#define MD_PWKEY_LOW()			HAL_GPIO_WritePin(MD_PWKEY_PORT, MD_PWKEY_PIN, GPIO_PIN_RESET)
#define MD_PWKEY_HIGH()			HAL_GPIO_WritePin(MD_PWKEY_PORT, MD_PWKEY_PIN, GPIO_PIN_SET)

#define MD_PWR_CTRL_PORT		GPIOC
#define MD_PWR_CTRL_PIN			GPIO_PIN_0

#define MD_PWR_CTRL_HIGH()		HAL_GPIO_WritePin(MD_PWR_CTRL_PORT, MD_PWR_CTRL_PIN, GPIO_PIN_SET)
#define MD_PWR_CTRL_LOW()		HAL_GPIO_WritePin(MD_PWR_CTRL_PORT, MD_PWR_CTRL_PIN, GPIO_PIN_RESET)

void CreateEC600Task(void);
void Module_EC600_Task(void const *argument);
void Power_On(void);
void Send_AT(UART_HandleTypeDef *huart, uint8_t *buf);
void Send_cmd(UART_HandleTypeDef *huart, uint8_t *cmd, uint8_t size_cmd, uint8_t *buf);
void Process_Data_EC600_IMEI(UART_HandleTypeDef *huart, uint8_t *buf, uint8_t size);

#endif /* Module_EC600_TASK_H */