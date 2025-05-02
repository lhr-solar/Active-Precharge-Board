#include "Task_Contactor.h"

// TODO: fix all of these!!!! MOVE TO DRIVER!!!!
// READ-ONLY drive/sense signals from controls-driven motor contactor
uint8_t motor_enable_read() {
  return HAL_GPIO_ReadPin(MOTOR_ENABLE_PORT, MOTOR_ENABLE_PIN);
}

uint8_t motor_sense() {
  return HAL_GPIO_ReadPin(MOTOR_SENSE_PORT, MOTOR_SENSE_PIN);
}

// drive/sense signals for motor precharge contactor
void motor_precharge_enable(uint8_t state) {
  HAL_GPIO_WritePin(MOTOR_PRECHARGE_ENABLE_PORT, MOTOR_PRECHARGE_ENABLE_PIN, state);
}

uint8_t motor_precharge_sense() {
  return HAL_GPIO_ReadPin(MOTOR_PRECHARGE_SENSE_PORT, MOTOR_PRECHARGE_SENSE_PIN);
}

// drive/sense signals for array precharge contactor
void array_precharge_enable(uint8_t state) {
  HAL_GPIO_WritePin(ARRAY_PRECHARGE_ENABLE_PORT, ARRAY_PRECHARGE_ENABLE_PIN, state);
}

uint8_t array_precharge_sense() {
  return HAL_GPIO_ReadPin(ARRAY_PRECHARGE_SENSE_PORT, ARRAY_PRECHARGE_SENSE_PIN);
}

// read precharge ready indicators from hardware comparison
uint8_t motor_precharge_ready() {
  return HAL_GPIO_ReadPin(MOTOR_PRECHARGE_READY_PORT, MOTOR_PRECHARGE_READY_PIN);
}

uint8_t array_precharge_ready() {
  return HAL_GPIO_ReadPin(ARRAY_PRECHARGE_READY_PORT, ARRAY_PRECHARGE_READY_PIN);
}

// fault/status LEDs
void motor_timeout_fault_led(uint8_t state) {
  HAL_GPIO_WritePin(MOTOR_TIMEOUT_FAULT_LED_PORT, MOTOR_TIMEOUT_FAULT_LED_PIN, state);
}

void motor_sense_fault_led(uint8_t state) {
  HAL_GPIO_WritePin(MOTOR_SENSE_FAULT_LED_PORT, MOTOR_SENSE_FAULT_LED_PIN, state);
}

void array_timeout_fault_led(uint8_t state) {
  HAL_GPIO_WritePin(ARRAY_TIMEOUT_FAULT_LED_PORT, ARRAY_TIMEOUT_FAULT_LED_PIN, state);
}

void array_sense_fault_led(uint8_t state) {
  HAL_GPIO_WritePin(ARRAY_SENSE_FAULT_LED_PORT, ARRAY_SENSE_FAULT_LED_PIN, state);
}

typedef struct Contactor {
  uint8_t state; // open, closed
  uint8_t enable_in;
  uint8_t sense; // GPIO pin to read the contactor's state
  uint8_t fault;
  uint32_t start_time; // HAL_getTick()
} Contactor;

typedef struct prechargeContactor {
  uint8_t state; // open, closed
  uint8_t pre_ready; // Precharge ready signal
  uint8_t sense;
  uint8_t enable_out; // GPIO pin to control the contactor
  uint8_t fault;
  uint32_t start_time;
} prechargeContactor;

Contactor motor = { OPEN, OPEN, OPEN, NO_FAULT, 0 };
Contactor array = { OPEN, OPEN, OPEN, NO_FAULT, 0 };
prechargeContactor motorPre = { OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0 };
prechargeContactor arrayPre = { OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0 };

int time;
uint8_t fault[10];

uint32_t mPre_initial_delay = 0;
uint32_t aPre_initial_delay = 0;
uint32_t mPre_timeout = 0;
uint32_t aPre_timeout = 0;
bool disable_motorPre = false; // semaphore, true when motor sucsessfully disabled in event in turn off sequence
bool disable_arrayPre = false; // semaphore, true when array sucsessfully disabled whenever its supposed to be
bool CAN_fault = false; // in CAN handler set this to true for any car faults such as BPS unsafe
// ^^ all that should not be here???

// TODO: ideally this task should just call the contactors function i think....
void Task_Contactors(void* pvParamters) {
  time = HAL_GetTick();

  // read pins
  motor.enable_in = (m_direct());
  motor.sense = m_sense();
  motorPre.pre_ready = m_pre_ready();
  motorPre.sense = m_pre_sense();

  // array.enable_in = from CAN  //fix this
  // array.sense = from CAN //fix this
  arrayPre.pre_ready = a_pre_ready();
  arrayPre.sense = a_pre_sense();

  // handle faults
  if ((fault[0] | fault[1] | fault[2] | fault[3] | fault[4] | fault[5] | fault[6] | fault[7] | fault[8] | fault[9] | CAN_fault) == FAULT) {

    // disable contactors under our domain (m_pre and a_pre)
    m_pre_enable(0);
    a_pre_enable(0);

    // send CAN specific message based on fault
    //  if (CAN_fault) {do nothing, only disable contactors;}
    if (fault[0]) { // m_enable unplug
      ms_fault(1);
      mt_fault(1);
      // send this specific CAN message
    }
    if (fault[1]) { // motor sense fault
      ms_fault(1);
      // send this specific CAN message
    }
    if (fault[2]) { // motor timeout fault
      mt_fault(1);
      // send this specific CAN message
    }
    if (fault[3]) { // motorPre sense fault
      ms_fault(1);
      // send this specific CAN message
    }
    if (fault[4]) { // motorPre timeout fault
      mt_fault(1);
      // send this specific CAN message
    }
    if (fault[5]) { // a_enable fault
      as_fault(1);
      at_fault(1);
      // send this specific CAN message
    }
    if (fault[6]) { // array sense fault
      as_fault(1);
      // send this specific CAN message
    }
    if (fault[7]) { // array timeout fault
      at_fault(1);
      // send this specific CAN message
    }
    if (fault[8]) { // arrayPre sense fault
      as_fault(1);
      // send this specific CAN message
    }
    if (fault[9]) { // arrayPre timeout fault
      at_fault(1);
      // send this specific CAN message
    }

  }

  else { // this else executes if no faults

    // motor logic
    if (motor.enable_in == CLOSED && motor.state == OPEN) {
      if (motor.start_time == 0) {
        motor.start_time = time;
      }
      else if ((motor.sense == OPEN) && (motor.start_time + 100 < time)) { // if 100ms after contactor enabled, fault, not reading sense right
        fault[1] = FAULT; // motor sense fault
      }
      else if (motor.sense == CLOSED) {
        if (mPre_initial_delay == 0) {
          mPre_initial_delay = time;
        }
        else if (mPre_initial_delay + 100 < time) { // bake in 100ms of delay for the contactor to fully close and reset precharge logic
          motor.state = CLOSED;
          motor.start_time = 0;
          mPre_initial_delay = 0;
          mPre_timeout = time;
        }
      }
    }
    else if (motor.enable_in == OPEN && motor.state == CLOSED) {
      if (false) { // change false to say if were in the right part if ignition state for m.enable to go low, then
        if (motor.start_time == 0) {
          motor.start_time = time;
        }
        else if ((motor.sense == CLOSED) && (motor.start_time + 100 < time)) {
          fault[2] = FAULT; // motor timeout fault
        }
        else if (motor.sense == OPEN) {
          motor.state = OPEN;
          motor.start_time = 0;
          disable_motorPre = true; // sempahore, turn off motorPre
        }
      }
      else {
        if (motor.start_time == 0) {
          motor.start_time = time;
        }
        else if (motor.start_time + 10 < time) {
          fault[0] = FAULT; // otherwise, m.enable probably unplugged, fault
        }
      }

    }
    else if (motor.enable_in == OPEN && motor.state == OPEN) { // if conactor is unenabled and sense high, wait 10ms and if still true, fault
      if (motor.sense == CLOSED) {
        if (motor.start_time == 0) {
          motor.start_time = time;
        }
        else if (motor.start_time + 10 > time) {
          fault[1] = FAULT; // motor sense fault
        }
      }
      else
        motor.start_time = 0;
    }
    else if (motor.enable_in == CLOSED && motor.state == CLOSED) { // if conactor is enabled and sense low, wait 10ms and if still true, fault
      if (motor.sense == OPEN) {
        if (motor.start_time == 0) {
          motor.start_time = time;
        }
        else if (motor.start_time + 10 > time) {
          fault[1] = FAULT; // motor sense fault
        }
      }
      else
        motor.start_time = 0;
    }

    // motorPre logic
    if (motor.state == CLOSED) { // handle event where pre_ready is high bc motor.state was just open and enabling motorPre after
      if (motorPre.pre_ready == OPEN) {
        if (mPre_timeout == 0) {
          mPre_timeout = time;
        }
        else if (motorPre.state == CLOSED && mPre_timeout + 100 < time) {
          fault[4] = FAULT; // motor precharge steady state timeout fault
        }
        else if (mPre_timeout + 1000 < time) {
          fault[4] = FAULT; // motor precharge initial timeout fault
        }
      }
      else if (motorPre.pre_ready == CLOSED) {
        m_pre_enable(1);
        if (motorPre.start_time == 0) {
          motorPre.start_time = time;
        }
        else if ((motorPre.sense == OPEN) && (motorPre.start_time + 100 < time)) { // if 100ms after contactor enabled, fault, not reading sense right
          fault[3] = FAULT; // motor precharge sense fault
        }
        else if (motorPre.sense == CLOSED) {
          motorPre.state = CLOSED;
          motorPre.start_time = 0;
          mPre_timeout = 0;
        }
      }
    }
    else if (disable_motorPre) { // handle motorPre disable when motor was just disabled
      if (motorPre.start_time == 0) {
        motorPre.start_time = time;
        m_pre_enable(0);
      }
      else if (motorPre.sense == CLOSED && motorPre.start_time + 100 < time) {
        fault[4] = FAULT; // motor precharge turn off timeout fault
      }
      else if (motorPre.sense == OPEN) {
        motorPre.state = OPEN;
        motorPre.start_time = 0;
        disable_motorPre = false;
      }
    }

    /* array logic never tested before but identical to motor
    //array logic
    if (array.enable_in == CLOSED && array.state == OPEN) {
      if(array.start_time == 0){
        array.start_time = time;
      }
      else if ((array.sense == OPEN) && (array.start_time + 100 < time)){  //if 100ms after contactor enabled, fault, not reading sense right
        fault[6] = FAULT; //array sense fault
      }
      else if(array.sense==CLOSED){
        if (aPre_initial_delay == 0){
          aPre_initial_delay = time;
        }
        else if (aPre_initial_delay + 100 < time){  //bake in 100ms of delay for the contactor to fully close and reset precharge logic
          array.state = CLOSED;
          array.start_time = 0;
          aPre_initial_delay = 0;
          aPre_timeout = time;
        }
      }
    }
    else if (array.enable_in == OPEN && array.state == CLOSED){
      if (false) { //change false to say if were in the right part if ignition state for a.enable to go low, then
        if (array.start_time == 0){
          array.start_time = time;
        }
        else if ((array.sense == CLOSED) && (array.start_time + 100 < time)){
          fault[7] = FAULT;  //array timeout fault
        }
        else if (array.sense == OPEN){
          array.state =  OPEN;
          array.start_time = 0;
          disable_arrayPre = true;  //sempahore, turn off arrayPre
        }
      }
      else {
        if (array.start_time == 0){
          array.start_time = time;
        }
        else if (array.start_time + 10 < time){
          fault[5] = FAULT; //otherwise, were not in the right part of ignition state for this to happen, fault
        }
      }

    }
    else if(array.enable_in == OPEN && array.state == OPEN){ //if conactor is unenabled and sense high, wait 10ms and if still true, fault
      if(array.sense == CLOSED){
        if(array.start_time == 0){
          array.start_time = time;
        }
        else if(array.start_time + 10 > time){
          fault[6] = FAULT; //array sense fault
        }
      }
      else array.start_time = 0;
    }
    else if(array.enable_in == CLOSED && array.state == CLOSED){ //if conactor is enabled and sense low, wait 10ms and if still true, fault
      if(array.sense == OPEN){
        if(array.start_time == 0){
          array.start_time = time;
        }
        else if(array.start_time + 10 > time){
          fault[6] = FAULT; //array sense fault
        }
      }
      else array.start_time = 0;
    }

    //arrayPre logic
    if (array.state == CLOSED){ //handle event where pre_ready is high bc array.state was just open and enabling arrayPre after
      if (arrayPre.pre_ready == OPEN){
        if (aPre_timeout == 0){
          aPre_timeout = time;
        }
        else if (arrayPre.state == CLOSED && aPre_timeout + 100 < time) {
          fault[9] = FAULT; //array precharge steady state timeout fault
        }
        else if (aPre_timeout + 1000 < time){
          fault[9] = FAULT; //array precharge initial timeout fault
        }
      }
      else if (arrayPre.pre_ready == CLOSED){
        a_pre_enable(1);
        if(arrayPre.start_time == 0){
          arrayPre.start_time = time;
        }
        else if ((arrayPre.sense == OPEN) && (arrayPre.start_time + 100 < time)){  //if 100ms after contactor enabled, fault, not reading sense right
          fault[8] = FAULT; //array precharge sense fault
        }
        else if (arrayPre.sense == CLOSED){
          arrayPre.state = CLOSED;
          arrayPre.start_time = 0;
          aPre_timeout = 0;
        }
      }
    }
    else if (disable_arrayPre){ //handle arrayPre disable when array was just disabled
      if (arrayPre.start_time == 0){
        arrayPre.start_time = time;
        a_pre_enable(0);
      }
      else if (arrayPre.sense == CLOSED && arrayPre.start_time + 100 < time){
        fault[9] = FAULT; //array precharge turn off timeout fault
      }
      else if (arrayPre.sense == OPEN){
        arrayPre.state = OPEN;
        arrayPre.start_time = 0;
        disable_arrayPre = false;
      }
    }
    */
  }
}