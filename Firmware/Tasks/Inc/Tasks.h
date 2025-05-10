#ifndef TASKS_H
#define TASKS_H

#include "common.h"
#include "stm32l4xx_hal.h"

// Task priorities
#define TASK_INIT_PRIO tskIDLE_PRIORITY + 1
#define TASK_CONTACTOR_PRIO tskIDLE_PRIORITY + 2

// Task stack sizes
#define TASK_INIT_STACK_SIZE configMINIMAL_STACK_SIZE
#define TASK_CONTACTOR_STACK_SIZE configMINIMAL_STACK_SIZE

// (exposed so that tests can init tasks)
// Task Stack Arrays 
extern StackType_t Task_Contactor_Stack_Array[TASK_CONTACTOR_STACK_SIZE];

// Task Buffers
extern StaticTask_t Task_Contactor_Buffer;

// Task Functions
void Task_Init();
void Task_Contactor();

#endif