#ifndef CONTACTORTASK_H
#define CONTACTORTASK_H

#include "common.h"
#include "stm32l4xx_hal.h"

typedef enum {
    IGNITION_OFF = 0,
    IGNITION_ARRAY,
    IGNITION_MOTOR
} ignition_state_t;

#endif