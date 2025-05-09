#include "Contactors.h"
#include "CANMetaData.h"
#include "StatusLEDs.h"

// Mutex to lock contactor state
static SemaphoreHandle_t contactorsMutex = NULL;
StaticSemaphore_t contactorsMutexBuffer;

// Array describes the state of the contactors
static contactor_t contactorState[NUM_CONTACTORS];

// Bitmap of current fault state
uint32_t fault_bitmap = 0;

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

/**
 * @brief   Helper function for setting contactors without mutex.
 *          Should only be called if mutex is held and struct contactor has been checked
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @param   state the state to set (ON/OFF)
 * @return  None
 */
static void setContactor(contactor_enum_t contactor, bool state) {
    switch (contactor) {
    case MOTOR_PRECHARGE_CONTACTOR:
        contactorState[MOTOR_PRECHARGE_CONTACTOR].state = state;
        HAL_GPIO_WritePin(MOTOR_PRECHARGE_ENABLE_PORT, MOTOR_PRECHARGE_ENABLE_PIN, state);
        break;
    case ARRAY_PRECHARGE_CONTACTOR:
        contactorState[ARRAY_PRECHARGE_CONTACTOR].state = state;
        HAL_GPIO_WritePin(ARRAY_PRECHARGE_ENABLE_PORT, ARRAY_PRECHARGE_ENABLE_PIN, state);
        break;
    default:
        break;
    }
}

/**
 * @brief   Helper function for handling precharge timeout fault - opens contactors, sets status LEDs, sends fault CAN message
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  None
 */
static void prechargeTimeoutFaultHandler(contactor_enum_t contactor) {
    // Disable all contactors regardless of which faulted
    setContactor(MOTOR_PRECHARGE_CONTACTOR, OFF);
    setContactor(ARRAY_PRECHARGE_CONTACTOR, OFF);

    // Setup fault CAN message
    CAN_TxHeaderTypeDef tx_header = { 0 };
    tx_header.StdId = PRECHARGE_TIMEOUT_;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.IDE = CAN_ID_STD;
    tx_header.DLC = 2;
    tx_header.TransmitGlobalTime = DISABLE;
    uint8_t tx_data[8] = { 0 };

    // Turn on fault LEDs and set fault bits
    switch (contactor) {
    case MOTOR_PRECHARGE_CONTACTOR:
        Status_Leds_Write(MOTOR_TIMEOUT_FAULT_LED, true);
        fault_bitmap |= FAULT_MOTOR_PRECHARGE_TIMEOUT;

        // Send fault CAN message
        tx_data[0] = (1 << 0); // Motor Precharge Timeout
        if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();
        break;
    case ARRAY_PRECHARGE_CONTACTOR:
        Status_Leds_Write(ARRAY_TIMEOUT_FAULT_LED, true);
        fault_bitmap |= FAULT_ARRAY_PRECHARGE_TIMEOUT;

        // Send fault CAN message
        tx_data[0] = (1 << 1); // Array Precharge Timeout
        if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();
        break;
    default:
        break;
    }

    while (1) {

    }
}

/**
 * @brief   Initializes contactors to be used
 *          in connection with the Motor and Array
 * @return  None
 */
void Contactors_Init() {
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    // Enable clock for GPIO A/B
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Reset GPIO
    HAL_GPIO_WritePin(GPIOA, MOTOR_ENABLE_PIN | MOTOR_PRECHARGE_ENABLE_PIN | ARRAY_PRECHARGE_ENABLE_PIN,
        GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = MOTOR_SENSE_PIN | MOTOR_PRECHARGE_SENSE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOTOR_ENABLE_PIN | MOTOR_PRECHARGE_ENABLE_PIN | ARRAY_PRECHARGE_ENABLE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOTOR_PRECHARGE_READY_PIN | ARRAY_PRECHARGE_READY_PIN | ARRAY_PRECHARGE_SENSE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Start precharge contactors disabled
    setContactor(MOTOR_PRECHARGE_CONTACTOR, OFF);
    setContactor(ARRAY_PRECHARGE_CONTACTOR, OFF);

    // Create a mutex type semaphore for contactors, don't check if null since not dynamically allocated
    contactorsMutex = xSemaphoreCreateMutexStatic(&contactorsMutexBuffer);
}

/**
 * @brief   Returns the current state of
 *          a specified contactor
 * @param   contactor the contactor
 *              (MOTOR_CONTACTOR/MOTOR_PRECHARGE_CONTACTOR/ARRAY_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  The contactor's state (ON/OFF) based on its sense pin (for motor, motor precharge, array precharge) or CAN message (for array)
 */
bool Contactors_Get(contactor_enum_t contactor) {
    switch (contactor) {
    case MOTOR_CONTACTOR:
        contactorState[MOTOR_CONTACTOR].state = HAL_GPIO_ReadPin(MOTOR_SENSE_PORT, MOTOR_SENSE_PIN);
        break;
    case MOTOR_PRECHARGE_CONTACTOR:
        contactorState[MOTOR_PRECHARGE_CONTACTOR].state = HAL_GPIO_ReadPin(MOTOR_PRECHARGE_SENSE_PORT, MOTOR_PRECHARGE_SENSE_PIN);
        break;
    case ARRAY_CONTACTOR:
        // Receive BPS contactor state
        CAN_RxHeaderTypeDef rx_header = { 0 };
        uint8_t rx_data[8] = { 0 };
        can_status_t status = CAN_EMPTY;

        status = can_recv(hcan1, BPS_CONTACTOR_STATE, &rx_header, rx_data, 0);
        if (status == CAN_RECV) {
            contactorState[ARRAY_CONTACTOR].state = rx_data[0] & 0x01 ? ON : OFF;
        }
        else if (status == CAN_ERR) {
            // TODO: handle CAN error
        }
        break;
    case ARRAY_PRECHARGE_CONTACTOR:
        contactorState[MOTOR_PRECHARGE_CONTACTOR].state = HAL_GPIO_ReadPin(MOTOR_PRECHARGE_SENSE_PORT, MOTOR_PRECHARGE_SENSE_PIN);
        break;
    default:
        break;
    }
    return contactorState[contactor].state;
}

/**
 * @brief   Sets the state of a specified contactor
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @param   state the state to set (ON/OFF) (true/false)
 * @param   blocking whether or not this should be a blocking call
 * @return  Whether or not the contactor was successfully set
 */
ErrorStatus Contactors_Set(contactor_enum_t contactor, bool state, bool blocking) {
    ErrorStatus result = ERROR;

    // Acquire mutex lock if available
    if (xSemaphoreTake(contactorsMutex, blocking ? portMAX_DELAY : 0) == pdFALSE) {
        // TODO: set fault in bitmap??? do we care?
        return ERROR;
    }

    // Check if precharge completed - if not completed we have a fault
    // TODO: this function should only be called from timer callback, delay for precharge should be built-in
    if (contactorState[contactor].isPrechargeContactor && !getPrecharge(contactor)) {
        prechargeTimeoutFaultHandler(contactor);
        return ERROR;
    }

    // Set contactor to new state
    setContactor(contactor, state);

    // TODO: start sense timer 

    bool ret = Contactors_Get(contactor);
    result = (ret == state) ? SUCCESS : ERROR;

    // Release mutex lock
    if (xSemaphoreGive(contactorsMutex) == pdFALSE) {
        // TODO: set fault in bitmap???
        return ERROR;
    }

    return result;
}