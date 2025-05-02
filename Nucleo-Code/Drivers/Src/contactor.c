#include "contactor.h"

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

void Status_LEDS_Toggle(int led){
  switch(led){
    case MT_LED:
      HAL_GPIO_TogglePin(mt_fault_port, mt_fault_pin);
      break;
    case MS_LED:
      HAL_GPIO_TogglePin(ms_fault_port, ms_fault_pin);
      break;
    case AT_LED:
      HAL_GPIO_TogglePin(as_fault_port, as_fault_pin);
      break;
    case AS_LED:
      HAL_GPIO_TogglePin(as_fault_port, as_fault_pin);
      break;
    default:
      break;
  }
}

void Status_LEDS_Write(status_led_t led, int state){
  switch(led){
    case MS_LED:
      motor_sense_fault(state);
      break;
    case MT_LED:
      motor_timeout_fault(state);
      break;
    case AS_LED:
      array_sense_fault(state);
      break;
    case AT_LED:
      array_timeout_fault(state);
      break;
    default:
      break;
  }
}

void error_handler(void)
{
  while (1)
  {
    Status_LEDS_Toggle(MT_LED);
    Status_LEDS_Toggle(MS_LED);
    Status_LEDS_Toggle(AT_LED);
    Status_LEDS_Toggle(AS_LED);
  }
}

void success_handler(void) {
  while(1){
    // mt_fault(1);
    // HAL_Delay(1000);
    // mt_fault(0);
    // HAL_Delay(1000);
  }
}
