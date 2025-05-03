#include "FreeRTOS.h" /* Must come first. */
#include "Tasks.h" 
#include "stm32xx_hal.h"
#include "statusLEDs.h"

// stack for tasks
StaticTask_t contactortask_test_buffer;
StackType_t contactortask_test_stack[configMINIMAL_STACK_SIZE];

int main() {
    HAL_Init();
    SystemClock_Config();
    Status_Leds_Init();

    xTaskCreateStatic(
        Task_Contactors,
        "Contactor_Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        tskIDLE_PRIORITY + TASK_CONTACTORS_PRIO,
        contactortask_test_stack,
        &contactortask_test_buffer
    );

    // Start the scheduler
    vTaskStartScheduler();

    error_handler();
    while (1) {
        // Scheduler should've started by now
        // Code should never enter this point
    }

    return 0;
}
