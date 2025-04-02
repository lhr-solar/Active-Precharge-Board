#include "contactor.h"

void CANInitTask(){

  // HAL_Delay(1000);
  CAN_FilterTypeDef  sFilterConfig;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  // setup can1 init
  hcan1->Init.Prescaler = 16;
  hcan1->Init.Mode = CAN_MODE_LOOPBACK;
  hcan1->Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1->Init.TimeSeg1 = CAN_BS1_16TQ;
  hcan1->Init.TimeSeg2 = CAN_BS2_8TQ;
  hcan1->Init.TimeTriggeredMode = DISABLE;
  hcan1->Init.AutoBusOff = DISABLE;
  hcan1->Init.AutoWakeUp = DISABLE;
  hcan1->Init.AutoRetransmission = ENABLE;
  hcan1->Init.ReceiveFifoLocked = DISABLE;

  // If TransmitFifoPriority is disabled, the hardware selects the mailbox based on the message ID priority. 
  // If enabled, the hardware uses a FIFO mechanism to select the mailbox based on the order of transmission requests.
  hcan1->Init.TransmitFifoPriority = ENABLE;

  // initialize CAN1
  if (can_init(hcan1, &sFilterConfig) != CAN_OK) error_handler();
  if (can_start(hcan1) != CAN_OK) error_handler();

}



void Task_Init(){
    Init_WDogTask();
    Heartbeat_Init();

    xTaskCreateStatic(
        CANInitTask,           /* The function that implements the task. */
        "CAN Init Task",         /* Text name for the task. */
        configMINIMAL_STACK_SIZE,/* The size (in words) of the stack that should be created for the task. */
        (void*)NULL,                        /* Paramter passed into the task. */
        tskIDLE_PRIORITY + 1,      /* Task Prioriy. */
        cantask_stack,       /* Stack array. */
        &cantask_buffer            /* Buffer for static allocation. */
   );

   // Task deletes itself after all other taks are init'd
    vTaskDelete(NULL);
}