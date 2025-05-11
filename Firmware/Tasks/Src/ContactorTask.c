#include "Tasks.h"
#include "Contactors.h"

// Bitmap of current fault state
uint32_t fault_bitmap = 0;

/**
 * @brief   Gets current state of BPS over CAN
 * @return  True/false corresponding to BPS fault/no fault
 */
static bool getBPSFault() {
  // use BPS TRIP to enter fault state
  // use BPS Contactor State to check if safe
  return true;
}

/**
 * @brief   Gets current state of Controls over CAN
 * @return  True/false corresponding to Controls fault/no fault
 */
static bool getControlsFault() {
  return true;
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