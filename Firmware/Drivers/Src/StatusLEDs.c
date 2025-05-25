#include "StatusLEDs.h"

/**
* @brief Initializes Status LED pins
*
*/
void Status_Leds_Init(void) {
    GPIO_InitTypeDef led_a_init = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Pin = (ARRAY_SENSE_FAULT_LED_PIN | MOTOR_SENSE_FAULT_LED_PIN | MOTOR_TIMEOUT_FAULT_LED_PIN)
    };

    __HAL_RCC_GPIOA_CLK_ENABLE();
    HAL_GPIO_Init(GPIOA, &led_a_init);

    GPIO_InitTypeDef led_b_init = {
      .Mode = GPIO_MODE_OUTPUT_PP,
      .Pull = GPIO_NOPULL,
      .Pin = (ARRAY_TIMEOUT_FAULT_LED_PIN)
    };
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_Init(GPIOB, &led_b_init);
}

/**
 * @brief   Writes data to a specified pin
 * @param   led The led to write to
 * @param   state true=ON or false=OFF
 * @return  None
 */
void Status_Leds_Write(status_led_t led, bool state) {
    switch (led) {
    case MOTOR_SENSE_FAULT_LED:
        HAL_GPIO_WritePin(MOTOR_SENSE_FAULT_LED_PORT, MOTOR_SENSE_FAULT_LED_PIN, state);
        break;
    case MOTOR_TIMEOUT_FAULT_LED:
        HAL_GPIO_WritePin(MOTOR_TIMEOUT_FAULT_LED_PORT, MOTOR_TIMEOUT_FAULT_LED_PIN, state);
        break;
    case ARRAY_SENSE_FAULT_LED:
        HAL_GPIO_WritePin(ARRAY_SENSE_FAULT_LED_PORT, ARRAY_SENSE_FAULT_LED_PIN, state);
        break;
    case ARRAY_TIMEOUT_FAULT_LED:
        HAL_GPIO_WritePin(ARRAY_TIMEOUT_FAULT_LED_PORT, ARRAY_TIMEOUT_FAULT_LED_PIN, state);
        break;
    case ONBOARD_LED:
        HAL_GPIO_WritePin(ONBOARD_LED_PORT, ONBOARD_LED_PIN, state);
        break;
    default:
        break;
    }
}

/**
 * @brief   Toggles a status led
 * @param   led The led to toggle
 * @return  None
 */
void Status_Leds_Toggle(status_led_t led) {
    switch (led) {
    case MOTOR_SENSE_FAULT_LED:
        HAL_GPIO_TogglePin(MOTOR_SENSE_FAULT_LED_PORT, MOTOR_SENSE_FAULT_LED_PIN);
        break;
    case MOTOR_TIMEOUT_FAULT_LED:
        HAL_GPIO_TogglePin(MOTOR_TIMEOUT_FAULT_LED_PORT, MOTOR_TIMEOUT_FAULT_LED_PIN);
        break;
    case ARRAY_SENSE_FAULT_LED:
        HAL_GPIO_TogglePin(ARRAY_SENSE_FAULT_LED_PORT, ARRAY_SENSE_FAULT_LED_PIN);
        break;
    case ARRAY_TIMEOUT_FAULT_LED:
        HAL_GPIO_TogglePin(ARRAY_TIMEOUT_FAULT_LED_PORT, ARRAY_TIMEOUT_FAULT_LED_PIN);
        break;
    default:
        break;
    }

}

/**
 * @brief   Turns all status LEDs on
 * @return  None
 */
void Status_Leds_All_On(void) {
    for (int i = 0; i < NUM_STATUS_LED; i++) {
        Status_Leds_Write(i, true);
    }
}