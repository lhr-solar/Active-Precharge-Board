#pragma once // TODO: why pragma....

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONTACTOR_H
#define CONTACTOR_H

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "stm32l4xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void error_handler(void);
void success_handler(void);
void MX_GPIO_Init(void);

// timeouts
#define MOTOR_TIMEOUT 1000
#define MOTOR_PRECHARGE_TIMEOUT 1000
#define ARRAY_PRECHARGE_TIMEOUT 1000

// TODO: remove ts
// #define blinky 1
// #define cantest 2
// #define contactorcode 3
#define validated_logic 4 // fully validated 4/29/2025

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

// Status LEDs
#define MOTOR_TIMEOUT_FAULT_LED_PORT GPIOA
#define MOTOR_TIMEOUT_FAULT_LED_PIN GPIO_PIN_10

#define MOTOR_SENSE_FAULT_LED_PORT GPIOA
#define MOTOR_SENSE_FAULT_LED_PIN GPIO_PIN_8

#define ARRAY_TIMEOUT_FAULT_LED_PORT GPIOB
#define ARRAY_TIMEOUT_FAULT_LED_PIN GPIO_PIN_3

#define ARRAY_SENSE_FAULT_LED_PORT GPIOA
#define ARRAY_SENSE_FAULT_LED_PIN GPIO_PIN_9

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

typedef enum {
    MOTOR_TIMEOUT_LED,
    MOTOR_SENSE_LED,
    ARRAY_TIMEOUT_LED,
    ARRAY_SENSE_LED,
    NUM_LEDS,
} status_led_t;

// Function definitions
// READ-ONLY drive/sense signals from controls-driven motor contactor
uint8_t motor_enable_read();
uint8_t motor_sense();

// drive/sense signals for motor precharge contactor
void motor_precharge_enable(uint8_t state);
uint8_t motor_precharge_sense();

// drive/sense signals for array precharge contactor
void array_precharge_enable(uint8_t state);
uint8_t array_precharge_sense();

// read precharge ready indicators from hardware comparison
uint8_t motor_precharge_ready();
uint8_t array_precharge_ready();

// fault/status LEDs
void motor_timeout_fault_led(uint8_t state);
void motor_sense_fault_led(uint8_t state);

void array_timeout_fault_led(uint8_t state);
void array_sense_fault_led(uint8_t state);

// TODO: get rid of all individual status LED functions and call this...
void Status_LEDS_Toggle(uint8_t statusLED);
void Status_LEDS_Write(status_led_t led, uint8_t state);

#endif /* CONTACTOR_H */
