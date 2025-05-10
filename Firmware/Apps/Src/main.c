#include "FreeRTOS.h" /* Must come first. */
#include "Tasks.h"
#include "common.h"
#include "stm32xx_hal.h"
#include "StatusLEDs.h"
#include "Contactors.h"
#include "CANbus.h"

StaticTask_t Task_Init_Buffer;
StackType_t Task_Init_Stack_Array[TASK_INIT_STACK_SIZE];

int main() {
    HAL_Init();
    SystemClock_Config();

    // Init drivers for status LEDs, contactors, CAN bus
    Status_Leds_Init();
    Contactors_Init();
    CAN_Init();

    // Create Init Task
    xTaskCreateStatic(
        Task_Init, /* The function that implements the task. */
        "Init Task", /* Text name for the task. */
        TASK_INIT_STACK_SIZE, /* The size (in words) of the stack that should be created for the task. */
        (void*)NULL, /* Paramter passed into the task. */
        TASK_INIT_PRIO, /* Task Prioriy. */
        Task_Init_Stack_Array, /* Stack array. */
        &Task_Init_Buffer  /* Buffer for static allocation. */
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