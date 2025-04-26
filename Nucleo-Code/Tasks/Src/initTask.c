#include "Tasks.h"

/* USER CODE BEGIN Private defines */
StaticTask_t contactortask_buffer;
StackType_t contactortask_stack[configMINIMAL_STACK_SIZE];

StaticTask_t cantask_buffer;
StackType_t cantask_stack[configMINIMAL_STACK_SIZE];

void Task_Init(){
    xTaskCreateStatic(
          Task_CAN, 
          "can task", 
          configMINIMAL_STACK_SIZE,
          (void*)NULL, 
          tskIDLE_PRIORITY + 2, 
          cantask_stack, 
          &cantask_buffer
    );

    xTaskCreateStatic(
          Task_Contactors, 
          "contactor task", 
          configMINIMAL_STACK_SIZE,
          NULL, 
          tskIDLE_PRIORITY + TASK_CONTACTORS_PRIO, 
          contactortask_stack, 
          &contactortask_buffer
    );

   // Task deletes itself after all other taks are init'd
    vTaskDelete(NULL);
}