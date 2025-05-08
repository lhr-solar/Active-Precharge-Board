#ifndef CONTACTORS_H
#define CONTACTORS_H

#include "common.h"
#include "stm32l4xx_hal.h"

#define NUM_CONTACTORS 4

// Contactor drive/sense pin definitions
#define MOTOR_ENABLE_PORT GPIOA
#define MOTOR_ENABLE_PIN GPIO_PIN_0
#define MOTOR_SENSE_PORT GPIOA
#define MOTOR_SENSE_PIN GPIO_PIN_1

#define MOTOR_PRECHARGE_ENABLE_PORT GPIOA
#define MOTOR_PRECHARGE_ENABLE_PIN GPIO_PIN_3
#define MOTOR_PRECHARGE_SENSE_PORT GPIOA
#define MOTOR_PRECHARGE_SENSE_PIN GPIO_PIN_4

#define ARRAY_PRECHARGE_ENABLE_PORT GPIOA
#define ARRAY_PRECHARGE_ENABLE_PIN GPIO_PIN_7
#define ARRAY_PRECHARGE_SENSE_PORT GPIOB
#define ARRAY_PRECHARGE_SENSE_PIN GPIO_PIN_0

// Precharge ready indicators (input from hardware comparison)
#define MOTOR_PRECHARGE_READY_PORT GPIOB
#define MOTOR_PRECHARGE_READY_PIN GPIO_PIN_4

#define ARRAY_PRECHARGE_READY_PORT GPIOB
#define ARRAY_PRECHARGE_READY_PIN GPIO_PIN_5

// Open/Closed definition
typedef enum { OFF = 0, ON } State;

// Fault state bitmap enum
typedef enum {
    FAULT_NONE = 0,
    FAULT_MOTOR_ENABLE_UNPLUG = 1 << 0,
    FAULT_MOTOR_SENSE = 1 << 1,
    FAULT_MOTOR_TIMEOUT = 1 << 2,
    FAULT_MOTOR_PRECHARGE_SENSE = 1 << 3,
    FAULT_MOTOR_PRECHARGE_TIMEOUT = 1 << 4,
    FAULT_ARRAY_ENABLE = 1 << 5,
    FAULT_ARRAY_SENSE = 1 << 6,
    FAULT_ARRAY_TIMEOUT = 1 << 7,
    FAULT_ARRAY_PRECHARGE_SENSE = 1 << 8,
    FAULT_ARRAY_PRECHARGE_TIMEOUT = 1 << 9,
    FAULT_BPS = 1 << 10,
    FAULT_CONTROLS = 1 << 11
} fault_state_t;

// Enum to index into contactors array
typedef enum {
    MOTOR_CONTACTOR = 0,
    MOTOR_PRECHARGE_CONTACTOR,
    ARRAY_CONTACTOR,
    ARRAY_PRECHARGE_CONTACTOR
} contactor_enum_t;

// Struct to define the current state of a contactor
typedef struct contactor_t {
    bool state; // ON or OFF
    bool isPrechargeContactor; // only true for motor/array precharge contactors
} contactor_t;

/**
 * @brief   Initializes contactors to be used
 *          in connection with the Motor and Array
 * @return  None
 */
void Contactors_Init();

/**
 * @brief   Returns the current state of
 *          a specified contactor
 * @param   contactor the contactor
 *              (MOTOR_CONTACTOR/MOTOR_PRECHARGE_CONTACTOR/ARRAY_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  The contactor's state (ON/OFF) based on its sense pin
 */
bool Contactors_Get(contactor_enum_t contactor);

/**
 * @brief   Sets the state of a specified contactor
 * @param   contactor the contactor
 *              (MOTOR_CONTACTOR/MOTOR_PRECHARGE_CONTACTOR/ARRAY_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @param   state the state to set (ON/OFF) (true/false)
 * @param   blocking whether or not this should be a blocking call
 * @return  Whether or not the contactor was successfully set
 */
ErrorStatus Contactors_Set(contactor_enum_t contactor, bool state, bool blocking);

#endif