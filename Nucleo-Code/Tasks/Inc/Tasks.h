#ifndef TASKS_H
#define TASKS_H

#include "Task_Contactor.h"
#include "Task_CAN.h"

// TODO: are these just arbitrary? @Tony5427
#define TASK_INIT_PRIO 0
#define TASK_CONTACTORS_PRIO 1
#define TASK_CANBUS_PRIO 2

void Task_Init();

void Task_Contactors(void *pvParamters);

void Task_CAN(void *pvParamters);

#endif