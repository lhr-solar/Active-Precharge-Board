#include "Tasks.h"
#include "Contactors.h"
#include "ContactorTask.h"
#include "CANMetaData.h"

// Bitmap of current contactor board fault state
uint32_t fault_bitmap = FAULT_NONE;

// Stores current BPS state
safety_status_t BPS_status = NOT_CHECKED;

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
 * @brief   Gets current ignition state over CAN
 * @return  Possible ignition states from controls: OFF, ARRAY, MOTOR
 */
static ignition_state_t getIgnitionState() {
  // Current ignition state
  static ignition_state_t ign_state = IGNITION_OFF;

  // Receive controls IO_STATE
  CAN_RxHeaderTypeDef rx_header = { 0 };
  uint8_t rx_data[8] = { 0 };
  can_status_t status = CAN_EMPTY;

  status = can_recv(hcan1, IO_STATE, &rx_header, rx_data, 0);
  if (status == CAN_RECV) {
    if (rx_data[2] & IGNITION_MOTOR) {
      ign_state = IGNITION_MOTOR;
    }
    else if (rx_data[2] & IGNITION_ARRAY) {
      ign_state = IGNITION_ARRAY;
    }
    else {
      ign_state = IGNITION_OFF;
    }
  }
  else if (status == CAN_ERR) {
    // TODO: handle CAN error
  }

  return ign_state;
}

/**
 * @brief   Executes contactor logic according to car state
 * @param   current_ign_state the current ignition state of the car
 *              (IGNITION_OFF, IGNITION_ARRAY, IGNITION_MOTOR)
 * @return  None
 */
static void logic_handler(ignition_state_t current_ign_state) {
  switch (current_ign_state) {
  case IGNITION_OFF:
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
    break;
  case IGNITION_ARRAY:
    // In array state, array and array precharge contactors should turn on
    // If HV+/- contactors are open, other contactors shouldn't be closed
    if (Contactors_Get(ARRAY_CONTACTOR) == ON && BPS_status == SAFE) {
      // Wait for precharge to finish, then close array precharge contactor (start timer if not active)
      if (Contactors_Get(ARRAY_PRECHARGE_CONTACTOR) == OFF && xTimerIsTimerActive(Contactors_GetPrechargeTimerHandle(ARRAY_PRECHARGE_CONTACTOR)) == pdFALSE) {
        // Start timer - callback will check if complete and either fault or close contactor
        xTimerStart(Contactors_GetPrechargeTimerHandle(ARRAY_PRECHARGE_CONTACTOR), 0);
      }
    }
    // If BPS has turned off array or HV+/- contactors, turn off precharge contactor as well (charging disabled)
    else if (Contactors_Get(ARRAY_PRECHARGE_CONTACTOR) == ON) {
      Contactors_Set(ARRAY_PRECHARGE_CONTACTOR, OFF, false);
    }
    break;
  case IGNITION_MOTOR:
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

    // Turn on array precharge if needed
    // If HV+/- contactors are open, other contactors shouldn't be closed
    if (Contactors_Get(ARRAY_CONTACTOR) == ON && BPS_status == SAFE) {
      // Wait for precharge to finish, then close array precharge contactor (start timer if not active)
      if (Contactors_Get(ARRAY_PRECHARGE_CONTACTOR) == OFF && xTimerIsTimerActive(Contactors_GetPrechargeTimerHandle(ARRAY_PRECHARGE_CONTACTOR)) == pdFALSE) {
        // Start timer - callback will check if complete and either fault or close contactor
        xTimerStart(Contactors_GetPrechargeTimerHandle(ARRAY_PRECHARGE_CONTACTOR), 0);
      }
    }
    // If BPS has turned off array or HV+/- contactors, turn off precharge contactor as well (charging disabled)
    else if (Contactors_Get(ARRAY_PRECHARGE_CONTACTOR) == ON) {
      Contactors_Set(ARRAY_PRECHARGE_CONTACTOR, OFF, false);
    }
    break;
  default:
    break;
  }
}

void Task_Contactor() {
  // Reset fault bitmap
  fault_bitmap = FAULT_NONE;

  while (1) {
    // If BPS fault, enter fault handler
    BPS_status = getBPS_State();

    if (BPS_status == FAULT) {
      fault_bitmap |= FAULT_BPS;
      fault_handler();
    }

    // If Controls fault, enter fault handler
    if (getControls_State() == FAULT) {
      fault_bitmap |= FAULT_CONTROLS;
      fault_handler();
    }

    // handle contactor logic based on OFF, ARRAY, or MOTOR state
    logic_handler(getIgnitionState());
  }
}