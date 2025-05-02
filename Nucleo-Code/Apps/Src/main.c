#include "FreeRTOS.h" /* Must come first. */
#include "Tasks.h" 
#include "stm32xx_hal.h"

StaticTask_t Task_Init_Buffer;
StackType_t Task_Init_Stack_Array[configMINIMAL_STACK_SIZE];

int main() {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    xTaskCreateStatic(
                    Task_Init, /* The function that implements the task. */
                    "Init_Task", /* Text name for the task. */
                    configMINIMAL_STACK_SIZE, /* The size (in words) of the stack that should be created for the task. */
                    (void*)NULL, /* Paramter passed into the task. */
                    tskIDLE_PRIORITY, /* Task Prioriy. */
                    Task_Init_Stack_Array, /* Stack array. */
                    &Task_Init_Buffer  /* Buffer for static allocation. */
   );
    // Start the scheduler
    vTaskStartScheduler();

    error_handler();
    while(1){
        // Scheduler should've started by now
        // Code should never enter this point
    }
    
    return 0;
}