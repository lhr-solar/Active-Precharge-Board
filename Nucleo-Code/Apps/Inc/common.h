#ifndef CONFIG_H
#define CONFIG_H
#include "stm32xx_hal.h"
#include "CAN.h"

void sys_clock_config(void);

void can_filter_config(CAN_FilterTypeDef* filter);
void can1_config(void);

void error_handler();
void success_handler();
#endif