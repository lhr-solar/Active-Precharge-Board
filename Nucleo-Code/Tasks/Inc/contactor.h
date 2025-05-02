#pragma once // TODO: why pragma....

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONTACTOR_H
#define CONTACTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "stm32l4xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void error_handler(void);
void success_handler(void);
void task_Init();
void contactorTask();
void MX_GPIO_Init(void);

// timeouts
// TODO: more descriptive names
#define M_TIMEOUT 1000
#define MPRE_TIMEOUT 1000
#define APRE_TIMEOUT 1000

// TODO: remove ts
// #define blinky 1
// #define cantest 2
// #define contactorcode 3
#define validated_logic 4 // fully validated 4/29/2025

// Contactor pin definitions
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

// Status LEDs
#define MOTOR_TIMEOUT_FAULT_LED_PORT GPIOA
#define MOTOR_TIMEOUT_FAULT_LED_PIN GPIO_PIN_10

#define MOTOR_SENSE_FAULT_LED_PORT GPIOA
#define MOTOR_SENSE_FAULT_LED_PIN GPIO_PIN_8

#define ARRAY_TIMEOUT_FAULT_LED_PORT GPIOB
#define ARRAY_TIMEOUT_FAULT_LED_PIN GPIO_PIN_3

#define ARRAY_SENSE_FAULT_LED_PORT GPIOA
#define ARRAY_SENSE_FAULT_LED_PIN GPIO_PIN_9

#define MOTOR_PRECHARGE_READY_LED_PORT GPIOB
#define MOTOR_PRECHARGE_READY_LED_PIN GPIO_PIN_4

#define ARRAY_PRECHARGE_READY_LED_PORT GPIOB
#define ARRAY_PRECHARGE_READY_LED_PIN GPIO_PIN_5

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

typedef enum {
    MOTOR_TIMEOUT_LED,
    MOTOR_SENSE_LED,
    ARRAY_TIMEOUT_LED,
    ARRAY_SENSE_LED,
    NUM_LEDS,
} status_led_t;

#ifdef __cplusplus
}
#endif

#endif /* CONTACTOR_H */
