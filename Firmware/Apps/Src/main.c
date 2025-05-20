#include "FreeRTOS.h" /* Must come first. */
#include "Tasks.h"
#include "common.h"
#include "stm32xx_hal.h"
#include "StatusLEDs.h"
#include "CANbus.h"
#include "Contactors.h"

// StaticTask_t Task_Init_Buffer;
// StackType_t Task_Init_Stack_Array[TASK_INIT_STACK_SIZE];

// Task Stack Arrays 
StackType_t Task_Contactor_Stack_Array[configMINIMAL_STACK_SIZE];

// Task Buffers
StaticTask_t Task_Contactor_Buffer;

int main() {
    HAL_Init();
    Sys_Clock_Config();

    // Init drivers for status LEDs, contactors, CAN bus
    Status_Leds_Init();
    if (!CAN_Init()) error_handler();
    if (can_start(hcan1) != CAN_OK) error_handler();

    // init task was fucking with timer prios so i got rid of it :D
    // // Create Init Task
    // xTaskCreateStatic(
    //     Task_Init, /* The function that implements the task. */
    //     "Init Task", /* Text name for the task. */
    //     TASK_INIT_STACK_SIZE, /* The size (in words) of the stack that should be created for the task. */
    //     (void*)NULL, /* Paramter passed into the task. */
    //     TASK_INIT_PRIO, /* Task Prioriy. */
    //     Task_Init_Stack_Array, /* Stack array. */
    //     &Task_Init_Buffer  /* Buffer for static allocation. */
    // );

    // Create contactor task
    xTaskCreateStatic(
        Task_Contactor,                 /* The function that implements the task. */
        "Contactor Task",               /* Text name for the task. */
        configMINIMAL_STACK_SIZE,      /* The size (in words) of the stack that should be created for the task. */
        (void*)NULL,                    /* Paramter passed into the task. */
        TASK_INIT_PRIO,                 /* Task Prioriy. */
        Task_Contactor_Stack_Array,     /* Stack array. */
        &Task_Contactor_Buffer          /* Buffer for static allocation. */
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