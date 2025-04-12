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

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "CAN.h"
// #include "../Drivers/Inc/can.h"
// #include "initTask.c"


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
void error_handler(void);
void success_handler(void);
void task_Init();
void contactorTask();
void MX_GPIO_Init(void);
// void contactorCAN_Init();
// void contactorCANTask(void *pvParamters);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

// #define blinky 1
// #define cantest 2
// #define contactorcode 3
#define sensetest 4

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


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* CONTACTOR_H */
