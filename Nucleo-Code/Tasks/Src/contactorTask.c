#include "contactor.h"

int m_direct(){return HAL_GPIO_ReadPin(GPIOA, m_enable_pin);}

void m_pre_enable(int state){ HAL_GPIO_WritePin(GPIOA, mpre_enable_pin, state);} 
void a_pre_enable(int state){ HAL_GPIO_WritePin(GPIOA, apre_enable_pin, state);}

int m_sense(){return HAL_GPIO_ReadPin(GPIOA, m_sense_pin);}
int m_pre_sense(){return HAL_GPIO_ReadPin(GPIOA, mpre_sense_pin);}
int a_pre_sense(){return HAL_GPIO_ReadPin(GPIOA, apre_sense_pin);}

int m_pre_ready(){return HAL_GPIO_ReadPin(GPIOB, mpre_ready_pin);}
int a_pre_ready(){return HAL_GPIO_ReadPin(GPIOB, apre_ready_pin);}

void mt_fault(int state){HAL_GPIO_WritePin(GPIOA, mt_fault_pin, state);}
void ms_fault(int state){HAL_GPIO_WritePin(GPIOA, ms_fault_pin, state);}
void at_fault(int state){HAL_GPIO_WritePin(GPIOB, at_fault_pin, state);}
void as_fault(int state){HAL_GPIO_WritePin(GPIOA, as_fault_pin, state);}

void error_handler(void)
{
  while (1)
  {
    ms_fault(1);
    HAL_Delay(1000);
    ms_fault(0);
    HAL_Delay(1000);
  }
}

void success_handler(void) {
  while(1){
    mt_fault(1);
    HAL_Delay(1000);
    mt_fault(0);
    HAL_Delay(1000);
  }
}



/*
// void SystemClock_Config(void)
// {
//   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

//   // Configure the main internal regulator output voltage
//   
//   if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
//   {
//     error_handler();
//   }

//   // Initializes the RCC Oscillators according to the specified parameters
//   // in the RCC_OscInitTypeDef structure.
//   
//   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//   RCC_OscInitStruct.HSIState = RCC_HSI_ON;
//   RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
//   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
//   RCC_OscInitStruct.PLL.PLLM = 2;
//   RCC_OscInitStruct.PLL.PLLN = 25;
//   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
//   RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
//   RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
//   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//   {
//     error_handler();
//   }

//   // Initializes the CPU, AHB and APB buses clocks
//   
//   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

//   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
//   {
//     error_handler();
//   }
// }
*/


/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, m_enable_pin | mpre_enable_pin | apre_enable_pin | mt_fault_pin | ms_fault_pin | as_fault_pin,
                    GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, at_fault_pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA4 */
  GPIO_InitStruct.Pin = m_sense_pin | mpre_sense_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA3 PA7 PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = m_enable_pin | mpre_enable_pin | apre_enable_pin | mt_fault_pin | ms_fault_pin | as_fault_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : VCP_RX_Pin */
  // GPIO_InitStruct.Pin = VCP_RX_Pin;
  // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  // GPIO_InitStruct.Pull = GPIO_NOPULL;
  // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  // GPIO_InitStruct.Alternate = GPIO_AF3_USART2;
  // HAL_GPIO_Init(VCP_RX_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 led */
  GPIO_InitStruct.Pin = at_fault_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = mpre_ready_pin | apre_ready_pin | apre_sense_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

typedef struct motorContactor
{
  int state; // open, closed
  int enable_in;
  int sense;  // GPIO pin to read the contactor's state
  // int enable_out; // GPIO pin to control the contactor
  int fault;
  uint32_t start_time; // HAL_getTick()
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
  uint32_t start_time;
  // const struct Precharge *next[8];
} Precharge;

motorContactor motor = {OPEN, OPEN, OPEN, NO_FAULT, 0};
Precharge motorPre = {OPEN, OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0};
Precharge arrayPre = {OPEN, OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0};
int time;
int BPS_safe;
int moco_fault;
int fault[5];
int on = 1;
int cycles = 0;

void contactorTask(void *pvParamters)
{
  // HAL_Delay(5000);
  while (1)
  {
    time = HAL_GetTick();
    // drive contactor based on sense
    #ifdef sensetest
    
    motor.enable_in = (m_direct());
    motor.sense = m_sense();

    motorPre.pre_ready = m_pre_ready();
    motorPre.pre_sense = m_pre_sense();
    motorPre.sense_in = motor.sense;

    if(motor.fault==NO_FAULT){
      if(motor.state==OPEN && motor.enable_in==CLOSED){ // closing motor
        if(motor.start_time==0){ //
          motor.start_time = time;
        }else if(motor.start_time+1000<time){
          motor.fault = MT_FAULT;
        }else if(motor.sense==CLOSED){
          motorPre.pre_sense = CLOSED;
          motor.state = CLOSED;
          motor.start_time = 0;
        }
      }else if(motor.state==CLOSED && motor.enable_in == OPEN){ // opening motor
        if(motor.start_time == 0){
          motor.start_time = time;
        }else if(motor.start_time+1000<time){
          motor.fault = MT_FAULT;
        }else if(motor.sense==OPEN){
          motor.state = OPEN;
          motorPre.sense_in = OPEN;
          motor.start_time = 0;
        }
      }else if(motor.state == OPEN && motor.enable_in == OPEN){
        if(motor.sense == CLOSED){
          if(motor.start_time==0){
            motor.start_time = time;
          }else if(motor.start_time+10>time){
            motor.fault = MS_FAULT;
          }
        }else{
          motor.start_time = 0;
        }
      }else if(motor.state == CLOSED && motor.enable_in == CLOSED){
        if(motor.sense == OPEN){
          if(motor.start_time==0){
            motor.start_time = time;
          }else if(motor.start_time+10>time){
            motor.fault =MS_FAULT;
          }
        }else{
          motor.start_time = 0;
        }
      }
    }else{
      fault[motor.fault] = 1;
    }

    if(motorPre.fault==NO_FAULT){
      if(motorPre.sense_in == OPEN){
        if(motorPre.pre_ready == CLOSED){
          motorPre.fault = MS_FAULT;
        }
      }else if(motorPre.state==OPEN && motorPre.pre_ready==CLOSED){ // closing motorPre
        if(motorPre.start_time==0){ //
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MT_FAULT;
        }else if(motorPre.pre_sense==CLOSED){
          motorPre.state = CLOSED;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state==CLOSED && motorPre.pre_ready == OPEN){ // opening motorPre
        if(motorPre.start_time == 0){
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MT_FAULT;
        }else if(motorPre.pre_sense==OPEN){
          motorPre.state = OPEN;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == OPEN && motorPre.pre_ready == OPEN){
        if(motorPre.pre_sense == CLOSED){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MS_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == CLOSED && motorPre.pre_ready == CLOSED){
        if(motorPre.pre_sense == OPEN){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MS_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }
    }else{
      fault[motorPre.fault] = 1;
    }

    if((fault[0]| fault[1] | fault[2] | fault[3] | fault[4]) == NO_FAULT){
      if(motorPre.start_time<time && motorPre.start_time!=0){
        m_pre_enable(motorPre.pre_ready);
      }
      // m_pre_enable(m_sense());
    }else{
      m_pre_enable(0);
    }

    mt_fault(fault[MT_FAULT]);
    ms_fault(fault[MS_FAULT]);

    #endif

    // blinky tester
    #ifdef blinky
    m_pre_enable(1);
    mt_fault(1);
    a_pre_enable(1);
    HAL_Delay(1000);
    m_pre_enable(0);
    mt_fault(0);
    a_pre_enable(0);
    HAL_Delay(1000);
    #endif

    #ifdef contactorcode
    // Read Inputs
    motor.enable_in = (m_direct());
    motor.sense = m_sense();

    motorPre.pre_ready = m_pre_ready();
    motorPre.pre_sense = m_pre_sense();
    // motorPre.sense_in = motor.sense;

    arrayPre.pre_ready = a_pre_ready();
    arrayPre.pre_sense = a_pre_sense();

    motorPre.enable_out = m_pre_ready();
    arrayPre.enable_out = a_pre_ready();

    //Update States and Faults
    if(motor.fault==NO_FAULT){
      if(motor.state==OPEN && motor.enable_in==CLOSED){ // closing motor
        if(motor.start_time==0){ //
          motor.start_time = time;
        }else if(motor.start_time+1000<time){
          motor.fault = MT_FAULT;
        }else if(motor.sense==CLOSED){
          motorPre.pre_sense = CLOSED;
          motor.state = CLOSED;
          motor.start_time = 0;
        }
      }else if(motor.state==CLOSED && motor.enable_in == OPEN){ // opening motor
        if(motor.start_time == 0){
          motor.start_time = time;
        }else if(motor.start_time+1000<time){
          motor.fault = MT_FAULT;
        }else if(motor.sense==OPEN){
          motor.state = OPEN;
          motorPre.sense_in = OPEN;
          motor.start_time = 0;
        }
      }else if(motor.state == OPEN && motor.enable_in == OPEN){
        if(motor.sense == CLOSED){
          if(motor.start_time==0){
            motor.start_time = time;
          }else if(motor.start_time+10>time){
            motor.fault = MS_FAULT;
          }
        }else{
          motor.start_time = 0;
        }
      }else if(motor.state == CLOSED && motor.enable_in == CLOSED){
        if(motor.sense == OPEN){
          if(motor.start_time==0){
            motor.start_time = time;
          }else if(motor.start_time+10>time){
            motor.fault =MS_FAULT;
          }
        }else{
          motor.start_time = 0;
        }
      }
    }else{
      fault[motor.fault] = 1;
    }

    if(motorPre.fault==NO_FAULT){
      if(motorPre.state==OPEN && motorPre.sense_in==CLOSED){ // closing motorPre
        if(motorPre.start_time==0){ //
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MT_FAULT;
        }else if(motorPre.pre_sense==CLOSED){
          motorPre.state = CLOSED;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state==CLOSED && motorPre.sense_in == OPEN){ // opening motorPre
        if(motorPre.start_time == 0){
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MT_FAULT;
        }else if(motorPre.pre_sense==OPEN){
          motorPre.state = OPEN;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == OPEN && motorPre.sense_in == OPEN){
        if(motorPre.pre_sense == CLOSED){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MS_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == CLOSED && motorPre.sense_in == CLOSED){
        if(motorPre.pre_sense == OPEN){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MS_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }
    }else{
      fault[motorPre.fault] = 1;
    }
    
    if(arrayPre.fault==NO_FAULT){
      if(arrayPre.state==OPEN && arrayPre.sense_in==CLOSED){ // closing arrayPre
        if(arrayPre.start_time==0){ //
          arrayPre.start_time = time;
        }else if(arrayPre.start_time+1000<time){
          arrayPre.fault = AT_FAULT;
        }else if(arrayPre.pre_sense==CLOSED){
          arrayPre.state = CLOSED;
          arrayPre.start_time = 0;
        }
      }else if(arrayPre.state==CLOSED && arrayPre.sense_in == OPEN){ // opening arrayPre
        if(arrayPre.start_time == 0){
          arrayPre.start_time = time;
        }else if(arrayPre.start_time+1000<time){
          arrayPre.fault = AT_FAULT;
        }else if(arrayPre.pre_sense==OPEN){
          arrayPre.state = OPEN;
          arrayPre.start_time = 0;
        }
      }else if(arrayPre.state == OPEN && arrayPre.sense_in == OPEN){
        if(arrayPre.pre_sense == CLOSED){
          if(arrayPre.start_time==0){
            arrayPre.start_time = time;
          }else if(arrayPre.start_time+10>time){
            arrayPre.fault = AS_FAULT;
          }
        }else{
          arrayPre.start_time = 0;
        }
      }else if(arrayPre.state == CLOSED && arrayPre.sense_in == CLOSED){
        if(arrayPre.pre_sense == OPEN){
          if(arrayPre.start_time==0){
            arrayPre.start_time = time;
          }else if(arrayPre.start_time+10>time){
            arrayPre.fault = AS_FAULT;
          }
        }else{
          arrayPre.start_time = 0;
        }
      }
    }else{
      fault[arrayPre.fault] = 1;
    }
    if(arrayPre.start_time == 0 && arrayPre.enable_out == CLOSED){
      arrayPre.start_time = time;
    }

    //Check Faults

    if ((motor.fault | motorPre.fault | arrayPre.fault | BPS_safe | moco_fault) == NO_FAULT)
    {
      motorPre.enable_out = motorPre.sense_in & motorPre.pre_ready;
      arrayPre.enable_out = arrayPre.sense_in & arrayPre.pre_ready;
    }else{
      motorPre.enable_out = 0;
      arrayPre.enable_out = 0;
      //sense fault on CAN
      // at_fault(1);
      if(arrayPre.fault==AS_FAULT){
        at_fault(1);
      }
    }
    mt_fault(fault[MT_FAULT]);
    ms_fault(fault[MS_FAULT]);
    at_fault(fault[AT_FAULT]);
    as_fault(fault[AS_FAULT]);

    
    m_pre_enable(motorPre.enable_out);
    a_pre_enable(arrayPre.enable_out);

    #endif

    // HAL_Delay(500);
    // HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5));
    //   if ((motor.enable_in == 1) && (motor.fault == 0)) {
    //     if (){
    //       motor.fault = 1;
    //     }

    //     if (motor.enable_out == 0) {
    //       motor.enable_out = 1;
    //       motor.start_time = HAL_GetTick();
    //     } else if (motor.sense_out == 0) {
    //       if (HAL_GetTick() > motor.start_time + 1000) {
    //         motor.timeout = 1;
    //       }

    //     }
    //   }else if(motor.enable_in == 0 && motor.fault == 0){
    //     if(motor.enable_out = 1){
    //       motor.enable_out = 0;
    //       motor.start_time = HAL_GetTick();
    //     }else if(motor.sense_out == 1){
    //       if(HAL_GetTick()>motor.start_time+1000){
    //         motor.timeout = 1;
    //       }
    //     }
    //   }

    //   if ((arrayPre.enable_out == 0) && (arrayPre.pre_sense == 0) &&
    //       (arrayPre.fault == 0)) {
    //     // if contactor was already open (state = 0) and CAN
    //     // tells it to close and no fault
    //     arrayPre.enable_out = 1;  // enable contactor
    //     arrayPre.start_time = HAL_GetTick();
    //   }
    // }

    // contactor logic

    // if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)) ) {
    //   // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    //   HAL_Delay(500);
    //   // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1);
    //   m_state = 1;
    // }else {
    //   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0);
    // }

    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, ~HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7));
    // int output = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
    // int out1 = output;
    // if(output!=0){
    //   out1=0;
    // }else{
    //   out1=1;
    // }
    // HAL_GPIO_WritePin(GPIOA, ( GPIO_PIN_6), out1);
    // HAL_GPIO_WritePin(GPIOA, (GPIO_PIN_1),out1);//HAL_GPIO_ReadPin(GPIOB,
    // GPIO_PIN_6)); HAL_GPIO_WritePin(GPIOA, (GPIO_PIN_4), output);//
    // HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6));

    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 1);
    // m_enable(GPIO_PIN_SET);

    // // if (m_CAN == 0) {  // if CAN tells contactor to close
    // //   m_enable(0);
    // //   m_state = 0;
    // // }
    // if (a_CAN == 0) {  // if CAN tells contactor to close
    //   a_enable(0);
    //   a_state = 0;
    // }

    // if ((m_sense == 1) && (m_pre_ready == 1) && (m_fault == 0) &&
    //     (m_pre_fault == 0)) {  // if m_contactor is closed (sense = 0) and
    //                            // precharge ready and neither has faulted
    //   m_pre_enable(1);
    //   m_pre_state = 1;
    // }
    // if ((a_sense == 1) && (a_pre_ready == 1) && (a_fault == 0) &&
    //     (a_pre_fault == 0)) {  // if a_contactor is closed (sense = 0) and
    //                            // precharge ready and neither has faulted
    //   a_pre_enable(1);
    //   a_pre_state = 1;
    // }

    // if ((m_sense == 0) &&
    //     (m_pre_sense ==
    //      1)) {          // if m contactor open and m_pre contactor is closed
    //   m_pre_enable(0);  // turn m_pre contactor off
    //   m_pre_state = 0;  // set state info off
    // }
    // if ((a_sense == 0) &&
    //     (a_pre_sense ==
    //      1)) {          // if a contactor open and a_pre contactor is closed
    //   a_pre_enable(0);  // turn a_pre contactor off
    //   a_pre_state = 0;  // set state info off
    // }

    // if ((m_fault == 1) || (m_pre_fault == 1)) {  // if either faults
    //   m_pre_enable(
    //       0);  // disable precharge for saftey and send fault message over CAN
    //   m_pre_state = 0;
    // }
    // if ((a_fault == 1) || (a_pre_fault == 1)) {  // if either faults
    //   a_pre_enable(
    //       0);  // disable precharge for saftey and send fault message over CAN
    //   a_pre_state = 0;
    // }
  }
}




/*
int main()
{
  HAL_Init();
  HAL_MspInit();
  MX_GPIO_Init();
  Task_Init();
  SystemClock_Config();

  
  // as_fault(1);
  // HAL_Delay(5000);
  // as_fault(0);

  // xTaskCreateStatic(canTask, "can task", configMINIMAL_STACK_SIZE,
  //   NULL, tskIDLE_PRIORITY + 2, cantask_stack, &cantask_buffer);
  // xTaskCreateStatic(contactorTask, "contactor task", configMINIMAL_STACK_SIZE,
  //                   NULL, tskIDLE_PRIORITY + 3, contactortask_stack, &contactortask_buffer);

  vTaskStartScheduler();

  error_handler();
  return 0;
}
*/