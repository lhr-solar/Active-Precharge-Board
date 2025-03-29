#include "contactor.h"

#include "stm32xx_hal.h"

#define m_enable_pin GPIO_PIN_0
#define mpre_enable_pin GPIO_PIN_3
#define apre_enable_pin GPIO_PIN_7
#define m_sense_pin GPIO_PIN_1
#define mpre_sense_pin GPIO_PIN_4
#define apre_sense_pin GPIO_PIN_2
#define mt_fault_pin GPIO_PIN_10
#define ms_fault_pin GPIO_PIN_8
#define at_fault_pin GPIO_PIN_3   //B PLEASE REMEMBER
#define as_fault_pin GPIO_PIN_9
#define mpre_ready_pin GPIO_PIN_4 //B PLEASE REMEMBER
#define apre_ready_pin GPIO_PIN_5 //B PLEASE REMEMBER

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
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

  /*Configure GPIO pins : PA0 PA2 PA3 PA5
                           PA7 */
  GPIO_InitStruct.Pin = m_sense_pin | mpre_sense_pin | apre_sense_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA4 PA6 PA9 PA10 */
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

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = at_fault_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = mpre_ready_pin | apre_ready_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

//  typedef enum {
//     OPEN,
//     CLOSED,
//     FAULT
// } State;

// #define start &contactorFSM[0]
// #define closing &contactorFSM[1]
// #define closed &contactorFSM[2]
// #define opening &contactorFSM[3]
// #define fault &contactorFSM[4]

// #define prestart &prechargeFSM[0]
// #define precharge &prechargeFSM[1]
// #define preclosing &prechargeFSM[2]
// #define preclosed &prechargeFSM[3]
// #define preopening &prechargeFSM[4]
// #define prefault &prechargeFSM[5]

typedef struct motorContactor {
  int state;  // open, closed
  int enable_in;
  int sense_out;   // GPIO pin to read the contactor's state
  int enable_out;  // GPIO pin to control the contactor
  int fault;
  int timeout;
  uint32_t start_time;  // HAL_getTick()
  // const struct motorContactor *next[4];
} motorContactor;

// const motorContactor motorContactorFSM[5] = {
//     {0, 0, 0, {start, fault, closing, fault}}, //open
//     {1, 0, 1, {opening, opening, closing, closed}},//closing
//     {1, 1, 1, {fault, opening, fault, closed}},//closed
//     {0, 1, 0, {start, opening, start, closed}},//opening
//     {0, 0, 0, {fault, fault, fault, fault}} // fault
// };

typedef struct Precharge {
  int state;      // open, closed
  int sense_in;   // GPIO pin to read the contactor's state
  int pre_ready;  // Precharge ready signal
  int pre_sense;
  int enable_out;  // GPIO pin to control the contactor
  int fault;
  int timeout;
  uint32_t start_time;
  // const struct Precharge *next[8];
} Precharge;

// const Contactor prechargeFSM[6] = {
//   {0, 0, 0, 0, {prestart, prefault, prestart, prefault, precharge, prefault,
//   precharge, prefault}}, //open {1, 0, 0, 0, {prestart, prefault, prestart,
//   prefault, precharge, prefault, preclosing, prefault}},//charging {1, 1, 0,
//   1, {prestart, prestart, prestart, prestart, precharge, prefault,
//   preclosing, preclosed}},//closing {1, 1, 1, 1, {prefault, prefault,
//   prefault, preopening, prefault, prefault, prefault, preclosed}},//closed
//   {0, 1, 1, 0, {prestart, preopening, prestart, preopening, preopening,
//   prefault, prestart, preopening}}, // opening {0, 0, 0, 0, {fault, fault,
//   fault, fault, fault, fault, fault, fault}} // fault
// };

// Contactor*
#define m_CAN HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9)      // D1 B
#define m_direct HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)  // D0 B

#define m_enable(state) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, state)  // A1 A
// #define a_enable(state) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, state)   // A
#define m_pre_enable(state) HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, state)  // D13 B
#define a_pre_enable(state) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, state)  // A5 A

#define m_sense HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)      // A2 A
#define a_sense HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)      // A3 A
#define m_pre_sense HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)  // A0 A
#define a_pre_sense HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)  // A6 A

#define m_pre_ready HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)  // D12 B
#define a_pre_ready HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)  // D11 B

motorContactor motor = {0, 0, 0, 0, 0, 0, 0};
Precharge motorPre = {0, 0, 0, 0, 0, 0};
Precharge arrayPre = {0, 0, 0, 0, 0, 0};
int time;
int BPS_safe;
int moco_fault;
int fault;
int on = 1;
int cycles =0;

static void contactorTask(void* pvParamters) {
  while(1){
  time = HAL_GetTick();
  /* // blinky tester
  m_pre_enable(1);
  m_enable(1);
  a_pre_enable(1);
  HAL_Delay(1000);
  m_pre_enable(0);
  m_enable(0);
  a_pre_enable(0);
  HAL_Delay(1000);
  */
  // if(time/1000 >cycles){
  //   if(on ==0){
  //     on = 1;
  //   }else{
  //     on = 0;
  //   }
  //   cycles++;
  // }
  // m_enable(on);
  // m_pre_enable(on);
  // a_pre_enable(on);
  // HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
  // m_enable(1);
  // return;

  // if ((motor.fault | motorPre.fault | arrayPre.fault | BPS_safe | moco_fault |
  //      motor.timeout | motorPre.timeout | arrayPre.timeout) != 0) {
  //   motor.enable_out = 0;
  //   motorPre.enable_out = 0;
  //   arrayPre.enable_out = 0;
  //   fault = 1;
  // } else {
    motor.enable_in = (m_CAN); //& m_direct);
    motor.sense_out = m_sense;

    motorPre.pre_ready = m_pre_ready;
    motorPre.pre_sense = m_pre_sense;
    motorPre.sense_in = motor.sense_out;

    arrayPre.pre_ready = a_pre_ready;
    arrayPre.pre_sense = a_pre_sense;
    arrayPre.sense_in = a_sense;

    motor.enable_out = motor.enable_in;
    motorPre.enable_out = motorPre.sense_in;
    arrayPre.enable_out = a_pre_ready;

    m_enable(motor.enable_out);
    m_pre_enable(motorPre.enable_out);
    // a_pre_enable(arrayPre.enable_out);
    a_pre_enable(1);

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

StaticTask_t task_buffer;
StackType_t task_stack[configMINIMAL_STACK_SIZE];

static void error_handler(void) {
  while (1) {
  }
}

int main() {
  HAL_Init();
  HAL_MspInit();
  MX_GPIO_Init();

  // __HAL_RCC_GPIOB_CLK_ENABLE(); // enable clock for GPIOA
  // HAL_GPIO_Init(GPIOB, &led_config); // initialize GPIOA with led_config

  // while(1){
  //     // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
  //     // update

  //     // error: send can error message
  //     HAL_Delay(1000);
  // }
  xTaskCreateStatic(contactorTask, "contactor task", configMINIMAL_STACK_SIZE,
                    NULL, tskIDLE_PRIORITY + 2, task_stack, &task_buffer);

  vTaskStartScheduler();

  error_handler();
  return 0;
}