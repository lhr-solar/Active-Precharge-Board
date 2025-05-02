#include "Tasks.h"

// stack for tasks
StaticTask_t contactortask_buffer;
StackType_t contactortask_stack[configMINIMAL_STACK_SIZE];
StaticTask_t cantask_buffer;
StackType_t cantask_stack[configMINIMAL_STACK_SIZE];

void Task_Init() {
      xTaskCreateStatic(
            Task_CAN,
            "CAN_Task",
            configMINIMAL_STACK_SIZE,
            (void*)NULL,
            tskIDLE_PRIORITY + TASK_CANBUS_PRIO,
            cantask_stack,
            &cantask_buffer
      );

      xTaskCreateStatic(
            Task_Contactors,
            "Contactor_Task",
            configMINIMAL_STACK_SIZE,
            NULL,
            tskIDLE_PRIORITY + TASK_CONTACTORS_PRIO,
            contactortask_stack,
            &contactortask_buffer
      );

      // Task deletes itself after all other taks are init'd
      vTaskDelete(NULL);
}