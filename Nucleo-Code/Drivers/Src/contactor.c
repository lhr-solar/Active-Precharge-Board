#include "contactor.h"

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, MOTOR_ENABLE_PIN | MOTOR_PRECHARGE_ENABLE_PIN | ARRAY_PRECHARGE_ENABLE_PIN | MOTOR_TIMEOUT_FAULT_LED_PIN | MOTOR_SENSE_FAULT_LED_PIN | ARRAY_SENSE_FAULT_LED_PIN,
    GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ARRAY_TIMEOUT_FAULT_LED_PORT, ARRAY_TIMEOUT_FAULT_LED_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA4 */
  GPIO_InitStruct.Pin = MOTOR_SENSE_PIN | MOTOR_PRECHARGE_SENSE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA3 PA7 PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = MOTOR_ENABLE_PIN | MOTOR_PRECHARGE_ENABLE_PIN | ARRAY_PRECHARGE_ENABLE_PIN | MOTOR_TIMEOUT_FAULT_LED_PIN | MOTOR_SENSE_FAULT_LED_PIN | ARRAY_SENSE_FAULT_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB3 led */
  GPIO_InitStruct.Pin = ARRAY_TIMEOUT_FAULT_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ARRAY_TIMEOUT_FAULT_LED_PORT, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = MOTOR_PRECHARGE_READY_PIN | ARRAY_PRECHARGE_READY_PIN | ARRAY_PRECHARGE_SENSE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// TODO: fix status LEDs...
void Status_LEDS_Toggle(uint8_t statusLED) {
  // switch(led){
  //   case MT_LED:
  //     HAL_GPIO_TogglePin(mt_fault_port, mt_fault_pin);
  //     break;
  //   case MS_LED:
  //     HAL_GPIO_TogglePin(ms_fault_port, ms_fault_pin);
  //     break;
  //   case AT_LED:
  //     HAL_GPIO_TogglePin(as_fault_port, as_fault_pin);
  //     break;
  //   case AS_LED:
  //     HAL_GPIO_TogglePin(as_fault_port, as_fault_pin);
  //     break;
  //   default:
  //     break;
  // }
}

void Status_LEDS_Write(status_led_t led, uint8_t state) {
  // switch(led){
  //   case MS_LED:
  //     motor_sense_fault(state);
  //     break;
  //   case MT_LED:
  //     motor_timeout_fault(state);
  //     break;
  //   case AS_LED:
  //     array_sense_fault(state);
  //     break;
  //   case AT_LED:
  //     array_timeout_fault(state);
  //     break;
  //   default:
  //     break;
  // }
}

void error_handler(void) {
  while (1)
  {
    // Status_LEDS_Toggle(MT_LED);
    // Status_LEDS_Toggle(MS_LED);
    // Status_LEDS_Toggle(AT_LED);
    // Status_LEDS_Toggle(AS_LED);
  }
}

void success_handler(void) {
  while (1) {
    // mt_fault(1);
    // HAL_Delay(1000);
    // mt_fault(0);
    // HAL_Delay(1000);
  }
}
