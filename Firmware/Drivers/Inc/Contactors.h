#ifndef CONTACTORS_H
#define CONTACTORS_H

#include "common.h"
#include "stm32l4xx_hal.h"

// Number of contactors controlled directly
#define NUM_CONTACTORS 2

// Sense pin delay for contactor set
#define CONTACTOR_SENSE_DELAY pdMS_TO_TICKS(2500)

// Timeout for precharge completion
#define PRECHARGE_TIMEOUT_DELAY pdMS_TO_TICKS(2500)

// Contactor drive/sense pin definitions
#define MOTOR_ENABLE_PORT GPIOA
#define MOTOR_ENABLE_PIN GPIO_PIN_0
#define MOTOR_SENSE_PORT GPIOA
#define MOTOR_SENSE_PIN GPIO_PIN_1

#define MOTOR_PRECHARGE_ENABLE_PORT GPIOA
#define MOTOR_PRECHARGE_ENABLE_PIN GPIO_PIN_3
#define MOTOR_PRECHARGE_SENSE_PORT GPIOA
#define MOTOR_PRECHARGE_SENSE_PIN GPIO_PIN_4

#define ARRAY_PRECHARGE_ENABLE_PORT GPIOA
#define ARRAY_PRECHARGE_ENABLE_PIN GPIO_PIN_7
#define ARRAY_PRECHARGE_SENSE_PORT GPIOB
#define ARRAY_PRECHARGE_SENSE_PIN GPIO_PIN_0

// Precharge ready indicators (input from hardware comparison)
#define MOTOR_PRECHARGE_READY_PORT GPIOB
#define MOTOR_PRECHARGE_READY_PIN GPIO_PIN_4

#define ARRAY_PRECHARGE_READY_PORT GPIOB
#define ARRAY_PRECHARGE_READY_PIN GPIO_PIN_5

// Open/Closed definition
typedef enum { OFF = 0, ON } State;

// Enum to index into contactors array
typedef enum {
    MOTOR_CONTACTOR = 0,
    MOTOR_PRECHARGE_CONTACTOR,
    ARRAY_CONTACTOR,
    ARRAY_PRECHARGE_CONTACTOR,
    HV_PLUS_CONTACTOR,
    HV_MINUS_CONTACTOR
} contactor_enum_t;

// Struct to define the current state of a contactor
typedef struct contactor_t {
    bool state; // ON or OFF
    bool isPrechargeContactor; // only true for motor/array precharge contactors
    TimerHandle_t prechargeTimer; // timer handle for checking if precharge completed
    StaticTimer_t prechargeTimerBuffer;
    TimerHandle_t senseTimer; // timer handle for checking if contactor closed/opened (non-blocking mode)
    StaticTimer_t senseTimerBuffer;
} contactor_t;

/**
 * @brief   Initializes contactors to be used
 *          in connection with the Motor and Array
 * @return  None
 */
void Contactors_Init(void);

/**
 * @brief   Returns the current state of
 *          a specified contactor
 * @param   contactor the contactor
 *              (MOTOR_CONTACTOR/MOTOR_PRECHARGE_CONTACTOR/ARRAY_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR/HV_PLUS_CONTACTOR/HV_MINUS_CONTACTOR)
 * @return  The contactor's state (ON/OFF) based on its sense pin (for motor, motor precharge, array precharge) or CAN message (for array, hv)
 */
bool Contactors_Get(contactor_enum_t contactor);

/**
 * @brief   Sets the state of a specified contactor
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @param   state the state to set (ON/OFF) (true/false)
 * @param   blocking whether or not this should be a blocking call
 * @return  Whether or not the contactor was successfully set
 */
ErrorStatus Contactors_Set(contactor_enum_t contactor, bool state, bool blocking);

/**
 * @brief   Disables all contactors and bypasses mutex
 *          Should only used in a fault state
 *          Note: NOT not turn off array/motor contactors (only precharge)
 * @return  None
 */
void Contactors_EmergencyDisable(void);

/**
 * @brief   Gets the sense timer handle associated with a contactor
 * @param   contactor the contactor
 *              (MOTOR_CONTACTOR/MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  FreeRTOS timer handle for the specified contactor's sense timer
 */
TimerHandle_t Contactors_GetSenseTimerHandle(contactor_enum_t contactor);

/**
 * @brief   Gets the precharge completion timer handle associated with a contactor
 * @param   contactor the contactor
 *              (MOTOR_PRECHARGE_CONTACTOR/ARRAY_PRECHARGE_CONTACTOR)
 * @return  FreeRTOS timer handle for the specified contactor's precharge timer
 */
TimerHandle_t Contactors_GetPrechargeTimerHandle(contactor_enum_t contactor);

bool getPrecharge(contactor_enum_t contactor);

#endif