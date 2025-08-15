#include "common.h"
#include "StatusLEDs.h"
#include "Contactors.h"
#include "CANMetaData.h"
#include "ContactorTask.h"
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
 * @brief   Reads fault bitmap - sets status LEDs and sends CAN message based on the fault type
 */
void fault_handler(void) {
  // Disable all contactors with direct control regardless of which faulted
  Contactors_EmergencyDisable();

  // Setup fault CAN message
  CAN_TxHeaderTypeDef tx_header = { 0 };
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.IDE = CAN_ID_STD;
  tx_header.DLC = CONTACTOR_SENSE_LENGTH;
  tx_header.TransmitGlobalTime = DISABLE;
  uint8_t tx_data[8] = { 0 };

  // Turn on fault LEDs and set message based on fault bits
  if (fault_bitmap & FAULT_MOTOR_PRECHARGE_TIMEOUT) {
    // Turn on fault status LED
    Status_Leds_Write(MOTOR_TIMEOUT_FAULT_LED, true);
    // Send fault CAN message
    tx_header.StdId = PRECHARGE_TIMEOUT_;
    tx_data[0] = (1 << 0); // Motor Precharge Timeout
  }
  else if (fault_bitmap & FAULT_ARRAY_PRECHARGE_TIMEOUT) {
    // Turn on fault status LED
    Status_Leds_Write(ARRAY_TIMEOUT_FAULT_LED, true);
    // Send fault CAN message
    tx_header.StdId = PRECHARGE_TIMEOUT_;
    tx_data[0] = (1 << 1); // Array Precharge Timeout
  }
  else if (fault_bitmap & FAULT_MOTOR_PRECHARGE_SENSE) {
    // Turn on fault status LED
    Status_Leds_Write(MOTOR_SENSE_FAULT_LED, true);
    // Send fault CAN message
    tx_header.StdId = CONTACTOR_SENSE;
    tx_data[0] = (1 << 5); // Motor Precharge Sense Fault
    // TODO: add other info to all sense fault messages (expected vs. actual sense value)
  }
  else if (fault_bitmap & FAULT_ARRAY_PRECHARGE_SENSE) {
    // Turn on fault status LED
    Status_Leds_Write(ARRAY_SENSE_FAULT_LED, true);
    // Send fault CAN message
    tx_header.StdId = CONTACTOR_SENSE;
    tx_data[1] = (1 << 0); // Array Precharge Sense Fault
    
    // TODO: add other info to all sense fault messages (expected vs. actual sense value)
  }

  // bool motor_precharge_fault = (fault_bitmap & FAULT_MOTOR_PRECHARGE_SENSE);
  bool array_precharge_fault = (fault_bitmap & FAULT_ARRAY_PRECHARGE_SENSE);
  if(array_precharge_fault) {
    Status_Leds_Write(ARRAY_SENSE_FAULT_LED, true);
  }
  // bool motor_fault = (fault_bitmap & FAULT_MOTOR);

  while (1) {
    // Send fault msg every 200ms
    if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();
    
    //vTaskDelay(FAULT_MESSAGE_DELAY);
    //if (can_send(hcan1, &tx_header, tx_data, portMAX_DELAY) != CAN_SENT) error_handler();
    
    //Status_Leds_Write(ONBOARD_LED, false);

    //vTaskDelay(FAULT_MESSAGE_DELAY);
    Status_Leds_Toggle(ONBOARD_LED);
    Contactors_EmergencyDisable();
    HAL_Delay(500);

  }
}

void HardFault_Handler(){
  error_handler();

}

void BusFault_Handler(){
  error_handler();
}

void UsageFault_Handler(){
  error_handler();
}