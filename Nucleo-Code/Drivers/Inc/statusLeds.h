#pragma once

#include "stm32xx_hal.h"

/**
 * Possible Status LED Pins
 */
 typedef enum
 {
    MOTOR_FAULT_LED = 0,
    MOTOR_TIMEOUT_FAULT_LED,
    ARRAY_FAULT_LED,
    ARRAY_TIMEOUT_FAULT_LED,
    NUM_STATUS_LED,
 } status_led_t;

#define ARRAY_FAULT_LED_PORT GPIOA
#define ARRAY_FAULT_LED_PIN GPIO_PIN_9

#define MOTOR_FAULT_LED_PORT GPIOA
#define MOTOR_FAULT_LED_PIN GPIO_PIN_8

#define MOTOR_TIMEOUT_FAULT_LED_PORT GPIOA
#define MOTOR_TIMEOUT_FAULT_LED_PIN  GPIO_PIN_10

#define ARRAY_TIMEOUT_FAULT_LED_PORT GPIOB
#define ARRAY_TIMEOUT_FAULT_LED_PIN  GPIO_PIN_3

 /**
 * @brief Initializes Status LED pins
 * 
 */
void Status_Leds_Init(void);

/**
 * @brief   Writes data to a specified pin
 * @param   led The led to write to
 * @param   state true=ON or false=OFF
 * @return  None
 */ 
void Status_Leds_Write(status_led_t led, bool state);

/**
 * @brief   Toggles a status led
 * @param   led The led to toggle
 * @return  None
 */ 
void Status_Leds_Toggle(status_led_t led);

/**
 * @brief   Turns all status LEDs on
 * @return  None
 */ 
void Status_Leds_All_On(void);