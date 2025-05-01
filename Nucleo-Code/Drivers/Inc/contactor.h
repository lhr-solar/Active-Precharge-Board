#pragma once
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONTACTOR_H
#define CONTACTOR_H

// #ifdef __cplusplus
// extern "C" {
// #endif

/* Includes ------------------------------------------------------------------*/
// #include "../Drivers/Inc/can.h"
// #include "initTask.c"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l4xx_hal.h"
#include "common.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum {
  OPEN,
  CLOSED,
  FAULT
} Contactor_State;

typedef enum{
NO_FAULT = 0,
MOTOR_TIMEOUT_FAULT,
MOTOR_SENSE_FAULT,
ARRAY_TIMEOUT_FAULT,
ARRAY_SENSE_FAULT
} Fault_Code;

typedef enum{
  MT_LED = 0,
  MS_LED,
  AT_LED,
  AS_LED,
  NUM_LEDS,
} status_led_t;


typedef struct motorContactor
{
  int state; // open, closed
  int enable_in;
  int sense;  // GPIO pin to read the contactor's state
  // int enable_out; // GPIO pin to control the contactor
  int fault;
  int start_time; // HAL_getTick()
  // const struct motorContactor *next[4];
} motorContactor;

typedef struct Precharge
{
  int state;     // open, closed
  int sense_in;  // GPIO pin to read the contactor's state
  int pre_ready; // Precharge ready signal
  int pre_sense;
  int enable_out; // GPIO pin to control the contactor
  int fault;
  int start_time;
  // const struct Precharge *next[8];
} Precharge;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

// #define blinky 1
// #define cantest 2
// #define contactorcode 3
#define sensetest 4

#define m_enable_port GPIOA
#define m_enable_pin GPIO_PIN_0

#define mpre_enable_port GPIOA
#define mpre_enable_pin GPIO_PIN_3

#define apre_enable_port GPIOA
#define apre_enable_pin GPIO_PIN_7

#define m_sense_port GPIOA
#define m_sense_pin GPIO_PIN_1

#define mpre_sense_port GPIOA
#define mpre_sense_pin GPIO_PIN_4

#define apre_sense_port GPIOB
#define apre_sense_pin GPIO_PIN_0 // B PLEASE REMEMBER

#define mt_fault_port GPIOA
#define mt_fault_pin GPIO_PIN_10

#define ms_fault_port GPIOA
#define ms_fault_pin GPIO_PIN_8

#define at_fault_port GPIOB
#define at_fault_pin GPIO_PIN_3 // B PLEASE REMEMBER

#define as_fault_port GPIOA
#define as_fault_pin GPIO_PIN_9

#define mpre_ready_port GPIOB
#define mpre_ready_pin GPIO_PIN_4 // B PLEASE REMEMBER

#define apre_ready_port GPIOB
#define apre_ready_pin GPIO_PIN_5 // B PLEASE REMEMBER

#define M_TIMEOUT 1000
#define MPRE_TIMEOUT 1000
#define APRE_TIMEOUT 1000

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */
void error_handler(void);
void success_handler(void);
void MX_GPIO_Init(void);

int motor_direct();

void motor_precharge_enable(int state);
void array_precharge_enable(int state);

int motor_sense();
int motor_precharge_sense();
int array_precharge_sense();

int motor_precharge_ready();
int array_precharge_ready();

void motor_timeout_fault(int state);
void motor_sense_fault(int state);
void array_timeout_fault(int state);
void array_sense_fault(int state);

void Status_LEDS_Toggle(int statusLED);
void Status_LEDS_Write(status_led_t led, int state);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN EFP */


/* USER CODE END Private defines */

// #ifdef __cplusplus
// }
// #endif

#endif /* CONTACTOR_H */
