// Tests ContactorTask using CAN loopback - simulates periodic BPS and controls CAN messages
// MUST be run with CAN_LOOPBACK = 1

#include "FreeRTOS.h" /* Must come first. */
#include "Tasks.h"
#include "common.h"
#include "stm32xx_hal.h"
#include "StatusLEDs.h"
#include "CANbus.h"
#include "Contactors.h"
#include "CANMetaData.h"

#define CAN_LOOPBACK_DELAY (250 / portTICK_PERIOD_MS)

// Task Stack Arrays 
StackType_t Task_Init_Stack_Array[TASK_INIT_STACK_SIZE];
StackType_t Task_CAN_Loopback_Stack_Array[TASK_CONTACTOR_STACK_SIZE];

// Task Buffers
StaticTask_t Task_CAN_Loopback_Buffer;
StaticTask_t Task_Init_Buffer;

void Task_CAN_Loopback() {
    CAN_TxHeaderTypeDef tx_header = { 0 };

    while (1) {
        // Send BPS Contactor State
        // create payload to send
        tx_header.StdId = BPS_CONTACTOR_STATE;
        tx_header.RTR = CAN_RTR_DATA;
        tx_header.IDE = CAN_ID_STD;
        tx_header.DLC = 1;
        tx_header.TransmitGlobalTime = DISABLE;

        // send payload
        uint8_t tx_data[8] = { 0 };
        tx_data[0] = 0x07;  // all contactors closed
        if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

        // Send Controls Fault State
        tx_header.StdId = CONTROLS_FAULT;
        tx_header.RTR = CAN_RTR_DATA;
        tx_header.IDE = CAN_ID_STD;
        tx_header.DLC = 1;
        tx_header.TransmitGlobalTime = DISABLE;

        // send payload
        uint8_t tx_data2[8] = { 0 };
        tx_data2[0] = 0x00;  // no fault
        if (can_send(hcan1, &tx_header, tx_data2, portMAX_DELAY) != CAN_SENT) error_handler();

        // Send Controls Ignition State
        tx_header.StdId = IO_STATE;
        tx_header.RTR = CAN_RTR_DATA;
        tx_header.IDE = CAN_ID_STD;
        tx_header.DLC = 3;
        tx_header.TransmitGlobalTime = DISABLE;

        // send payload
        uint8_t tx_data3[8] = { 0 };
        tx_data3[0] = 0x00;
        tx_data3[1] = 0x00;
        tx_data3[2] = 0x02; // motor ign state
        if (can_send(hcan1, &tx_header, tx_data3, portMAX_DELAY) != CAN_SENT) error_handler();
    }
}

void Task_Inits() {
    // Initialize contactors driver
    Contactors_Init();

    // Create CAN loopback task
    TaskHandle_t result = xTaskCreateStatic(
        Task_CAN_Loopback,                 /* The function that implements the task. */
        "CAN Loopback Task",               /* Text name for the task. */
        configMINIMAL_STACK_SIZE,      /* The size (in words) of the stack that should be created for the task. */
        (void*)NULL,                    /* Paramter passed into the task. */
        tskIDLE_PRIORITY + 2,            /* Task Prioriy. */
        Task_CAN_Loopback_Stack_Array,     /* Stack array. */
        &Task_CAN_Loopback_Buffer          /* Buffer for static allocation. */
    );

    // Create contactor task
    result = xTaskCreateStatic(
        Task_Contactor,                 /* The function that implements the task. */
        "Contactor Task",               /* Text name for the task. */
        TASK_CONTACTOR_STACK_SIZE,      /* The size (in words) of the stack that should be created for the task. */
        (void*)NULL,                    /* Paramter passed into the task. */
        tskIDLE_PRIORITY + 3,            /* Task Prioriy. */
        Task_Contactor_Stack_Array,     /* Stack array. */
        &Task_Contactor_Buffer          /* Buffer for static allocation. */
    );

    if (result == NULL) {

    }

    // Task deletes itself after all other tasks are init'd
    vTaskDelete(NULL);
}

int main() {
    HAL_Init();
    SystemClock_Config();

    // Init drivers for status LEDs, contactors, CAN bus
    Status_Leds_Init();
    CAN_Init();

    // Create Init Task
    xTaskCreateStatic(
        Task_Inits, /* The function that implements the task. */
        "Init Task", /* Text name for the task. */
        TASK_INIT_STACK_SIZE, /* The size (in words) of the stack that should be created for the task. */
        (void*)NULL, /* Paramter passed into the task. */
        TASK_INIT_PRIO, /* Task Prioriy. */
        Task_Init_Stack_Array, /* Stack array. */
        &Task_Init_Buffer  /* Buffer for static allocation. */
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