#include "stm32xx_hal.h"

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
 static void MX_GPIO_Init(void)
 {
   GPIO_InitTypeDef GPIO_InitStruct = {0};
 /* USER CODE BEGIN MX_GPIO_Init_1 */
 /* USER CODE END MX_GPIO_Init_1 */
 
   /* GPIO Ports Clock Enable */
   __HAL_RCC_GPIOC_CLK_ENABLE();
   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
 
   /*Configure GPIO pin Output Level */
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_6, GPIO_PIN_RESET);
 
   /*Configure GPIO pin Output Level */
   HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
 
   /*Configure GPIO pins : PA0 PA2 PA3 PA5
                            PA7 */
   GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5
                           |GPIO_PIN_7;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
   /*Configure GPIO pins : PA1 PA4 PA6 */
   GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_6;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
   /*Configure GPIO pin : VCP_RX_Pin */
   GPIO_InitStruct.Pin = VCP_RX_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF3_USART2;
   HAL_GPIO_Init(VCP_RX_GPIO_Port, &GPIO_InitStruct);
 
   /*Configure GPIO pin : LD3_Pin */
   GPIO_InitStruct.Pin = LD3_Pin;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);
 
   /*Configure GPIO pins : PB4 PB5 */
   GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
 /* USER CODE BEGIN MX_GPIO_Init_2 */
 /* USER CODE END MX_GPIO_Init_2 */
 }

 typedef enum {
    OPEN,
    CLOSED,
    FAULT
} State;

typedef struct {
    int id; // 0=motor, 1=motorpre, 2=array, 3=arraypre
    State state;        // Current state of the contactor
    int enable_pin;     // GPIO pin to control the contactor
    int sense_pin;      // GPIO pin to read the contactor's state
    int pre_ready;      // Precharge ready signal
    State next[5];
} Contactor;

const Contactor* FSM{
    
};

int main(){
    HAL_Init();
    MX_GPIO_Init();
    
    // __HAL_RCC_GPIOB_CLK_ENABLE(); // enable clock for GPIOA
    // HAL_GPIO_Init(GPIOB, &led_config); // initialize GPIOA with led_config

    while(1){
        // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
        // update

        // error: send can error message
        HAL_Delay(1000);
    }

    return 0;
}