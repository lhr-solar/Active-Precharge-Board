#include "CANbus.h"
#include "common.h"

void CAN_Init(){
  // initialize CAN1
  CAN_FilterTypeDef sFilterConfig;
  can_filter_config(&sFilterConfig);

  can1_config();

  if (can_init(hcan1, &sFilterConfig) != CAN_OK) error_handler();
  if (can_start(hcan1) != CAN_OK) error_handler();
}
