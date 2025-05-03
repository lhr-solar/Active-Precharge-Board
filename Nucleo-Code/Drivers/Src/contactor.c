#include "contactor.h"
#include "statusLEDs.h"

// structs to manage contactor states
Contactor motor = { OPEN, OPEN, OPEN, NO_FAULT, 0 };
Contactor array = { OPEN, OPEN, OPEN, NO_FAULT, 0 };
prechargeContactor motorPre = { OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0 };
prechargeContactor arrayPre = { OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0 };

uint32_t time;  // HAL tick value (ms)
uint8_t fault[10];  // TODO: handle this better, struct?

// various timing stuff
// TODO: better names for these
uint32_t mPre_initial_delay = 0;
uint32_t aPre_initial_delay = 0;
uint32_t mPre_timeout = 0;
uint32_t aPre_timeout = 0;

bool disable_motorPre = false; // semaphore, true when motor successfully disabled in event in turn off sequence
bool disable_arrayPre = false; // semaphore, true when array successfully disabled whenever its supposed to be
bool CAN_fault = false; // in CAN handler set this to true for any car faults such as BPS unsafe

// GPIO Init Function
void contactors_init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
}

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

// handles all contactor and precharge logic
void contactors_handler(void) {
  time = HAL_GetTick();

  // read pins
  motor.enable_in = (motor_enable_read());
  motor.sense = motor_sense();
  motorPre.pre_ready = motor_precharge_ready();
  motorPre.sense = motor_precharge_sense();

  // TODO: handle array enable/sense from BPS/Controls....
  // array.enable_in = from CAN
  // array.sense = from CAN
  arrayPre.pre_ready = array_precharge_ready();
  arrayPre.sense = array_precharge_sense();

  // handle faults
  // TODO: make this a function
  if ((fault[0] | fault[1] | fault[2] | fault[3] | fault[4] | fault[5] | fault[6] | fault[7] | fault[8] | fault[9] | CAN_fault) == FAULT) {

    // disable motor & array precharge contactors
    motor_precharge_enable(0);
    array_precharge_enable(0);

    // TODO: send CAN specific message based on fault

    // TODO: if (CAN_fault) {do nothing, only disable contactors;}

    if (fault[0]) { // m_enable unplug
      motor_sense_fault_led(1);
      motor_timeout_fault_led(1);
      // send this specific CAN message
    }
    if (fault[1]) { // motor sense fault
      motor_sense_fault_led(1);
      // send this specific CAN message
    }
    if (fault[2]) { // motor timeout fault
      motor_timeout_fault_led(1);
      // send this specific CAN message
    }
    if (fault[3]) { // motorPre sense fault
      motor_sense_fault_led(1);
      // send this specific CAN message
    }
    if (fault[4]) { // motorPre timeout fault
      motor_timeout_fault_led(1);
      // send this specific CAN message
    }
    if (fault[5]) { // a_enable fault
      array_sense_fault_led(1);
      array_timeout_fault_led(1);
      // send this specific CAN message
    }
    if (fault[6]) { // array sense fault
      array_sense_fault_led(1);
      // send this specific CAN message
    }
    if (fault[7]) { // array timeout fault
      array_timeout_fault_led(1);
      // send this specific CAN message
    }
    if (fault[8]) { // arrayPre sense fault
      array_sense_fault_led(1);
      // send this specific CAN message
    }
    if (fault[9]) { // arrayPre timeout fault
      array_timeout_fault_led(1);
      // send this specific CAN message
    }

  }

  else { // executes if faults are all clear
    // motor contactor logic
    if (motor.enable_in == CLOSED && motor.state == OPEN) {
      if (motor.start_time == 0) {
        motor.start_time = time;
      }
      else if ((motor.sense == OPEN) && (motor.start_time + MOTOR_TIMEOUT < time)) { // if 100ms after contactor enabled, fault, not reading sense right
        fault[1] = FAULT; // motor sense fault
      }
      else if (motor.sense == CLOSED) {
        if (mPre_initial_delay == 0) {
          mPre_initial_delay = time;
        }
        else if (mPre_initial_delay + MOTOR_PRECHARGE_STEADYSTATE_TIMEOUT < time) { // bake in 100ms of delay for the contactor to fully close and reset precharge logic
          motor.state = CLOSED;
          motor.start_time = 0;
          mPre_initial_delay = 0;
          mPre_timeout = time;
        }
      }
    }
    else if (motor.enable_in == OPEN && motor.state == CLOSED) {
      if (false) { // TODO: change false to say if were in the right part if ignition state for m.enable to go low, then
        if (motor.start_time == 0) {
          motor.start_time = time;
        }
        else if ((motor.sense == CLOSED) && (motor.start_time + MOTOR_TIMEOUT < time)) {
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
        else if (motor.start_time + UNPLUG_DELAY < time) {
          fault[0] = FAULT; // otherwise, m.enable probably unplugged, fault
        }
      }

    }
    else if (motor.enable_in == OPEN && motor.state == OPEN) { // if conactor is unenabled and sense high, wait 10ms and if still true, fault
      if (motor.sense == CLOSED) {
        if (motor.start_time == 0) {
          motor.start_time = time;
        }
        else if (motor.start_time + UNPLUG_DELAY > time) {
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
        else if (motor.start_time + UNPLUG_DELAY > time) {
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
        else if (motorPre.state == CLOSED && mPre_timeout + MOTOR_PRECHARGE_STEADYSTATE_TIMEOUT < time) {
          fault[4] = FAULT; // motor precharge steady state timeout fault
        }
        else if (mPre_timeout + MOTOR_PRECHARGE_INITIAL_TIMEOUT < time) {
          fault[4] = FAULT; // motor precharge initial timeout fault
        }
      }
      else if (motorPre.pre_ready == CLOSED) {
        motor_precharge_enable(1);
        if (motorPre.start_time == 0) {
          motorPre.start_time = time;
        }
        else if ((motorPre.sense == OPEN) && (motorPre.start_time + MOTOR_PRECHARGE_STEADYSTATE_TIMEOUT < time)) { // if 100ms after contactor enabled, fault, not reading sense right
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
        motor_precharge_enable(0);
      }
      else if (motorPre.sense == CLOSED && motorPre.start_time + MOTOR_PRECHARGE_STEADYSTATE_TIMEOUT < time) {
        fault[4] = FAULT; // motor precharge turn off timeout fault
      }
      else if (motorPre.sense == OPEN) {
        motorPre.state = OPEN;
        motorPre.start_time = 0;
        disable_motorPre = false;
      }
    }

    // array logic never tested before but identical to motor
    // array contactor logic
    if (array.enable_in == CLOSED && array.state == OPEN) {
      if (array.start_time == 0) {
        array.start_time = time;
      }
      else if ((array.sense == OPEN) && (array.start_time + ARRAY_TIMEOUT < time)) {  //if 100ms after contactor enabled, fault, not reading sense right
        fault[6] = FAULT; //array sense fault
      }
      else if (array.sense == CLOSED) {
        if (aPre_initial_delay == 0) {
          aPre_initial_delay = time;
        }
        else if (aPre_initial_delay + ARRAY_TIMEOUT < time) {  //bake in 100ms of delay for the contactor to fully close and reset precharge logic
          array.state = CLOSED;
          array.start_time = 0;
          aPre_initial_delay = 0;
          aPre_timeout = time;
        }
      }
    }
    else if (array.enable_in == OPEN && array.state == CLOSED) {
      if (false) { //change false to say if were in the right part if ignition state for a.enable to go low, then
        if (array.start_time == 0) {
          array.start_time = time;
        }
        else if ((array.sense == CLOSED) && (array.start_time + ARRAY_TIMEOUT < time)) {
          fault[7] = FAULT;  //array timeout fault
        }
        else if (array.sense == OPEN) {
          array.state = OPEN;
          array.start_time = 0;
          disable_arrayPre = true;  //sempahore, turn off arrayPre
        }
      }
      else {
        if (array.start_time == 0) {
          array.start_time = time;
        }
        else if (array.start_time + UNPLUG_DELAY < time) {
          fault[5] = FAULT; //otherwise, were not in the right part of ignition state for this to happen, fault
        }
      }

    }
    else if (array.enable_in == OPEN && array.state == OPEN) { //if conactor is unenabled and sense high, wait 10ms and if still true, fault
      if (array.sense == CLOSED) {
        if (array.start_time == 0) {
          array.start_time = time;
        }
        else if (array.start_time + UNPLUG_DELAY > time) {
          fault[6] = FAULT; //array sense fault
        }
      }
      else array.start_time = 0;
    }
    else if (array.enable_in == CLOSED && array.state == CLOSED) { //if conactor is enabled and sense low, wait 10ms and if still true, fault
      if (array.sense == OPEN) {
        if (array.start_time == 0) {
          array.start_time = time;
        }
        else if (array.start_time + UNPLUG_DELAY > time) {
          fault[6] = FAULT; //array sense fault
        }
      }
      else array.start_time = 0;
    }

    // arrayPre logic
    if (array.state == CLOSED) { //handle event where pre_ready is high bc array.state was just open and enabling arrayPre after
      if (arrayPre.pre_ready == OPEN) {
        if (aPre_timeout == 0) {
          aPre_timeout = time;
        }
        else if (arrayPre.state == CLOSED && aPre_timeout + ARRAY_PRECHARGE_STEADYSTATE_TIMEOUT < time) {
          fault[9] = FAULT; //array precharge steady state timeout fault
        }
        else if (aPre_timeout + ARRAY_PRECHARGE_INITIAL_TIMEOUT < time) {
          fault[9] = FAULT; //array precharge initial timeout fault
        }
      }
      else if (arrayPre.pre_ready == CLOSED) {
        array_precharge_enable(1);
        if (arrayPre.start_time == 0) {
          arrayPre.start_time = time;
        }
        else if ((arrayPre.sense == OPEN) && (arrayPre.start_time + ARRAY_PRECHARGE_INITIAL_TIMEOUT < time)) {  //if 100ms after contactor enabled, fault, not reading sense right
          fault[8] = FAULT; //array precharge sense fault
        }
        else if (arrayPre.sense == CLOSED) {
          arrayPre.state = CLOSED;
          arrayPre.start_time = 0;
          aPre_timeout = 0;
        }
      }
    }
    else if (disable_arrayPre) { //handle arrayPre disable when array was just disabled
      if (arrayPre.start_time == 0) {
        arrayPre.start_time = time;
        array_precharge_enable(0);
      }
      else if (arrayPre.sense == CLOSED && arrayPre.start_time + ARRAY_PRECHARGE_INITIAL_TIMEOUT < time) {
        fault[9] = FAULT; //array precharge turn off timeout fault
      }
      else if (arrayPre.sense == OPEN) {
        arrayPre.state = OPEN;
        arrayPre.start_time = 0;
        disable_arrayPre = false;
      }
    }
  }
}