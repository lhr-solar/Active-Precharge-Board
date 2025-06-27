#ifndef CONTACTORTASK_H
#define CONTACTORTASK_H

#include "common.h"
#include "stm32l4xx_hal.h"

#define CONTACTOR_SENSE_LENGTH 2

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

/** 
 * @brief   Sends contactor sense message over CAN
 * @param   motor_fault true if motor contactor sense fault, false otherwise
 * @param   motor_precharge_fault true if motor precharge contactor sense fault, false otherwise
 * @param   array_precharge_fault true if array precharge contactor sense fault, false otherwise
 * @return  None
*/
void send_contactor_sense(bool motor_fault, bool motor_precharge_fault, bool array_precharge_fault);

#endif