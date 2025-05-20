#include "Tasks.h"
#include "Contactors.h"
#include "ContactorTask.h"
#include "CANMetaData.h"
#include "StatusLEDs.h"

// Bitmap of current contactor board fault state
uint32_t fault_bitmap = FAULT_NONE;

// Stores current BPS state
safety_status_t BPS_status = NOT_CHECKED;

// Bitmap of current Controls ignition state
uint8_t ignition_bitmap = IGNITION_OFF;

/**
 * @brief   Helper function for BPS state - checks if BPS_TRIP message was receieved
 * @return  True if BPS has tripped, false otherwise
 */
static bool getBPS_TRIP() {
  bool result = false;

  // Receive BPS_TRIP
  CAN_RxHeaderTypeDef rx_header = { 0 };
  uint8_t rx_data[8] = { 0 };
  can_status_t status = CAN_EMPTY;

  status = can_recv(hcan1, BPS_TRIP, &rx_header, rx_data, 0);
  if (status == CAN_RECV) {
    result = rx_data[0];  // 1 (true) if BPS has tripped, 0 (false) if not tripped
  }
  else if (status == CAN_ERR) {
    // TODO: handle CAN error
  }

  return result;
}

/**
 * @brief   Gets current BPS fault state over CAN
 * @return  SAFE if HV+ and HV- contactors are closed, FAULT if BPS has tripped, NOT_CHECKED if neither
 */
static safety_status_t getBPS_State() {
  if (getBPS_TRIP()) {
    return FAULT;
  }

  if ((Contactors_Get(HV_PLUS_CONTACTOR) == ON) && (Contactors_Get(HV_MINUS_CONTACTOR) == ON)) {
    return SAFE;
  }

  return NOT_CHECKED;
}

/**
 * @brief   Gets current Controls fault state over CAN
 * @return  SAFE if there are no controls faults, FAULT if any faults are present, NOT_CHECKED if haven't yet received controls fault state
 */
static safety_status_t getControls_State() {
  // Holds current Controls fault state - NOT_CHECKED by default
  static safety_status_t controls_status = NOT_CHECKED;

  // Receive Controls_Fault
  CAN_RxHeaderTypeDef rx_header = { 0 };
  uint8_t rx_data[8] = { 0 };
  can_status_t status = CAN_EMPTY;

  status = can_recv(hcan1, CONTROLS_FAULT, &rx_header, rx_data, 0);
  if (status == CAN_RECV) {
    // If any bit is set (not 0), fault has occurred
    controls_status = rx_data[0] ? FAULT : SAFE;
  }
  else if (status == CAN_ERR) {
    // TODO: handle CAN error
  }

  return controls_status;
}

/**
 * @brief   Gets current ignition state over CAN - updates ignition bitmap
 *          Bit 0 = Motor, Bit 1 = Array
 */
static void updateIgnitionState() {
  // Receive controls IO_STATE
  CAN_RxHeaderTypeDef rx_header = { 0 };
  uint8_t rx_data[8] = { 0 };
  can_status_t status = CAN_EMPTY;

  status = can_recv(hcan1, IO_STATE, &rx_header, rx_data, 0);
  if (status == CAN_RECV) {
    // Ignition state is bit 0 and 1 of IO_STATE message
    // Directly map those bits to ignition_bitmap
    ignition_bitmap = rx_data[2] & 0x03;
  }
  else if (status == CAN_ERR) {
    // TODO: handle CAN error
  }
}

/**
 * @brief   Executes contactor logic according to car state
 * @return  None
 */
static void logic_handler() {
  // In motor ignition state
  if (ignition_bitmap & IGNITION_MOTOR) {
    // In motor state, motor, motor precharge, array, and array precharge contactors should turn on
    // If HV+/- contactors are open, other contactors shouldn't be closed
    if (Contactors_Get(MOTOR_CONTACTOR) == ON && BPS_status == SAFE) {
      // Wait for precharge to finish, then close motor precharge contactor (start timer if not active)
      if (Contactors_Get(MOTOR_PRECHARGE_CONTACTOR) == OFF && xTimerIsTimerActive(Contactors_GetPrechargeTimerHandle(MOTOR_PRECHARGE_CONTACTOR)) == pdFALSE) {
        // Start timer - callback will check if complete and either fault or close contactor
        xTimerStart(Contactors_GetPrechargeTimerHandle(MOTOR_PRECHARGE_CONTACTOR), 0);
      }
    }
    // If Controls has turned off motor contactor or BPS has turned off HV+/- contactors, turn off precharge contactor as well
    else if (Contactors_Get(MOTOR_PRECHARGE_CONTACTOR) == ON) {
      Contactors_Set(MOTOR_PRECHARGE_CONTACTOR, OFF, false);
    }
  }
  if (ignition_bitmap & IGNITION_ARRAY) {
    // In array state, array and array precharge contactors should turn on
    // If HV+/- contactors are open, other contactors shouldn't be closed
    if (Contactors_Get(ARRAY_CONTACTOR) == ON && BPS_status == SAFE) {
      // Wait for precharge to finish, then close array precharge contactor (start timer if not active)
      if (Contactors_Get(ARRAY_PRECHARGE_CONTACTOR) == OFF && xTimerIsTimerActive(Contactors_GetPrechargeTimerHandle(ARRAY_PRECHARGE_CONTACTOR)) == pdFALSE) {
        // Start timer - callback will check if complete and either fault or close contactor
        volatile BaseType_t result = xTimerStart(Contactors_GetPrechargeTimerHandle(ARRAY_PRECHARGE_CONTACTOR), portMAX_DELAY);
        if (result != pdPASS) {
          Status_Leds_All_On();
        }
      }
    }
    // If BPS has turned off array or HV+/- contactors, turn off precharge contactor as well (charging disabled)
    else if (Contactors_Get(ARRAY_PRECHARGE_CONTACTOR) == ON) {
      Contactors_Set(ARRAY_PRECHARGE_CONTACTOR, OFF, false);
    }
  }
  // Must be in OFF state if array bit is not set
  else {
    // In off state, all contactors should be off
    if (Contactors_Get(MOTOR_PRECHARGE_CONTACTOR) == ON) {
      // Turn off motor precharge contactor in blocking mode
      if (Contactors_Set(MOTOR_PRECHARGE_CONTACTOR, OFF, true) == ERROR) {
        fault_bitmap |= FAULT_MOTOR_PRECHARGE_SENSE;
        fault_handler();
      }
    }
    if (Contactors_Get(ARRAY_PRECHARGE_CONTACTOR) == ON) {
      // Turn off array precharge contactor in blocking mode
      if (Contactors_Set(ARRAY_PRECHARGE_CONTACTOR, OFF, true) == ERROR) {
        fault_bitmap |= FAULT_ARRAY_PRECHARGE_SENSE;
        fault_handler();
      }
    }
  }
}

void Task_Contactor() {
  // Initialize contactors driver
  // This MUST happen here....or else
  Contactors_Init();

  // DELETE THIS LATER - SEND HELLOWORLD CAN MSG
  // create payload to send
  CAN_TxHeaderTypeDef tx_header = { 0 };
  tx_header.StdId = 0x650;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.IDE = CAN_ID_STD;
  tx_header.DLC = 1;
  tx_header.TransmitGlobalTime = DISABLE;

  uint8_t tx_data[8] = { 0 };

  tx_data[0] = 0x01;
  if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

  // Reset status variables/bitmaps
  fault_bitmap = FAULT_NONE;
  BPS_status = NOT_CHECKED;
  ignition_bitmap = IGNITION_OFF;

  while (1) {
    // Update current BPS state
    BPS_status = getBPS_State();

    // If BPS fault, enter fault handler
    if (BPS_status == FAULT) {
      fault_bitmap |= FAULT_BPS;
      fault_handler();
    }

    // If Controls fault, enter fault handler
    if (getControls_State() == FAULT) {
      fault_bitmap |= FAULT_CONTROLS;
      fault_handler();
    }

    // Update current ignition state bitmap based on controls
    updateIgnitionState();

    // Handle contactor logic based on OFF, ARRAY, or MOTOR state
    logic_handler();
  }
}