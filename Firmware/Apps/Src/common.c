#include "common.h"
#include "StatusLEDs.h"
#include "Contactors.h"
#include "CANMetaData.h"

void error_handler(void) {
  while(1) {}
}

void success_handler(void) {
  Status_Leds_Init();
  while(1){
    Status_Leds_Toggle(MOTOR_SENSE_FAULT_LED);
    HAL_Delay(500);
  }
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

  // TODO: add fault LEDs for all other faults...

  if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();

  while (1) {
    // blah blah
  }
}