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

typedef struct Contactor
{
  int state; // open, closed
  int enable_in;
  int sense;  // GPIO pin to read the contactor's state
  int fault;
  uint32_t start_time; // HAL_getTick()
} Contactor;

typedef struct prechargeContactor
{
  int state;     // open, closed
  int pre_ready; // Precharge ready signal
  int sense;
  int enable_out; // GPIO pin to control the contactor
  int fault;
  uint32_t start_time;
} prechargeContactor;

Contactor motor = {OPEN, OPEN, OPEN, NO_FAULT, 0};
Contactor array = {OPEN, OPEN, OPEN, NO_FAULT, 0};
prechargeContactor motorPre = {OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0};
prechargeContactor arrayPre = {OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0};
int time;
int fault[10];
uint32_t mPre_initial_delay = 0;
uint32_t aPre_initial_delay = 0;
uint32_t mPre_timeout = 0;
uint32_t aPre_timeout = 0;
bool disable_motorPre = false; //semaphore, true when motor sucsessfully disabled in event in turn off sequence
bool disable_arrayPre = false; //semaphore, true when array sucsessfully disabled whenever its supposed to be
bool CAN_fault = false; // in CAN handler set this to true for any car faults such as BPS unsafe

/* unused globals
int BPS_safe;
int moco_fault;
int cycles = 0;
int on = 1;
*/


void contactorTask(void *pvParamters) {

  for (int i = 0; i < 10; i++){ //initialize system to be faultless
    fault[i] = NO_FAULT;
  }

  while (false){ //change this to wait here for BPS safe
    //waiting for BPS safe or waiting for HV+/- contactors to close
    //no need to execute through the while loop on start up before BPS safe
    //no HV behavior to control
  }


  while (1)
  {
    time = HAL_GetTick();

    //logic working and validated 4/29/2025
    #ifdef validated_logic
    
    //read pins
    motor.enable_in = (m_direct());
    motor.sense = m_sense();
    motorPre.pre_ready = m_pre_ready();
    motorPre.sense = m_pre_sense();

    //array.enable_in = from CAN  //fix this
    //array.sense = from CAN //fix this
    arrayPre.pre_ready = a_pre_ready();
    arrayPre.sense = a_pre_sense();

    //handle faults
    if((fault[0]| fault[1] | fault[2] | fault[3] | fault[4] | fault[5]| fault[6] | fault[7] | fault[8] | fault[9] |CAN_fault) == FAULT){

      //disable contactors under our domain (m_pre and a_pre)
      m_pre_enable(0);
      a_pre_enable(0);

      //send CAN specific message based on fault
      // if (CAN_fault) {do nothing, only disable contactors;}
      if (fault[0]){  //m_enable unplug       
        ms_fault(1); 
        mt_fault(1); 
        //send this specific CAN message
      } 
      if (fault[1]){  //motor sense fault
        ms_fault(1); 
        //send this specific CAN message
      } 
      if (fault[2]){  //motor timeout fault
        mt_fault(1); 
        //send this specific CAN message
      } 
      if (fault[3]){  //motorPre sense fault
        ms_fault(1); 
        //send this specific CAN message
      } 
      if (fault[4]){  //motorPre timeout fault
        mt_fault(1); 
        //send this specific CAN message
      } 
      if (fault[5]){  //a_enable fault       
        as_fault(1); 
        at_fault(1); 
        //send this specific CAN message
      } 
      if (fault[6]){  //array sense fault
        as_fault(1); 
        //send this specific CAN message
      } 
      if (fault[7]){  //array timeout fault
        at_fault(1); 
        //send this specific CAN message
      } 
      if (fault[8]){  //arrayPre sense fault
        as_fault(1); 
        //send this specific CAN message
      } 
      if (fault[9]){  //arrayPre timeout fault
        at_fault(1); 
        //send this specific CAN message
      } 

    }

    else{ //this else executes if no faults

      //motor logic
      if (motor.enable_in == CLOSED && motor.state == OPEN) {
        if(motor.start_time == 0){ 
          motor.start_time = time;
        }
        else if ((motor.sense == OPEN) && (motor.start_time + 100 < time)){  //if 100ms after contactor enabled, fault, not reading sense right
          fault[1] = FAULT; //motor sense fault
        }
        else if(motor.sense==CLOSED){
          if (mPre_initial_delay == 0){
            mPre_initial_delay = time;
          }
          else if (mPre_initial_delay + 100 < time){  //bake in 100ms of delay for the contactor to fully close and reset precharge logic
            motor.state = CLOSED;
            motor.start_time = 0;
            mPre_initial_delay = 0;
            mPre_timeout = time;
          }
        }
      }
      else if (motor.enable_in == OPEN && motor.state == CLOSED){
        if (false) { //change false to say if were in the right part if ignition state for m.enable to go low, then
          if (motor.start_time == 0){
            motor.start_time = time;
          }
          else if ((motor.sense == CLOSED) && (motor.start_time + 100 < time)){
            fault[2] = FAULT; //motor timeout fault
          }
          else if (motor.sense == OPEN){
            motor.state =  OPEN;
            motor.start_time = 0;
            disable_motorPre = true;  //sempahore, turn off motorPre
          }
        }
        else {
          if (motor.start_time == 0){
            motor.start_time = time;
          }
          else if (motor.start_time + 10 < time){
            fault[0] = FAULT; //otherwise, m.enable probably unplugged, fault
          }
        }
        
      } 
      else if(motor.enable_in == OPEN && motor.state == OPEN){ //if conactor is unenabled and sense high, wait 10ms and if still true, fault
        if(motor.sense == CLOSED){
          if(motor.start_time == 0){ 
            motor.start_time = time;
          }
          else if(motor.start_time + 10 > time){
            fault[1] = FAULT; //motor sense fault
          }
        }
        else motor.start_time = 0; 
      }
      else if(motor.enable_in == CLOSED && motor.state == CLOSED){ //if conactor is enabled and sense low, wait 10ms and if still true, fault
        if(motor.sense == OPEN){
          if(motor.start_time == 0){
            motor.start_time = time;
          }
          else if(motor.start_time + 10 > time){
            fault[1] = FAULT; //motor sense fault
          }
        }
        else motor.start_time = 0;
      }

      //motorPre logic
      if (motor.state == CLOSED){ //handle event where pre_ready is high bc motor.state was just open and enabling motorPre after
        if (motorPre.pre_ready == OPEN){
          if (mPre_timeout == 0){
            mPre_timeout = time;
          } 
          else if (motorPre.state == CLOSED && mPre_timeout + 100 < time) {
            fault[4] = FAULT; //motor precharge steady state timeout fault
          }
          else if (mPre_timeout + 1000 < time){
            fault[4] = FAULT; //motor precharge initial timeout fault
          }
        }
        else if (motorPre.pre_ready == CLOSED){
          m_pre_enable(1);
          if(motorPre.start_time == 0){ 
            motorPre.start_time = time;
          }
          else if ((motorPre.sense == OPEN) && (motorPre.start_time + 100 < time)){  //if 100ms after contactor enabled, fault, not reading sense right
            fault[3] = FAULT; //motor precharge sense fault 
          }
          else if (motorPre.sense == CLOSED){
            motorPre.state = CLOSED;
            motorPre.start_time = 0;
            mPre_timeout = 0;
          }
        }
      }
      else if (disable_motorPre){ //handle motorPre disable when motor was just disabled
        if (motorPre.start_time == 0){
          motorPre.start_time = time;
          m_pre_enable(0);
        }
        else if (motorPre.sense == CLOSED && motorPre.start_time + 100 < time){
          fault[4] = FAULT; //motor precharge turn off timeout fault
        }
        else if (motorPre.sense == OPEN){
          motorPre.state = OPEN;
          motorPre.start_time = 0;
          disable_motorPre = false;
        }
      }
    
      /* array logic never tested before but identical to motor
      //array logic
      if (array.enable_in == CLOSED && array.state == OPEN) {
        if(array.start_time == 0){ 
          array.start_time = time;
        }
        else if ((array.sense == OPEN) && (array.start_time + 100 < time)){  //if 100ms after contactor enabled, fault, not reading sense right
          fault[6] = FAULT; //array sense fault
        }
        else if(array.sense==CLOSED){
          if (aPre_initial_delay == 0){
            aPre_initial_delay = time;
          }
          else if (aPre_initial_delay + 100 < time){  //bake in 100ms of delay for the contactor to fully close and reset precharge logic
            array.state = CLOSED;
            array.start_time = 0;
            aPre_initial_delay = 0;
            aPre_timeout = time;
          }
        }
      }
      else if (array.enable_in == OPEN && array.state == CLOSED){
        if (false) { //change false to say if were in the right part if ignition state for a.enable to go low, then
          if (array.start_time == 0){
            array.start_time = time;
          }
          else if ((array.sense == CLOSED) && (array.start_time + 100 < time)){
            fault[7] = FAULT;  //array timeout fault
          }
          else if (array.sense == OPEN){
            array.state =  OPEN;
            array.start_time = 0;
            disable_arrayPre = true;  //sempahore, turn off arrayPre
          }
        }
        else {
          if (array.start_time == 0){
            array.start_time = time;
          }
          else if (array.start_time + 10 < time){
            fault[5] = FAULT; //otherwise, were not in the right part of ignition state for this to happen, fault
          }
        }
        
      } 
      else if(array.enable_in == OPEN && array.state == OPEN){ //if conactor is unenabled and sense high, wait 10ms and if still true, fault
        if(array.sense == CLOSED){
          if(array.start_time == 0){ 
            array.start_time = time;
          }
          else if(array.start_time + 10 > time){
            fault[6] = FAULT; //array sense fault
          }
        }
        else array.start_time = 0; 
      }
      else if(array.enable_in == CLOSED && array.state == CLOSED){ //if conactor is enabled and sense low, wait 10ms and if still true, fault
        if(array.sense == OPEN){
          if(array.start_time == 0){
            array.start_time = time;
          }
          else if(array.start_time + 10 > time){
            fault[6] = FAULT; //array sense fault
          }
        }
        else array.start_time = 0;
      }

      //arrayPre logic
      if (array.state == CLOSED){ //handle event where pre_ready is high bc array.state was just open and enabling arrayPre after
        if (arrayPre.pre_ready == OPEN){
          if (aPre_timeout == 0){
            aPre_timeout = time;
          } 
          else if (arrayPre.state == CLOSED && aPre_timeout + 100 < time) {
            fault[9] = FAULT; //array precharge steady state timeout fault
          }
          else if (aPre_timeout + 1000 < time){
            fault[9] = FAULT; //array precharge initial timeout fault
          }
        }
        else if (arrayPre.pre_ready == CLOSED){
          a_pre_enable(1);
          if(arrayPre.start_time == 0){ 
            arrayPre.start_time = time;
          }
          else if ((arrayPre.sense == OPEN) && (arrayPre.start_time + 100 < time)){  //if 100ms after contactor enabled, fault, not reading sense right
            fault[8] = FAULT; //array precharge sense fault
          }
          else if (arrayPre.sense == CLOSED){
            arrayPre.state = CLOSED;
            arrayPre.start_time = 0;
            aPre_timeout = 0;
          }
        }
      }
      else if (disable_arrayPre){ //handle arrayPre disable when array was just disabled
        if (arrayPre.start_time == 0){
          arrayPre.start_time = time;
          a_pre_enable(0);
        }
        else if (arrayPre.sense == CLOSED && arrayPre.start_time + 100 < time){
          fault[9] = FAULT; //array precharge turn off timeout fault
        }
        else if (arrayPre.sense == OPEN){
          arrayPre.state = OPEN;
          arrayPre.start_time = 0;
          disable_arrayPre = false;
        }
      }
      */
    }

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