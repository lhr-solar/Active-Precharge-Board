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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "CAN.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

#define blinky 1
// #define cantest 2
// #define contactorcode 3

#define m_enable_pin GPIO_PIN_0
#define mpre_enable_pin GPIO_PIN_3
#define apre_enable_pin GPIO_PIN_7
#define m_sense_pin GPIO_PIN_1
#define mpre_sense_pin GPIO_PIN_4
#define apre_sense_pin GPIO_PIN_0 // B PLEASE REMEMBER
#define mt_fault_pin GPIO_PIN_10
#define ms_fault_pin GPIO_PIN_8
#define at_fault_pin GPIO_PIN_3 // B PLEASE REMEMBER
#define as_fault_pin GPIO_PIN_9
#define mpre_ready_pin GPIO_PIN_4 // B PLEASE REMEMBER
#define apre_ready_pin GPIO_PIN_5 // B PLEASE REMEMBER

#define m_direct HAL_GPIO_ReadPin(GPIOA, m_enable_pin)

#define m_pre_enable(state) HAL_GPIO_WritePin(GPIOA, mpre_enable_pin, state)
#define a_pre_enable(state) HAL_GPIO_WritePin(GPIOA, apre_enable_pin, state)

#define m_sense HAL_GPIO_ReadPin(GPIOA, m_sense_pin)
#define m_pre_sense HAL_GPIO_ReadPin(GPIOA, mpre_sense_pin)
#define a_pre_sense HAL_GPIO_ReadPin(GPIOA, apre_sense_pin)

#define m_pre_ready HAL_GPIO_ReadPin(GPIOB, mpre_ready_pin)
#define a_pre_ready HAL_GPIO_ReadPin(GPIOB, apre_ready_pin)

#define mt_fault(state) HAL_GPIO_WritePin(GPIOA, mt_fault_pin, state)
#define ms_fault(state) HAL_GPIO_WritePin(GPIOA, ms_fault_pin, state)
#define at_fault(state) HAL_GPIO_WritePin(GPIOB, at_fault_pin, state)
#define as_fault(state) HAL_GPIO_WritePin(GPIOA, as_fault_pin, state)

 typedef enum {
    OPEN,
    CLOSED,
    FAULT
} State;

typedef enum{
  NO_FAULT,
  MT_FAULT,
  MS_FAULT,
  AT_FAULT,
  AS_FAULT
} Fault;

#define M_TIMEOUT 1000
#define MPRE_TIMEOUT 1000
#define APRE_TIMEOUT 1000

/* USER CODE BEGIN Private defines */
StaticTask_t contactortask_buffer;
StackType_t contactortask_stack[configMINIMAL_STACK_SIZE];

StaticTask_t cantask_buffer;
StackType_t cantask_stack[configMINIMAL_STACK_SIZE];

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
