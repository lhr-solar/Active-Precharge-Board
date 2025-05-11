#include "Tasks.h"
#include "Contactors.h"
#include "ContactorTask.h"
#include "CANMetaData.h"

// Bitmap of current fault state
uint32_t fault_bitmap = 0;

// Current ignition state
ignition_state_t ign_state = IGNITION_OFF;

/**
 * @brief   Gets current BPS fault state over CAN
 * @return  True/false corresponding to BPS fault/no fault
 */
static bool getBPSFault() {
  // use BPS TRIP to enter fault state
  // use BPS Contactor State to check if safe
  return true;
}

/**
 * @brief   Gets current Controls fault state over CAN
 * @return  True/false corresponding to Controls fault/no fault
 */
static bool getControlsFault() {
  return true;
}

/**
 * @brief   Gets current ignition state over CAN
 * @return  Possible ignition states from controls: OFF, ARRAY, MOTOR
 */
static ignition_state_t getIgnitionState() {
  // Receive controls IO_STATE
  CAN_RxHeaderTypeDef rx_header = { 0 };
  uint8_t rx_data[8] = { 0 };
  can_status_t status = CAN_EMPTY;

  status = can_recv(hcan1, IO_STATE, &rx_header, rx_data, 0);
  if (status == CAN_RECV) {
    if (rx_data[2] & IGNITION_MOTOR) {
      ign_state = IGNITION_MOTOR;
    } else if (rx_data[2] & IGNITION_ARRAY) {
      ign_state = IGNITION_ARRAY;
    } else {
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
    if (getBPSFault()) {
      fault_bitmap |= FAULT_BPS;
      fault_handler();
    }

    // If Controls fault, enter fault handler
    if (getControlsFault()) {
      fault_bitmap |= FAULT_CONTROLS;
      fault_handler();
    }

    /*
          get bps safe
          get ignition state
         if(!bps_safe){
          fault
         }
        if(controls_fault){
          fault
        }
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
}