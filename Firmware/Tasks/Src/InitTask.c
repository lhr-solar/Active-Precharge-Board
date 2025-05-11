#include "Tasks.h"
#include "Contactors.h"

// Task Stack Arrays 
StackType_t Task_Contactor_Stack_Array[TASK_CONTACTOR_STACK_SIZE];

// Task Buffers
StaticTask_t Task_Contactor_Buffer;

void Task_Init() {
    // Initialize contactors driver
    Contactors_Init();

    // Create contactor task
    xTaskCreateStatic(
        Task_Contactor,                 /* The function that implements the task. */
        "Contactor Task",               /* Text name for the task. */
        TASK_CONTACTOR_STACK_SIZE,      /* The size (in words) of the stack that should be created for the task. */
        (void*)NULL,                    /* Paramter passed into the task. */
        TASK_CONTACTOR_PRIO,            /* Task Prioriy. */
        Task_Contactor_Stack_Array,     /* Stack array. */
        &Task_Contactor_Buffer          /* Buffer for static allocation. */
    );

    // Task deletes itself after all other taks are init'd
    vTaskDelete(NULL);
}