#include "common.h"
#include "StatusLEDs.h"
#include "Contactors.h"
#include "CANMetaData.h"

void error_handler(void) {
  while (1) {
    Status_Leds_Toggle(MOTOR_SENSE_FAULT_LED);
    HAL_Delay(500);
  }
}

void success_handler(void) {
  while (1) {
    Status_Leds_Toggle(ARRAY_SENSE_FAULT_LED);
    HAL_Delay(500);
  }
}

/**
* @brief Configure the can filter
* @retval None
*/
void can_filter_config(CAN_FilterTypeDef* sFilterConfig) {
  sFilterConfig->FilterBank = 0;
  sFilterConfig->FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig->FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig->FilterIdHigh = 0x0000;
  sFilterConfig->FilterIdLow = 0x0000;
  sFilterConfig->FilterMaskIdHigh = 0x0000;
  sFilterConfig->FilterMaskIdLow = 0x0000;
  sFilterConfig->FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig->FilterActivation = ENABLE;
  sFilterConfig->SlaveStartFilterBank = 14;
}

/**
* @brief Configure can1
* @retval None
*/
void can1_config(void) {
  hcan1->Init.Prescaler = 40;

#ifdef CAN_LOOPBACK
  hcan1->Init.Mode = CAN_MODE_LOOPBACK;
#else
  hcan1->Init.Mode = CAN_MODE_NORMAL;
#endif

  hcan1->Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1->Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1->Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1->Init.TimeTriggeredMode = DISABLE;
  hcan1->Init.AutoBusOff = DISABLE;
  hcan1->Init.AutoWakeUp = DISABLE;
  hcan1->Init.AutoRetransmission = ENABLE;
  hcan1->Init.ReceiveFifoLocked = DISABLE;

  // If TransmitFifoPriority is disabled, the hardware selects the mailbox based on the message ID priority. 
  // If enabled, the hardware uses a FIFO mechanism to select the mailbox based on the order of transmission requests.
  hcan1->Init.TransmitFifoPriority = ENABLE;
}

/**
 * @brief   Reads fault bitmap - sets status LEDs and sends CAN message based on the fault type
 */
void fault_handler(void) {
  // Disable all contactors with direct control regardless of which faulted
  Contactors_EmergencyDisable();

  // Setup fault CAN message
  CAN_TxHeaderTypeDef tx_header = { 0 };
  tx_header.StdId = PRECHARGE_TIMEOUT_;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.IDE = CAN_ID_STD;
  tx_header.DLC = 2;
  tx_header.TransmitGlobalTime = DISABLE;
  uint8_t tx_data[8] = { 0 };

  // Turn on fault LEDs and set message based on fault bits
  if (fault_bitmap & FAULT_MOTOR_PRECHARGE_TIMEOUT) {
    // Turn on fault status LED
    Status_Leds_Write(MOTOR_TIMEOUT_FAULT_LED, true);
    // Send fault CAN message
    tx_data[0] = (1 << 0); // Motor Precharge Timeout
  } else if (fault_bitmap & FAULT_ARRAY_PRECHARGE_TIMEOUT) {
    // Turn on fault status LED
    Status_Leds_Write(ARRAY_TIMEOUT_FAULT_LED, true);
    // Send fault CAN message
    tx_data[0] = (1 << 1); // Array Precharge Timeout
  }

  if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

  while (1) {
    // blah blah
  }
}