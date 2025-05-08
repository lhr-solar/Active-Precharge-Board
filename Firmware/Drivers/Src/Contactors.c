#include "Contactors.h"

// Mutex to lock contactor state
static SemaphoreHandle_t contactorsMutex = NULL;
StaticSemaphore_t contactorsMutexBuffer;

// Array describes the state of the contactors
static contactor_t contactorState[NUM_CONTACTORS];

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
            break;
        case ARRAY_PRECHARGE_CONTACTOR:
            contactorState[ARRAY_PRECHARGE_CONTACTOR].state = state;
            break;
        default:
            break;
    }
}

/**
 * @brief   Initializes contactors to be used
 *          in connection with the Motor and Array
 * @return  None
 */
void Contactors_Init() {
    // Create a mutex type semaphore for contactors, don't check if null since not dynamically allocated
    contactorsMutex = xSemaphoreCreateMutexStatic(&contactorsMutexBuffer);
}

/**
 * @brief   Returns the current state of
 *          a specified contactor
 * @param   contactor the contactor
 *              (MOTOR_CONTACTOR/MOTOR_PRECHARGE_CONTACTOR/ARRAY_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  The contactor's state (ON/OFF) based on its sense pin
 */
bool Contactors_Get(contactor_t contactor) {

}

/**
 * @brief   Sets the state of a specified contactor
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @param   state the state to set (ON/OFF) (true/false)
 * @param   blocking whether or not this should be a blocking call
 * @return  Whether or not the contactor was successfully set
 */
ErrorStatus Contactors_Set(contactor_t contactor, bool state, bool blocking) {

}