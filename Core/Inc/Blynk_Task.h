#ifndef BLYNK_TASK_H
#define BLYNK_TASK_H

#include "cmsis_os.h"
#include "stm32f3xx_hal.h"

// Declare Task function
void Blynk_Task(void const *argument);

// Function to create the Blynk task
void CreateBlynkTask(void);

#endif /* BLYNK_TASK_H */
