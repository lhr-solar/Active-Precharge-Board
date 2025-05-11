#include "Tasks.h"
#include "Contactors.h"
#include "ContactorTask.h"
#include "CANMetaData.h"

// Bitmap of current contactor board fault state
uint32_t fault_bitmap = 0;

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

void Task_Contactor() {
  // Reset fault bitmap
  fault_bitmap = FAULT_NONE;

  while (1) {
    // If BPS fault, enter fault handler
    if (getBPS_State() == FAULT) {
      fault_bitmap |= FAULT_BPS;
      fault_handler();
    }

    // If Controls fault, enter fault handler
    if (getControls_State() == FAULT) {
      fault_bitmap |= FAULT_CONTROLS;
      fault_handler();
    }

    ignition_state_t current_ign_state = getIgnitionState();

    if (current_ign_state == IGNITION_MOTOR) {
      // TODO: logic - static function

    /*
        if(motorPre.state == CLOSED && !MOTOR_IGNITION_STATE){ fault }
         // Controls has turned the motor contactor on
          if(motor.enable_in == CLOSED){
            if(!BPS_SAFE || !MOTOR_IGNITION_STATE){
              // fault
            }
            // controls turned motor on, but not yet closed
            if(motor.state == OPEN && motorCLoseTimerNotStarted){
              // start FreeRTOS timer to ensure that the Contactor closes
            }
            // Motor Contactor is closed, need to start precharge
            if(motor.state == CLOSED){
              if(motorPre.state == OPEN){
                // start precharge timer if not alr started
              }
            }
          }
    */

    }
    else if (current_ign_state == IGNITION_ARRAY) {
      // TODO: logic - static function
    }
  }
}