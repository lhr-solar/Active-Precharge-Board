#include "Contactors.h"
#include "CANMetaData.h"
#include "StatusLEDs.h"

// Mutex to lock contactor state
static SemaphoreHandle_t contactorsMutex = NULL;
StaticSemaphore_t contactorsMutexBuffer;

// Array describes the state of the contactors
static contactor_t contactorState[NUM_CONTACTORS];

/**
 * @brief   Helper function for receiving BPS contactor state over CAN to update contactorState
 * @return  None
 */
static void updateBPSContactors() {
    // Receive BPS contactor state
    CAN_RxHeaderTypeDef rx_header = { 0 };
    uint8_t rx_data[8] = { 0 };
    can_status_t status = CAN_EMPTY;

    status = can_recv(hcan1, BPS_CONTACTOR_STATE, &rx_header, rx_data, 0);
    if (status == CAN_RECV) {
        contactorState[ARRAY_CONTACTOR].state = rx_data[0] & 0x01 ? ON : OFF;
        contactorState[HV_PLUS_CONTACTOR].state = rx_data[0] & 0x04 ? ON : OFF;
        contactorState[HV_MINUS_CONTACTOR].state = rx_data[0] & 0x02 ? ON : OFF;
    }
    else if (status == CAN_ERR) {
        // TODO: handle CAN error
    }
}

/**
 * @brief   Helper function for reading precharge completion state
 *          Should only be called if contactor is a precharge contactor
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  The contactor's precharge completion state from the output of hardware comparison
 */
bool getPrecharge(contactor_enum_t contactor) {
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
    case MOTOR_CONTACTOR:
        contactorState[MOTOR_CONTACTOR].state = state;
        HAL_GPIO_WritePin(MOTOR_ENABLE_PORT, MOTOR_ENABLE_PIN, state);
        break;
    default:
        break;
    }
}

/**
 * @brief   Checks if contactor is in expected state, fault otherwise
 * @param   senseTimer handle to the contactor's sense timer
 * @return  None
 */
static void senseTimerCallback(TimerHandle_t senseTimer) {
    // Timer ID holds the contactor ID
    contactor_enum_t contactor = (contactor_enum_t)pvTimerGetTimerID(senseTimer);
    // If sense doesn't match state, enter fault
    if (contactorState[contactor].state != Contactors_Get(contactor)) {
        if (contactor == MOTOR_PRECHARGE_CONTACTOR) {
            fault_bitmap |= FAULT_MOTOR_PRECHARGE_SENSE;
        }
        else if (contactor == ARRAY_PRECHARGE_CONTACTOR) {
            fault_bitmap |= FAULT_ARRAY_PRECHARGE_SENSE;
        }
        else if(contactor == MOTOR_CONTACTOR){
            fault_bitmap |= FAULT_MOTOR;
        }
        fault_handler();
    }
    // Update state to match sense pin
    contactorState[contactor].state = Contactors_Get(contactor); 
}

/**
 * @brief   Turns on contactor if precharge is complete, otherwise enters fault state
 * @param   prechargeTimer handle to the contactor's precharge completion timer
 * @return  None
 */
static void prechargeTimerCallback(TimerHandle_t prechargeTimer) {
    // Timer ID holds the contactor ID
    contactor_enum_t contactor = (contactor_enum_t)pvTimerGetTimerID(prechargeTimer);
    // Check if precharge completed - if not completed we have a fault
    if (!getPrecharge(contactor)) {
        if (contactor == MOTOR_PRECHARGE_CONTACTOR) {
            fault_bitmap |= FAULT_MOTOR_PRECHARGE_TIMEOUT;
        }
        else if (contactor == ARRAY_PRECHARGE_CONTACTOR) {
            fault_bitmap |= FAULT_ARRAY_PRECHARGE_TIMEOUT;
        }
        fault_handler();
    }
    // Precharge is complete - turn on the contactor (non-blocking mode)
    // This also starts the sense timer to confirm that it closed
    Contactors_Set(contactor, ON, false);
}

/**
 * @brief   Gets the sense timer handle associated with a contactor
 * @param   contactor the contactor
 *              (MOTOR_CONTACTOR/MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  FreeRTOS timer handle for the specified contactor's sense timer
 */
TimerHandle_t Contactors_GetSenseTimerHandle(contactor_enum_t contactor) {
    switch (contactor) {
    case MOTOR_CONTACTOR:
        return contactorState[MOTOR_CONTACTOR].senseTimer;
        break;
    case MOTOR_PRECHARGE_CONTACTOR:
        return contactorState[MOTOR_PRECHARGE_CONTACTOR].senseTimer;
        break;
    case ARRAY_PRECHARGE_CONTACTOR:
        return contactorState[ARRAY_PRECHARGE_CONTACTOR].senseTimer;
        break;
    default:
        return NULL;
        break;
    }
}

/**
 * @brief   Gets the precharge completion timer handle associated with a contactor
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  FreeRTOS timer handle for the specified contactor's precharge timer
 */
TimerHandle_t Contactors_GetPrechargeTimerHandle(contactor_enum_t contactor) {
    switch (contactor) {
    case MOTOR_PRECHARGE_CONTACTOR:
        return contactorState[MOTOR_PRECHARGE_CONTACTOR].prechargeTimer;
        break;
    case ARRAY_PRECHARGE_CONTACTOR:
        return contactorState[ARRAY_PRECHARGE_CONTACTOR].prechargeTimer;
        break;
    default:
        return NULL;
        break;
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

    // Set precharge contactors
    contactorState[MOTOR_PRECHARGE_CONTACTOR].isPrechargeContactor = true;
    contactorState[ARRAY_PRECHARGE_CONTACTOR].isPrechargeContactor = true;

    // Start precharge contactors disabled
    setContactor(MOTOR_PRECHARGE_CONTACTOR, OFF);
    setContactor(ARRAY_PRECHARGE_CONTACTOR, OFF);

    // Create a mutex type semaphore for contactors, don't check if null since not dynamically allocated
    contactorsMutex = xSemaphoreCreateMutexStatic(&contactorsMutexBuffer);

    // Create sense timers
    contactorState[MOTOR_CONTACTOR].senseTimer = xTimerCreateStatic(
        "Motor Contactor Sense Timer",                      /* Name of the timer */
        CONTACTOR_SENSE_DELAY,                              /* Timer period in ticks */
        pdFALSE,                                            /* Don't auto-reload */
        (void*)MOTOR_CONTACTOR,                             /* Timer ID */
        senseTimerCallback,                                 /* Callback function */
        &(contactorState[MOTOR_CONTACTOR].senseTimerBuffer) /* Buffer to hold timer data */
    );
    contactorState[MOTOR_PRECHARGE_CONTACTOR].senseTimer = xTimerCreateStatic(
        "Motor Precharge Contactor Sense Timer",            /* Name of the timer */
        CONTACTOR_SENSE_DELAY,                              /* Timer period in ticks */
        pdFALSE,                                            /* Don't auto-reload */
        (void*)MOTOR_PRECHARGE_CONTACTOR,                   /* Timer ID */
        senseTimerCallback,                                 /* Callback function */
        &(contactorState[MOTOR_PRECHARGE_CONTACTOR].senseTimerBuffer) /* Buffer to hold timer data */
    );
    contactorState[ARRAY_PRECHARGE_CONTACTOR].senseTimer = xTimerCreateStatic(
        "Array Precharge Contactor Sense Timer",            /* Name of the timer */
        CONTACTOR_SENSE_DELAY,                              /* Timer period in ticks */
        pdFALSE,                                            /* Don't auto-reload */
        (void*)ARRAY_PRECHARGE_CONTACTOR,                   /* Timer ID */
        senseTimerCallback,                                 /* Callback function */
        &(contactorState[ARRAY_PRECHARGE_CONTACTOR].senseTimerBuffer) /* Buffer to hold timer data */
    );

    // Create precharge timers
    contactorState[MOTOR_PRECHARGE_CONTACTOR].prechargeTimer = xTimerCreateStatic(
        "Motor Precharge Completion Timer",                               /* Name of the timer */
        PRECHARGE_TIMEOUT_DELAY,                                          /* Timer period in ticks */
        pdFALSE,                                                          /* Don't auto-reload */
        (void*)MOTOR_PRECHARGE_CONTACTOR,                                 /* Timer ID */
        prechargeTimerCallback,                                           /* Callback function */
        &(contactorState[MOTOR_PRECHARGE_CONTACTOR].prechargeTimerBuffer) /* Buffer to hold timer data */
    );
    contactorState[ARRAY_PRECHARGE_CONTACTOR].prechargeTimer = xTimerCreateStatic(
        "Array Precharge Completion Timer",                               /* Name of the timer */
        PRECHARGE_TIMEOUT_DELAY,                                          /* Timer period in ticks */
        pdFALSE,                                                          /* Don't auto-reload */
        (void*)ARRAY_PRECHARGE_CONTACTOR,                                 /* Timer ID */
        prechargeTimerCallback,                                           /* Callback function */
        &(contactorState[ARRAY_PRECHARGE_CONTACTOR].prechargeTimerBuffer) /* Buffer to hold timer data */
    );
}

/**
 * @brief   Returns the current state of a Contactor, for direct connections it returns the state of the sense pin
 * @param   contactor the contactor
 *              (MOTOR_CONTACTOR/MOTOR_PRECHARGE_CONTACTOR/ARRAY_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR/HV_PLUS_CONTACTOR/HV_MINUS_CONTACTOR)
 * @return  The contactor's state (ON/OFF) based on its sense pin (for motor, motor precharge, array precharge) or CAN message (for array, hv)
 */
bool Contactors_Get(contactor_enum_t contactor) {
    switch (contactor) {
    case MOTOR_CONTACTOR:
        return HAL_GPIO_ReadPin(MOTOR_SENSE_PORT, MOTOR_SENSE_PIN);
        break;
    case MOTOR_PRECHARGE_CONTACTOR:
        return HAL_GPIO_ReadPin(MOTOR_PRECHARGE_SENSE_PORT, MOTOR_PRECHARGE_SENSE_PIN);
        break;
    case ARRAY_CONTACTOR:
        updateBPSContactors();
        break;
    case ARRAY_PRECHARGE_CONTACTOR:
        return HAL_GPIO_ReadPin(ARRAY_PRECHARGE_SENSE_PORT, ARRAY_PRECHARGE_SENSE_PIN);
        break;
    case HV_PLUS_CONTACTOR:
        updateBPSContactors();
        break;
    case HV_MINUS_CONTACTOR:
        updateBPSContactors();
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
        return ERROR;
    }

    // Set contactor to new state
    setContactor(contactor, state);

    if(contactor == ARRAY_PRECHARGE_CONTACTOR || contactor == MOTOR_PRECHARGE_CONTACTOR || contactor == MOTOR_CONTACTOR) {
        // Start a timer that will check if the sense pin is on after a delay, or it will fault
        xTimerStart(Contactors_GetSenseTimerHandle(contactor), blocking ? portMAX_DELAY : 0);
        result = SUCCESS; // MUST check sense pin using timer
    }
    
    // Release mutex lock
    if (xSemaphoreGive(contactorsMutex) == pdFALSE) {
        return ERROR;
    }

    return result;
}

/**
 * @brief   Disables all contactors and bypasses mutex
 *          Should only used in a fault state
 *          Note: NOT not turn off array/motor contactors (only precharge)
 * @return  None
 */
void Contactors_EmergencyDisable() {
    setContactor(MOTOR_PRECHARGE_CONTACTOR, OFF);
    setContactor(ARRAY_PRECHARGE_CONTACTOR, OFF);
    setContactor(MOTOR_CONTACTOR, OFF);
}