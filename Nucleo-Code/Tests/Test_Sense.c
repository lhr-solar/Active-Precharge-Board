#include "FreeRTOS.h" /* Must come first. */
#include "task.h" 
#include "contactor.h"
#include "CAN.h"
#include "common.h"
// #include "pinConfig.h"
#include "stm32xx_hal.h"

void error_handler(void)
{
  while (1)
  {
    mt_fault(1);
    ms_fault(1);
    at_fault(1);
    as_fault(1);
    HAL_Delay(1000);
    mt_fault(0);
    ms_fault(0);
    at_fault(0);
    as_fault(0);
    HAL_Delay(1000);
  }
}

void contactorTask(void *pvParamters)
{
  // HAL_Delay(5000);
  while (1)
  {
    // error_handler();
    time = HAL_GetTick();
    // drive contactor based on sense
    
    motor.enable_in = (m_direct());
    motor.sense = m_sense();

    motorPre.pre_ready = m_pre_ready();
    motorPre.pre_sense = m_pre_sense();
    motorPre.sense_in = motor.sense;

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
      if(motorPre.sense_in == OPEN){
        if(motorPre.pre_ready == CLOSED){
          motorPre.fault = MS_FAULT;
        }
      }else if(motorPre.state==OPEN && motorPre.pre_ready==CLOSED){ // closing motorPre
        if(motorPre.start_time==0){ //
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MT_FAULT;
        }else if(motorPre.pre_sense==CLOSED){
          motorPre.state = CLOSED;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state==CLOSED && motorPre.pre_ready == OPEN){ // opening motorPre
        if(motorPre.start_time == 0){
          motorPre.start_time = time;
        }else if(motorPre.start_time+1000<time){
          motorPre.fault = MT_FAULT;
        }else if(motorPre.pre_sense==OPEN){
          motorPre.state = OPEN;
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == OPEN && motorPre.pre_ready == OPEN){
        if(motorPre.pre_sense == CLOSED){
          if(motorPre.start_time==0){
            motorPre.start_time = time;
          }else if(motorPre.start_time+10>time){
            motorPre.fault = MS_FAULT;
          }
        }else{
          motorPre.start_time = 0;
        }
      }else if(motorPre.state == CLOSED && motorPre.pre_ready == CLOSED){
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

    if((fault[0]| fault[1] | fault[2] | fault[3] | fault[4]) == NO_FAULT){
      if(motorPre.start_time<time && motorPre.start_time!=0){
        m_pre_enable(motorPre.pre_ready);
      }
      // m_pre_enable(m_sense());
    }else{
      m_pre_enable(0);
    }

    mt_fault(fault[MT_FAULT]);
    ms_fault(fault[MS_FAULT]);

  }
}

StaticTask_t Task_Init_Buffer;
StackType_t Task_Init_Stack_Array[configMINIMAL_STACK_SIZE];
int main() {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    xTaskCreateStatic(
                    contactorTask, /* The function that implements the task. */
                    "Contactor Task", /* Text name for the task. */
                    configMINIMAL_STACK_SIZE, /* The size (in words) of the stack that should be created for the task. */
                    (void*)NULL, /* Paramter passed into the tas    k. */
                    tskIDLE_PRIORITY, /* Task Prioriy. */
                    Task_Init_Stack_Array, /* Stack array. */
                    &Task_Init_Buffer  /* Buffer for static allocation. */
   );
    // Start the scheduler
    vTaskStartScheduler();

    error_handler();
    while(1){
        // Scheduler should've started by now
        // Code should never enter this point
    }
    
    return 0;
}