#include "contactor.h"

void contactorCAN_Init(){

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
  hcan1->Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan1->Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan1->Init.TimeSeg2 = CAN_BS2_6TQ;
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

void contactorCANTask(void *pvParamters)
{
  
  // create payload to send
  CAN_TxHeaderTypeDef tx_header = {0};   
  tx_header.StdId = 0x1;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.IDE = CAN_ID_STD;
  tx_header.DLC = 2;
  tx_header.TransmitGlobalTime = DISABLE;

  // send two payloads to 0x1
  uint8_t tx_data[8] = {0};
  tx_data[0] = 0x01;
  tx_data[1] = 0x00;
  //HAL_Delay(5000);
  if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

  success_handler();
}

