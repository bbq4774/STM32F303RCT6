#include "blynk_task.h"

// Function to create and start the Blynk task
void CreateBlynkTask(void) {
    osThreadDef(Blynk, Blynk_Task, osPriorityAboveNormal, 0, 128);
    osThreadId BlynkTaskHandle = osThreadCreate(osThread(Blynk), NULL);
}

// Function to toggle GPIO pin PC8 every 500ms
void Blynk_Task(void const *argument) {
    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
        osDelay(500);
    }
}
