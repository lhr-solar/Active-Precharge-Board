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
    OPEN,
    CLOSED,
    FAULT
} contactor_state_t;

typedef enum {
    NO_FAULT,
    MOTOR_TIMEOUT_FAULT,
    MOTOR_SENSE_FAULT,
    ARRAY_TIMEOUT_FAULT,
    ARRAY_SENSE_FAULT
} contactor_fault_t;

// structs to manage contactor states
typedef struct Contactor {
    uint8_t state; // open, closed
    uint8_t enable_in;
    uint8_t sense; // GPIO pin to read the contactor's state
    uint8_t fault;
    uint32_t start_time; // HAL_getTick()
} Contactor;

typedef struct prechargeContactor {
    uint8_t state; // open, closed
    uint8_t pre_ready; // Precharge ready signal
    uint8_t sense;
    uint8_t enable_out; // GPIO pin to control the contactor
    uint8_t fault;
    uint32_t start_time;
} prechargeContactor;

// initialization & handler for contactor and precharge logic
void contactors_init(void);
bool contactors_fault_handler(void);
void contactors_handler(void);

#endif /* CONTACTOR_H */
