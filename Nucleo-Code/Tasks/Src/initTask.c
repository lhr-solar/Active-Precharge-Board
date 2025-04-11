#include "contactor.h"

/* USER CODE BEGIN Private defines */
StaticTask_t contactortask_buffer;
StackType_t contactortask_stack[configMINIMAL_STACK_SIZE];

StaticTask_t cantask_buffer;
StackType_t cantask_stack[configMINIMAL_STACK_SIZE];

void task_Init(){
    // xTaskCreateStatic(
    //     contactorCANTask,           /* The function that implements the task. */
    //     "CAN Init Task",         /* Text name for the task. */
    //     configMINIMAL_STACK_SIZE,/* The size (in words) of the stack that should be created for the task. */
    //     (void*)NULL,                        /* Paramter passed into the task. */
    //     tskIDLE_PRIORITY + 1,      /* Task Prioriy. */
    //     cantask_stack,       /* Stack array. */
    //     &cantask_buffer            /* Buffer for static allocation. */
    // );

    // xTaskCreateStatic(
    //       contactorCANTask, 
    //       "can task", 
    //       configMINIMAL_STACK_SIZE,
    //       (void*)NULL, 
    //       tskIDLE_PRIORITY + 2, 
    //       cantask_stack, 
    //       &cantask_buffer
    // );

    xTaskCreateStatic(
          contactorTask, 
          "contactor task", 
          configMINIMAL_STACK_SIZE,
          NULL, 
          tskIDLE_PRIORITY + 3, 
          contactortask_stack, 
          &contactortask_buffer
    );

   // Task deletes itself after all other taks are init'd
    vTaskDelete(NULL);
}