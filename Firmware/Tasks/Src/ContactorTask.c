#include "Tasks.h"
#include "Contactors.h"

// Bitmap of current fault state
uint32_t fault_bitmap = 0;

/**
 * @brief   Gets current state of BPS over CAN
 * @return  True/false corresponding to BPS safe/fault
 */
static bool getBPSSafe() {
    // use BPS TRIP to enter fault state
    // use BPS Contactor State to check if safe
    return false;
}

/**
 * @brief   Gets current state of Controls over CAN
 * @return  True/false corresponding to Controls safe/fault
 */
static bool getControlsFault() {
    return false;
}

void Task_Contactor() {
    fault_bitmap = FAULT_NONE;

    // maybe have initial delay?

    while (1) {
        fault_bitmap |= FAULT_BPS & !getBPSSafe();
        fault_bitmap |= FAULT_CONTROLS & !getControlsFault();

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