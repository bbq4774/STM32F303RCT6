#ifndef GPS_GNSS_TASK_H
#define GPS_GNSS_TASK_H

#include "stm32f3xx_hal.h"
#include "cmsis_os.h"

// ============================ MACROS =============================

#define GNSS_PWR_CTRL_PORT      GPIOC
#define GNSS_PWR_CTRL_PIN       GPIO_PIN_2
#define GPS_LINE_BUFFER_SIZE    50

// ========================= TYPE DEFINITIONS ======================

typedef struct {
    uint8_t rx_char;
    uint16_t index;
    uint8_t line[GPS_LINE_BUFFER_SIZE];
} GPS_Data_t;

// ========================= GLOBAL VARIABLES ======================

static GPS_Data_t *gps_data_ptr = NULL;

// ============================ PUBLIC API =========================

void CreateGPSTask(void);
void GPS_GNSS_Task(void const *argument);
void GPS_UART_Callback(GPS_Data_t *gps);
int Check_Data(uint8_t *data);

// ============================ GPIO CONTROL ========================

#define GPS_ENABLE()   HAL_GPIO_WritePin(GNSS_PWR_CTRL_PORT, GNSS_PWR_CTRL_PIN, GPIO_PIN_RESET)
#define GPS_DISABLE()  HAL_GPIO_WritePin(GNSS_PWR_CTRL_PORT, GNSS_PWR_CTRL_PIN, GPIO_PIN_SET)

#endif /* GPS_GNSS_TASK_H */
