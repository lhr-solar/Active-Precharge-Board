// Test precharge and sense timers

#include "FreeRTOS.h" /* Must come first. */
#include "Tasks.h"
#include "common.h"
#include "stm32xx_hal.h"
#include "StatusLEDs.h"
#include "CANbus.h"
#include "Contactors.h"

StaticTask_t Task_Timers_Buffer;
StackType_t Task_Timers_Stack_Array[configMINIMAL_STACK_SIZE];

void Task_Timers() {
    Contactors_Init();

    fault_bitmap = FAULT_NONE;

    // Test 50ms timer - you won't be able to see this one
    // Can breakpoint at the callback and make sure it fires
    volatile BaseType_t result = xTimerStart(Contactors_GetSenseTimerHandle(ARRAY_PRECHARGE_CONTACTOR), 0);
    if (result != pdPASS) {
        Status_Leds_All_On();
    }
    result = xTimerIsTimerActive(Contactors_GetSenseTimerHandle(ARRAY_PRECHARGE_CONTACTOR));

    // Test 1s timer - should visibly delay before turning on timeout fault LED
    result = xTimerStart(Contactors_GetPrechargeTimerHandle(ARRAY_PRECHARGE_CONTACTOR), 0);
    if (result != pdPASS) {
        Status_Leds_All_On();
    }
    result = xTimerIsTimerActive(Contactors_GetPrechargeTimerHandle(ARRAY_PRECHARGE_CONTACTOR));
\
    // Test 50ms timer - you won't be able to see this one
    // Can breakpoint at the callback and make sure it fires
    result = xTimerStart(Contactors_GetSenseTimerHandle(MOTOR_PRECHARGE_CONTACTOR), 0);
    if (result != pdPASS) {
        Status_Leds_All_On();
    }
    result = xTimerIsTimerActive(Contactors_GetSenseTimerHandle(MOTOR_PRECHARGE_CONTACTOR));

    // Test 1s timer - should visibly delay before turning on timeout fault LED
    result = xTimerStart(Contactors_GetPrechargeTimerHandle(MOTOR_PRECHARGE_CONTACTOR), 0);
    if (result != pdPASS) {
        Status_Leds_All_On();
    }
    result = xTimerIsTimerActive(Contactors_GetPrechargeTimerHandle(MOTOR_PRECHARGE_CONTACTOR));

    while (1) {

    }
}

int main() {
    HAL_Init();
    SystemClock_Config();

    // Init drivers for status LEDs, contactors, CAN bus
    Status_Leds_Init();
    CAN_Init();

    // Create Init Task
    xTaskCreateStatic(
        Task_Timers, /* The function that implements the task. */
        "Timers Task", /* Text name for the task. */
        configMINIMAL_STACK_SIZE, /* The size (in words) of the stack that should be created for the task. */
        (void*)NULL, /* Paramter passed into the task. */
        TASK_INIT_PRIO, /* Task Prioriy. */
        Task_Timers_Stack_Array, /* Stack array. */
        &Task_Timers_Buffer  /* Buffer for static allocation. */
    );

    // Start the scheduler
    vTaskStartScheduler();

    error_handler();
    while (1) {
        // Scheduler should've started by now
        // Code should never enter this point
    }

    return 0;
}