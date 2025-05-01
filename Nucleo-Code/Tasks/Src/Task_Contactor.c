#include "Task_Contactor.h"


motorContactor motor = {OPEN, OPEN, OPEN, NO_FAULT, 0};
Precharge motorPre = {OPEN, OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0};
Precharge arrayPre = {OPEN, OPEN, OPEN, OPEN, OPEN, NO_FAULT, 0};

int time = 0;
int fault[5];

void Task_Contactors(void *pvParamters)
{
  // HAL_Delay(5000);
  while (1)
  {
    time = HAL_GetTick();
    // drive contactor based on sense
    
    motor.enable_in = (motor_direct());
    motor.sense = motor_sense();

    motorPre.pre_ready = motor_precharge_ready();
    motorPre.pre_sense = motor_precharge_sense();
    motorPre.sense_in = motor.sense;
    // motorPre.pre_ready = motorPre.sense_in;

    if(motor.fault==NO_FAULT){
      if(motor.state==OPEN && motor.enable_in==CLOSED){ // closing motor
        if(motor.start_time==0){ //
          motor.start_time = time;
        }else if(motor.start_time+1000<time){
          motor.fault = MOTOR_TIMEOUT_FAULT;
        }else if(motor.sense==CLOSED){
          motorPre.pre_sense = CLOSED;
          motor.state = CLOSED;
          motor.start_time = 0;
        }
      }else if(motor.state==CLOSED && motor.enable_in == OPEN){ // opening motor
        if(motor.start_time == 0){
          motor.start_time = time;
        }else if(motor.start_time+1000<time){
          motor.fault = MOTOR_TIMEOUT_FAULT;
        }else if(motor.sense==OPEN){
          motor.state = OPEN;
          motorPre.sense_in = OPEN;
          motor.start_time = 0;
        }
      }else if(motor.state == OPEN && motor.enable_in == OPEN){
        if(motor.sense == CLOSED){
          if(motor.start_time==0){
            motor.start_time = time;
          }else if(motor.start_time+10>time){
            motor.fault = MOTOR_SENSE_FAULT;
          }
        }else{
          motor.start_time = 0;
        }
      }else if(motor.state == CLOSED && motor.enable_in == CLOSED){
        if(motor.sense == OPEN){
          if(motor.start_time==0){
            motor.start_time = time;
          }else if(motor.start_time+10>time){
            motor.fault = MOTOR_SENSE_FAULT;
          }
        }else{
          motor.start_time = 0;
        }
      }
    }else{
      fault[motor.fault] = 1;
    }

    if(motorPre.fault==NO_FAULT){
      if(motorPre.sense_in == OPEN){
        if(motorPre.pre_ready == CLOSED){
          motorPre.fault = MOTOR_SENSE_FAULT;
        }
      }else if(motorPre.state==OPEN && motorPre.pre_ready==CLOSED){ // closing motorPre
        if(motorPre.start_time==0){
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MOTOR_TIMEOUT_FAULT;
        }else if(motorPre.pre_sense==CLOSED){
          motorPre.state = CLOSED;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state==CLOSED && motorPre.pre_ready == OPEN){ // opening motorPre
        if(motorPre.start_time == 0){
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MOTOR_TIMEOUT_FAULT;
        }else if(motorPre.pre_sense==OPEN){
          motorPre.state = OPEN;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == OPEN && motorPre.pre_ready == OPEN){
        if(motorPre.pre_sense == CLOSED){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MOTOR_SENSE_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == CLOSED && motorPre.pre_ready == CLOSED){
        if(motorPre.pre_sense == OPEN){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MOTOR_SENSE_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }
    }else{
      fault[motorPre.fault] = 1;
    }

    if((fault[0]| fault[1] | fault[2] | fault[3] | fault[4]) == NO_FAULT){
      if(motorPre.start_time+500<time && motorPre.start_time!=0){
        motor_precharge_enable(motorPre.pre_ready);
      }
      // motor_precharge_enable(motor_sense());
    }else{
      motor_precharge_enable(0);
    }

    motor_timeout_fault(fault[MOTOR_TIMEOUT_FAULT]);
    motor_sense_fault(fault[MOTOR_TIMEOUT_FAULT]);

    #ifdef contactorcode
    // Read Inputs
    motor.enable_in = (motor_direct());
    motor.sense = motor_sense();

    motorPre.pre_ready = motor_precharge_ready();
    motorPre.pre_sense = motor_precharge_sense();
    // motorPre.sense_in = motor.sense;

    arrayPre.pre_ready = array_precharge_ready();
    arrayPre.pre_sense = array_precharge_sense();

    motorPre.enable_out = motor_precharge_ready();
    arrayPre.enable_out = array_precharge_ready();

    //Update States and Faults
    if(motor.fault==NO_FAULT){
      if(motor.state==OPEN && motor.enable_in==CLOSED){ // closing motor
        if(motor.start_time==0){ //
          motor.start_time = time;
        }else if(motor.start_time+1000<time){
          motor.fault = MT_FAULT;
        }else if(motor.sense==CLOSED){
          motorPre.pre_sense = CLOSED;
          motor.state = CLOSED;
          motor.start_time = 0;
        }
      }else if(motor.state==CLOSED && motor.enable_in == OPEN){ // opening motor
        if(motor.start_time == 0){
          motor.start_time = time;
        }else if(motor.start_time+1000<time){
          motor.fault = MT_FAULT;
        }else if(motor.sense==OPEN){
          motor.state = OPEN;
          motorPre.sense_in = OPEN;
          motor.start_time = 0;
        }
      }else if(motor.state == OPEN && motor.enable_in == OPEN){
        if(motor.sense == CLOSED){
          if(motor.start_time==0){
            motor.start_time = time;
          }else if(motor.start_time+10>time){
            motor.fault = MS_FAULT;
          }
        }else{
          motor.start_time = 0;
        }
      }else if(motor.state == CLOSED && motor.enable_in == CLOSED){
        if(motor.sense == OPEN){
          if(motor.start_time==0){
            motor.start_time = time;
          }else if(motor.start_time+10>time){
            motor.fault =MS_FAULT;
          }
        }else{
          motor.start_time = 0;
        }
      }
    }else{
      fault[motor.fault] = 1;
    }

    if(motorPre.fault==NO_FAULT){
      if(motorPre.state==OPEN && motorPre.sense_in==CLOSED){ // closing motorPre
        if(motorPre.start_time==0){ //
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MT_FAULT;
        }else if(motorPre.pre_sense==CLOSED){
          motorPre.state = CLOSED;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state==CLOSED && motorPre.sense_in == OPEN){ // opening motorPre
        if(motorPre.start_time == 0){
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MT_FAULT;
        }else if(motorPre.pre_sense==OPEN){
          motorPre.state = OPEN;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == OPEN && motorPre.sense_in == OPEN){
        if(motorPre.pre_sense == CLOSED){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MS_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == CLOSED && motorPre.sense_in == CLOSED){
        if(motorPre.pre_sense == OPEN){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MS_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }
    }else{
      fault[motorPre.fault] = 1;
    }
    
    if(arrayPre.fault==NO_FAULT){
      if(arrayPre.state==OPEN && arrayPre.sense_in==CLOSED){ // closing arrayPre
        if(arrayPre.start_time==0){ //
          arrayPre.start_time = time;
        }else if(arrayPre.start_time+1000<time){
          arrayPre.fault = AT_FAULT;
        }else if(arrayPre.pre_sense==CLOSED){
          arrayPre.state = CLOSED;
          arrayPre.start_time = 0;
        }
      }else if(arrayPre.state==CLOSED && arrayPre.sense_in == OPEN){ // opening arrayPre
        if(arrayPre.start_time == 0){
          arrayPre.start_time = time;
        }else if(arrayPre.start_time+1000<time){
          arrayPre.fault = AT_FAULT;
        }else if(arrayPre.pre_sense==OPEN){
          arrayPre.state = OPEN;
          arrayPre.start_time = 0;
        }
      }else if(arrayPre.state == OPEN && arrayPre.sense_in == OPEN){
        if(arrayPre.pre_sense == CLOSED){
          if(arrayPre.start_time==0){
            arrayPre.start_time = time;
          }else if(arrayPre.start_time+10>time){
            arrayPre.fault = AS_FAULT;
          }
        }else{
          arrayPre.start_time = 0;
        }
      }else if(arrayPre.state == CLOSED && arrayPre.sense_in == CLOSED){
        if(arrayPre.pre_sense == OPEN){
          if(arrayPre.start_time==0){
            arrayPre.start_time = time;
          }else if(arrayPre.start_time+10>time){
            arrayPre.fault = AS_FAULT;
          }
        }else{
          arrayPre.start_time = 0;
        }
      }
    }else{
      fault[arrayPre.fault] = 1;
    }
    if(arrayPre.start_time == 0 && arrayPre.enable_out == CLOSED){
      arrayPre.start_time = time;
    }

    //Check Faults

    if ((motor.fault | motorPre.fault | arrayPre.fault | BPS_safe | moco_fault) == NO_FAULT)
    {
      motorPre.enable_out = motorPre.sense_in & motorPre.pre_ready;
      arrayPre.enable_out = arrayPre.sense_in & arrayPre.pre_ready;
    }else{
      motorPre.enable_out = 0;
      arrayPre.enable_out = 0;
      //sense fault on CAN
      // at_fault(1);
      if(arrayPre.fault==AS_FAULT){
        at_fault(1);
      }
    }
    mt_fault(fault[MT_FAULT]);
    ms_fault(fault[MS_FAULT]);
    at_fault(fault[AT_FAULT]);
    as_fault(fault[AS_FAULT]);

    
    motor_precharge_enable(motorPre.enable_out);
    array_precharge_enable(arrayPre.enable_out);

    #endif

  }
}




/*
int main()
{
  HAL_Init();
  HAL_MspInit();
  MX_GPIO_Init();
  Task_Init();
  SystemClock_Config();

  
  // as_fault(1);
  // HAL_Delay(5000);
  // as_fault(0);

  // xTaskCreateStatic(canTask, "can task", configMINIMAL_STACK_SIZE,
  //   NULL, tskIDLE_PRIORITY + 2, cantask_stack, &cantask_buffer);
  // xTaskCreateStatic(contactorTask, "contactor task", configMINIMAL_STACK_SIZE,
  //                   NULL, tskIDLE_PRIORITY + 3, contactortask_stack, &contactortask_buffer);

  vTaskStartScheduler();

  error_handler();
  return 0;
}
*/