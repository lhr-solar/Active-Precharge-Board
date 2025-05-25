#ifndef CONTACTORTASK_H
#define CONTACTORTASK_H

#include "common.h"
#include "stm32l4xx_hal.h"

typedef enum {
    IGNITION_OFF = 0x00,
    IGNITION_ARRAY = 0x01,
    IGNITION_MOTOR = 0x02
} ignition_state_t;

typedef enum {
    NOT_CHECKED = 0,
    SAFE,
    FAULT
} safety_status_t;

#endif