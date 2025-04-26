#include "contactor.h"
#include "CANbus.h"

#ifndef TASKS_H
#define TASKS_H

#define TASK_INIT_PRIO 0
#define TASK_CONTACTORS_PRIO 1
#define TASK_CANBUS_PRIO 2

void Task_Init();

void Task_Contactors();

void Task_CAN();

#endif