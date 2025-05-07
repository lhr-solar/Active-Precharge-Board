#ifndef CONTACTOR_H
#define CONTACTOR_H

#include "common.h"
#include "stm32l4xx_hal.h"

// timeouts
#define MOTOR_TIMEOUT 100
#define MOTOR_PRECHARGE_STEADYSTATE_TIMEOUT 100
#define MOTOR_PRECHARGE_INITIAL_TIMEOUT 1000

#define ARRAY_TIMEOUT 100
#define ARRAY_PRECHARGE_STEADYSTATE_TIMEOUT 100
#define ARRAY_PRECHARGE_INITIAL_TIMEOUT 1000

#define UNPLUG_DELAY 10

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

// Enums for various device state

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
    // TODO: add CAN faults here.....
} fault_state_t;

typedef enum {
    OPEN,
    CLOSED,
    FAULT
} contactor_state_t;

// structs to manage contactor states
typedef struct Contactor {
    contactor_state_t state; // open, closed
    contactor_state_t enable_in;
    contactor_state_t sense; // GPIO pin to read the contactor's state
    uint32_t start_time; // HAL_getTick()
} Contactor;

typedef struct prechargeContactor {
    contactor_state_t state; // open, closed
    contactor_state_t pre_ready; // Precharge ready signal
    contactor_state_t sense;
    contactor_state_t enable_out; // GPIO pin to control the contactor
    uint32_t start_time; // HAL_getTick()
} prechargeContactor;

// initialization & handler for contactor and precharge logic
void contactors_init(void);
bool contactors_fault_handler(void);
void contactors_handler(void);

#endif /* CONTACTOR_H */
