#ifndef TASKS_H
#define TASKS_H

// Task priorities
#define TASK_INIT_PRIO tskIDLE_PRIORITY + 1

// Task stack sizes
#define TASK_INIT_STACK_SIZE configMINIMAL_STACK_SIZE

// (exposed so that tests can init tasks)
// Task Stack Arrays 

// Task Buffers

// Task Inits
void Task_Init();

#endif