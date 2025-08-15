#include "StatusLEDs.h"
#include "Contactors.h"

/**
 * @brief   Helper function for reading precharge completion state
 *          Should only be called if contactor is a precharge contactor
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  The contactor's precharge completion state from the output of hardware comparison
 */
static bool getPrecharge(contactor_enum_t contactor) {
    switch (contactor) {
    case MOTOR_PRECHARGE_CONTACTOR:
        return HAL_GPIO_ReadPin(MOTOR_PRECHARGE_READY_PORT, MOTOR_PRECHARGE_READY_PIN);
        break;
    case ARRAY_PRECHARGE_CONTACTOR:
        return HAL_GPIO_ReadPin(ARRAY_PRECHARGE_READY_PORT, ARRAY_PRECHARGE_READY_PIN);
        break;
    default:
        break;
    }
    return false;
}

int main(void) {
    Status_Leds_Init();
    Contactors_Init();

    while (1) {
        if (getPrecharge(MOTOR_PRECHARGE_CONTACTOR)) {
            Status_Leds_Write(MOTOR_SENSE_FAULT_LED, true);
            Status_Leds_Write(MOTOR_TIMEOUT_FAULT_LED, false);
        } else {
            Status_Leds_Write(MOTOR_SENSE_FAULT_LED, false);
            Status_Leds_Write(MOTOR_TIMEOUT_FAULT_LED, true);
        }

        if (getPrecharge(ARRAY_PRECHARGE_CONTACTOR)) {
            Status_Leds_Write(ARRAY_SENSE_FAULT_LED, true);
            Status_Leds_Write(ARRAY_TIMEOUT_FAULT_LED, false);
        } else {
            Status_Leds_Write(ARRAY_SENSE_FAULT_LED, false);
            Status_Leds_Write(ARRAY_TIMEOUT_FAULT_LED, true);
        }
    }

    return 0;
}